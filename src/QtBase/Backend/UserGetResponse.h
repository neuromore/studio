/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_USERSGETRESPONSE
#define __NEUROMORE_USERSGETRESPONSE

// include required headers
#include "Response.h"
#include <Core/String.h>

// response
class QTBASE_API UserGetResponse : public Response
{
	public:
		// constructor
		UserGetResponse(QNetworkReply* reply, bool handleCustomErrorCodes=false);

    bool UserIsExist() const          { return mIsExist; }
    User GetUser() const              { return mUser; }

    private:
        User mUser;
        bool mIsExist;
};


#endif
