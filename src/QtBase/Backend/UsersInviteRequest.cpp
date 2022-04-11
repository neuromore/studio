/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "UsersInviteRequest.h"


using namespace Core;

// constructor
UsersInviteRequest::UsersInviteRequest(const char* token, const char* userId, Core::String email) : Request()
{
	// display text
	SetDisplayText("Inviting user ...");

	// url
	String urlString;
	urlString.Format("users/%s/invite", userId);
	SetUrl( urlString.AsChar() );

	// request method
	SetRequestMethod( REQUESTMETHOD_GET );

	// url parameters
	AddUrlParameter( "token", token );
	email.Trim();
	email.Trim( StringCharacter('\t') );
	AddUrlParameter( "email", email );
}