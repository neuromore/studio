/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FILESUPDATEREQUEST
#define __NEUROMORE_FILESUPDATEREQUEST

// include required headers
#include "Request.h"


class QTBASE_API FilesUpdateRequest : public Request
{
	public:
		FilesUpdateRequest(const char* token, const char* fileId, const char* jsonContent, const Core::String& newName = "", const Core::String& newFolderId = "");
};


#endif
