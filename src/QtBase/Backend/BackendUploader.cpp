/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required files
#include "BackendUploader.h"
#include <Core/LogManager.h>
#include <Core/Json.h>
#include <EngineManager.h>
#include <License.h>
#include <Core/EventManager.h>
#include <Core/Timer.h>
#include "../QtBaseManager.h"
#include "../Windows/StatusPopupWindow.h"
#include "BackendInterface.h"
#include "BackendHelpers.h"
#include <QDateTime>
#include <QHttpPart>
#include <QMessageBox>
#include <QDir>


using namespace Core;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BackendUploaderCallback
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DefaultUploaderCallback : public BackendUploader::Callback
{
	public:
		DefaultUploaderCallback()
		{
			mStatusWindow = NULL;
		}

		void OnStartUpload()
		{
			mStatusWindow = GetQtBaseManager()->GetStatusPopupManager()->Create();
		}

		void OnFinishedUpload()					
		{
			GetQtBaseManager()->GetStatusPopupManager()->Remove(mStatusWindow);
			mStatusWindow = NULL;
		}

		void OnUploadProgress(const char* progressText, const char* subProgressText, float progress, float subProgress)
		{
			if (mStatusWindow != NULL)
			{
				String text;
				text.Format( "%s", progressText );
				mStatusWindow->ShowMessage( text.AsChar() );
				mStatusWindow->SetProgress(progress);
			}
		}

	private:
		StatusPopupWindow* mStatusWindow;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BackendUploader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// constructor
BackendUploader::BackendUploader(NetworkAccessManager* networkAccessManager, QObject* parent) : QObject(parent)
{
	mQueue					= new Queue(this);
	mNetworkAccessManager	= networkAccessManager;
	mCallback				= NULL;
	mIsBusy					= false;
	mIsEnabled				= true;
}


// destructor
BackendUploader::~BackendUploader()
{
	delete mQueue;
}


void BackendUploader::Start()
{
	if (mAutoStartTimer.isActive() == false)
		mAutoStartTimer.start( 1000, this );
}


// update the uploader
void BackendUploader::StartUpload(const char* token)
{
	// in case there is no callback defined yet, use the default callback
	if (mCallback == NULL)
		mCallback = new DefaultUploaderCallback();

	// store the token
	mToken = token;

	// update the upload queue and return directly if there is no file to be uploaded
	mQueue->ReInit( "*.nmd" );
	if (mQueue->IsEmpty() == true)
		return;

	mIsBusy = true;
	if (mCallback != NULL)
		mCallback->OnStartUpload();

	// start the upload process
	UploadNextFile();
}


// callback for upload status updates
void BackendUploader::OnUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
	mSubProgress = bytesSent / (float)bytesTotal;
	UpdateProgressCallback();
}


