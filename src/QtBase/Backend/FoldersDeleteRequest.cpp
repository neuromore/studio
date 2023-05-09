/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "FoldersDeleteRequest.h"


using namespace Core;

// constructor
FoldersDeleteRequest::FoldersDeleteRequest(const char* token, const char* folderId) : Request()
{
	// display text
	SetDisplayText("Deleting folder ...");

	// url
	String urlString;
	urlString.Format( "folders/%s/delete", folderId );
	SetUrl( urlString.AsChar() );

	// request method
	SetRequestMethod( REQUESTMETHOD_DELETE );

	// url parameters
	AddUrlParameter( "token", token );
}