/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_PARAMETERSFINDREQUEST
#define __NEUROMORE_PARAMETERSFINDREQUEST

// include required headers
#include "Request.h"
#include <CloudParameters.h>


// request
class QTBASE_API ParametersFindRequest : public Request
{
	public:
		ParametersFindRequest(const char* token, const char* userId, const Experience* experience, const Classifier* classifier = NULL, const StateMachine* stateMachine = NULL);

		uint32 GetNumParameters() const		{ return mNumParameters; }

	private:
		uint32 mNumParameters;
};


#endif