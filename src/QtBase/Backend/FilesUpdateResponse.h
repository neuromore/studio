/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FILESUPDATERESPONSE
#define __NEUROMORE_FILESUPDATERESPONSE

// include required headers
#include "Response.h"


// response
class QTBASE_API FilesUpdateResponse : public Response
{
	public:
		// constructor
		FilesUpdateResponse(QNetworkReply* reply);
};


#endif
