// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "UserGetRequest.h"

#include <QString>


using namespace Core;

// constructor
UserGetRequest::UserGetRequest(const char* token, const char* userID) : Request()
{
	// display text
	SetDisplayText("Getting users ...");

	// url
	SetUrl(QString("/api/users/%1/get").arg(userID).toUtf8().data());

	// request method
	SetRequestMethod( REQUESTMETHOD_GET );

	// url parameters
	AddUrlParameter( "token", token );
}