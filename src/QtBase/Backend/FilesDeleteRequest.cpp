/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "FilesDeleteRequest.h"


using namespace Core;

// constructor
FilesDeleteRequest::FilesDeleteRequest(const char* token, const char* fileId) : Request()
{
	// display text
	SetDisplayText("Deleting file ...");

	// url
	String urlString;
	urlString.Format( "files/%s/delete", fileId );
	SetUrl( urlString.AsChar() );

	// request method
	SetRequestMethod( REQUESTMETHOD_DELETE );

	// url parameters
	AddUrlParameter( "token", token );
}