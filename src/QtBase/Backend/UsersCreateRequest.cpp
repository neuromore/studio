/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "UsersCreateRequest.h"


using namespace Core;

// constructor
UsersCreateRequest::UsersCreateRequest(const char* token, Core::String email, Core::String firstName, Core::String lastName, Core::String birthday, const Array<String>& parentIds, int role) : Request()
{
	// display text
	SetDisplayText("Creating user ...");

	// url
	SetUrl( "users/create" );

	// request method
	SetRequestMethod( REQUESTMETHOD_POST );

	// url parameters
	AddUrlParameter( "token", token );


	// body
	Json::Item rootItem = GetBodyJsonRoot();

	// name
	firstName.Trim();
	rootItem.AddString( "firstname", firstName.AsChar() );
	lastName.Trim();
	rootItem.AddString( "lastname", lastName.AsChar() );

	// email
	if (email.IsEmpty() == false)
	{
		email.Trim();
		rootItem.AddString( "email", email.AsChar() );
	}

	// birthday
	if (birthday.IsEmpty() == false)
	{
		birthday.Trim();
		rootItem.AddString( "birthday", birthday.AsChar() );
	}

	// parent ids
	Json::Item parentsItem = rootItem.AddArray("appendParentIds");
	const uint32 numParentIds = parentIds.Size();
	String tempParentId;
	for (uint32 i=0; i<numParentIds; ++i)
	{
		tempParentId = parentIds[i];
		tempParentId.Trim();
		parentsItem.AddString( tempParentId.AsChar() );
	}

   // role
   if (role > 0)
      rootItem.AddInt("role", role);
}