/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "UsersGetRequest.h"


using namespace Core;

// constructor
UsersGetRequest::UsersGetRequest(const char* token, uint32 page, uint32 pageSize, Core::String nameFilter, Core::String roleFilter) : Request()
{
	// display text
	SetDisplayText("Getting users ...");

	// url
	SetUrl( "/api/users/get" );

	// request method
	SetRequestMethod( REQUESTMETHOD_GET );

	// url parameters
	AddUrlParameter( "token", token );
	AddUrlParameter( "page", page );
	AddUrlParameter( "size", pageSize);

   nameFilter.Trim();
   if (!nameFilter.IsEmpty())
      AddUrlParameter("nameFilter", nameFilter.AsChar());

   roleFilter.Trim();
   if (!roleFilter.IsEmpty())
      AddUrlParameter("roleFilter", roleFilter.AsChar());
}