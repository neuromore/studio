/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "FoldersCreateResponse.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
FoldersCreateResponse::FoldersCreateResponse(QNetworkReply* reply) : Response(reply)
{
	// check for errors
	if (mHasError == true)
		return;

	// data
	Json::Item dataItem = mJson.Find("data");
	if (dataItem.IsNull() == true)
	{
		mHasError = true;
		return;
	}

	// item id
	Json::Item itemFileId = dataItem.Find("folderId");
	if (itemFileId.IsString() == true)
		mFolderId = itemFileId.GetString();
}
