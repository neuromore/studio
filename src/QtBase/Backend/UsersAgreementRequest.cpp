/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "UsersAgreementRequest.h"


using namespace Core;

// constructor
UsersAgreementRequest::UsersAgreementRequest(const char* token, const char* userId, const char* agreementName) : Request()
{
	// display text
	SetDisplayText("Signing agreements ...");

	// url
	String urlString;
	urlString.Format("users/%s/agreement", userId);
	SetUrl( urlString.AsChar() );

	// request method
	SetRequestMethod( REQUESTMETHOD_GET );

	// url parameters
	AddUrlParameter( "token", token );
	AddUrlParameter( "agreement", agreementName );
}