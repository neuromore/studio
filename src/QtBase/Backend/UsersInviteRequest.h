/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_USERSINVITEREQUEST
#define __NEUROMORE_USERSINVITEREQUEST

// include required headers
#include "Request.h"
#include <Core/Array.h>


class QTBASE_API UsersInviteRequest : public Request
{
	public:
		UsersInviteRequest(const char* token, const char* userId, Core::String email);
};


#endif
