/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

 // include precompiled header
#include <QtBase/Precompiled.h>

// include required files
#include "BackendHelpers.h"
#include <Core/LogManager.h>
#include "../QtBaseManager.h"
#include "../QtBaseConfig.h"
#include <QMessageBox>
#include <QUrl>
#include <QDesktopServices>
#include <QUrlQuery>


using namespace Core;

// log network reply header
void BackendHelpers::LogHeader(QNetworkReply* reply)
{
	LogInfo( " - Network Reply Header" );
	LogInfo( "    + Content Type Header:  %s", reply->header(QNetworkRequest::ContentTypeHeader).toString().toLatin1().data() );
	LogInfo( "    + Header Length:        %i", reply->header(QNetworkRequest::ContentLengthHeader).toULongLong() );
	LogInfo( "    + Location Header:      %i", reply->header(QNetworkRequest::LocationHeader).toString().toLatin1().data() );
	LogInfo( "    + Last Modified Header: %s", reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString().toLatin1().data() );
	//LogInfo( "    + Cookie Header:        %s", reply->header(QNetworkRequest::CookieHeader).toDateTime().toString().toLatin1().data() );
	//LogInfo( "    + Set Cookie Header:    %s", reply->header(QNetworkRequest::SetCookieHeader).toDateTime().toString().toLatin1().data() );
	LogInfo( " - Network Reply Attributes" );
	LogInfo( "    + HTTP Status Code:     %i", reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() );
	LogInfo( "    + HTTP Reason Phrase:   %s", reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString().toLatin1().data() );
	LogInfo( "    + Connection Encrypted: %i", reply->attribute(QNetworkRequest::ConnectionEncryptedAttribute).toInt() );
}


// logging service helper
void BackendHelpers::ReportError(const char* errorMessage, bool allowMessageBox)
{
	LogCritical(errorMessage);

	if (allowMessageBox == true)
		QMessageBox::critical( NULL, "Backend interface error", errorMessage );
}


// encode url parameter
String BackendHelpers::EncodeUrlParameter(const char* parameterValue)
{
	QByteArray parametersEncoded = QUrl::toPercentEncoding( parameterValue );

	//LogInfo( "Url Parameter (source):  %s", parameterValue );
	//LogInfo( "Url Parameter (encoded): %s", parametersEncoded.data() );

	return parametersEncoded.data();
}


QUrl BackendHelpers::ConvertToSecureUrl(QUrl url)
{
	QUrlQuery urlQuery(url);

	// encode token
#ifdef PRODUCTION_BUILD
	if (urlQuery.hasQueryItem("token") == true)
	{
		LogDetailedInfo("Due to security restrictions, the url query item 'token' has been removed from the url and won't be visible");
		urlQuery.removeQueryItem("token");
	}
#endif

	url.setQuery(urlQuery);
	return url;
}


// helper to check if the network reply reports success or failure
bool BackendHelpers::HasError(QNetworkReply* networkReply, const char* replyDataString, int expectedReturnCode, bool allowMessageBox)
{
	// check if the is completly empty
	if (String(replyDataString).IsEmpty() == true)
	{
		// get access to the return code
		if (networkReply->error() != QNetworkReply::NoError)
		{
			String errorMsg;
			errorMsg.Format( "%s (ErrorCode=%i).", FromQtString(networkReply->errorString()).AsChar(), networkReply->error() );
			LogError("ReplyDataString: %s", replyDataString);
			ReportError( errorMsg.AsChar(), allowMessageBox );
			return true;
		}
		else
		{
			LogError("ReplyDataString: %s", replyDataString);
			ReportError( "Failed. Network reply is empty.", allowMessageBox );
			return true;
		}
	}

	// parse the json network reply
	Json json;
	if (json.Parse(replyDataString) == false)
	{
#ifndef PRODUCTION_BUILD
		String errorFilename = GetQtBaseManager()->GetAppDataFolder() + "Error.html";

		QFile outFile(errorFilename.AsChar());
		outFile.open(QIODevice::WriteOnly | QIODevice::Text);
		QTextStream outStream(&outFile);
		outStream << replyDataString;
		outFile.close();

		String urlString;
		urlString.Format( "file:///%s", errorFilename.AsChar() );
		QUrl url( urlString.AsChar(), QUrl::TolerantMode );
		QDesktopServices::openUrl( url );
#endif

		ReportError( "Network reply is is no valid JSON.", allowMessageBox );
		return true;
	}

	// get the meta item
	Json::Item metaItem = json.Find("meta");
	if (metaItem.IsNull() == true)
	{
		String errorMsg = "ReplyDataString: ";
		errorMsg += replyDataString;

		LogError( errorMsg.AsChar() );
		ReportError( "Network reply does not contain a meta item.", allowMessageBox );
		return true;
	}

	// get the reply code item
	Json::Item codeItem = metaItem.Find("code");
	if (codeItem.IsNull() == true)
	{
		LogError("ReplyDataString: %s", replyDataString);
		ReportError( "Network reply does not contain a code item.", allowMessageBox );
		return true;
	}

	// check the return code
	if (codeItem.GetInt() != expectedReturnCode)
	{
		// get the error message and error type items
		Json::Item errorTypeItem	= metaItem.Find("type");
		Json::Item errorMsgItem	= metaItem.Find("message");

		if (errorTypeItem.IsNull() == false && errorMsgItem.IsNull() == false)
		{
			String errorReport;
			errorReport += errorTypeItem.GetString();
			errorReport += ": ";
			errorReport += errorMsgItem.GetString();
			ReportError( errorReport.AsChar(), allowMessageBox );
		}
		else
		{
			LogError("ReplyDataString: %s", replyDataString);
			ReportError( "Network reply contains an undefined error.", allowMessageBox );
		}

		#ifndef PRODUCTION_BUILD
			String replyData;
			json.WriteToString(replyData);

			// write the reply data to a html file
			String errorFilename = GetQtBaseManager()->GetAppDataFolder() + "Error.html";

			QFile outFile(errorFilename.AsChar());
			outFile.open(QIODevice::WriteOnly | QIODevice::Text);
			QTextStream outStream(&outFile);
			outStream << replyData.AsChar();
			outFile.close();

			// show the reply data html file in the default browser
			String urlString;
			urlString.Format( "file:///%s", errorFilename.AsChar() );
			QUrl url( urlString.AsChar(), QUrl::TolerantMode );
			QDesktopServices::openUrl( url );

		#endif

		return true;
	}

	// get access to the return code
	if (networkReply->error() != QNetworkReply::NoError)
	{
		String errorMsg;
		errorMsg.Format( "%s (ErrorCode=%i).", FromQtString(networkReply->errorString()).AsChar(), networkReply->error() );
		LogError("ReplyDataString: %s", replyDataString);
		ReportError( errorMsg.AsChar(), allowMessageBox );
		return true;
	}

	// no error detected
	return false;
}
