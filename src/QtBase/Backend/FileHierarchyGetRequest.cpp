/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "FileHierarchyGetRequest.h"


using namespace Core;

// constructor
FileHierarchyGetRequest::FileHierarchyGetRequest(const char* token, const Core::String& userId) : Request()
{
	// display text
	SetDisplayText("Loading file hierarchy ...");

	// url
	String url;
	url.Format( "filesystem/filehierarchy/%s/get", userId.AsChar() );
	SetUrl( url.AsChar() );

	// request method
	SetRequestMethod( REQUESTMETHOD_GET );

	// url parameters
	AddUrlParameter( "token", token );
}