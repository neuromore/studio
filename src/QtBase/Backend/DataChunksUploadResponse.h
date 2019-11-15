/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_DATACHUNKSUPLOADRESPONSE
#define __NEUROMORE_DATACHUNKSUPLOADRESPONSE

// include required headers
#include "Response.h"


// response
class QTBASE_API DataChunksUploadResponse : public Response
{
	public:
		DataChunksUploadResponse(QNetworkReply* reply);
};


#endif