// helper function to calculate and set the progress value
void BackendUploader::UpdateProgressValue(uint32 currentFileIndex, bool isProcessing)
{
	const float uploadRange	= 1.0f - UPLOADER_PERCENTAGE_PROCESSING;
	float finalPercentage	= 0.0f;

	// uploading
	const float uploadPercentage= (float)currentFileIndex / (float)mQueue->GetNumEntriesInitTime();
	finalPercentage				= uploadPercentage * uploadRange;

	mTempString.Format( "Processing biodata (%i/%i)", currentFileIndex, mQueue->GetNumEntriesInitTime() );

	mProgressText	= mTempString.AsChar();
	mProgress		= finalPercentage;

	UpdateProgressCallback();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Uploading
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// upload the next file from the upload queue
void BackendUploader::UploadNextFile()
{
	// in case we are already uploading a file, skip directly
	if (mQueue->IsUploading() == true)
		return;

	// find the entry to be uploaded as next, return in case there is none
	QueueEntry* nextEntry = mQueue->FindNextUploadEntry();
	if (nextEntry == NULL)
	{
		emit UploadFinished();
		
		if (mCallback != NULL)
			mCallback->OnFinishedUpload();

		mIsBusy = false;

		return;
	}

	// start uploading the next entry
	if (UploadEntry(nextEntry) == false)
	{
		LogError( "Uploading '%s' failed. Removing it from queue.", nextEntry->GetAbsoluteFilePath() );
		RemoveCorrespondingFiles( nextEntry );
		UploadNextFile();
	}
}


// start uploading a file
bool BackendUploader::UploadEntry(QueueEntry* entry)
{
	// do not return false in case we're already uploading, only leave this assert here due to:
	// in case we return false we remove the data chunk channel files from disk and go on with the next entry
	CORE_ASSERT( mQueue->IsUploading() == false );

	// prepare entry for the upload process
	if (entry->OnStartUpload() == false)
		return false;

	// let the progress window know we're about to upload the next file
	mSubProgressText	= entry->GetFilenameString();
	mSubProgress		= 0.0f;
	UpdateProgressValue( entry->GetIndex(), false );


	String jsonFilename = entry->GetAbsoluteFilePathString();
	jsonFilename.RemoveFileExtension();
	jsonFilename += ".json";

	Json json;
	if (json.ParseFile(jsonFilename.AsChar()) == false)
		return false;

	Json::Item rootItem = json.GetRootItem();

	Json::Item userIdItem		= rootItem.Find("userId");
//	Json::Item classifierIdItem	= rootItem.Find("classifierId");
	Json::Item dataChunkIdItem	= rootItem.Find("dataChunkId");
	Json::Item nodeIdItem		= rootItem.Find("nodeId");
	Json::Item channelNameItem	= rootItem.Find("channelName");
	Json::Item typeItem			= rootItem.Find("type");
	Json::Item offsetItem		= rootItem.Find("offset");
	Json::Item sampleRateItem	= rootItem.Find("sampleRate");
//	Json::Item startTimeItem	= rootItem.Find("startDateTime");
	Json::Item visualMinItem	= rootItem.Find("visualMin");
	Json::Item visualMaxItem	= rootItem.Find("visualMax");

	String urlParameters, urlTemp;
	urlParameters.Format( "token=%s", mToken.AsChar() );

	if (userIdItem.IsString() == true)			{ urlTemp.Format( "&userId=%s", BackendHelpers::EncodeUrlParameter(userIdItem.GetString()).AsChar() ); urlParameters += urlTemp; }
	//if (classifierIdItem.IsString() == true)	{ urlTemp.Format( "&classifierId=%s", BackendHelpers::EncodeUrlParameter(classifierIdItem.GetString()).AsChar() ); urlParameters += urlTemp; }
	if (dataChunkIdItem.IsString() == true)		{ urlTemp.Format( "&dataChunkId=%s", BackendHelpers::EncodeUrlParameter(dataChunkIdItem.GetString()).AsChar() ); urlParameters += urlTemp; }
	if (nodeIdItem.IsString() == true)			{ urlTemp.Format( "&nodeId=%s", BackendHelpers::EncodeUrlParameter(nodeIdItem.GetString()).AsChar() ); urlParameters += urlTemp; }
	if (channelNameItem.IsString() == true)		{ urlTemp.Format( "&channelName=%s", BackendHelpers::EncodeUrlParameter(channelNameItem.GetString()).AsChar() ); urlParameters += urlTemp; }
	if (typeItem.IsInt() == true)				{ urlTemp.Format( "&type=%i", typeItem.GetInt() ); urlParameters += urlTemp; }
	if (offsetItem.IsInt() == true)				{ urlTemp.Format( "&offset=%i", offsetItem.GetInt() ); urlParameters += urlTemp; }
	if (sampleRateItem.IsDouble() == true)		{ urlTemp.Format( "&sampleRate=%f", sampleRateItem.GetDouble() ); urlParameters += urlTemp; }
	if (visualMinItem.IsNumber() == true)		{ urlTemp.Format( "&visualMin=%f", visualMinItem.GetDouble() ); urlParameters += urlTemp; }
	if (visualMaxItem.IsNumber() == true)		{ urlTemp.Format( "&visualMax=%f", visualMaxItem.GetDouble() ); urlParameters += urlTemp; }

	// create the network request
	QNetworkRequest request = mNetworkAccessManager->ConstructNetworkRequest( "datachunks/upload", urlParameters );

	// create the http multi part
	QHttpMultiPart* httpMultiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
	
	// create a http part and leave the content type empty
	QHttpPart contentPart;
	//contentPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(""));

	// construct and set the disposition header
	String dispositionHeader;
	dispositionHeader.Format("form-data; name=\"content\"; filename=\"%s\"", entry->GetFilenameString().AsChar());
	contentPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(dispositionHeader.AsChar()));
	
	// set the file as content body and parent it to the multi part object
	contentPart.setBodyDevice( entry->GetFile() );
	entry->GetFile()->setParent(httpMultiPart); // we cannot delete the file now, so delete it with the multiPart
	
	// add the content part to the http multi part
	httpMultiPart->append(contentPart);


	// post the http multi part
	QNetworkReply* reply = mNetworkAccessManager->post_Deprecated( request, httpMultiPart, NULL );
	httpMultiPart->setParent(reply); // delete the multiPart with the reply
	connect(reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(OnUploadProgress(qint64, qint64)));
	connect(reply, SIGNAL(finished()), this, SLOT(OnUploadFinished()));

	LogInfo( " - Starting upload: File='%s', AttemptNr=%i", entry->GetFilenameString().AsChar(), entry->GetUploadAttemptNr() );

	return true;
}


