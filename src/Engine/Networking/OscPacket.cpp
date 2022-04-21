/****************************************************************************
**
** Copyright 2019 neuromore co
** Contact: https://neuromore.com/contact
**
** Commercial License Usage
** Licensees holding valid commercial neuromore licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and neuromore. For licensing terms
** and conditions see https://neuromore.com/licensing. For further
** information use the contact form at https://neuromore.com/contact.
**
** neuromore Public License Usage
** Alternatively, this file may be used under the terms of the neuromore
** Public License version 1 as published by neuromore co with exceptions as 
** appearing in the file neuromore-class-exception.md included in the 
** packaging of this file. Please review the following information to 
** ensure the neuromore Public License requirements will be met: 
** https://neuromore.com/npl
**
****************************************************************************/

// include precompiled header
#include <Engine/Precompiled.h>

// include required headers
#include "OscPacket.h"
#include "../Core/LogManager.h"


using namespace Core;

// constructor
OscPacket::OscPacket(uint32 numBytes) : OscPacketParser::OutStream()
{
	// packet data 
	mData = (char*)Allocate(numBytes);
	mMaxSize = numBytes;

	// also use packet data as write buffer
	OutStream::Init(mData, mMaxSize);

	mState = EMPTY;
}


// destructor
OscPacket::~OscPacket()
{
	Free(mData);
}


void OscPacket::Clear()
{
	// packet must not be cleared if a write is in progress
	CORE_ASSERT(mState != WRITE_BEGIN);
	
	// reset osc outstream
	OscPacketParser::OutStream::Clear();

	// reset osc parser
	mOscParser.Clear();
	
	mState = EMPTY;
}

// copy data before reading
void OscPacket::Read(const char* packet, uint32 size)
{
	CORE_ASSERT(mState == EMPTY);
	CORE_ASSERT(size <= mMaxSize);
	
	// copy data into packet buffer
	MemCopy(mData, packet, size);

	Read(size);
}


// read from internal array
void OscPacket::Read(uint32 size)
{
	CORE_ASSERT(mState == EMPTY);
	CORE_ASSERT(size <= mMaxSize);

	mOscParser.ParsePacket(mData, size);
}

void OscPacket::BeginWrite()
{
	CORE_ASSERT(mState == EMPTY);

	BeginPacket();
	
	mState = WRITE_BEGIN;
}


void OscPacket::EndWrite()
{
	CORE_ASSERT(mState == WRITE_BEGIN);

	EndPacket();

	Write();

	mState = WRITE_END;
}


void OscPacket::SetIsReady(bool isReady)
{
	const uint32 numMessages = GetNumMessages();
	for (uint32 i = 0; i < numMessages; ++i)
		GetMessage(i)->mIsReady = isReady;
}
