/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "UsersCreateResponse.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
UsersCreateResponse::UsersCreateResponse(QNetworkReply* reply) : Response(reply, true)
{
	// check for errors
	if (mHasError == true)
		return;
}