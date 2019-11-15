/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_PARAMETERSSETRESPONSE
#define __NEUROMORE_PARAMETERSSETRESPONSE

// include required headers
#include "Response.h"


// response
class QTBASE_API ParametersSetResponse : public Response
{
	public:
		ParametersSetResponse(QNetworkReply* reply);
};


#endif
