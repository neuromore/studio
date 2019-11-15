/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_LOGSCREATEREQUEST
#define __NEUROMORE_LOGSCREATEREQUEST

// include required headers
#include "Request.h"
#include <Core/LogCallbacks.h>


class QTBASE_API LogsCreateRequest : public Request
{
	public:
		LogsCreateRequest(const char* token, const char* text, Core::ELogLevel logLevel);
};


#endif
