/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "FilesUpdateRequest.h"


using namespace Core;

// constructor
FilesUpdateRequest::FilesUpdateRequest(const char* token, const char* fileId, const char* jsonContent, const String& newName, const String& newFolderId) : Request()
{
	// display text
	SetDisplayText("Updating file ...");


	// url
	String urlString;
	urlString.Format( "files/%s/update", fileId );
	SetUrl( urlString.AsChar() );

	// request method
	SetRequestMethod( REQUESTMETHOD_PUT );

	// url parameters
	AddUrlParameter( "token", token );


	// body
	Json::Item rootItem = GetBodyJsonRoot();

	// content
	if (jsonContent != NULL)
	{
		// add json data content as base64
		QByteArray	base64Data	 = jsonContent;
		QString		base64String = base64Data.toBase64();
		rootItem.AddString( "content", FromQtString(base64String).AsChar() );
	}

	// name
	if (newName.IsEmpty() == false)
		rootItem.AddString( "name", newName.AsChar() );

	// folder id
	if (newFolderId.IsEmpty() == false)
		rootItem.AddString( "folderId", newFolderId.AsChar() );
}