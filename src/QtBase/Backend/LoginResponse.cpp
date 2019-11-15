/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "LoginResponse.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
LoginResponse::LoginResponse(QNetworkReply* reply) : Response(reply)
{
	// check for errors
	if (mHasError == true)
		return;

	// data
	Json::Item dataItem = mJson.Find("data");
	if (dataItem.IsNull() == true)
		return;

	// token
	Json::Item tokenItem = dataItem.Find("token");
	if (tokenItem.IsNull() == false)
		mUser.SetToken( tokenItem.GetString() );

	// user id
	Json::Item userIdItem = dataItem.Find("userId");
	if (userIdItem.IsNull() == false)
		mUser.SetId( userIdItem.GetString() );

	// power line frequency
	Json::Item powerLineFreqItem = dataItem.Find("powerLineFrequency");
	if (powerLineFreqItem.IsNull() == false && powerLineFreqItem.IsNumber() == true)
		mUser.SetAutoPowerLineFrequency( powerLineFreqItem.GetDouble() );

	// display name
	Json::Item displayItem = dataItem.Find("displayName");
	if (displayItem.IsNull() == false)
		mUser.SetDisplayName( displayItem.GetString() );

	// parent company ids
	Json::Item parentCompanyIdsItem = dataItem.Find("parentCompanyIds");
	if (parentCompanyIdsItem.IsNull() == false && parentCompanyIdsItem.IsArray() == true)
	{
		const uint32 numParentCompanyIds = parentCompanyIdsItem.Size();
		for (uint32 i=0; i<numParentCompanyIds; ++i)
		{
			Json::Item parentCompanyIdItem = parentCompanyIdsItem[i];
			if (parentCompanyIdItem.IsNull() == false)
				mUser.AddParentCompanyId( parentCompanyIdItem.GetString() );
		}
	}

	// read all rules
	mUser.ReadRules( dataItem );
	//mUser.LogRules();

	// in case we didn't get a token, report an error
	if (mUser.GetTokenString().IsEmpty() == true)
	{
		mHasError = true;
		ShowError( NULL, "", "Login Failed", "No valid token received or priviledges missing." );
	}
}