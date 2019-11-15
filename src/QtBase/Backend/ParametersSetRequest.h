/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_PARAMETERSSETREQUEST
#define __NEUROMORE_PARAMETERSSETREQUEST

// include required headers
#include "Request.h"
#include <CloudParameters.h>


class QTBASE_API ParametersSetRequest : public Request
{
	public:
		ParametersSetRequest(const char* token, const char* userId, const CloudParameters& parameters);

		uint32 GetNumParameters() const		{ return mNumParameters; }

	private:
		uint32 mNumParameters;
};


#endif
