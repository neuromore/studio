/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include the required headers
#include "FileHierarchyItem.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
FileHierarchyItem::FileHierarchyItem() 
{
	mParent = NULL;
}


// destructor
FileHierarchyItem::~FileHierarchyItem()
{
	// recursively get rid of all folders
	const uint32 numFolders = mFolders.Size();
	for (uint32 i=0; i<numFolders; ++i)
		delete mFolders[i];
	mFolders.Clear();

	// recursively get rid of all files
	const uint32 numFiles = mFiles.Size();
	for (uint32 i=0; i<numFiles; ++i)
		delete mFiles[i];
	mFiles.Clear();
}


// load file hierarchy from json
FileHierarchyItem* FileHierarchyItem::LoadHierarchy(const char* jsonString)
{
	// parse json
	Json json;
	if (json.Parse(jsonString) == false)
	{
		LogError("FileHierarchyItem::Load: Could not parse json.");
		return NULL;
	}

	// get the root item
	Json::Item rootJsonItem = json.GetRootItem();
	if (rootJsonItem.IsNull() == true)
	{
		LogError("FileHierarchyItem::Load: Root item invalid.");
		return NULL;
	}

	// get the data item
	Json::Item dataItem = rootJsonItem.Find("data");
	if (dataItem.IsNull() == true)
	{
		LogError("FileHierarchyItem::Load: Data item invalid.");
		return NULL;
	}

	// start recursion at the root
	FileHierarchyItem* rootItem = new FileHierarchyItem();
	CreateFolderItems( rootItem, dataItem, "rootFolders" );

	return rootItem;
}


// load file hierarchy from json
FileHierarchyItem* FileHierarchyItem::LoadPlayerHierarchy(const char* jsonString)
{
	// parse json
	Json json;
	if (json.Parse(jsonString) == false)
	{
		LogError("LoadPlayerHierarchy::Load: Could not parse json.");
		return NULL;
	}

	// get the root item
	Json::Item rootJsonItem = json.GetRootItem();
	if (rootJsonItem.IsNull() == true)
	{
		LogError("LoadPlayerHierarchy::Load: Root item invalid.");
		return NULL;
	}

	// get the data item
	Json::Item dataItem = rootJsonItem.Find("data");
	if (dataItem.IsNull() == true)
	{
		LogError("LoadPlayerHierarchy::Load: Data item invalid.");
		return NULL;
	}

	// start recursion at the root
	FileHierarchyItem* rootItem = new FileHierarchyItem();
	CreateFolderItems( rootItem, dataItem, "folders" );

	return rootItem;
}


// create items for all subfolders
void FileHierarchyItem::CreateFolderItems(FileHierarchyItem* item, Json::Item jsonItem, const char* folderItemId)
{
	// get access to the folders item and return directly in case it is invalid
	Json::Item foldersItem = jsonItem.Find(folderItemId);
	if (foldersItem.IsNull() == true)
		return;

	const uint32 numFolders = foldersItem.Size();
	for (uint32 i=0; i<numFolders; ++i)
	{
		FileHierarchyItem* childItem = CreateFileHierarchyItem( item, "folderId", foldersItem[i] );
		item->AddFolder( childItem );
	}
}


// create items for all subfiles
void FileHierarchyItem::CreateFileItems(FileHierarchyItem* item, Json::Item jsonItem)
{
	// get access to the files item and return directly in case it is invalid
	Json::Item filesItem = jsonItem.Find("files");
	if (filesItem.IsNull() == true)
		return;

	const uint32 numFiles = filesItem.Size();
	for (uint32 i=0; i<numFiles; ++i)
	{
		FileHierarchyItem* childItem = CreateFileHierarchyItem( item, "fileId", filesItem[i] );
		item->AddFile( childItem );
	}
}


// shared create file hierarchy item function
FileHierarchyItem* FileHierarchyItem::CreateFileHierarchyItem(FileHierarchyItem* parent, const char* idName, Json::Item jsonItem)
{
	FileHierarchyItem* item = new FileHierarchyItem();

	// parent
	item->SetParent(parent);

	// item id
	Json::Item itemIdItem = jsonItem.Find(idName);
	if (itemIdItem.IsString() == true)
		item->SetItemId( itemIdItem.GetString() );

	// type
	Json::Item typeItem = jsonItem.Find("type");
	if (typeItem.IsString() == true)
		item->SetType( typeItem.GetString() );

	// name
	Json::Item nameItem = jsonItem.Find("name");
	if (nameItem.IsString() == true)
		item->SetName( nameItem.GetString() );

	// attributes
	Json::Item attributesItem = jsonItem.Find("attributes");
	if (attributesItem.IsNull() == false)
	{
		// summary
		Json::Item summaryItem = attributesItem.Find("summary");
		if (summaryItem.IsString() == true)
			item->SetSummary( summaryItem.GetString() );

		// icon url
		Json::Item iconUrlItem = attributesItem.Find("iconUrl");
		if (iconUrlItem.IsString() == true)
			item->SetIconUrl( iconUrlItem.GetString() );

		// background url
		Json::Item backgroundUrlItem = attributesItem.Find("backgroundUrl");
		if (backgroundUrlItem.IsString() == true)
			item->SetBackgroundUrl( backgroundUrlItem.GetString() );
	}

	// folders
	CreateFolderItems( item, jsonItem );

	// files
	CreateFileItems( item, jsonItem );

	return item;
}


// start recursive logging at the current item
void FileHierarchyItem::Log()
{
	Log( this, 0 );
}


// recursive logging
void FileHierarchyItem::Log(FileHierarchyItem* item, uint32 hierarchyLevel)
{
	// prepare the prefixes for nice alignment
	String spacesBase;
	for (uint32 i=0; i<hierarchyLevel; ++i)
		spacesBase += "    ";
	String spaces = spacesBase + "    ";

	// log item info
	LogInfo( "%s- Name: %s", spacesBase.AsChar(), item->GetName() );
	LogInfo( "%s+ Type: %s", spaces.AsChar(), item->GetType() );
	LogInfo( "%s+ ID: %s", spaces.AsChar(), item->GetItemId() );
	LogInfo( "%s+ Summary: %s", spaces.AsChar(), item->GetSummary() );
	LogInfo( "%s+ Icon URL: %s", spaces.AsChar(), item->GetIconUrl() );
	LogInfo( "%s+ Background URL: %s", spaces.AsChar(), item->GetBackgroundUrl() );
	LogInfo( "%s+ Parent: 0x%d", spaces.AsChar(), item->GetParent() );

	// files
	const uint32 numFiles = item->GetNumFiles();
	if (numFiles > 0)
	{
		LogInfo("%s+ Files: %i", spaces.AsChar(), numFiles);
		for (uint32 i=0; i<numFiles; ++i)
			Log( item->GetFile(i), hierarchyLevel+1 );
	}

	// folders
	const uint32 numFolders = item->GetNumFolders();
	if (numFolders > 0)
	{
		LogInfo("%s+ Folders: %i", spaces.AsChar(), numFolders);
		for (uint32 i=0; i<numFolders; ++i)
			Log( item->GetFolder(i), hierarchyLevel+1 );
	}
}
