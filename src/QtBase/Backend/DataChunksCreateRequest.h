/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_DATACHUNKSCREATEREQUEST
#define __NEUROMORE_DATACHUNKSCREATEREQUEST

// include required headers
#include "Request.h"
#include <Core/Time.h>


class QTBASE_API DataChunksCreateRequest : public Request
{
	public:
		DataChunksCreateRequest(const char* token, const char* userId, const char* supervisorId, const char* debitorId, const char* classifierId, int classifierRevision, const char* stateMachineId, int stateMachineRevision, const char* experienceId, int experienceRevision, const Core::Time& startTime, const Core::Time& stopTime, bool isValid);
};


#endif
