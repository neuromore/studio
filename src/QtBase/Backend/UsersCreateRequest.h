/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_USERSCREATEREQUEST
#define __NEUROMORE_USERSCREATEREQUEST

// include required headers
#include "Request.h"
#include <Core/Array.h>
#include <Core/String.h>


class QTBASE_API UsersCreateRequest : public Request
{
	public:
		UsersCreateRequest(const char* token, Core::String email, Core::String firstName, Core::String lastName, Core::String birthday, const Core::Array<Core::String>& parentIds);
};


#endif
