/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required files
#include "BackendFileSystem.h"
#include "BackendHelpers.h"
#include "../QtBaseManager.h"
#include <Core/LogManager.h>
#include <EngineManager.h>
#include <Graph/GraphExporter.h>
#include "../Backend/FilesGetRequest.h"
#include "../Backend/FilesGetResponse.h"
#include "../Backend/FilesUpdateRequest.h"
#include "../Backend/FilesUpdateResponse.h"
#include "../Backend/FilesDeleteRequest.h"
#include "../Backend/FilesDeleteResponse.h"
#include "../Backend/FoldersCreateRequest.h"
#include "../Backend/FoldersCreateResponse.h"
#include "../Backend/FoldersUpdateRequest.h"
#include "../Backend/FoldersUpdateResponse.h"
#include "../Backend/FoldersDeleteRequest.h"
#include "../Backend/FoldersDeleteResponse.h"
#include <QMessageBox>


using namespace Core;

// constructor
BackendFileSystem::BackendFileSystem(QObject* parent) : QObject(parent)
{
}


// destructor
BackendFileSystem::~BackendFileSystem()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Create folder
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// send http request to the web services server
void BackendFileSystem::CreateFolder(const char* token, const Core::String& folderName, const Core::String& parentFolderId)
{
	// 1. construct /folders/update request
	FoldersCreateRequest request( token, folderName, parentFolderId );

	// 2. process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request );
	connect(reply, &QNetworkReply::finished, this, [reply, this]()
	{
		QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );

		// 3. construct and parse response
		FoldersCreateResponse response(networkReply);
		if (response.HasError() == true)
			return;

		// 4. handle response
		emit FolderCreated();
	});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Update folder
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// send http request to the web services server
void BackendFileSystem::UpdateFolder(const char* token, const String& folderId, const Core::String& parentFolderId, const Core::String& newName)
{
	// make sure the folder id is set as we'll need that for the url
	if (folderId.IsEmpty() == true)
	{
		BackendHelpers::ReportError( "BackendFileSystem::UpdateFolder(): Cannot update folder. Folder ID is empty." );
		return;
	}

	// 1. construct /folders/update request
	FoldersUpdateRequest request( token, folderId, parentFolderId, newName );

	// 2. process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request );
	connect(reply, &QNetworkReply::finished, this, [reply, this]()
	{
		QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );

		// 3. construct and parse response
		FoldersUpdateResponse response(networkReply);
		if (response.HasError() == true)
			return;

		// 4. handle response
		emit FolderUpdated();
	});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Delete folder
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// send http request to the web services server
void BackendFileSystem::DeleteFolder(const char* token, const String& folderId)
{
	// make sure the folder id is set as we'll need that for the url
	if (folderId.IsEmpty() == true)
	{
		BackendHelpers::ReportError( "BackendFileSystem::DeleteFolder(): Cannot delete folder. Folder ID is empty." );
		return;
	}

	// 1. construct /folders/delete request
	FoldersDeleteRequest request( token, folderId.AsChar() );

	// 2. process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request );
	connect(reply, &QNetworkReply::finished, this, [reply, this]()
	{
		QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );

		// 3. construct and parse response
		FoldersDeleteResponse response(networkReply);
		if (response.HasError() == true)
			return;

		// 4. handle response
		emit FolderDeleted();
	});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Retrieve file
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// send http request to the web services server
void BackendFileSystem::RetrieveFile(const char* token, const String& fileId, int revision)
{
	// make sure the file id is set as we'll need that for the url
	if (fileId.IsEmpty() == true)
	{
		BackendHelpers::ReportError( "BackendFileSystem::RetrieveFile(): Cannot retrieve file. File ID is empty." );
		return;
	}

	// 1. construct /files/get request
	FilesGetRequest request( token, fileId, revision );

	// 2. process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request );
	connect(reply, &QNetworkReply::finished, this, [reply, this]()
	{
		QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );

		// 3. construct and parse response
		FilesGetResponse response(networkReply);
		if (response.HasError() == true)
			return;

		// 4. handle response
		emit FileRetrieved( response.GetJsonContent(), response.GetFileId(), response.GetFileType(), response.GetCreud(), response.GetRevision() );
	});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Update file
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BackendFileSystem::UpdateFile(const char* token, const String& fileId, const char* jsonContent, const String& newName, const String& newFolderId)
{
	// 1. construct /files/update request
	FilesUpdateRequest request( token, fileId, jsonContent, newName, newFolderId );

	// 2. process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request );
	connect(reply, &QNetworkReply::finished, this, [reply, this]()
	{
		QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );

		// 3. construct and parse response
		FilesUpdateResponse response(networkReply);
		if (response.HasError() == true)
			return;

		// 4. handle response
		emit FileUpdated();
	});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Save file
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// save the given graph, return true if successful
bool BackendFileSystem::SaveGraph(const char* token, const char* uuid, Graph* graph)
{
	// check if the graph is valid
	if (graph == NULL)
	{
		QMessageBox::critical( GetQtBaseManager()->GetMainWindow(), "ERROR", "Cannot save graph. Graph is not valid." );
		return false;
	}

	// save the graph to string
	String jsonContent;
	bool result = GraphExporter::Save( &jsonContent, graph );
	if (result == false)
	{
		String errorMsg;
		errorMsg.Format( "Cannot save graph (name='%s', UUID=%s).", graph->GetName(), uuid );
		LogCritical( errorMsg.AsChar() );
		QMessageBox::critical( GetQtBaseManager()->GetMainWindow(), "ERROR", errorMsg.AsChar() );
		return false;
	}

	// call the REST function
	UpdateFile( token, uuid, jsonContent.AsChar() );
	return true;
}


// save the given experience, return true if successful
bool BackendFileSystem::SaveExperience(const char* token, const char* uuid, Experience* experience)
{
	// check if the graph is valid
	if (experience == NULL)
	{
		QMessageBox::critical( GetQtBaseManager()->GetMainWindow(), "ERROR", "Cannot save experience. Experience is not valid." );
		return false;
	}

	// create our json parser
	Json json;
	Json::Item rootItem = json.GetRootItem();

	// save experience (without attached graphs)
	experience->Save( json, rootItem );

	// write experience to json string
	String jsonContent;
	json.WriteToString(jsonContent);

	// call the REST function
	UpdateFile( token, uuid, jsonContent.AsChar() );
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Delete file
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// send http request to the web services server
void BackendFileSystem::DeleteFile(const char* token, const String& fileId)
{
	// make sure the file id is set as we'll need that for the url
	if (fileId.IsEmpty() == true)
	{
		BackendHelpers::ReportError( "BackendFileSystem::DeleteFile(): Cannot delete file. File ID is empty." );
		return;
	}

	// 1. construct /files/delete request
	FilesDeleteRequest request( token, fileId.AsChar() );

	// 2. process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request );
	connect(reply, &QNetworkReply::finished, this, [reply, this]()
	{
		QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );

		// 3. construct and parse response
		FilesDeleteResponse response(networkReply);
		if (response.HasError() == true)
			return;

		// 4. handle response
		emit FileDeleted();
	});
}