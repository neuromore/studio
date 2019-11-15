/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FOLDERSUPDATEREQUEST
#define __NEUROMORE_FOLDERSUPDATEREQUEST

// include required headers
#include "Request.h"


class QTBASE_API FoldersUpdateRequest : public Request
{
	public:
		FoldersUpdateRequest(const char* token, const Core::String& folderId, const Core::String& parentFolderId, const Core::String& newName);
};


#endif
