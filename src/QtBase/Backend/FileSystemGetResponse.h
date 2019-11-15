/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FILESYSTEMGETRESPONSE
#define __NEUROMORE_FILESYSTEMGETRESPONSE

// include required headers
#include "Response.h"
#include "FileSystemItem.h"


// response
class QTBASE_API FileSystemGetResponse : public Response
{
	public:
		// constructor
		FileSystemGetResponse(QNetworkReply* reply);

		// data
		Core::Array<FileSystemItem> mParents;
		Core::Array<FileSystemItem> mItems;

		// meta
		uint32 mTotalPages;
		uint32 mPageIndex;
};


#endif