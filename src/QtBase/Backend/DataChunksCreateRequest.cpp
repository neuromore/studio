/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "DataChunksCreateRequest.h"


using namespace Core;

// constructor
DataChunksCreateRequest::DataChunksCreateRequest(const char* token, const char* userId, const char* supervisorId, const char* debitorId, const char* classifierId, int classifierRevision, const char* stateMachineId, int stateMachineRevision, const char* experienceId, int experienceRevision, const Time& startTime, const Time& stopTime, bool isValid)
{
	// display text
	SetDisplayText("Creating data chunk ...");

	// url
	SetUrl( "datachunks/create" );

	// request method
	SetRequestMethod( REQUESTMETHOD_POST );

	// url parameters
	AddUrlParameter( "token", token );
	AddBoolUrlParameter( "valid", isValid );


	// body
	Json::Item rootItem = GetBodyJsonRoot();

	rootItem.AddString( "userId", userId);
	rootItem.AddString( "supervisorId", supervisorId );
	rootItem.AddString( "debitorId", debitorId );

	rootItem.AddString( "classifierId", classifierId );
	rootItem.AddInt( "classifierRevision", classifierRevision );

	if (String(stateMachineId).IsEmpty() == false)
	{
		rootItem.AddString("stateMachineId", stateMachineId);
		rootItem.AddInt("stateMachineRevision", stateMachineRevision);
	}

	if (String(experienceId).IsEmpty() == false)
	{
		rootItem.AddString("experienceId", experienceId);
		rootItem.AddInt("experienceRevision", experienceRevision);
	}

	// start and stop time
	rootItem.AddString( "startDateTime", startTime.AsUtcString() );
	rootItem.AddString( "stopDateTime", stopTime.AsUtcString() );
}