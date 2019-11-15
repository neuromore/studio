/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FILESYSTEMITEM_H
#define __NEUROMORE_FILESYSTEMITEM_H

// include required headers
#include <Core/String.h>
#include <Core/Color.h>
#include <Core/Json.h>
#include <Core/Array.h>
#include <Creud.h>
#include <User.h>
#include "../QtBaseConfig.h"


class QTBASE_API FileSystemItem
{
	public:
		// constructor & destructor
		FileSystemItem();
		~FileSystemItem();

		bool Parse(const Core::Json::Item& jsonItem);
		static Core::Array<FileSystemItem> ParseArrayItem(const Core::Json::Item& jsonItem);
		void Log();

		bool IsFolder() const									{ return mType.IsEqualNoCase("folder"); }
		bool IsExperience() const								{ return mType.IsEqualNoCase("experience"); }

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

		// color
		void SetColor(const Core::Color& color)					{ mColor = color; }
		Core::Color GetColor() const							{ return mColor; }

		// icon url
		void SetIconUrl(const char* iconUrl)					{ mIconUrl = iconUrl; }
		const char* GetIconUrl() const							{ return mIconUrl.AsChar(); }
		const Core::String& GetIconUrlString() const			{ return mIconUrl; }

		// type
		void SetBackgroundUrl(const char* backgroundUrl)		{ mBackgroundUrl = backgroundUrl; }
		const char* GetBackgroundUrl() const					{ return mBackgroundUrl.AsChar(); }
		const Core::String& GetBackgroundUrlString() const		{ return mBackgroundUrl; }


		// parent name
		void SetParentName(const char* parentName)				{ mParentName = parentName; }
		const char* GetParentName() const						{ return mParentName.AsChar(); }
		const Core::String& GetParentNameString() const			{ return mParentName; }

		// parent id
		void SetParentId(const char* parentId)					{ mParentId = parentId; }
		const char* GetParentId() const							{ return mParentId.AsChar(); }
		const Core::String& GetParentIdString() const			{ return mParentId; }

		// parent id
		void SetCreud(const Creud& creud)						{ mCreud = creud; }
		const Creud& GetCreud() const							{ return mCreud; }


		// meta

		// page
		void SetPage(uint32 page)								{ mPage = page; }
		uint32 GetPage() const									{ return mPage; }

	private:
		static void Log(FileSystemItem* item, uint32 hierarchyLevel);

		Core::String						mType;
		Core::String						mName;
		Core::String						mItemId;
		Core::String						mSummary;
		Core::String						mIconUrl;
		Core::String						mBackgroundUrl;
		Core::String						mParentName;
		Core::String						mParentId;
		Core::Color							mColor;
		Creud								mCreud;

		// meta info
		uint32								mPage;

		// custom attributes
		//Core::Color							mCustomColor;
		//double								mCustomDuration;
		//double								mCustomDurationMin;
		//double								mCustomDurationMax;
};


#endif