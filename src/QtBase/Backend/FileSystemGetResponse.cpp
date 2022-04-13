/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "FileSystemGetResponse.h"


using namespace Core;

// constructor
FileSystemGetResponse::FileSystemGetResponse(QNetworkReply* reply) : Response(reply)
{
	mTotalPages	= 0;
	mPageIndex	= CORE_INVALIDINDEX32;

	// check for errors
	if (mHasError == true)
		return;


	// meta
	Json::Item metaItem = mJson.Find("meta");
	if (metaItem.IsNull() == true)
		return;

	// pagination
	Json::Item paginationItem = metaItem.Find("pagination");
	if (paginationItem.IsNull() == false)
	{
		// total pages
		Json::Item totalPagesItem = paginationItem.Find("totalPages");
		if (totalPagesItem.IsInt() == true)
			mTotalPages = totalPagesItem.GetInt();

		// page index
		Json::Item pageIndexItem = paginationItem.Find("pageIndex");
		if (pageIndexItem.IsInt() == true)
			mPageIndex = pageIndexItem.GetInt();
	}


	// data
	Json::Item dataItem = mJson.Find("data");
	if (dataItem.IsNull() == true)
		return;

	// parents
	Json::Item parentsItem = dataItem.Find("parentItems");
	mParents = FileSystemItem::ParseArrayItem( parentsItem );

	// items
	Json::Item itemsItem = dataItem.Find("items");
	mItems = FileSystemItem::ParseArrayItem( itemsItem );
}