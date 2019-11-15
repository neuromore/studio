/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_DATACHUNKSUPLOADREQUEST
#define __NEUROMORE_DATACHUNKSUPLOADREQUEST

// include required headers
#include "Request.h"


class QTBASE_API DataChunksUploadRequest : public Request
{
	public:
		DataChunksUploadRequest(const char* token, const char* userId, const char* dataChunkId, const char* nodeId, const char* channelName, int type, int offset, double sampleRate, double visualMin, double visualMax);
};


#endif