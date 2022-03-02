/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "FileManager.h"
#include <Core/LogManager.h>
#include <EngineManager.h>
#include "QtBaseManager.h"

#include "Backend/FilesGetRequest.h"
#include "Backend/FilesGetResponse.h"
#include "Backend/FilesUpdateRequest.h"
#include "Backend/FilesUpdateResponse.h"
#include "Backend/ParametersFindRequest.h"
#include "Backend/ParametersFindResponse.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>


using namespace Core;

// constructor
FileManager::FileManager()
{
	LogDetailedInfo("Constructing file manager ...");

	mLoadingExperience = NULL;
	mLoadingClassifier = NULL;
	mLoadingStateMachine = NULL;

	mExperienceLoaded = false;
	mClassifierLoaded = false;
	mStateMachineLoaded = false;

	mIsInFileSaving = false;
}


// destructor
FileManager::~FileManager()
{
	LogDetailedInfo("Destructing file manager ...");

	DestructArray(mOpenFiles);
}


void FileManager::Init()
{

}


//
// The Studio Side
//

// close the file (ignore dirty state)
bool FileManager::Close(const char* identifier, bool askSaveChanges)
{
	// find file
	const uint32 index = FindOpenFile(identifier);
	
	// NOTE: no need for this assert. in case you delete a file in the hierarchy that you didn't load before, this will assert, while it is all fine and we can just skip
	//CORE_ASSERT(index != CORE_INVALIDINDEX32);
	
	if (index == CORE_INVALIDINDEX32)
		return false;

	OpenFile* file = mOpenFiles[index];

	if (askSaveChanges == true && file->IsDirty() == true && file->GetCreud().Update() == true)
	{
		mTempString.Format("File was modified. Save changes?");
		int result = QMessageBox::warning(GetQtBaseManager()->GetMainWindow(), "Save changes?", mTempString.AsChar(), QMessageBox::Save,  QMessageBox::Discard,  QMessageBox::Cancel);
		
		if (result == QMessageBox::Save) {
			mIsInFileSaving = true;
			Save(file);
		}
		else if (result == QMessageBox::Cancel) {
			mIsInFileSaving = false;
			return false;
		}
	}

	// if Experiences: also close referenced files
	if (file->GetType() == ExperienceOpenFile::TYPE_ID)
	{
		Experience* experience = static_cast<ExperienceOpenFile*>(file)->mExperience;

		String classifierUuid = experience->GetClassifierUuid();
		if (classifierUuid.GetLength() > 0 && FindOpenFile(classifierUuid) != CORE_INVALIDINDEX32)
			if (Close(experience->GetClassifierUuid(), askSaveChanges) == false)
				return false;
		
		String stateMachineUuid = experience->GetStateMachineUuid();
		if (stateMachineUuid.GetLength() > 0 && FindOpenFile(stateMachineUuid) != CORE_INVALIDINDEX32)
			if (Close(experience->GetStateMachineUuid(), askSaveChanges) == false)
				return false;
	}

	bool closeResult = true;
	if (!mIsInFileSaving) {
		closeResult = Close(file);
	}
	// else: do nothing, file will be closed from WriteDataToBackend

	return closeResult;
}


// close the file (internal func)
bool FileManager::Close(OpenFile* file)
{
	LogDebug("Closing File '%s'", file->GetName().AsChar());

	if (mOpenFiles.RemoveByValue(file) == true)
		delete file;

	return true;
}


// save the file
bool FileManager::Save(const char* identifier)
{
	// find file
	const uint32 index = FindOpenFile(identifier);
	CORE_ASSERT(index != CORE_INVALIDINDEX32);
	if (index == CORE_INVALIDINDEX32)
		return false;

	Save(mOpenFiles[index]);
	return true;
}


// save the file (internal func)
bool FileManager::Save(OpenFile* file)
{
	if (file->Serialize() == false)
		return false;

	WriteData(file);

	return true;
}


