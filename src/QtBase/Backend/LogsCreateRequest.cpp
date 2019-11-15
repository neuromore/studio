/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "LogsCreateRequest.h"
#include <QDateTime>


using namespace Core;

// constructor
LogsCreateRequest::LogsCreateRequest(const char* token, const char* text, ELogLevel logLevel) : Request()
{
	// display text
	SetDisplayText("Logging ...");


	// url
	SetUrl( "logs/create" );

	// request method
	SetRequestMethod( REQUESTMETHOD_POST );

	// url parameters
	AddUrlParameter( "token", token );


	// body
	Json::Item rootItem = GetBodyJsonRoot();

	// add the log level parameter
	switch (logLevel)
	{
		case LOGLEVEL_CRITICAL:		{ rootItem.AddString( "logLevel", "ERROR" ); break; }
		case LOGLEVEL_ERROR:		{ rootItem.AddString( "logLevel", "ERROR" ); break; }
		case LOGLEVEL_WARNING:		{ rootItem.AddString( "logLevel", "WARNING" ); break; }
		case LOGLEVEL_INFO:			{ rootItem.AddString( "logLevel", "INFO" ); break; }
		case LOGLEVEL_DETAILEDINFO:	{ rootItem.AddString( "logLevel", "DETAILEDINFO" ); break; }
		case LOGLEVEL_DEBUG:		{ rootItem.AddString( "logLevel", "DEBUG" ); break; }
		default:					{ rootItem.AddString( "logLevel", "NONE" ); }
	}

	// add the time parameter
	QDateTime	dateTime			= QDateTime::currentDateTimeUtc();
	QString		endDateTimeString	= dateTime.toString("yyyy-MM-ddThh:mm:ss.zzz+00:00");

	rootItem.AddString( "logTime", FromQtString(endDateTimeString).AsChar() );
	
	// add the actual log message parameter
	rootItem.AddString( "message", text );
}