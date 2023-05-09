/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "ParametersSetResponse.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
ParametersSetResponse::ParametersSetResponse(QNetworkReply* reply) : Response(reply)
{
	// check for errors
	if (mHasError == true)
		return;

	// nothing TODO ??? set request has no response
}
