/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_PARAMETERSFINDRESPONSE
#define __NEUROMORE_PARAMETERSFINDRESPONSE

// include required headers
#include "Response.h"
#include <CloudParameters.h>


// response
class QTBASE_API ParametersFindResponse : public Response
{
	public:
		ParametersFindResponse(QNetworkReply* reply);

		const CloudParameters& GetParameters() const	{ return mParameters; }

	private:
		CloudParameters mParameters;
};


#endif