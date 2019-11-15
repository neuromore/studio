/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "FileSystemGetRequest.h"


using namespace Core;

// constructor
FileSystemGetRequest::FileSystemGetRequest(const char* token, const char* userId, uint32 page, uint32 size, const String& mode, const String& itemId, const String& itemTypes, const String& sortKey, const String& sort) : Request()
{
	// display text
	SetDisplayText("Loading file system ...");


	// url
	SetUrl( "filesystem/get" );

	// request method
	SetRequestMethod( REQUESTMETHOD_GET );


	// url parameters

	// required
	AddUrlParameter( "token", token );
	AddUrlParameter( "userId", userId );
	AddIntUrlParameter( "page", page );
	AddIntUrlParameter( "size", size );

	// optional
	if (mode.IsEmpty() == false)		AddUrlParameter( "mode", mode.AsChar() );
	if (itemId.IsEmpty() == false)		AddUrlParameter( "itemId", itemId.AsChar() );
	if (itemTypes.IsEmpty() == false)	AddUrlParameter( "itemTypes", itemTypes.AsChar() );
	if (sortKey.IsEmpty() == false)		AddUrlParameter( "sortKey", sortKey.AsChar() );
	if (sort.IsEmpty() == false)		AddUrlParameter( "sort", sort.AsChar() );
}