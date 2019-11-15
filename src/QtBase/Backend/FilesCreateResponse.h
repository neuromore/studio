/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FILESCREATERESPONSE
#define __NEUROMORE_FILESCREATERESPONSE

// include required headers
#include "Response.h"


// response
class QTBASE_API FilesCreateResponse : public Response
{
	public:
		// constructor
		FilesCreateResponse(QNetworkReply* reply);
};


#endif
