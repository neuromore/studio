/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "FilesCreateResponse.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
FilesCreateResponse::FilesCreateResponse(QNetworkReply* reply) : Response(reply)
{
	// check for errors
	if (mHasError == true)
		return;
}
