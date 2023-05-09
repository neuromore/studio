/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "Request.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
Request::UrlParameter::UrlParameter(const char* name, const char* value)
{
	mName = name;
	mValue= value;
}


// constructor
Request::Request()
{
	mRequestMethod = REQUESTMETHOD_GET;
}


// destructor
Request::~Request()
{
}


// set the url
void Request::SetUrl(const char* url)
{
	// TODO: needed?
	/*QByteArray parametersEncoded = QUrl::toPercentEncoding( parameterValue );

	//LogInfo( "Url Parameter (source):  %s", parameterValue );
	//LogInfo( "Url Parameter (encoded): %s", parametersEncoded.data() );

	return parametersEncoded.data();*/

	mUrl = url;
}


// get the url
const char* Request::GetUrl() const
{
	return mUrl.AsChar();
}


// set the request method
void Request::SetRequestMethod(Method requestMethod)
{
	mRequestMethod = requestMethod;
}


// get the request method
Request::Method Request::GetRequestMethod() const
{
	return mRequestMethod;
}


// add URL parameter
void Request::AddUrlParameter(const char* name, const char* value)
{
	// is name valid?
	if (String::CalcLength(name) == 0)
		LogWarning( "Request::AddUrlParameter(): Cannot add parameter to url '%s'. Parameter name is invalid.", mUrl.AsChar() );

	mUrlParameters.addQueryItem( name, QUrl::toPercentEncoding(value));
}


// add integer URL parameter
void Request::AddUrlParameter(const char* name, uint32 value)
{
	AddUrlParameter( name, String(value).AsChar() );
}


// add integer URL parameter
void Request::AddIntUrlParameter(const char* name, int value)
{
	AddUrlParameter( name, String(value).AsChar() );
}


// add bool URL parameter
void Request::AddBoolUrlParameter(const char* name, bool value)
{
	if (value == true)
		AddUrlParameter( name, "true" );
	else
		AddUrlParameter( name, "false" );
}


// add double URL parameter
void Request::AddDoubleUrlParameter(const char* name, double value)
{
	String stringValue;
	stringValue.Format("%f", value);
	AddUrlParameter( name, stringValue.AsChar() );
}


// has URL parameters?
bool Request::HasUrlParameters() const
{
	return (mUrlParameters.isEmpty() == false);
}


// get URL query object
QUrlQuery Request::GetUrlQueryObject() const
{
	return mUrlParameters;
}


// construct the URL query string
QString Request::ConstructQueryString()
{
	LogInfo( "QueryString: %s", mUrlParameters.query(QUrl::PrettyDecoded).toLatin1().data() );
	return mUrlParameters.query(QUrl::FullyEncoded);
}


// get body
Json::Item Request::GetBodyJsonRoot()
{
	return mBodyJson.GetRootItem();
}


// get body string
String Request::GetBodyString(bool pretty) const
{
	String result;
	mBodyJson.WriteToString( result, pretty );
	return result;
}


// set display text
void Request::SetDisplayText(const char* displayText)
{
	mDisplayText = displayText;
}


// get display text
const char* Request::GetDisplayText() const
{
	return mDisplayText.AsChar();
}
