/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "FileSystemItem.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
FileSystemItem::FileSystemItem() 
{
	mPage	= CORE_INVALIDINDEX32;
	mColor	= Color( 1.0, 1.0, 1.0, 1.0 );
}


// destructor
FileSystemItem::~FileSystemItem()
{
}


// load file hierarchy from json
bool FileSystemItem::Parse(const Json::Item& jsonItem)
{
	// item id
	Json::Item itemIdItem = jsonItem.Find("id");
	if (itemIdItem.IsString() == true)
		mItemId = itemIdItem.GetString();

	// type
	Json::Item typeItem = jsonItem.Find("type");
	if (typeItem.IsString() == true)
		mType = typeItem.GetString();

	// name
	Json::Item nameItem = jsonItem.Find("name");
	if (nameItem.IsString() == true)
		mName = nameItem.GetString();

	// summary
	Json::Item summaryItem = jsonItem.Find("summary");
	if (summaryItem.IsString() == true)
		mSummary = summaryItem.GetString();

	// icon url
	Json::Item iconUrlItem = jsonItem.Find("iconUrl");
	if (iconUrlItem.IsString() == true)
		mIconUrl = iconUrlItem.GetString();

	// background url
	Json::Item backgroundUrlItem = jsonItem.Find("backgroundUrl");
	if (backgroundUrlItem.IsString() == true)
		mBackgroundUrl = backgroundUrlItem.GetString();

	// parent name
	Json::Item parentNameItem = jsonItem.Find("parentName");
	if (parentNameItem.IsString() == true)
		mParentName = parentNameItem.GetString();

	// parent id
	Json::Item parentIdItem = jsonItem.Find("parentId");
	if (parentIdItem.IsString() == true)
		mParentId = parentIdItem.GetString();

	// attributes
	Json::Item attributesItem = jsonItem.Find("attributes");
	if (attributesItem.IsObject() == true)
	{
		// color
		Json::Item colorItem = attributesItem.Find("color");
		if (colorItem.IsObject() == true)
		{
			Json::Item rItem = colorItem.Find("r");
			Json::Item gItem = colorItem.Find("g");
			Json::Item bItem = colorItem.Find("b");
			Json::Item aItem = colorItem.Find("a");

			if (rItem.IsNumber() == true && gItem.IsNumber() == true && bItem.IsNumber() == true && aItem.IsNumber() == true)
				mColor = Color( rItem.GetDouble() / 255.0, gItem.GetDouble() / 255.0, bItem.GetDouble() / 255.0, aItem.GetDouble() / 255.0 );
		}
	}

	// creud
	mCreud.ReadFromJson( jsonItem );

	return true;
}


// parse and return array of file system items
Array<FileSystemItem> FileSystemItem::ParseArrayItem(const Json::Item& jsonItem)
{
	Array<FileSystemItem> result;

	// make sure the given json item is an array
	if (jsonItem.IsArray() == false)
		return result;

	// iterate through the json items and create file system items
	const uint32 numItems = jsonItem.Size();
	for (uint32 i=0; i<numItems; ++i)
	{
		FileSystemItem item;
		item.Parse( jsonItem[i] );

		result.Add( item );
	}

	return result;
}


// start recursive logging at the current item
void FileSystemItem::Log()
{
	Log( this, 0 );
}


// recursive logging
void FileSystemItem::Log(FileSystemItem* item, uint32 hierarchyLevel)
{
	// prepare the prefixes for nice alignment
	String spacesBase;
	for (uint32 i=0; i<hierarchyLevel; ++i)
		spacesBase += "    ";
	String spaces = spacesBase + "    ";

	// log item info
	LogInfo( "%s- Name: %s", spacesBase.AsChar(), item->GetName() );
	LogInfo( "%s+ Type: %s", spaces.AsChar(), item->GetType() );
	LogInfo( "%s+ Id: %s", spaces.AsChar(), item->GetItemId() );
	LogInfo( "%s+ Summary: %s", spaces.AsChar(), item->GetSummary() );
	LogInfo( "%s+ Icon URL: %s", spaces.AsChar(), item->GetIconUrl() );
	LogInfo( "%s+ Background URL: %s", spaces.AsChar(), item->GetBackgroundUrl() );
	LogInfo( "%s+ Parent: %s (Id=%s)", spaces.AsChar(), item->GetParentName(), item->GetParentId() );
	//LogInfo( "%s+ Creud: %s", spaces.AsChar(), item->GetCreud().ToString().AsChar() );

	// files
	/*const uint32 numFiles = GetNumFiles();
	if (numFiles > 0)
	{
		LogInfo("%s+ Files: %i", spaces.AsChar(), numFiles);
		for (uint32 i=0; i<numFiles; ++i)
			Log( GetFile(i), hierarchyLevel+1 );
	}

	// folders
	const uint32 numFolders = GetNumFolders();
	if (numFolders > 0)
	{
		LogInfo("%s+ Folders: %i", spaces.AsChar(), numFolders);
		for (uint32 i=0; i<numFolders; ++i)
			Log( GetFolder(i), hierarchyLevel+1 );
	}*/
}