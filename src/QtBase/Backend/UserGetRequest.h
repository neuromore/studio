/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_USERGETREQUEST
#define __NEUROMORE_USERGETREQUEST

// include required headers
#include "Request.h"


class QTBASE_API UserGetRequest : public Request
{
	public:
		UserGetRequest(const char* token, const char* userID);
};


#endif
