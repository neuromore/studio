/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "WebDataCache.h"
#include <Core/Math.h>
#include <Core/LogManager.h>
#include "../QtBaseManager.h"
#include <QCryptographicHash>
#include <QDir>

using namespace Core;

// constructor
WebDataCache::WebDataCache(const char* subfolderPath, QObject *parent) : QObject(parent)
{
   mFolder = GetQtBaseManager()->GetAppDataFolder() + String(subfolderPath);
   mFolder.ConvertToNativePath();

   // make sure the cache folder exists
   if (!std::filesystem::exists(mFolder.AsChar()) && !std::filesystem::create_directories(mFolder.AsChar()))
      LogError("WebDataCache: Can't create cache folder '%s'", mFolder.AsChar());

   mStatusWindow                    = NULL;
   mCurrentDownload                 = 0;
   mMaxNumFails                     = 4;
   mMaxCacheRestrictionEnabled      = false;
   mMaxCacheSizeInMb                = 1024; // 1GB default
   mMaxAliveTimeRestrictionEnabled  = false;
   mMaxAliveTimeInDays              = 90; // 3 months default

#ifndef PRODUCTION_BUILD
   // log
   Log();
#endif
}


// destructor
WebDataCache::~WebDataCache()
{
}


bool WebDataCache::Download(const Core::Array<Core::String>& urls)
{
	// get the number of urls
	const uint32 numUrls = urls.Size();

	// in case we are already downloading
	// don't skip in case there is no file to download here!
	if (IsDownloading() == true)
		return false;

	// we're ready here already
	// don't forget to emit the finish signal in case a responder is waiting for it (shouldn't be skipped even if there is nothing to download)
	if (numUrls == 0)
	{
		emit FinishedDownload();
		return true;
	}

	// create a download item for each url
	mDownloadQueue.Resize( numUrls );
	String downloadFromUrl, saveToFilename;
	for (uint32 i=0; i<numUrls; ++i)
	{
		downloadFromUrl	= urls[i];
		saveToFilename	= UrlToFilename( downloadFromUrl.AsChar() );

		FileDownloader* downloader = new FileDownloader( QUrl(downloadFromUrl.AsChar()), saveToFilename.AsChar(), this );
		connect( downloader, SIGNAL(FinishedDownload(FileDownloader*)), this, SLOT(OnFileDownloaded(FileDownloader*)) );
		connect( downloader, SIGNAL(DownloadFailed(FileDownloader*)), this, SLOT(OnFileFailed(FileDownloader*)) );
		connect( downloader, SIGNAL(DownloadProgressChanged(qint64, qint64)), this, SLOT(OnDownloadProgressChanged(qint64, qint64)) );
		mDownloadQueue[i] = downloader;
	}

	// start downloading the queue
	Start();

	return true;
}


// start downloading all items in the queue
void WebDataCache::Start()
{
	// create the status popup window
	if (mStatusWindow != NULL)
	{
		GetQtBaseManager()->GetStatusPopupManager()->Remove(mStatusWindow);
	}
	mStatusWindow = GetQtBaseManager()->GetStatusPopupManager()->Create();

	// store the number of items to download that are in the queue at the beginning
	mNumDownloadItems = mDownloadQueue.Size();
	mCurrentDownload = 0;

	DownloadNext();
}


// called when a download status changed
void WebDataCache::OnDownloadProgressChanged(qint64 bytesReceived, qint64 bytesTotal)
{
	mCurrentBytesReceived	= bytesReceived;
	mCurrentBytesTotal		= bytesTotal;

	UpdateProgressBar();
}


// update progress bar
void WebDataCache::UpdateProgressBar()
{
	if (mStatusWindow == NULL)
		return;

	// inform the status pop-up window
	const float kbReceived	= (float)mCurrentBytesReceived / 1024.0f;
	const float kbTotal		= (float)mCurrentBytesTotal / 1024.0f;

	// download progress
	float downloadProgress = 0.0f;
	if (kbTotal > Math::epsilon)
	{
		downloadProgress = kbReceived / kbTotal;
		mTempStatusString.Format( "Downloading '%s' (%.0f/%.0f KB)", mCurrentUrl.ExtractFilename().AsChar(), kbReceived, kbTotal );
	}
	else
	{
		mTempStatusString.Format( "Downloading '%s'", mCurrentUrl.ExtractFilename().AsChar() );
	}

	// set the status text
	mStatusWindow->ShowMessage( mTempStatusString.AsChar() );

	// set the progress
	const float oneItem				= 1.0f / (float)mNumDownloadItems;
	const float downloadGlobal		= downloadProgress * oneItem;
	const float itemProgress		= ((float)mCurrentDownload) / ((float)mNumDownloadItems);
	const float finalProgress		= itemProgress + downloadGlobal;

	mStatusWindow->SetProgress( finalProgress );
}


