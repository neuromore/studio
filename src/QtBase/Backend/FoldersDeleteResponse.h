/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FOLDERSDELETERESPONSE
#define __NEUROMORE_FOLDERSDELETERESPONSE

// include required headers
#include "Response.h"


// response
class QTBASE_API FoldersDeleteResponse : public Response
{
	public:
		// constructor
		FoldersDeleteResponse(QNetworkReply* reply);
};


#endif