// access all opened files
uint32 FileManager::FindOpenFile(const char* identifier) const
{
	const uint32 numFiles = mOpenFiles.Size();
	for (uint32 i=0; i<numFiles; ++i)
	{
		if (mOpenFiles[i]->GetIdentifier().IsEqual(identifier))
			return i;
	}

	return CORE_INVALIDINDEX32;
}


//
// The Open functions
// 



// open any kind of graph (can be nested graph, classifier, statemachine,...)
void FileManager::OpenGraph(Graph* graph, ELocation location, const char* identifier, const char* name, int revision)
{
	GraphOpenFile* file = new GraphOpenFile(graph, location, identifier, name, revision);
	ReadData(file);
}


// open classifier and load it in the engine
void FileManager::OpenClassifier(ELocation location, const char* identifier, const char* name, int revision)
{
	// active classifier must be closed first (with savechanges dialog)
	Classifier* activeClassifier = GetEngine()->GetActiveClassifier();
	if (activeClassifier != NULL && FindOpenFile(activeClassifier->GetUuid()) != CORE_INVALIDINDEX32)
		if (CloseWithPrompt(activeClassifier->GetUuid()) == false)
			return;	// user aborted

	if (mIsInFileSaving)
	{
		//wait for backend reply
		QTimer timer;
		timer.setSingleShot(true);
		QEventLoop loop;
		connect(this, &FileManager::writeToBackendFinished, &loop, &QEventLoop::quit);
		connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
		timer.start(10000);
		loop.exec();
		if (!timer.isActive())
		{
			Core::LogError("Timeout on writing data to backend");
		}
	}

	Classifier* classifier = new Classifier();
	OpenGraph(classifier, location, identifier, name, revision);

	mIsInFileSaving = false;
}


// open state machine and load it in the engine
void FileManager::OpenStateMachine(ELocation location, const char* identifier, const char* name, int revision)
{
	// active statemachine must be closed first (with savechanges dialog)
	StateMachine* activeStateMachine = GetEngine()->GetActiveStateMachine();
	if (activeStateMachine != NULL && FindOpenFile(activeStateMachine->GetUuid()) != CORE_INVALIDINDEX32)
		if (CloseWithPrompt(activeStateMachine->GetUuid()) == false)
			return;	// user aborted

	StateMachine* stateMachine = new StateMachine();
	OpenGraph(stateMachine, location, identifier, name, revision);
}


// open experience and its dependencies
void FileManager::OpenExperience(ELocation location, const char* identifier, const char* name, int revision, bool loadReferencedGraphs)
{
	// active Experience must be closed first (with savechanges dialog)
	Experience* activeExperience = GetEngine()->GetActiveExperience();
	if (activeExperience != NULL && FindOpenFile(activeExperience->GetUuid()) != CORE_INVALIDINDEX32)
		if (CloseWithPrompt(activeExperience->GetUuid()) == false)
			return;	// user aborted

	// active classifier must be closed first (with savechanges dialog)
	Classifier* activeClassifier = GetEngine()->GetActiveClassifier();
	if (activeClassifier != NULL && FindOpenFile(activeClassifier->GetUuid()) != CORE_INVALIDINDEX32)
		if (CloseWithPrompt(activeClassifier->GetUuid()) == false)
			return;	// user aborted

	// active statemachine must be closed first (with savechanges dialog)
	StateMachine* activeStateMachine = GetEngine()->GetActiveStateMachine();
	if (activeStateMachine != NULL && FindOpenFile(activeStateMachine->GetUuid()) != CORE_INVALIDINDEX32)
		if (CloseWithPrompt(activeStateMachine->GetUuid()) == false)
			return;	// user aborted

	Experience* experience = new Experience();
	if (loadReferencedGraphs == true)
	{
		mLoadingExperienceIdentifier = identifier;
		mLoadingExperience = experience;
	}

	LogDebug("Begin opening experience");

	// request experience
	ExperienceOpenFile* file = new ExperienceOpenFile(mLoadingExperience, location, identifier, name, revision);
	ReadData(file);	
}


