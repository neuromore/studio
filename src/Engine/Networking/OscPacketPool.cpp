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
#include "OscPacketPool.h"
#include "../Core/LogManager.h"


using namespace Core;

OscPacketPool::OscPacketPool()
{
}


OscPacketPool::~OscPacketPool()
{
	Clear();
}


void OscPacketPool::ReleaseProcessedPackets()
{
	// in case there are no 
	if (mUsedPackets.Size() == 0)
		return;

	// iterate over packets in reverse order and check if all messages are processed; if so, put the packet back into the pool
	for (int32 p = mUsedPackets.Size() - 1; p >= 0; p--)
	{
		bool finished = true;

		// get the packet
		OscPacket1k* packet = mUsedPackets[p];

		// get the number of messages and iterate through them
		const uint32 numMessages = packet->GetNumMessages();
		for (uint32 m = 0; m<numMessages; ++m)
		{
			OscMessageParser* message = packet->GetMessage(m);
			if (message->mIsReady == false)
			{
				finished = false;
				break;
			}
		}

		// in case all messages of the packet are processed, release the packet
		if (finished == true)
		{
			// put packet back into pool 

			mLock.Lock();
				bool removed = mUsedPackets.RemoveByValue( packet );
				CORE_ASSERT( removed == true );

				mFreePackets.Add( packet );
			mLock.Unlock();
		}
	}
}


OscPacket1k* OscPacketPool::AcquirePacket()
{
	mLock.Lock();

	const uint32 numFree = mFreePackets.Size();
	if (numFree == 0)
	{
		AddNewObjects( 128 );
		CORE_ASSERT( mFreePackets.Size() > 0 );
	}
	OscPacket1k* result = mFreePackets[mFreePackets.Size()-1];
	mFreePackets.Remove(mFreePackets.Size()-1);
	mUsedPackets.Add(result);

	mLock.Unlock();
	return result;
}


void OscPacketPool::AddNewObjects(uint32 numObjects)
{
	const uint32 oldLength = mPackets.Size();
	
	mPackets.Resize( oldLength + numObjects );
	mFreePackets.Reserve( mPackets.Size() );
	mUsedPackets.Reserve( mPackets.Size() );

	for (uint32 i=0; i<numObjects; ++i)
	{
		const uint32 newObjectIndex = oldLength + i;
		OscPacket1k* newObject = new OscPacket1k();
		mPackets[newObjectIndex] = newObject;
		mFreePackets.Add( newObject );
	}
}


void OscPacketPool::Resize(uint32 numObjects)
{
	mLock.Lock();

	const uint32 oldSize = mPackets.Size();
	if (numObjects == oldSize)
	{
		mLock.Unlock();
		return;
	}

	if (oldSize < numObjects)
		AddNewObjects( numObjects - oldSize );
	else
		RemoveObjects( oldSize - numObjects );

	mLock.Unlock();
}


void OscPacketPool::RemoveObjects(uint32 numObjects)
{
	mLock.Lock();

	CORE_ASSERT(mFreePackets.Size() >= numObjects);
	for (uint32 i=0; i<numObjects; ++i)
	{
		const uint32 lastIndex = mFreePackets.Size() - 1;
		OscPacket1k* object = mFreePackets[lastIndex];
		mPackets.RemoveByValue(object);
		mFreePackets.Remove( mFreePackets.Size() - 1 );

		delete object;
	}

	mLock.Unlock();
}


void OscPacketPool::Clear()
{
	const uint32 numObjects = mPackets.Size();
	for (uint32 i=0; i<numObjects; ++i)
		delete mPackets[i];
	mPackets.Clear();

	mFreePackets.Clear();
	mUsedPackets.Clear();
}
