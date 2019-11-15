/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FILEHIERARCHYITEM_H
#define __NEUROMORE_FILEHIERARCHYITEM_H

// include required headers
#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include <Core/Json.h>
#include <Core/Array.h>
#include "../QtBaseConfig.h"


class QTBASE_API FileHierarchyItem
{
	public:
		// constructor & destructor
		FileHierarchyItem();
		virtual ~FileHierarchyItem();

		// load from json and create data model for the whole file system
		static FileHierarchyItem* LoadHierarchy(const char* jsonString);
		static FileHierarchyItem* LoadPlayerHierarchy(const char* jsonString);

		// recursive logging
		void Log();


		// type
		void SetType(const char* type)							{ mType = type; }
		const char* GetType() const								{ return mType.AsChar(); }
		const Core::String& GetTypeString() const				{ return mType; }

		// name
		void SetName(const char* name)							{ mName = name; }
		const char* GetName() const								{ return mName.AsChar(); }
		const Core::String& GetNameString() const				{ return mName; }

		// item id
		void SetItemId(const char* itemId)						{ mItemId = itemId; }
		const char* GetItemId() const							{ return mItemId.AsChar(); }
		const Core::String& GetItemIdString() const				{ return mItemId; }

		// summary
		void SetSummary(const char* summary)					{ mSummary = summary; }
		const char* GetSummary() const							{ return mSummary.AsChar(); }
		const Core::String& GetSummaryString() const			{ return mSummary; }

		// icon url
		void SetIconUrl(const char* iconUrl)					{ mIconUrl = iconUrl; }
		const char* GetIconUrl() const							{ return mIconUrl.AsChar(); }
		const Core::String& GetIconUrlString() const			{ return mIconUrl; }

		// type
		void SetBackgroundUrl(const char* backgroundUrl)		{ mBackgroundUrl = backgroundUrl; }
		const char* GetBackgroundUrl() const					{ return mBackgroundUrl.AsChar(); }
		const Core::String& GetBackgroundUrlString() const		{ return mBackgroundUrl; }


		// parent
		void SetParent(FileHierarchyItem* parent)				{ mParent = parent; }
		FileHierarchyItem* GetParent() const					{ return mParent; }

		// folders
		void AddFolder(FileHierarchyItem* folderItem)			{ mFolders.Add(folderItem); }
		FileHierarchyItem* GetFolder(uint32 index) const		{ return mFolders[index]; }
		uint32 GetNumFolders() const							{ return mFolders.Size(); }
		bool HasFolders() const									{ return mFolders.Size() > 0; }

		// files
		void AddFile(FileHierarchyItem* fileItem)				{ mFiles.Add(fileItem); }
		FileHierarchyItem* GetFile(uint32 index) const			{ return mFiles[index]; }
		uint32 GetNumFiles() const								{ return mFiles.Size(); }
		bool HasFiles() const									{ return mFiles.Size() > 0; }

	private:
		static void CreateFolderItems(FileHierarchyItem* item, Core::Json::Item jsonItem, const char* folderItemId="folders");
		static void CreateFileItems(FileHierarchyItem* item, Core::Json::Item jsonItem);
		static FileHierarchyItem* CreateFileHierarchyItem(FileHierarchyItem* parent, const char* idName, Core::Json::Item item);

		static void Log(FileHierarchyItem* item, uint32 hierarchyLevel);

		Core::String						mType;
		Core::String						mName;
		Core::String						mItemId;
		Core::String						mSummary;
		Core::String						mIconUrl;
		Core::String						mBackgroundUrl;

		Core::Array<FileHierarchyItem*>		mFolders;
		Core::Array<FileHierarchyItem*>		mFiles;
		FileHierarchyItem*					mParent;
};


#endif
