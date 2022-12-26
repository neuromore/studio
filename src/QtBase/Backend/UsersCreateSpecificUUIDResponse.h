/*
 * Qt Base
 * Copyright (c) 2012-2022 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_USERSCREATESPECIFICUUIDRESPONSE
#define __NEUROMORE_USERSCREATESPECIFICUUIDRESPONSE

// include required headers
#include "Response.h"


// response
class QTBASE_API UsersCreateSpecificUUIDResponse : public Response
{
	public:
		// constructor
		UsersCreateSpecificUUIDResponse(QNetworkReply* reply);
};


#endif
