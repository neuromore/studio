/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FILEHIERARCHYGETREQUEST
#define __NEUROMORE_FILEHIERARCHYGETREQUEST

// include required headers
#include "Request.h"


class QTBASE_API FileHierarchyGetRequest : public Request
{
	public:
		FileHierarchyGetRequest(const char* token, const Core::String& userId);
};


#endif
