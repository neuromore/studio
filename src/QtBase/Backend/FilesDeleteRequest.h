/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FILESDELETEREQUEST
#define __NEUROMORE_FILESDELETEREQUEST

// include required headers
#include "Request.h"


class QTBASE_API FilesDeleteRequest : public Request
{
	public:
		FilesDeleteRequest(const char* token, const char* fileId);
};


#endif