// called when the file got uploaded
void BackendUploader::OnUploadFinished()
{
	QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );
	if (GetBackendInterface()->GetNetworkAccessManager()->IsLogEnabled() == true && CORE_LOGMANAGER.GetLogLevels() & LOGLEVEL_DETAILEDINFO)
		LogDetailedInfo("REST REPLY: url=\"%s\"", FromQtString(BackendHelpers::ConvertToSecureUrl(networkReply->request().url()).toString()).AsChar());
	QByteArray replyData = networkReply->readAll();
	String replyDataString = FromQtString( QString(replyData) );
	bool hasError = BackendHelpers::HasError( networkReply, replyDataString.AsChar(), BACKEND_RETURNCODE_UPLOADRECEIVED );
	Json json;
	if (hasError == false)
		json.Parse( replyDataString.AsChar() );

	// find the entry that got uploaded
	QueueEntry* uploadEntry = mQueue->FindUploadingEntry();

	if (hasError == false)
	{
		// check if the type meta item is all fine
		Json::Item metaItem = json.Find("meta");
		if (metaItem.IsNull() == false)
		{
			Json::Item typeItem = metaItem.Find("type");
			if (typeItem.IsNull() == true)
			{
				hasError = true;
			}
			else
			{
				String type = typeItem.GetString();
				if (type.IsEqual("UPLOAD_RECEIVED") == false)
					hasError = true;
			}
		}

		LogInfo( "    + Finished upload: File='%s', AttemptNr=%i", uploadEntry->GetFilenameString().AsChar(), uploadEntry->GetUploadAttemptNr() );
	}

	bool cancelUpload = false;
	if (hasError == true)
	{
		// spread the error message
		String errorMsg;
		errorMsg.Format("Could not upload '%s'.", uploadEntry->GetFilename()/*, FromQtString(networkReply->errorString()).AsChar()*/);
		BackendHelpers::ReportError( errorMsg.AsChar() );
		//if (QMessageBox::critical( GetQtBaseManager()->GetMainWindow(), "Network Error", errorMsg.AsChar(), QMessageBox::Retry | QMessageBox::Cancel) == QMessageBox::Cancel)
		//	cancelUpload = true;
	}

	// cleanup the network reply
	mNetworkAccessManager->NetworkReplyAftermath( networkReply );

	// entry upload aftermath
	uploadEntry->OnUploadFinished( hasError );

	// remove files from disk and the entry from the queue
	if (hasError == false)
		RemoveCorrespondingFiles( uploadEntry );

	// go on with the next file, in case there is no next file it will automatically handle the next steps
	if (cancelUpload == false)
		UploadNextFile();
}


