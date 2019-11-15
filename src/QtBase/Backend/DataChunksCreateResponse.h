/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_DATACHUNKSCREATERESPONSE
#define __NEUROMORE_DATACHUNKSCREATERESPONSE

// include required headers
#include "Response.h"


// response
class QTBASE_API DataChunksCreateResponse : public Response
{
	public:
		DataChunksCreateResponse(QNetworkReply* reply);

		const char* GetDataChunkId() const					{ return mDataChunkId; }

	private:
		Core::String mDataChunkId;
};


#endif
