/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "FileHierarchyGetResponse.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
FileHierarchyGetResponse::FileHierarchyGetResponse(QNetworkReply* reply) : Response(reply)
{
	// check for errors
	if (mHasError == true)
		return;
}