/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FOLDERSUPDATERESPONSE
#define __NEUROMORE_FOLDERSUPDATERESPONSE

// include required headers
#include "Response.h"


// response
class QTBASE_API FoldersUpdateResponse : public Response
{
	public:
		// constructor
		FoldersUpdateResponse(QNetworkReply* reply);
};


#endif
