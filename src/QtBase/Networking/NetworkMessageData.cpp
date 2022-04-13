/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

 // include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "NetworkMessageData.h"
#include <Core/LogManager.h>
#include <QHostAddress>


using namespace Core;

// constructors
NetworkMessageData::NetworkMessageData(uint32 size) : NetworkMessage(new QByteArray())
{
	SetPayloadSize(size);
	SetSequenceNumber(0);
}

// create a message from raw data
NetworkMessageData::NetworkMessageData(QByteArray* data) : NetworkMessage(data)
{
	Read();
}


// destructor
NetworkMessageData::~NetworkMessageData()
{
}


// write information to byte array
void NetworkMessageData::Write()
{
}


// read information from byte array
void NetworkMessageData::Read()
{
	// read primary header
	ReadHeader();

	// read the extended header
	const char* headerData = GetExtendedHeaderPointer();
	const uint32 headerSize = GetExtendedHeaderSize();
	memcpy(&mHeader, headerData, headerSize);
}
