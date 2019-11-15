/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_BACKENDFILESYSTEM_H
#define __NEUROMORE_BACKENDFILESYSTEM_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include <User.h>
#include <Creud.h>
#include <Graph/GraphManager.h>
#include <Experience.h>
#include "NetworkAccessManager.h"


class QTBASE_API BackendFileSystem : public QObject
{
	Q_OBJECT

	public:
		// constructor & destructor
		BackendFileSystem(QObject* parent);
		virtual ~BackendFileSystem();

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Folders
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	// Create folder
	public:
		void CreateFolder(const char* token, const Core::String& folderName, const Core::String& parentFolderId);
	signals:
		void FolderCreated();

	// Update folder
	public:
		void UpdateFolder(const char* token, const Core::String& folderId, const Core::String& parentFolderId, const Core::String& newName);
	signals:
		void FolderUpdated();

	// Delete folder
	public:
		void DeleteFolder(const char* token, const Core::String& folderId);
	signals:
		void FolderDeleted();

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Files
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	// Retrieve file
	public:
		void RetrieveFile(const char* token, const Core::String& fileId, int revision=-1);
	signals:
		void FileRetrieved(Core::String jsonContent, Core::String fileId, Core::String fileType, Creud creud, uint32 revision);

	// Update file
	public:
		void UpdateFile(const char* token, const Core::String& fileId, const char* jsonContent, const Core::String& newName="", const Core::String& newFolderId="");
	signals:
		void FileUpdated();

	// Helpers
	public:
		bool SaveGraph(const char* token, const char* uuid, Graph* graph);
		bool SaveExperience(const char* token, const char* uuid, Experience* experience);

	// Delete file
	public:
		void DeleteFile(const char* token, const Core::String& fileId);
	signals:
		void FileDeleted();
};


#endif
