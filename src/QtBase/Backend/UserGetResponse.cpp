/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "UserGetResponse.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
UserGetResponse::UserGetResponse(QNetworkReply* reply, bool handleCustomErrorCodes) : Response(reply, handleCustomErrorCodes), mIsNotFoundError(false)
{
   Json::Item dataItem = mJson.Find("data");

   // no data
   if (dataItem.IsNull())
   {
      Json::Item metaItem = mJson.Find("meta");

      if (metaItem.IsNull()) {
         mHasError = true;
         return;
      }

      Json::Item codeItem = metaItem.Find("code");
      Json::Item typeItem = metaItem.Find("type");

      if (codeItem.IsNull() || typeItem.IsNull()) {
         mHasError = true;
         return;
      }

      const int   code = codeItem.GetInt();
      const char* type = typeItem.GetString();

      // test for 404 / USER_NOT_FOUND case
      if (code == 404 && 0 == ::strcmp(type, "USER_NOT_FOUND")) {
         std::cout << "User is not exist" << std::endl;
         mIsNotFoundError = true;
      }
   }

   // data
   else 
   {
      Json::Item userItem = dataItem.Find("user");

      if (userItem.IsNull()) {
         mHasError = true;
         return;
      }

      Json::Item userIdItem = userItem.Find("userId");
      if (userIdItem.IsString())
         mUser.SetId(userIdItem.GetString());

      // first name
      Json::Item firstNameItem = userItem.Find("firstname");
      if (firstNameItem.IsString())
         mUser.SetFirstName(firstNameItem.GetString());

      // middle name
      Json::Item middleNameItem = userItem.Find("middlename");
      if (middleNameItem.IsString())
         mUser.SetMiddleName(middleNameItem.GetString());

      // last name
      Json::Item lastNameItem = userItem.Find("lastname");
      if (lastNameItem.IsString())
         mUser.SetLastName(lastNameItem.GetString());

      // email
      Json::Item emailItem = userItem.Find("email");
      if (emailItem.IsString())
         mUser.SetEmail(emailItem.GetString());

      // birthday
      Json::Item birthdayItem = userItem.Find("birthday");
      if (birthdayItem.IsString()) {
         mUser.SetBirthday(birthdayItem.GetString());
      }
   }
}
