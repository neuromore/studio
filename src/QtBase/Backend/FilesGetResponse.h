/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FILESGETRESPONSE
#define __NEUROMORE_FILESGETRESPONSE

// include required headers
#include "Response.h"
#include <User.h>


// response
class QTBASE_API FilesGetResponse : public Response
{
	public:
		// constructor
		FilesGetResponse(QNetworkReply* reply);
		
		const char* GetJsonContent() const				{ return mJsonContent.AsChar(); }
		const char* GetFileId() const					{ return mFileId.AsChar(); }
		const char* GetName() const						{ return mName.AsChar(); }
		const char* GetFileType() const					{ return mFileType.AsChar(); }
		const Creud& GetCreud() const					{ return mCreud; }
		uint32 GetRevision() const						{ return mRevision; }

	private:
		Core::String	mJsonContent;
		Core::String	mFileId;
		Core::String	mName;
		Core::String	mFileType;
		Creud			mCreud;
		uint32			mRevision;
};


#endif
