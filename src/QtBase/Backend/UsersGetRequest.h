/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_USERSGETREQUEST
#define __NEUROMORE_USERSGETREQUEST

// include required headers
#include "Request.h"


class QTBASE_API UsersGetRequest : public Request
{
	public:
		UsersGetRequest(const char* token, uint32 page = 0, uint32 pageSize = 100, Core::String nameFilter = "", Core::String roleFilter = "");
};


#endif
