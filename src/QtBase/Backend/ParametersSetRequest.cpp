/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "ParametersSetRequest.h"
#include <Core/LogManager.h>
#include <Core/String.h>
#include <Core/Json.h>


using namespace Core;

// constructor
ParametersSetRequest::ParametersSetRequest(const char* token, const char* userId, const CloudParameters& parameters)
{
	// display text
	SetDisplayText("Setting Parameters ...");

	// url
	String url;
	url.Format("users/%s/parameters/set", userId);
	SetUrl( url.AsChar() );

	// request method
	SetRequestMethod( REQUESTMETHOD_PUT );

	// url parameters
	AddUrlParameter( "token", token );

	// body
	Json::Item rootItem = GetBodyJsonRoot();

	mNumParameters = parameters.GetParameters().Size();

	LogDebug("ParametersSetRequest(): have %i parameters", mNumParameters);
	parameters.Log();

	// add set parameter request to body
	parameters.CreateSetRequestJson(rootItem);
}
