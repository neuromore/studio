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
UserGetResponse::UserGetResponse(QNetworkReply* reply, bool handleCustomErrorCodes) : Response(reply, handleCustomErrorCodes)
{
    mIsExist = true;

	// data
	Json::Item dataItem = mJson.Find("data");
	if (dataItem.IsNull() == true) {
        Json::Item metaItem = mJson.Find("meta");

        if (metaItem.IsNull() == true) {
            mHasError = true;
            return;
        }

        Json::Item codeItem = metaItem.Find("code");
        Json::Item typeItem = metaItem.Find("type");

        if (codeItem.IsNull() && typeItem.IsNull()) {
            mHasError = true;
            return;
        }

        if (codeItem.GetInt() == 403 && 0 == ::strcmp(typeItem.GetString(), "GET_USER_FAILED")) {
			std::cout << "User is not exist" << std::endl;
            mIsExist = false;
        }

    } else {
        Json::Item userItem = dataItem.Find("user");

		if (userItem.IsNull()) {
			mHasError = true;
            return;
		}

        Json::Item userIdItem = userItem.Find("userId");
        if (userIdItem.IsString() == true)
			mUser.SetId( userIdItem.GetString() );

		// first name
		Json::Item firstNameItem = userItem.Find("firstname");
		if (firstNameItem.IsString() == true)
			mUser.SetFirstName( firstNameItem.GetString() );

		// middle name
		Json::Item middleNameItem = userItem.Find("middlename");
		if (middleNameItem.IsString() == true)
			mUser.SetMiddleName( middleNameItem.GetString() );

		// last name
		Json::Item lastNameItem = userItem.Find("lastname");
		if (lastNameItem.IsString() == true)
			mUser.SetLastName( lastNameItem.GetString() );

		// email
		Json::Item emailItem = userItem.Find("email");
		if (emailItem.IsString() == true)
			mUser.SetEmail( emailItem.GetString() );

		// birthday
		Json::Item birthdayItem = userItem.Find("birthday");
		if (birthdayItem.IsString() == true) {
			mUser.SetBirthday( birthdayItem.GetString() );
		}
    }
}
