/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_LOGINREQUEST
#define __NEUROMORE_LOGINREQUEST

// include required headers
#include "Request.h"


class QTBASE_API LoginRequest : public Request
{
	public:
		LoginRequest(const char* systemName, const char* version, Core::String userId, const char* password);
};


#endif