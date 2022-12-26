#include "Impersonation.h"
#include "Backend/UserGetRequest.h"
#include "Backend/UserGetResponse.h"
#include "Backend/UsersCreateSpecificUUIDRequest.h"
#include "Backend/UsersCreateSpecificUUIDResponse.h"
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
    if (mJson.Parse(msg.toUtf8().data()) == false)
	{
        // cannot parse the message
		return;
	}

    Json::Item dataItem = mJson.Find("data");
	if (dataItem.IsNull() == true)
		return;

    // uuid
	Json::Item uuidItem = dataItem.Find("uuid");
	if (uuidItem.IsNull() == true)
		return;

    checkUserExistence(uuidItem.GetString());
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

		if (response.HasError() == true)
			return;

		if (response.UserIsExist()) {
			GetMainWindow()->OnSessionUserSelected(response.GetUser());
		} else {
			createUser();
		}
	});
}

void Impersonation::createUser()
{
	Json::Item dataItem = mJson.Find("data");
	if (dataItem.IsNull() == true)
		return;

	Json::Item uuidItem = dataItem.Find("uuid");
	if (uuidItem.IsNull() == true)
		return;

	Json::Item firstNameItem = dataItem.Find("firstName");
	if (uuidItem.IsNull() == true)
		return;

	Json::Item lastNameItem = dataItem.Find("lastName");
	if (uuidItem.IsNull() == true)
		return;

	String userUuid = uuidItem.GetString();
	String firstName = firstNameItem.GetString();
	String lastName = lastNameItem.GetString();

	Array<String> parentIds;
	const uint32 numCompanies = GetUser()->GetNumParentCompanyIds();
	for (uint32 i = 0; i < numCompanies; i++)
		parentIds.Add( GetUser()->GetParentCompanyId(i) );

	// 1. construct invite request
	UsersCreateSpecificUUIDRequest request( GetUser()->GetToken(), userUuid, firstName, lastName, parentIds, 3004 );

	// 2. process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request, Request::UIMODE_SILENT );
	connect(reply, &QNetworkReply::finished, this, [reply, this, userUuid]()
	{
		// 3. construct and parse response
		QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );
		UsersCreateSpecificUUIDResponse response(networkReply);

		if (response.HasError()) {
			return;
		}
		checkUserExistence(userUuid);
	});
}