// auto-start timer
void BackendUploader::timerEvent(QTimerEvent* event)
{
	if (mIsEnabled == false || mIsBusy == true)
		return;

	// get access to the folder
	QDir directory( GetQtBaseManager()->GetPhysiologicalDataFolder() );
	QStringList filenames = directory.entryList(QDir::NoDotAndDotDot | QDir::Files);

	// only start the uploader in case there are files waiting and we have a valid token
	if (filenames.count() > 0 && GetUser()->GetTokenString().IsEmpty() == false)
		StartUpload( GetUser()->GetToken() );
}


bool BackendUploader::RemoveCorrespondingFiles(QueueEntry* entry)
{
	bool result = true;

	String nmdFilename = entry->GetAbsoluteFilePathString();
	String jsonFilename = entry->GetAbsoluteFilePathString();

	jsonFilename.RemoveFileExtension();
	jsonFilename += ".json";

	// 1. remove the entry from the queue
	// NOTE: this will release the QFile objects and will make them deletable
	if (mQueue->RemoveEntry(entry) == false)
	{
		mTempString.Format("Cannot queue entry (filename='%s'.", nmdFilename.AsChar());
		BackendHelpers::ReportError( mTempString.AsChar() );
		result = false;
	}

	// 2. delete the .NMD file locally from disk
	if (QFile::exists(nmdFilename.AsChar()) == true && QtBaseManager::RemoveFileFromDisk(nmdFilename.AsChar()) == false)
	{
		mTempString.Format("Cannot remove file '%s' from disk.", nmdFilename.AsChar());
		BackendHelpers::ReportError( mTempString.AsChar() );
		result = false;
	}

	// 3. delete the .JSON file locally from disk
	if (QFile::exists(jsonFilename.AsChar()) == true && QtBaseManager::RemoveFileFromDisk(jsonFilename.AsChar()) == false)
	{
		mTempString.Format("Cannot remove file '%s' from disk.", jsonFilename.AsChar());
		BackendHelpers::ReportError( mTempString.AsChar() );
		result = false;
	}

	return result;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// QueueEntry
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// constructor
BackendUploader::QueueEntry::QueueEntry(BackendUploader* uploader, const char* fullFilePath, uint32 index)
{
	mUploader			= uploader;
	mFullFilePath		= fullFilePath;
	mFilename			= mFullFilePath.ExtractFilename();
	mIndex				= index;
	mIsUploading		= false;
	mFinishedUpload		= false;
	mUploadAttemptNr	= 0;
	mFile				= NULL;
}


// destructor
BackendUploader::QueueEntry::~QueueEntry()
{
	if (mFile != NULL)
	{
		mFile->close();
		mFile->deleteLater();
	}

	mFile = NULL;
}


// called before uploading the given entry
bool BackendUploader::QueueEntry::OnStartUpload()
{
	// the file should not be loaded yet
	CORE_ASSERT( mFile == NULL );
	if (mFile == NULL)
	{
		// load the file
		mFile = new QFile( mFullFilePath.AsChar(), mUploader );
		if (mFile->open(QIODevice::ReadOnly) == false)
		{
			QMessageBox::critical( GetQtBaseManager()->GetMainWindow(), "File Error", "Cannot open file in read-mode." );
			CORE_ASSERT( false );
			mFile->deleteLater();
			mFile = NULL;
			return false;
		}
	}

	// enable the uploading flag and return success
	mIsUploading = true;
	mUploadAttemptNr++;
	return true;
}


// called when the upload finished
bool BackendUploader::QueueEntry::OnUploadFinished(bool error)
{
	mFile->close();
	mFile->deleteLater();
	mFile = NULL;

	// disable the uploading flag and enable the upload finished flag
	mIsUploading	= false;
	mFinishedUpload	= true;

	// error handling
	if (error == true)
	{
		mIsUploading	= false;
		mFinishedUpload	= false;
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Queue
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// log the current upload queue
void BackendUploader::Queue::Log()
{
	// get the number of files in the upload queue and log the filenames
	const uint32 numFiles = mEntries.Size();
	LogInfo(" - Upload Queue (NumFiles=%i)", numFiles);
	for (uint32 i=0; i<numFiles; ++i)
	{
		LogInfo("    + #%i: %s", mEntries[i]->GetIndex(), mEntries[i]->GetFilename() );
	}
}


// get rid of all elements in the upload queue
void BackendUploader::Queue::Clear()
{
	// get the number of entries in the queue and iterate through them
	const uint32 numEntries = mEntries.Size();
	for (uint32 i=0; i<numEntries; ++i)
		delete mEntries[i];

	// clear the array
	mEntries.Clear();
}


// put all files from the physiological data folder into the queue
void BackendUploader::Queue::ReInit(const char* fileExtension)
{
	// clear the queue upfront
	Clear();

	// fill it with all the files with the given file extension
	AddFiles( GetQtBaseManager()->GetPhysiologicalDataFolder(), fileExtension );

	// get and store the number of files to be uploaded in total
	mNumEntries = mEntries.Size();

	// log the upload queue
	Log();
}


// get the filenames of all files in the given directory and put them into the queue
void BackendUploader::Queue::AddFiles(const char* folderPath, const char* extensionFilter)
{
	// get access to the folder
	QDir directory(folderPath);

	// filter out given file types
    QStringList filters;
    filters << extensionFilter;
    directory.setNameFilters(filters);
	
	// get the filenames of all files in our directory
	QStringList		filenames		= directory.entryList(QDir::NoDotAndDotDot | QDir::Files);
	const uint32	numFilenames	= filenames.count();
	
	// return directly in case there are no files in the given directory
	if (numFilenames == 0)
		return;

	// iterate through all files
	for (uint32 i=0; i<numFilenames; ++i)
	{
		// construct the final filename
		String filename = folderPath;
		filename += FromQtString( filenames[i] );

		// only add the file to the queue in case it is not in yet
		if (IsInQueue(filename.AsChar()) == false)
			mEntries.Add( new QueueEntry(mUploader, filename.AsChar(), mEntries.Size()) );
	}
}


// check if the given filename is already in the queue and return the object
BackendUploader::QueueEntry* BackendUploader::Queue::FindEntry(const char* filename)
{
	// get the number of entries in the queue and iterate through them
	const uint32 numEntries = mEntries.Size();
	for (uint32 i=0; i<numEntries; ++i)
	{
		if (mEntries[i]->GetAbsoluteFilePathString().IsEqual(filename) == true)
			return mEntries[i];
	}

	// failure, the filename is not in the queue
	return NULL;
}


// remove the given file from the upload queue
bool BackendUploader::Queue::RemoveEntry(QueueEntry* entry)
{
	// get the number of entries in the queue and iterate through them
	const uint32 numEntries = mEntries.Size();
	for (uint32 i=0; i<numEntries; ++i)
	{
		if (mEntries[i] == entry)
		{
			delete mEntries[i];
			mEntries.Remove(i);
			return true;
		}
	}

	// failure, the filename is not in the queue
	return false;
}


// find the currently being upadloaded entry
BackendUploader::QueueEntry* BackendUploader::Queue::FindUploadingEntry() const
{
	// get the number of entries in the queue and iterate through them
	const uint32 numEntries = mEntries.Size();
	for (uint32 i=0; i<numEntries; ++i)
	{
		QueueEntry* entry = mEntries[i];
		if (entry->IsUploading() == true)
			return entry;
	}

	// failure, no entry is currently being uploaded
	return NULL;
}


// get the file that is next in the queue
BackendUploader::QueueEntry* BackendUploader::Queue::FindNextUploadEntry()
{
	// get the number of entries in the queue and iterate through them
	const uint32 numEntries = mEntries.Size();
	for (uint32 i=0; i<numEntries; ++i)
	{
		QueueEntry* entry = mEntries[i];
		if (entry->FinishedUpload() == false && entry->IsUploading() == false && entry->GetUploadAttemptNr() < mMaxNumAttempts)
			return entry;
	}

	// no more file to upload
	return NULL;
}
