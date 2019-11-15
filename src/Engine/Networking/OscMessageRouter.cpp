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

// include required headers
#include "OscMessageRouter.h"
#include "../Core/LogManager.h"


// constructor
OscMessageRouter::OscMessageRouter()
{
	mNumMessagesRoutedTotal		= 0;
	mNumMessagesInvalidAddress	= 0;
	mCatchAllReceiver			= NULL;

	mReceiverLinkObjects.Resize(0);

	// initialize the osc packet pool
	mPacketPool.Resize(1024);
}


// destructor
OscMessageRouter::~OscMessageRouter()
{
	UnregisterAllReceivers();
}
 

void OscMessageRouter::ProcessData()
{
	mFpsCounter.BeginTiming();

	// get the number of receiving objects and iterate through them
	const uint32 numReceivers = mReceiverLinkObjects.Size();
	for (uint32 i=0; i<numReceivers; ++i)
	{
		ReceiverLinkObject* linkObject = mReceiverLinkObjects[i];

		// process the data for the given receiver
		linkObject->mReceiver->ProcessData( &linkObject->mQueue );
	}

	// also call the ProcessData method of the catch all receiver (if any)
	if (mCatchAllReceiver != NULL)
		mCatchAllReceiver->mReceiver->ProcessData( &mCatchAllReceiver->mQueue );

	mFpsCounter.StopTiming();
}


// actual routing
void OscMessageRouter::RouteMessages(OscPacketParser* packet)
{
	mLock.Lock();

	// get the number of messages inside the packet and return directly if there are none
	const uint32 numMessages = packet->GetNumMessages();
	if (numMessages == 0)
		return;

	// iterate through the messages and route them to the correct message queues
	for (uint32 i=0; i<numMessages; ++i)
		RouteMessage( packet->GetMessage(i) );

	mLock.Unlock();
}


// route the given message
void OscMessageRouter::RouteMessage(OscMessageParser* message)
{
	// 1. find the correct queue based on the osc path
	OscMessageQueue* queue = FindMessageQueueByOscAddress( message->GetAddress() );
	if (queue == NULL && mCatchAllReceiver != NULL)
		queue = &mCatchAllReceiver->mQueue;
		
	// unknown address
	if (queue == NULL)
	{
		// if the message is unroutable, mark the message as processed
		message->mIsReady = true;
		mNumMessagesInvalidAddress++;
		return;
	}
	
	// processable message
	message->mIsReady = false;

	// 2. put the message into the queue
	queue->Push( message );

	mNumMessagesRoutedTotal++;
}


// get the queue used by a receiver
OscMessageQueue* OscMessageRouter::GetMessageQueue(OscReceiver* receiver)
{
	// get the number of receiving objects and iterate through them
	const uint32 numReceivers = mReceiverLinkObjects.Size();
	for (uint32 i=0; i<numReceivers; ++i)
	{
		ReceiverLinkObject* linkobject = mReceiverLinkObjects[i];
		if (linkobject->mReceiver == receiver)
			return &linkobject->mQueue;
	}

	// also check catch all receiver
	if (mCatchAllReceiver->mReceiver == receiver)
		return &mCatchAllReceiver->mQueue;
	
	return NULL;
}


// find the correct receiver message queue based on the osc prefix
OscMessageQueue* OscMessageRouter::FindMessageQueueByOscAddress(const char* address)
{
	// get the number of receiving objects and iterate through them
	const uint32 numReceivers = mReceiverLinkObjects.Size();
	for (uint32 i=0; i<numReceivers; ++i)
	{
		ReceiverLinkObject* linkobject = mReceiverLinkObjects[i];
		OscReceiver* receiver = linkobject->mReceiver;
		
		// is this message addressed to this receiver?
		const bool match = OscMessageParser::MatchAddress(address, receiver->GetOscAddress());

		if (match == true)
			return &mReceiverLinkObjects[i]->mQueue;
	}

	// no queue found
	return NULL;
}


// register a new receiver
void OscMessageRouter::RegisterReceiver(OscReceiver* receiver)
{
	mLock.Lock();

	// create the receiver link object
	ReceiverLinkObject* linkObject = new ReceiverLinkObject();
	linkObject->mReceiver = receiver;

	// add the link object to the managed array
	mReceiverLinkObjects.Add( linkObject );

	mLock.Unlock();
}


// register catch-all receiver that receives all unroutable messages
void OscMessageRouter::RegisterCatchAllReceiver(OscReceiver* receiver)
{
	mLock.Lock();

	// create the catch all receiver link object
	mCatchAllReceiver = new ReceiverLinkObject();
	mCatchAllReceiver->mReceiver = receiver;

	mLock.Unlock();
}

// unregister receiver
void OscMessageRouter::UnregisterReceiver(OscReceiver* receiver)
{
	mLock.Lock();

	ReceiverLinkObject* linkObject = NULL;

	// find the receiver link object and return in case we didn't find it
	const uint32 linkObjectIndex = FindLinkObjectIndex(receiver);
	if (linkObjectIndex == CORE_INVALIDINDEX32)
	{
		// check if this is the catch all receiver
		if (mCatchAllReceiver != NULL && mCatchAllReceiver->mReceiver == receiver)
		{
			// delete catch all receiver
			delete mCatchAllReceiver;
			mCatchAllReceiver = NULL;
		}
		else
		{
			mLock.Unlock();
			return;
		}
	}
	else
	{
		// remove the link object pointer and destruct the object
		linkObject = mReceiverLinkObjects[linkObjectIndex];
		mReceiverLinkObjects.Remove(linkObjectIndex);
		delete linkObject;
	}

	mLock.Unlock();
}


// unregister all receivers
void OscMessageRouter::UnregisterAllReceivers()
{
	mLock.Lock();

	// get the number of receiving objects, iterate through and destruct them
	const uint32 numReceivers = mReceiverLinkObjects.Size();
	for (uint32 i=0; i<numReceivers; ++i)
		delete mReceiverLinkObjects[i];

	mReceiverLinkObjects.Clear();

	// unregister catch all receiver
	delete mCatchAllReceiver;
	mCatchAllReceiver = NULL;

	mLock.Unlock();
}


// find receiver link object for a given device
uint32 OscMessageRouter::FindLinkObjectIndex(OscReceiver* receiver) const
{
	// get the number of receiving objects and iterate through them
	const uint32 numReceivers = mReceiverLinkObjects.Size();
	for (uint32 i=0; i<numReceivers; ++i)
	{
		if (mReceiverLinkObjects[i]->mReceiver == receiver)
			return i;
	}

	return CORE_INVALIDINDEX32;
}


// free processed  (if pool usage is greate a certain factor)
void OscMessageRouter::ScrubPacketPool()
{
	const float scrubFactor = 0.1f;
	if (mPacketPool.GetNumUsedPackets() > scrubFactor * mPacketPool.GetNumPackets())
		mPacketPool.ReleaseProcessedPackets();
}
