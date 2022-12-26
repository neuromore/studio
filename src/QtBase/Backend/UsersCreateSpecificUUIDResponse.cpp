/*
 * Qt Base
 * Copyright (c) 2012-2022 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "UsersCreateSpecificUUIDResponse.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
UsersCreateSpecificUUIDResponse::UsersCreateSpecificUUIDResponse(QNetworkReply* reply) : Response(reply, true)
{
	// check for errors
	if (mHasError == true)
		return;
}