//
// The Data Side
// 


// tells file manager you want that file
bool FileManager::ReadData(OpenFile* file)
{
	switch(file->GetLocation())
	{
		// request file from backend
		case LOCATION_BACKEND: return ReadDataFromBackend(file);
		case LOCATION_HTTP: 
		case LOCATION_LOCAL: return false;
			// TODO use qt to request and read the file (with lambda slots)

		default: return false;
	}
}


// internally called after file data was received -> deserialize and add to open list
void FileManager::FinishReadData(OpenFile* file)
{
	if (file->Deserialize() == false)
	{
		mTempString.Format("There were errors loading the file. Do you still want to open it? It might not work correctly.");
		int result = QMessageBox::warning(GetQtBaseManager()->GetMainWindow(), "Corrupted File", mTempString.AsChar(), QMessageBox::Yes, QMessageBox::No);
		
		if (result == QMessageBox::No)
		{
			emit OnFileOpenFailed(file->GetIdentifier());
			delete file;
			return;
		}
	}

	LogInfo("File '%s' was successfully retrieved", file->GetName().AsChar());

	mOpenFiles.Add(file);
	emit OnFileOpened(file->GetIdentifier());

	// retreived file is an experience: chain load the attached graphs
	if (file->GetType() == FileManager::ExperienceOpenFile::TYPE_ID)
	{
		// also load classifier / state machine if experience has only references
		if (file->GetIdentifier().IsEqual(mLoadingExperienceIdentifier))
		{
			// load classifier
			String classifierUuid = mLoadingExperience->GetClassifierUuid();
			if (classifierUuid.IsEmpty() == false)
			{
				// load from same location, with latest revision
				mLoadingClassifierIdentifier = classifierUuid;
				mLoadingClassifier = new Classifier();
				OpenGraph( mLoadingClassifier, file->GetLocation(), classifierUuid.AsChar(), file->GetName() );
			}

			// load state machine
			String stateMachineUuid = mLoadingExperience->GetStateMachineUuid();
			if (stateMachineUuid.IsEmpty() == false)
			{
				// load from same location, with latest revision
				mLoadingStateMachineIdentifier = stateMachineUuid;
				mLoadingStateMachine = new StateMachine();
				OpenGraph( mLoadingStateMachine, file->GetLocation(), stateMachineUuid.AsChar(), file->GetName() );
			}
		}

		// experiences (without dependencies) is now loaded
		mExperienceLoaded = true;

		// in case we load an empty experience: try initialize now
		TryInitializeExperience();
	}

	// file is a graph: check if this is part of the experience we are loading
	if (file->GetType() == FileManager::GraphOpenFile::TYPE_ID)
	{
		if (mLoadingExperience != NULL)
		{
			if (mLoadingClassifierIdentifier.IsEqual(file->GetIdentifier()))
				mClassifierLoaded = true;
		
			if (mLoadingStateMachineIdentifier.IsEqual(file->GetIdentifier()))
				mStateMachineLoaded = true;
			
			TryInitializeExperience();
		}
		else
		{
			// if it doesn't belong to the experience loading process, load the graph into the engine (could be a nested graph, or a classifier that is added to the experience caused by the user)
			GetEngine()->LoadGraph( static_cast<FileManager::GraphOpenFile*>(file)->mGraph);
			GetEngine()->Reset();
		}
	}
}


