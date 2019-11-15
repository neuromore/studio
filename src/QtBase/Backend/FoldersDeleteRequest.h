/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FOLDERSDELETEREQUEST
#define __NEUROMORE_FOLDERSDELETEREQUEST

// include required headers
#include "Request.h"


class QTBASE_API FoldersDeleteRequest : public Request
{
	public:
		FoldersDeleteRequest(const char* token, const char* folderId);
};


#endif
