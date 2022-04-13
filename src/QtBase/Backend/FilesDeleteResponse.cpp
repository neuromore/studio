/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "FilesDeleteResponse.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
FilesDeleteResponse::FilesDeleteResponse(QNetworkReply* reply) : Response(reply)
{
	mRevision = CORE_INVALIDINDEX32;

	// check for errors
	if (mHasError == true)
		return;
}
