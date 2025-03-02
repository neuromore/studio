/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "FilesCreateResponse.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
FilesCreateResponse::FilesCreateResponse(QNetworkReply* reply) : Response(reply)
{
   // data
   Json::Item dataItem = mJson.Find("data");
   if (dataItem.IsNull() == true)
   {
      mHasError = true;
      return;
   }

   // get the data chunk id
   Json::Item fileIdItem = dataItem.Find("fileId");
   if (fileIdItem.IsNull() == true)
   {
      mHasError = true;
      return;
   }

   mFileId = fileIdItem.GetString();
}
