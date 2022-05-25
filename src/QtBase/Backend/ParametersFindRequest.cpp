/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "ParametersFindRequest.h"
#include <Core/String.h>


using namespace Core;

// constructor
ParametersFindRequest::ParametersFindRequest(const char* token, const char* userId, const Experience* experience, const Classifier* classifier, const StateMachine* stateMachine)
{
	// display text
	SetDisplayText("Loading Parameters ...");

	// url
	String url;
	url.Format("users/%s/parameters/find", userId);
	SetUrl( url.AsChar() );

	// request method
	SetRequestMethod( REQUESTMETHOD_POST );

	// url parameters
	AddUrlParameter( "token", token );

	// body
	Json::Item rootItem = GetBodyJsonRoot();

	// parameters array
	Json::Item arrayItem = rootItem.AddArray("parameters");

	mNumParameters = 0;

	// Note: no parameters for experience yet
	//if (experience != NULL)
	//	mNumParameters += CloudParameters::CreateFindParametersJson(*experience, arrayItem);

	// classifier parameters (graph settings + cloud input nodes)
	if (classifier != NULL)
		mNumParameters += CloudParameters::CreateFindParametersJson(*classifier, arrayItem);
}