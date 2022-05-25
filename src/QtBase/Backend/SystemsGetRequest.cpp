/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "SystemsGetRequest.h"


using namespace Core;

// constructor
SystemsGetRequest::SystemsGetRequest(const char* systemName) : Request()
{
	// display text
	SetDisplayText("Getting version information ...");

	// url
	String urlString;
	urlString.Format("systems/%s/get", systemName );
	SetUrl( urlString.AsChar() );

	// request method
	SetRequestMethod( REQUESTMETHOD_GET );
}