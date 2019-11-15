/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FOLDERSCREATEREQUEST
#define __NEUROMORE_FOLDERSCREATEREQUEST

// include required headers
#include "Request.h"


class QTBASE_API FoldersCreateRequest : public Request
{
	public:
		FoldersCreateRequest(const char* token, const Core::String& folderName, const Core::String& parentFolderId);
};


#endif
