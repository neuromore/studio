/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "FilesCreateRequest.h"


using namespace Core;

// constructor
FilesCreateRequest::FilesCreateRequest(const char* token, const char* name, const char* folderId, const char* type, const char* jsonContent) : Request()
{
	// display text
	SetDisplayText("Creating file ...");


	// url
	SetUrl( "files/create" );

	// request method
	SetRequestMethod( REQUESTMETHOD_POST );

	// url parameters
	AddUrlParameter( "token", token );


	// body
	Json::Item rootItem = GetBodyJsonRoot();

	// convert json data content to base64
	QByteArray	base64Data	 = jsonContent;
	QString		base64String = base64Data.toBase64();

	rootItem.AddString( "name", name );
	rootItem.AddString( "folderId", folderId );
	rootItem.AddString( "type", type );
	rootItem.AddString( "content", FromQtString(base64String).AsChar() );
}