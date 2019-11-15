/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "UsersInviteResponse.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
UsersInviteResponse::UsersInviteResponse(QNetworkReply* reply) : Response(reply, true)
{
	// check for errors
	if (mHasError == true)
		return;
}