/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required files
#include "BackendInterface.h"
#include <EngineManager.h>
#include <License.h>
#include "../QtBaseManager.h"
#include "BackendHelpers.h"
#include "LogsCreateRequest.h"
#include "LogsCreateResponse.h"
#include <QDateTime>


using namespace Core;

// constructor
BackendInterface::BackendInterface(QObject* parent) : QObject(parent)
{
	LogDetailedInfo("Constructing backend interface ...");

	// create the network access manager
	mNetworkAccessManager = new NetworkAccessManager(parent);

	// add the log callback
	mLogCallback = new BackendLogCallback(mNetworkAccessManager);
	CORE_LOGMANAGER.AddLogCallback(mLogCallback);

	// construct the sub-systems
	mUploader		= new BackendUploader( mNetworkAccessManager, parent );
	mFileSystem		= new BackendFileSystem( parent );
	mParameters		= new BackendParameters( mNetworkAccessManager, parent );
}


// destructor
BackendInterface::~BackendInterface()
{
	LogDetailedInfo("Destructing backend interface ...");

	// get rid of the log callback
	CORE_LOGMANAGER.RemoveLogCallback( mLogCallback, false );
	delete mLogCallback;

	// get rid of the sub-systems
	delete mUploader;
	delete mFileSystem;
	delete mParameters;

	// get rid of the network access manager
	delete mNetworkAccessManager;
}


// back-end log callback
void BackendLogCallback::ForceLog(const char* text, Core::ELogLevel logLevel)
{
	// 1. construct /logs/create request
	LogsCreateRequest request( GetUser()->GetToken(), text, logLevel );

	// 2. process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request, Request::UIMODE_SILENT, true );
	connect(reply, &QNetworkReply::finished, this, [reply, this]()
	{
		QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );

		// 3. construct and parse response
		LogsCreateResponse response(networkReply);
		if (response.HasError() == true)
			return;

		// 4. handle response
		// nothing to do here
	});
}


// back-end log callback
void BackendLogCallback::Log(const char* text, Core::ELogLevel logLevel)
{
	// only log warnings, errors and fatal errors
	if ((logLevel & LOGLEVEL_CRITICAL) == false)
		return;

	ForceLog(text, logLevel);
}