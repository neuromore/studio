/*
 * Qt Base
 * Copyright (c) 2012-2022 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_USERSCREATESPECIFICUUIDREQUEST
#define __NEUROMORE_USERSCREATESPECIFICUUIDREQUEST

// include required headers
#include "Request.h"
#include <Core/Array.h>
#include <Core/String.h>


class QTBASE_API UsersCreateSpecificUUIDRequest : public Request
{
	public:
		UsersCreateSpecificUUIDRequest(const char* token, const Core::String& userID, Core::String firstName, Core::String lastName, const Core::Array<Core::String>& parentIds, int role = 0);
};


#endif
