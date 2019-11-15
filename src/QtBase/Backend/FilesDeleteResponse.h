/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FILESDELETERESPONSE
#define __NEUROMORE_FILESDELETERESPONSE

// include required headers
#include "Response.h"
#include <User.h>


// response
class QTBASE_API FilesDeleteResponse : public Response
{
	public:
		// constructor
		FilesDeleteResponse(QNetworkReply* reply);
		
		const char* GetJsonContent() const				{ return mJsonContent.AsChar(); }
		const char* GetFileId() const					{ return mFileId.AsChar(); }
		const char* GetFileType() const					{ return mFileType.AsChar(); }
		const Creud& GetCreud() const					{ return mCreud; }
		uint32 GetRevision() const						{ return mRevision; }

	private:
		Core::String	mJsonContent;
		Core::String	mFileId;
		Core::String	mFileType;
		Creud			mCreud;
		uint32			mRevision;
};


#endif
