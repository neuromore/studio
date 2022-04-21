/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

 // include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "NetworkMessage.h"
#include <Core/LogManager.h>


using namespace Core;

NetworkMessage::NetworkMessage(QByteArray* data)
{
	CORE_ASSERT(data != NULL);
	mData = data;
}

NetworkMessage::~NetworkMessage()
{
	delete mData;
}


// total size of the message header (including extended header)
uint32 NetworkMessage::GetHeaderSize() const
{
	return sizeof(Header) + GetExtendedHeaderSize();
}


// set up header
void NetworkMessage::FinalizeHeader()
{ 
	mHeader.mMessageLength = (uint32)mData->length(); 
	mHeader.mMessageType = GetType(); 
}


// write primary and extended header to data array
void NetworkMessage::WriteHeader()
{
	// finalize header first
	FinalizeHeader();

	char* data = mData->data();

	// write primary header
	const uint32 headerSize = sizeof(Header);
	memcpy( data, &mHeader, headerSize);
	data += headerSize;

	// write extended header (if any)
	const uint32 extendedHeaderSize = GetExtendedHeaderSize();
	if (extendedHeaderSize > 0)
	{
		const char* extendedHeader = GetExtendedHeader();
		memcpy( data, extendedHeader, extendedHeaderSize);
		data += extendedHeaderSize;
	}
}


// read primary header from data array
void NetworkMessage::ReadHeader()
{
	const char* data = mData->data();

	// read primary header
	memcpy( &mHeader, data, sizeof(Header));

}
