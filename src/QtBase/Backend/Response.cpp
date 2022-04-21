/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "Response.h"
#include "BackendHelpers.h"
#include "../QtBaseManager.h"
#include <Core/LogManager.h>
#include <QDesktopServices>
#include <QMessageBox>


using namespace Core;

// constructor
Response::Response(QNetworkReply* reply, bool handleCustomErrorCodes)
{
	// cleanup the network reply
	GetBackendInterface()->GetNetworkAccessManager()->NetworkReplyAftermath( reply );

	mHasError			= false;
	mExpectedReturnCode	= BACKEND_RETURNCODE_OK;
	mReply				= reply;

	// cache control
	QVariant fromCache = mReply->attribute(QNetworkRequest::SourceIsFromCacheAttribute);
    const bool cachedResponse = fromCache.toBool();

	const bool logging = (GetBackendInterface()->GetNetworkAccessManager()->IsLogEnabled() == true && CORE_LOGMANAGER.GetLogLevels() & LOGLEVEL_DETAILEDINFO);
	if (logging == true)
		LogDetailedInfo("REST REPLY%s: url=\"%s\"", cachedResponse ? " (CACHED)" : "", FromQtString(BackendHelpers::ConvertToSecureUrl(mReply->request().url()).toString()).AsChar());

	// TODO: optimization possible?
	String replyDataString = mReply->readAll().data();

	// is the reply empty?
	if (replyDataString.IsEmpty() == true)
	{
		mHasError = true;

		String errorMessage;
		if (reply->error() != QNetworkReply::NoError)
			errorMessage.Format( "%s (ErrorCode=%i).", FromQtString(reply->errorString()).AsChar(), reply->error() );
		else
			errorMessage = "Response is empty.";

		ShowError( NULL, replyDataString.AsChar(), "REST Response Error", errorMessage.AsChar() );
		return;
	}

	// parse the json network reply
	if (mJson.Parse(replyDataString) == false)
	{
		mHasError = true;
		ShowError( NULL, replyDataString.AsChar(), "REST Response Error", "Response is no valid json." );
		return;
	}

	// log json body
	if (logging == true)
		mJson.Log(true);

	// meta
	Json::Item metaItem = mJson.Find("meta");
	if (metaItem.IsNull() == true)
	{
		mHasError = true;
		ShowError( NULL, replyDataString.AsChar(), "REST Response Error", "Network reply does not contain a meta item." );
		return;
	}

	// code
	Json::Item codeItem = metaItem.Find("code");
	if (codeItem.IsNull() == true || codeItem.IsInt() == false)
	{
		mHasError = true;
		ShowError( NULL, replyDataString.AsChar(), "REST Response Error", "Network reply does not contain a code item or is not a valid integer." );
		return;
	}

	// check the meta return code
	int32 code = codeItem.GetInt();
	if (code != mExpectedReturnCode)
	{
		mHasError = true;

		// show error
		if (handleCustomErrorCodes == false)
			ShowError( NULL, replyDataString.AsChar(), GetErrorType(), GetErrorMessage() );

		return;
	}

	// network reply errors
	if (reply->error() != QNetworkReply::NoError)
	{
		mHasError = true;

		String errorMessage;
		errorMessage.Format( "%s (ErrorCode=%i).", FromQtString(reply->errorString()).AsChar(), reply->error() );

		ShowError( NULL, replyDataString.AsChar(), "QNetworkReply Error", errorMessage.AsChar() );
		return;
	}
}


// destructor
Response::~Response()
{
}


bool Response::HasError() const
{
	return mHasError;
}


bool Response::HasErrorOfType(const char* errorType)
{
	String returnType = GetErrorType();

	if (HasError() == true && returnType.IsEqual(errorType) == true)
		return true;

	return false;
}


// read and return the error type from the response
const char* Response::GetErrorType()
{
	// meta
	Json::Item metaItem = mJson.Find("meta");
	if (metaItem.IsNull() == true)
		return "";

	// get the error type
	Json::Item errorTypeItem = metaItem.Find("type");
	if (errorTypeItem.IsString() == false)
		return "";

	return errorTypeItem.GetString();
}


// read and return the error message from the response
const char* Response::GetErrorMessage()
{
	// meta
	Json::Item metaItem = mJson.Find("meta");
	if (metaItem.IsNull() == true)
		return "";

	// get the error message
	Json::Item errorMessageItem = metaItem.Find("message");
	if (errorMessageItem.IsString() == false)
		return "";

	return errorMessageItem.GetString();
}


void Response::ShowErrorDialog(QWidget* parent)
{
	String replyData;
	mJson.WriteToString( replyData );

	ShowError( parent, replyData.AsChar(), GetErrorType(), GetErrorMessage() );
}


void Response::ShowError(QWidget* parent, const char* replyData, const char* title, const char* errorMessage)
{
#ifndef PRODUCTION_BUILD
	// write the reply data to a html file
	String errorFilename = GetQtBaseManager()->GetAppDataFolder() + "Error.html";

	QFile outFile(errorFilename.AsChar());
	outFile.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream outStream(&outFile);
	outStream << replyData;
	outFile.close();

	// show the reply data html file in the default browser
	String urlString;
	urlString.Format( "file:///%s", errorFilename.AsChar() );
	QUrl url( urlString.AsChar(), QUrl::TolerantMode );
	QDesktopServices::openUrl( url );
	
	QMessageBox::critical( parent, title, errorMessage );
#endif

	String text = "REST ERROR: ";
	text += errorMessage;
	LogError( text.AsChar() );
}