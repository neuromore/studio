/****************************************************************************
**
** Copyright 2019 neuromore co
** Contact: https://neuromore.com/contact
**
** Commercial License Usage
** Licensees holding valid commercial neuromore licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and neuromore. For licensing terms
** and conditions see https://neuromore.com/licensing. For further
** information use the contact form at https://neuromore.com/contact.
**
** neuromore Public License Usage
** Alternatively, this file may be used under the terms of the neuromore
** Public License version 1 as published by neuromore co with exceptions as
** appearing in the file neuromore-class-exception.md included in the
** packaging of this file. Please review the following information to
** ensure the neuromore Public License requirements will be met:
** https://neuromore.com/npl
**
****************************************************************************/

// include precompiled header
#include <Studio/Precompiled.h>

#include "Impersonation.h"
#include "Backend/UserGetRequest.h"
#include "Backend/UserGetResponse.h"
#include "Backend/UsersCreateRequest.h"
#include "Backend/UsersCreateResponse.h"
#include "EngineManager.h"
#include "QtBase/QtBaseManager.h"
#include "AppManager.h"
#include "MainWindow.h"

#include <QNetworkReply>

using namespace Core;

Impersonation::Impersonation(QObject* parent) : QObject(parent)
{
}

void Impersonation::handleOnImpersonation(const QString& msg)
{
   if (!mJson.Parse(msg.toUtf8().data()))
      return;

   Json::Item dataItem = mJson.Find("data");
   if (dataItem.IsNull())
      return;

   // uuid
   Json::Item uuidItem = dataItem.Find("uuid");
   if (uuidItem.IsNull())
      return;

   const char* s = uuidItem.GetString();

   // validate uuid string length
   if (::strlen(s) != 36)
      return;

   // validate hyphen characters
   if (s[8] != '-' || s[13] != '-' || s[18] != '-' || s[23] != '-')
      return;

   // validate hex characters
   const bool ISHEXCHARS = 
      isxdigit(s[0])  && isxdigit(s[1])  && isxdigit(s[2])  && isxdigit(s[3]) &&
      isxdigit(s[4])  && isxdigit(s[5])  && isxdigit(s[6])  && isxdigit(s[7]) &&
      isxdigit(s[9])  && isxdigit(s[10]) && isxdigit(s[11]) && isxdigit(s[12]) &&
      isxdigit(s[14]) && isxdigit(s[15]) && isxdigit(s[16]) && isxdigit(s[17]) &&
      isxdigit(s[19]) && isxdigit(s[20]) && isxdigit(s[21]) && isxdigit(s[22]) &&
      isxdigit(s[24]) && isxdigit(s[25]) && isxdigit(s[26]) && isxdigit(s[27]) &&
      isxdigit(s[28]) && isxdigit(s[29]) && isxdigit(s[30]) && isxdigit(s[31]) &&
      isxdigit(s[32]) && isxdigit(s[33]) && isxdigit(s[34]) && isxdigit(s[35]);

   if (!ISHEXCHARS)
      return;

   checkUserExistence(s);
}

void Impersonation::checkUserExistence(const Core::String& uuid)
{
   // construct /users/{userID}/get request
   UserGetRequest request(GetUser()->GetToken(), uuid.AsChar());

   // process request and connect to the reply
   QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request, Request::UIMODE_SILENT );
   connect(reply, &QNetworkReply::finished, this, [reply, this]()
   {
      QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );
      UserGetResponse response(networkReply, true);

      // CASE1: User does not exist
      if (response.IsNotFoundError())
         createUser();

      // CASE2: Some other error
      else if (response.HasError())
      {
         const char* errortype = response.GetErrorType();
         const char* errormsg = response.GetErrorMessage();
         LogError(errormsg);
         return;
      }

      // CASE3: User exists
      else
         GetMainWindow()->OnSessionUserSelected(response.GetUser());
   });
}

void Impersonation::createUser()
{
   Json::Item dataItem = mJson.Find("data");
   if (dataItem.IsNull())
      return;

   // mandatory: uuid
   Json::Item uuidItem = dataItem.Find("uuid");
   if (uuidItem.IsNull())
      return;

   // optional
   Json::Item emailItem     = dataItem.Find("email");
   Json::Item firstNameItem = dataItem.Find("firstName");
   Json::Item lastNameItem  = dataItem.Find("lastName");
   Json::Item birthdayItem  = dataItem.Find("birthday");

   // string request parameters
   const String userUuid  = uuidItem.GetString();
   const String email     = emailItem.IsNull()     ? "" : emailItem.GetString();
   const String firstName = firstNameItem.IsNull() ? "" : firstNameItem.GetString();
   const String lastName  = lastNameItem.IsNull()  ? "" : lastNameItem.GetString();
   const String birthday  = birthdayItem.IsNull()  ? "" : birthdayItem.GetString();

   // array request parameter
   Array<String> parentIds;
   const uint32 numCompanies = GetUser()->GetNumParentCompanyIds();
   for (uint32 i = 0; i < numCompanies; i++)
      parentIds.Add( GetUser()->GetParentCompanyId(i) );

   // construct request
   UsersCreateRequest request(GetUser()->GetToken(), firstName, lastName, parentIds, email, birthday, userUuid, 3004);

   // process request and connect to the reply
   QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request, Request::UIMODE_SILENT );
   connect(reply, &QNetworkReply::finished, this, [reply, this, userUuid]()
   {
      QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );
      UsersCreateResponse response(networkReply);

      if (response.HasError()) {
         return;
      }

      checkUserExistence(userUuid);
   });
}
