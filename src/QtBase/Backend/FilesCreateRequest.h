/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FILESCREATEREQUEST
#define __NEUROMORE_FILESCREATEREQUEST

// include required headers
#include "Request.h"


class QTBASE_API FilesCreateRequest : public Request
{
	public:
		FilesCreateRequest(const char* token, const char* name, const char* folderId, const char* type, const char* jsonContent);
};


#endif
