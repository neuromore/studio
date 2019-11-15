/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "UsersGetResponse.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
UsersGetResponse::UsersGetResponse(QNetworkReply* reply) : Response(reply)
{
	mPageIndex = CORE_INVALIDINDEX32;
	mTotalPages= CORE_INVALIDINDEX32;

	// check for errors
	if (mHasError == true)
		return;


	// data
	Json::Item dataItem = mJson.Find("data");
	if (dataItem.IsNull() == true)
		return;

	// users
	Json::Item usersItem = dataItem.Find("users");
	if (usersItem.IsNull() == true)
		return;

	// iterate through users
	const uint32 numUsers = usersItem.Size();
	for (uint32 i=0; i<numUsers; ++i)
	{
		Json::Item userItem = usersItem[i];
		User user;

		// user id
		Json::Item userIdItem = userItem.Find("userId");
		if (userIdItem.IsString() == true)
			user.SetId( userIdItem.GetString() );

		// first name
		Json::Item firstNameItem = userItem.Find("firstname");
		if (firstNameItem.IsString() == true)
			user.SetFirstName( firstNameItem.GetString() );

		// middle name
		Json::Item middleNameItem = userItem.Find("middlename");
		if (middleNameItem.IsString() == true)
			user.SetMiddleName( middleNameItem.GetString() );

		// last name
		Json::Item lastNameItem = userItem.Find("lastname");
		if (lastNameItem.IsString() == true)
			user.SetLastName( lastNameItem.GetString() );

		// email
		Json::Item emailItem = userItem.Find("email");
		if (emailItem.IsString() == true)
			user.SetEmail( emailItem.GetString() );

		//LogInfo( "User: Id=%s, Name=%s %s %s", user.GetId(), user.GetFirstName(), user.GetMiddleName(), user.GetLastName() );

		mUsers.Add(user);
	}

	// meta
	Json::Item metaItem = mJson.Find("meta");
	if (metaItem.IsNull() == true)
		return;

	// pagination
	Json::Item paginationItem = metaItem.Find("pagination");
	if (paginationItem.IsNull() == true)
		return;

	// page index
	Json::Item pageIndexItem = paginationItem.Find("pageIndex");
	if (pageIndexItem.IsInt() == true)	
		mPageIndex = pageIndexItem.GetInt();

	// total pages
	Json::Item totalPagesItem = paginationItem.Find("totalPages");
	if (totalPagesItem.IsInt() == true)
		mTotalPages = totalPagesItem.GetInt();
}
