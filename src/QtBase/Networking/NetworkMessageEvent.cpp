/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "NetworkMessageEvent.h"
#include <Core/LogManager.h>


using namespace Core;

// constructors

NetworkMessageEvent::NetworkMessageEvent(Core::String string) : NetworkMessage(new QByteArray())
{
	mMessage = string; 
	Write();
}

// create a message from raw data
NetworkMessageEvent::NetworkMessageEvent(QByteArray* data) :  NetworkMessage(data)
{
	Read();
}


// destructor
NetworkMessageEvent::~NetworkMessageEvent()
{
}

// write information to byte array
void NetworkMessageEvent::Write()
{
	// calculate the size of the message and resize the byte array
	SetPayloadSize(mMessage.GetLength());

	// get access to the byte array data and copy the memory over to it
	char* data = GetPayload();

	// copy the message
	memcpy( data, mMessage.AsChar(), mMessage.GetLength() );

	// write header
	WriteHeader();
}


// read information from byte array
void NetworkMessageEvent::Read()
{
	// parse the message header
	ReadHeader();

	// resize message string
	mMessage.Resize(GetPayloadSize());

	// get access to the byte array data
	char* data = GetPayload();

	// copy the message
	memcpy(mMessage.AsChar(), data, mMessage.GetLength());
}