void FileManager::TryInitializeExperience()
{
	if (mExperienceLoaded == false || mLoadingExperience == NULL)
		return;

	// classifier missing
	String classifierUuid = mLoadingExperience->GetClassifierUuid();
	if (classifierUuid.GetLength() > 0 && mClassifierLoaded  == false)
		return;

	// state machine missing
	String stateMachineUuid = mLoadingExperience->GetStateMachineUuid();
	if (stateMachineUuid.GetLength() > 0 && mStateMachineLoaded == false)
		return;

	// load cloud parameters

	// construct /files/${fileId}/get request
	ParametersFindRequest request( GetUser()->GetToken(), GetUser()->GetId(), mLoadingExperience, mLoadingClassifier,  mLoadingStateMachine);

	// process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request );

	// lambda slot for processing reply
	connect(reply, &QNetworkReply::finished, this, [reply, this]()
	{
		// translate it to our data model
		ParametersFindResponse response(reply);
		const bool haveCloudParameters = (response.HasError() == false);
		
		// now we have everything: load it all into the engine

		// unload first
		GetEngine()->Unload();
		GetEngine()->Reset();

		if (mLoadingClassifier != NULL)
			GetEngine()->LoadClassifier(mLoadingClassifier);

		if (mLoadingStateMachine != NULL)
			GetEngine()->LoadStateMachine(mLoadingStateMachine);

		GetEngine()->LoadExperience(mLoadingExperience);

		if (haveCloudParameters == true)
			GetEngine()->LoadCloudParameters(response.GetParameters());

		// reset again
		GetEngine()->Reset();
		
		// reset loading state flags
		mLoadingExperience = NULL;
		mLoadingClassifier = NULL;
		mLoadingStateMachine = NULL;
		mLoadingExperienceIdentifier.Clear();
		mLoadingClassifierIdentifier.Clear();
		mLoadingStateMachineIdentifier.Clear();
		mExperienceLoaded = false;
		mClassifierLoaded = false;
		mStateMachineLoaded = false;

		// preload assets
		GetQtBaseManager()->GetExperienceAssetCache()->PreloadAssets();
	});
}


// request data for file
bool FileManager::ReadDataFromBackend(OpenFile* file)
{
	// construct /files/${fileId}/get request
	FilesGetRequest request( GetUser()->GetToken(), file->GetIdentifier(), file->GetRevision());

	// process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request );

	// lambda slot for processing reply
	connect(reply, &QNetworkReply::finished, this, [reply, this, file]()
	{
		// translate it to our data model
		FilesGetResponse response(reply);
		if (response.HasError() == true)
		{
			delete file;
			return;
		}

 		// get data
		const uint32 numBytes = String::CalcLength(response.GetJsonContent()) + 1; // Null byte
		file->mData.Resize(numBytes);

		Core::MemCopy( file->mData.GetPtr(), response.GetJsonContent(), file->mData.Size() );

		file->mCreud = response.GetCreud();
		file->mName = response.GetName();

		// deserialize and load
		FinishReadData(file);
	});

	return true;
}



// tells file manager you want that file
bool FileManager::WriteData(OpenFile* file)
{
	switch(file->GetLocation())
	{
		// request file from backend
		case LOCATION_BACKEND:	return WriteDataToBackend(file);
		case LOCATION_LOCAL: return false;
			// TODO use qt to request and write the file (with lambda slots)
		
		default:
		case LOCATION_HTTP: /* never writeable */ return false;
	}
}


// update file on backend
bool FileManager::WriteDataToBackend(OpenFile* file)
{
	// construct /files/${fileId}/update request
	FilesUpdateRequest request( GetUser()->GetToken(), file->GetIdentifier(), file->mData.GetPtr());

	// process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request );

	// lambda slot for processing reply
	connect(reply, &QNetworkReply::finished, this, [reply, this, file]()
	{
		if (file == nullptr || !mIsInFileSaving)
		{
			return;
		}

		// translate it to our data model
		FilesUpdateResponse response(reply);
		if (response.HasError())
		{
			Core::LogError(response.GetErrorMessage());
		}
		else
		{
			file->SetUndirty();
		}

		if (!Close(file))
		{
			Core::LogError("Cannot close the file");
		}
		emit writeToBackendFinished();
	});

	return true;

	return false;
}