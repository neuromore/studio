/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "FilesGetRequest.h"


using namespace Core;

// constructor
FilesGetRequest::FilesGetRequest(const char* token, const char* fileId, int revision) : Request()
{
	// display text
	SetDisplayText("Loading file ...");

	// url
	String urlString;
	urlString.Format( "files/%s/get", fileId );
	SetUrl( urlString.AsChar() );

	// request method
	SetRequestMethod( REQUESTMETHOD_GET );

	// url parameters
	AddUrlParameter( "token", token );
	if (revision != -1)
		AddIntUrlParameter( "revision", revision );
}