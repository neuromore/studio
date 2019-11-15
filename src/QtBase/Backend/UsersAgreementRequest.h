/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_USERSAGREEMENTREQUEST
#define __NEUROMORE_USERSAGREEMENTREQUEST

// include required headers
#include "Request.h"


class QTBASE_API UsersAgreementRequest : public Request
{
	public:
		UsersAgreementRequest(const char* token, const char* userId, const char* agreementName);
};


#endif
