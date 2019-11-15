/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_LOGSCREATERESPONSE
#define __NEUROMORE_LOGSCREATERESPONSE

// include required headers
#include "Response.h"


// response
class QTBASE_API LogsCreateResponse : public Response
{
	public:
		// constructor
		LogsCreateResponse(QNetworkReply* reply);
};


#endif
