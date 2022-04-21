/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "FoldersUpdateRequest.h"


using namespace Core;

// constructor
FoldersUpdateRequest::FoldersUpdateRequest(const char* token, const String& folderId, const String& parentFolderId, const String& newName) : Request()
{
	// display text
	SetDisplayText("Updating folder ...");


	// url
	String urlString;
	urlString.Format( "folders/%s/update", folderId.AsChar() );
	SetUrl( urlString.AsChar() );

	// request method
	SetRequestMethod( REQUESTMETHOD_PUT );

	// url parameters
	AddUrlParameter( "token", token );


	// body
	Json::Item rootItem = GetBodyJsonRoot();

	// relinking folder to new parent
	if (parentFolderId.IsEmpty() == false)
		rootItem.AddString( "parentFolderId", parentFolderId.AsChar() );

	// renaming
	if (newName.IsEmpty() == false)
		rootItem.AddString( "name", newName.AsChar() );
}