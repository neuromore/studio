/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "LoginRequest.h"
#include <Core/String.h>


using namespace Core;

// constructor
LoginRequest::LoginRequest(const char* systemName, const char* version, String userId, const char* password) : Request()
{
	// display text
	SetDisplayText("Logging in ...");

	// url
	String url;
	url.Format("systems/%s/login", systemName);
	SetUrl( url.AsChar() );

	// request method
	SetRequestMethod( REQUESTMETHOD_POST );


	// body
	Json::Item rootItem = GetBodyJsonRoot();

	userId.Trim();
	rootItem.AddString( "userId", userId.AsChar() );
	rootItem.AddString( "password", password );
	rootItem.AddString( "version", version );
}