/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_USERSCREATERESPONSE
#define __NEUROMORE_USERSCREATERESPONSE

// include required headers
#include "Response.h"


// response
class QTBASE_API UsersCreateResponse : public Response
{
	public:
		// constructor
		UsersCreateResponse(QNetworkReply* reply);
};


#endif
