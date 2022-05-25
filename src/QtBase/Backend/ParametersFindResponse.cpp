/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "ParametersFindResponse.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
ParametersFindResponse::ParametersFindResponse(QNetworkReply* reply) : Response(reply)
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

	// get the parameters
	Json::Item parametersItem = dataItem.Find("parameters");
	if (parametersItem.IsArray() == false)
	{
		mHasError = true;
		return;
	}
	
	// load the parameters	
	if (mParameters.Load(parametersItem) == false)
	{
		mHasError = true;
		return;
	}

	LogDebug("ParametersFindResponse(): parsed %i parameters", mParameters.GetParameters().Size());
	mParameters.Log();
}