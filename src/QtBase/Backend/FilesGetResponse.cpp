/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "FilesGetResponse.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
FilesGetResponse::FilesGetResponse(QNetworkReply* reply) : Response(reply)
{
	mRevision = CORE_INVALIDINDEX32;

	// check for errors
	if (mHasError == true)
		return;

	// data
	Json::Item dataItem = mJson.Find("data");
	if (dataItem.IsNull() == true)
	{
		mHasError = true;
		return;
	}

	// file
	Json::Item fileItem = dataItem.Find("file");
	if (fileItem.IsNull() == true)
	{
		mHasError = true;
		return;
	}

	// content
	Json::Item contentItem = fileItem.Find("content");
	if (contentItem.IsNull() == false)
	{
		QString content = QByteArray::fromBase64( contentItem.GetString() );
		mJsonContent = FromQtString( content );

		if (mJsonContent.IsEmpty() == true)
		{
			//BackendHelpers::ReportError("File content empty");
			mHasError = true;
		}
	}

	// type
	Json::Item typeItem = fileItem.Find("type");
	if (typeItem.IsString() == true)
		mFileType = typeItem.GetString();

	// creud
	mCreud.ReadFromJson( fileItem );

	// revision
	Json::Item revisionItem = fileItem.Find("revision");
	if (revisionItem.IsInt() == true)
		mRevision = revisionItem.GetInt();

	// name
	Json::Item nameItem = fileItem.Find("name");
	if (nameItem.IsString() == true)
		mName = nameItem.GetString();

	// item id
	Json::Item itemFileId = fileItem.Find("fileId");
	if (itemFileId.IsString() == true)
		mFileId = itemFileId.GetString();
}
