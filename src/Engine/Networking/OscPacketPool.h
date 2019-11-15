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

#ifndef __NEUROMORE_OSCPACKETPOOL_H
#define __NEUROMORE_OSCPACKETPOOL_H

// include required headers
#include "../Config.h"
#include "OscPacket.h"
#include "../Core/Mutex.h"


// fixed 1k byte osc packet
class ENGINE_API OscPacket1k: public OscPacket
{
	public:
		OscPacket1k() : OscPacket(1024)		{}
		virtual ~OscPacket1k()				{}
};


// osc packet pooling class
class ENGINE_API OscPacketPool
{
	public:
		// constructor & destructor
		OscPacketPool();
		virtual ~OscPacketPool();

		// remove all packets with processed messages
		void ReleaseProcessedPackets();

		OscPacket1k* AcquirePacket();

		void Resize(uint32 numObjects);
		void Clear();

		uint32 GetNumPackets() const			{ return mPackets.Size(); }
		uint32 GetNumFreePackets() const		{ return mFreePackets.Size(); }
		uint32 GetNumUsedPackets() const		{ return mUsedPackets.Size(); }
		
	private:
		void AddNewObjects(uint32 numObjects);
		void RemoveObjects(uint32 numObjects);

		Core::Array<OscPacket1k*>		mPackets;
		Core::Array<OscPacket1k*>		mFreePackets;
		Core::Array<OscPacket1k*>		mUsedPackets;
		Core::Mutex						mLock;
};


#endif
