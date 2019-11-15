/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_USERSGETRESPONSE
#define __NEUROMORE_USERSGETRESPONSE

// include required headers
#include "Response.h"
#include <User.h>
#include <Core/Array.h>


// response
class QTBASE_API UsersGetResponse : public Response
{
	public:
		// constructor
		UsersGetResponse(QNetworkReply* reply);
		
		const Core::Array<User>& GetUsers() const		{ return mUsers; }
		uint32 GetPageIndex() const						{ return mPageIndex; }
		uint32 GetTotalPages() const					{ return mTotalPages; }

	private:
		Core::Array<User> mUsers;
		uint32 mPageIndex;
		uint32 mTotalPages;
};


#endif
