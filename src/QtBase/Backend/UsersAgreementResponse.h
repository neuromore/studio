/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_USERSAGREEMENTRESPONSE
#define __NEUROMORE_USERSAGREEMENTRESPONSE

// include required headers
#include "Response.h"
#include <User.h>
#include <Core/Array.h>


// response
class QTBASE_API UsersAgreementResponse : public Response
{
	public:
		// constructor
		UsersAgreementResponse(QNetworkReply* reply);
};


#endif
