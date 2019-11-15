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

#ifndef __NEUROMORE_OSCPACKET_H
#define __NEUROMORE_OSCPACKET_H

// include required headers
#include "../Config.h"
#include "OscPacketParser.h"
#include "../Core/String.h"


// osc packet data class
class ENGINE_API OscPacket : public OscPacketParser::OutStream
{
	public:
		// constructor & destructor
		OscPacket(uint32 numBytes);
		virtual ~OscPacket();

		// clear the packet data
		void Clear() override;
		
		// READING

		// parse raw osc packet
		void Read(const char* packet, uint32 size);
		void Read(uint32 size);
		// access raw data array (for parsing incoming udp packets)
		char* GetData()											{ return mData; }
		
		// getter for the parsed osc messages
		OscPacketParser* GetOscPacketParser() 					{ return &mOscParser; }
		uint32  GetNumMessages() const							{ return mOscParser.GetNumMessages(); }
		OscMessageParser* GetMessage(uint32 index)				{ return mOscParser.GetMessage(index); }

		// WRITING

		// write osc messages via an outstream (directly writes to the packet buffer)
		void BeginWrite();
		void EndWrite();

		// MISC

		// mark all messages in packet as processed
		void SetIsReady(bool isReady = true);

	private:
		// raw data and total size of the buffer
		char*					mData;
		uint32					mMaxSize;

		OscPacketParser			mOscParser;

		// keep track of the state
		enum EPacketState
		{
			EMPTY,			// nothing written or read yet
			READ,			// osc packet was parsed from raw data, messages can be read
			WRITE_BEGIN,	// started writing osc messages to raw packet (header was already written)
			WRITE_END,		// finished writing osc messages to raw packet
		};
		EPacketState			mState;

		// can be implemented the derived classes for writing/reading values from members etc.
		virtual void Read() {};
		virtual void Write() {};
};


#endif
