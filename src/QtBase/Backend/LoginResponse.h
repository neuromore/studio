/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_LOGINRESPONSE
#define __NEUROMORE_LOGINRESPONSE

// include required headers
#include "Response.h"
#include <User.h>


// response
class QTBASE_API LoginResponse : public Response
{
	public:
		LoginResponse(QNetworkReply* reply);

		const User& GetUser() const				{ return mUser; }
	
	private:
		User mUser;
};


#endif
