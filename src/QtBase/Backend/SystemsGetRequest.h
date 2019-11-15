/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_SYSTEMSGETREQUEST
#define __NEUROMORE_SYSTEMSGETREQUEST

// include required headers
#include "Request.h"


class QTBASE_API SystemsGetRequest : public Request
{
	public:
		SystemsGetRequest(const char* systemName);
};


#endif
