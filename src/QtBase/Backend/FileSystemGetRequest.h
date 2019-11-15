/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FILESYSTEMGETREQUEST
#define __NEUROMORE_FILESYSTEMGETREQUEST

// include required headers
#include "Request.h"


class QTBASE_API FileSystemGetRequest : public Request
{
	public:
		FileSystemGetRequest(const char* token, const char* userId, uint32 page, uint32 size, const Core::String& mode, const Core::String& itemId, const Core::String& itemTypes, const Core::String& sortKey="", const Core::String& sort="asc");
};


#endif