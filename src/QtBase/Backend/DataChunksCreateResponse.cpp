/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "DataChunksCreateResponse.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
DataChunksCreateResponse::DataChunksCreateResponse(QNetworkReply* reply) : Response(reply)
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

	// get the data chunk id
	Json::Item dataChunkIdItem = dataItem.Find("dataChunkId");
	if (dataChunkIdItem.IsNull() == true)
	{
		mHasError = true;
		return;
	}

	mDataChunkId = dataChunkIdItem.GetString();
}
