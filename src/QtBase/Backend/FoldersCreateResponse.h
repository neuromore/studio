/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FOLDERSCREATERESPONSE
#define __NEUROMORE_FOLDERSCREATERESPONSE

// include required headers
#include "Response.h"


// response
class QTBASE_API FoldersCreateResponse : public Response
{
	public:
		// constructor
		FoldersCreateResponse(QNetworkReply* reply);

		const char* GetFolderId() const					{ return mFolderId.AsChar(); }

	private:
		Core::String	mFolderId;
};


#endif
