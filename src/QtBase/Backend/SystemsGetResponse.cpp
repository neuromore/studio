/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "SystemsGetResponse.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
SystemsGetResponse::SystemsGetResponse(QNetworkReply* reply) : Response(reply)
{
	// check for errors
	if (mHasError == true)
		return;


	// data
	Json::Item dataItem = mJson.Find("data");
	if (dataItem.IsNull() == true)
		return;

	// system
	Json::Item systemItem = dataItem.Find("system");
	if (systemItem.IsNull() == true)
		return;

	// get the version
	Json::Item versionItem = systemItem.Find("version");
	if (versionItem.IsNull() == false)
	{
		Json::Item majorItem = versionItem.Find("major");
		Json::Item minorItem = versionItem.Find("minor");
		Json::Item patchItem = versionItem.Find("patch");

		mVersion = Version( majorItem.GetInt(), minorItem.GetInt(), patchItem.GetInt() );
	}
}
