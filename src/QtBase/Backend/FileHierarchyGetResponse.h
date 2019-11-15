/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FILEHIERARCHYGETRESPONSE
#define __NEUROMORE_FILEHIERARCHYGETRESPONSE

// include required headers
#include "Response.h"


// response
class QTBASE_API FileHierarchyGetResponse : public Response
{
	public:
		// constructor
		FileHierarchyGetResponse(QNetworkReply* reply);

		const Core::Json& GetJson() const					{ return mJson; }
};


#endif