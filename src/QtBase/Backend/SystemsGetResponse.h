/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_SYSTEMSGETRESPONSE
#define __NEUROMORE_SYSTEMSGETRESPONSE

// include required headers
#include "Response.h"
#include <Core/Version.h>


// response
class QTBASE_API SystemsGetResponse : public Response
{
	public:
		// constructor
		SystemsGetResponse(QNetworkReply* reply);
		
		const Core::Version& GetVersion() const				{ return mVersion; }

	private:
		Core::Version mVersion;
};


#endif
