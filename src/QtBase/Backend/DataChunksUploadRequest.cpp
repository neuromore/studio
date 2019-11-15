/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "DataChunksUploadRequest.h"


using namespace Core;

// constructor
DataChunksUploadRequest::DataChunksUploadRequest(const char* token, const char* userId, const char* dataChunkId, const char* nodeId, const char* channelName, int type, int offset, double sampleRate, double visualMin, double visualMax)
{
	// display text
	SetDisplayText("Uploading data chunk ...");

	// url
	SetUrl( "datachunks/upload" );

	// request method
	SetRequestMethod( REQUESTMETHOD_POST );

	// url parameters
	AddUrlParameter( "token", token );
	AddUrlParameter( "userId", userId );
	AddUrlParameter( "dataChunkId", dataChunkId );
	AddUrlParameter( "nodeId", nodeId );
	AddUrlParameter( "channelName", channelName );
	AddIntUrlParameter( "type", type );
	AddIntUrlParameter( "offset", offset );
	AddDoubleUrlParameter( "sampleRate", sampleRate );
	AddDoubleUrlParameter( "visualMin", visualMin );
	AddDoubleUrlParameter( "visualMax", visualMax );
}