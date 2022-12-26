/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "UsersCreateSpecificUUIDRequest.h"


using namespace Core;

// constructor
UsersCreateSpecificUUIDRequest::UsersCreateSpecificUUIDRequest(const char* token, const Core::String& userID, Core::String firstName, Core::String lastName, const Core::Array<Core::String>& parentIds, int role) : Request()
{
	// display text
	SetDisplayText("Creating user ...");

	// url
	SetUrl(QString("/api/users/anonymous/%1/create").arg(userID.AsChar()).toUtf8().data());

	// request method
	SetRequestMethod( REQUESTMETHOD_POST );

	// url parameters
	AddUrlParameter( "token", token );
	// AddUrlParameter( "userId", userID.AsChar() );


// 	// body
// 	Json::Item rootItem = GetBodyJsonRoot();

// 	// name
// 	firstName.Trim();
// 	rootItem.AddString( "firstname", firstName.AsChar() );
// 	lastName.Trim();
// 	rootItem.AddString( "lastname", lastName.AsChar() );

// 	// parent ids
// 	Json::Item parentsItem = rootItem.AddArray("appendParentIds");
// 	const uint32 numParentIds = parentIds.Size();
// 	String tempParentId;
// 	for (uint32 i=0; i<numParentIds; ++i)
// 	{
// 		tempParentId = parentIds[i];
// 		tempParentId.Trim();
// 		parentsItem.AddString( tempParentId.AsChar() );
// 	}

//    // role
//    if (role > 0)
//       rootItem.AddInt("role", role);
}