// start downloading the next file in the queue
void WebDataCache::DownloadNext()
{
	// return directly in case the download queue is empty
	if (mDownloadQueue.IsEmpty() == true)
	{
		// hide and destruct the status popup window
		if (mStatusWindow != NULL)
		{
			GetQtBaseManager()->GetStatusPopupManager()->Remove(mStatusWindow);
		}

		emit FinishedDownload();
		return;
	}

	const uint32 randomNext = Min<uint32>( Math::RandSmallRange( 0, mDownloadQueue.Size() ), mDownloadQueue.Size()-1 );
	FileDownloader* downloader = mDownloadQueue[randomNext];
	//LogInfo("Random next download is nr %i", randomNext);

	String url = downloader->GetUrl().url().toLatin1().constData();

	// check if the download failed too often already, if yes, just remove it from the queue
	if (downloader->GetNumFails() >= mMaxNumFails)
	{
		LogInfo("WebDataCache: Download failed %i times '%s'. Skipping file.", mMaxNumFails, url.AsChar() );
		RemoveFromQueue(downloader);
		DownloadNext();
		return;
	}

	mCurrentUrl				= url;
	mCurrentBytesReceived	= 0;
	mCurrentBytesTotal		= 0;

	UpdateProgressBar();

	if (FileExists(url.AsChar()) == true)
	{
		LogInfo("WebDataCache: Skipping download for '%s'. File already downloaded.", url.AsChar() );
		RemoveFromQueue(downloader);
		DownloadNext();
	}
	else
	{
		LogInfo("WebDataCache: Starting download '%s'", url.AsChar() );
		downloader->Start();
	}
}


void WebDataCache::RemoveFromQueue(FileDownloader* downloader)
{
	mDownloadQueue.RemoveByValue(downloader);
	downloader->deleteLater();
}


void WebDataCache::OnFileDownloaded(FileDownloader* downloader)
{
	String url = downloader->GetUrl().url().toLatin1().constData();
	LogInfo("WebDataCache: Finished download '%s'", url.AsChar() );

	// increase the current download
	mCurrentDownload++;

	RemoveFromQueue(downloader);

	DownloadNext();
}


void WebDataCache::OnFileFailed(FileDownloader* downloader)
{
	String url = downloader->GetUrl().url().toLatin1().constData();
	LogWarning("WebDataCache: Failed downloading '%s'", url.AsChar() );

	DownloadNext();
}


bool WebDataCache::FileExists(const char* url)
{
	return QFile::exists( UrlToFilename(url).AsChar() );
}


String WebDataCache::UrlToFilename(const char* url)
{
	QByteArray base64Data = url;
	String safeBase64String = base64Data.toBase64().constData();
	safeBase64String.Replace( StringCharacter('/'), StringCharacter('_') );

	mTempString = url;

	String result;
	result.Reserve( 4096 );

	result = mFolder;
	result += safeBase64String;
	result += ".";
	result += mTempString.ExtractFileExtension();
	
	return result;
}


String WebDataCache::FilenameToUrl(const char* filename)
{
	String result = QByteArray::fromBase64( filename ).constData();
	result.Replace( StringCharacter('_'), StringCharacter('/') );
	return result;
}


// checks if the file is cached and returns the cache file name in this case, otherwise the url is returned
Core::String WebDataCache::GetCacheFilenameForUrl(const char* url)
{
	if (FileExists(url) == true)
		return UrlToFilename(url);

	return url;
}


