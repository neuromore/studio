/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "FoldersCreateRequest.h"


using namespace Core;

// constructor
FoldersCreateRequest::FoldersCreateRequest(const char* token, const String& folderName, const String& parentFolderId) : Request()
{
	// display text
	SetDisplayText("Creating folder ...");


	// url
	SetUrl( "folders/create" );

	// request method
	SetRequestMethod( REQUESTMETHOD_POST );

	// url parameters
	AddUrlParameter( "token", token );


	// body
	Json::Item rootItem = GetBodyJsonRoot();

	if (folderName.IsEmpty() == false)
		rootItem.AddString( "name", folderName.AsChar() );

	if (parentFolderId.IsEmpty() == false)
		rootItem.AddString( "parentFolderId", parentFolderId.AsChar() );
}