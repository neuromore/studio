/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FILESGETREQUEST
#define __NEUROMORE_FILESGETREQUEST

// include required headers
#include "Request.h"


class QTBASE_API FilesGetRequest : public Request
{
	public:
		FilesGetRequest(const char* token, const char* fileId, int revision=-1);
};


#endif