// clean cache
bool WebDataCache::CleanCache()
{
	// remove files based on the max cache size restriction
	if (mMaxCacheRestrictionEnabled == true)
	{
		// calculate the currently used and the exceeded cache sizes
		const uint64 usedCacheSizeInMb	= CalcUsedCacheSizeInBytes() / 1024 / 1024;
		const int64 exceededCacheSize	= usedCacheSizeInMb - mMaxCacheSizeInMb;

		// check if the cache exceeded the limit
		if (exceededCacheSize > 0)
		{
			// cache exceeded the limit, remove files!

			// date sorted cache folder (oldest on top)
			QDir cacheFolder( mFolder.AsChar() );
			cacheFolder.setSorting( QDir::Time | QDir::Reversed );
	
			// get the filenames of all files in our directory
			QList<QFileInfo>	fileInfos	= cacheFolder.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
			const uint32		numFiles	= fileInfos.count();

			// iterate until we removed enough files to be under the max cache size limit
			float removedFileSizeInMb = 0.0f;
			for (uint32 i=0; i<numFiles; ++i)
			{
				const QFileInfo& fileInfo = fileInfos[i];

				if (QFile::remove(fileInfo.filePath()) == true)
				{
					LogInfo( "Removing file '%s' from cache.", FromQtString( fileInfo.filePath() ).AsChar() );
					removedFileSizeInMb += fileInfo.size() / 1024 / 1024;
				}
				else
					LogError( "Failed to remove file '%s' from cache.", FromQtString( fileInfo.filePath() ).AsChar() );

				// stop removing files in case we're under the the limit again
				if (removedFileSizeInMb > exceededCacheSize)
					break;
			}

			#ifndef PRODUCTION_BUILD
				const uint64 newUsedCacheSizeInMb = CalcUsedCacheSizeInBytes() / 1024 / 1024;
				LogInfo("New Cache Size: %d", newUsedCacheSizeInMb);
			#endif
		}
	}

	// remove files based on the max alive time restriction
	if (mMaxAliveTimeRestrictionEnabled == true)
	{
			// remove all files older than XX days

			// date sorted cache folder (oldest on top)
			QDir cacheFolder( mFolder.AsChar() );
			cacheFolder.setSorting( QDir::Time | QDir::Reversed );
	
			// get the filenames of all files in our directory
			QList<QFileInfo>	fileInfos	= cacheFolder.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
			const uint32		numFiles	= fileInfos.count();

			// iterate until we found a file newer than our deadline
			for (uint32 i=0; i<numFiles; ++i)
			{
				const QFileInfo& fileInfo = fileInfos[i];

				QDateTime currentDateTime = QDateTime::currentDateTime();
				QDateTime lastModifiedDate = fileInfo.lastModified();

				// check if a given file is too old
				if (lastModifiedDate.daysTo(currentDateTime) > mMaxAliveTimeInDays)
				{
					if (QFile::remove(fileInfo.filePath()) == true)
					{
						LogInfo( "Removing file '%s' from cache (too old).", FromQtString( fileInfo.filePath() ).AsChar() );
					}
					else
						LogError( "Failed to remove file '%s' from cache.", FromQtString( fileInfo.filePath() ).AsChar() );
				}
				else
				{
					// break the loop as soon as we reach a file younger than our deadline
					// we can do that cause we sorted the files from old to new
					break;
				}
			}
	}

	return true;
}


// gather the total size in bytes from all files that are currently cached
uint64 WebDataCache::CalcUsedCacheSizeInBytes()
{
	QDir cacheFolder( mFolder.AsChar() );

	// get the filenames of all files in our directory
	QList<QFileInfo>	fileInfos	= cacheFolder.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	const uint32		numFiles	= fileInfos.count();

	// iterate through all files and accumulate the file sizes
	uint64 result = 0;
	for (uint32 i=0; i<numFiles; ++i)
		result += fileInfos[i].size();

	return result;
}


// log all files in the cache
void WebDataCache::Log()
{
	QDir cacheFolder( mFolder.AsChar() );

	// sort by date
	cacheFolder.setSorting(QDir::Time);
	
	// get the filenames of all files in our directory
	QList<QFileInfo>	fileInfos	= cacheFolder.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	const uint32		numFiles	= fileInfos.count();
	
	// return directly in case there are no files in the given directory
	if (numFiles == 0)
		return;

	LogInfo("================================================================================================================================================================================================================================");
	LogInfo("WebDataCache: '%s'", mFolder.AsChar());
	LogInfo("================================================================================================================================================================================================================================");

	// gather some general cache statistics
	const float usedCacheSizeInMb = CalcUsedCacheSizeInBytes() / 1024 / 1024;

	LogInfo("Used Cache Size:     %.1f MB", usedCacheSizeInMb);

	// iterate through all files
	LogInfo("Files: %i", numFiles);
	for (uint32 i=0; i<numFiles; ++i)
	{
		const QFileInfo& fileInfo = fileInfos[i];

		float		fileSizeInMb		= (float)fileInfo.size() / 1024.0f / 1024.0f;
		String		filePath			= FromQtString( fileInfo.filePath() );
		QDateTime	lastModifiedDate	= fileInfo.lastModified();
		String		lastModifiedString	= FromQtString( lastModifiedDate.toString("yyyy-MM-dd hh:mm:ss") );

		LogInfo( "%s\t\t%.1f MB\t\t%s", lastModifiedString.AsChar(), fileSizeInMb, filePath.AsChar() );
	}
	
	LogInfo("================================================================================================================================================================================================================================");
}