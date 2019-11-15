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

#ifndef __NEUROMORE_OSCMESSAGEROUTER_H
#define __NEUROMORE_OSCMESSAGEROUTER_H

// include required headers
#include "../Config.h"
#include "../Core/Mutex.h"
#include "../Core/FpsCounter.h"
#include "../Device.h"
#include "OscPacketParser.h"
#include "OscReceiver.h"
#include "OscMessageParser.h"
#include "OscMessageQueue.h"
#include "OscPacketPool.h"


// the osc message router
class ENGINE_API OscMessageRouter
{
	public:
		// constructor & destructor
		OscMessageRouter();
		virtual ~OscMessageRouter();
		
		// INCOMING Packets
		// route the messages inside OSC packets from Network -> OscReceiver

		void ProcessData();

		// route all messages inside the given packet to the corresponding receiver message queues
		void RouteMessages(OscPacketParser* packet);

		// each registered receiver is a possible candidate for receiving osc messages
		void RegisterReceiver(OscReceiver* receiver);
		void UnregisterReceiver(OscReceiver* receiver);
		void UnregisterAllReceivers();
		
		// catch all receiver receives all unroutable messages
		void RegisterCatchAllReceiver(OscReceiver* receiver);

		OscMessageQueue* GetMessageQueue(OscReceiver* receiver);

		// statistics
		uint32 GetNumRegisteredReceivers() const				{ return mReceiverLinkObjects.Size(); }
		uint32 GetNumMessagesReceived() const					{ return mNumMessagesRoutedTotal; }
		uint32 GetNumMessageUnroutable() const					{ return mNumMessagesInvalidAddress; }


		// OUTGOING packets
		// routing packets from anywhere in studio -> Network

		// a Simple queue for outgoing packets because OscServer is not accessible from Engine
		void QueueOutputPacket(OscPacket* packet)				{ mOutputPacketQueue.Add(packet); }
		Core::Array<OscPacket*>& GetOutputPacketQueue()			{ return mOutputPacketQueue; }
		
		OscPacket* GetOscPacketFromPool()						{ return mPacketPool.AcquirePacket(); }
		void ScrubPacketPool();
		uint32 GetNumPooledPacketsFree() const					{ return mPacketPool.GetNumFreePackets(); }
		uint32 GetNumPooledPacketsUsed() const					{ return mPacketPool.GetNumUsedPackets(); }

		// performance statistics
		const Core::FpsCounter& GetFpsCounter() const			{ return mFpsCounter; }
		
		OscMessageQueue* FindMessageQueueByOscAddress(const char* oscPath);
	private:
		struct ReceiverLinkObject
		{
			OscMessageQueue		mQueue;
			OscReceiver*		mReceiver;
		};

		// internal routing helper functions
		uint32 FindLinkObjectIndex(OscReceiver* receiver) const;
		void RouteMessage(OscMessageParser* message);

		Core::Array<ReceiverLinkObject*>	mReceiverLinkObjects;
		Core::Mutex							mLock;

		ReceiverLinkObject*					mCatchAllReceiver;

		// statistics
		uint32								mNumMessagesRoutedTotal;
		uint32								mNumMessagesInvalidAddress;

		// packet pool for outgoing packets
		OscPacketPool						mPacketPool;
		Core::Array<OscPacket*>				mOutputPacketQueue;
		
		// FPS counter
		Core::FpsCounter					mFpsCounter;
};


#endif
