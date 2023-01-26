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
UsersCreateRequest::UsersCreateRequest(
   const char*      token, 
   Core::String     firstName, 
   Core::String     lastName, 
   const ParentIds& parentIds,
   Core::String     email,
   Core::String     birthday,
   Core::String     userId,
   int              role) : Request()
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

   // trim the strings
   firstName.Trim();
   lastName.Trim();
   email.Trim();
   birthday.Trim();
   userId.Trim();

   // parent ids are mandatory
   Json::Item parentsItem = rootItem.AddArray("appendParentIds");
   const uint32 numParentIds = parentIds.Size();
   String tempParentId;
   for (uint32 i=0; i<numParentIds; ++i)
   {
      tempParentId = parentIds[i];
      tempParentId.Trim();
      parentsItem.AddString(tempParentId.AsChar());
   }

   // firstname (optional)
   if (!firstName.IsEmpty())
      rootItem.AddString("firstname", firstName.AsChar());

   // lastname (optional)
   if (!lastName.IsEmpty())
      rootItem.AddString("lastname", lastName.AsChar());

   // email (optional)
   if (!email.IsEmpty())
      rootItem.AddString("email", email.AsChar());

   // birthday (optional)
   if (!birthday.IsEmpty())
      rootItem.AddString("birthday", birthday.AsChar());

   // role (optional)
   if (role > 0)
      rootItem.AddInt("role", role);

   // userId (optional)
   if (!userId.IsEmpty())
      rootItem.AddString("userId", userId.AsChar());
}
