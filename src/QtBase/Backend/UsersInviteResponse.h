/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_USERSINVITERESPONSE
#define __NEUROMORE_USERSINVITERESPONSE

// include required headers
#include "Response.h"


// response
class QTBASE_API UsersInviteResponse : public Response
{
	public:
		// constructor
		UsersInviteResponse(QNetworkReply* reply);
};


#endif
