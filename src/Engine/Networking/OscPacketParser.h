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

#ifndef __NEUROMORE_OSCPACKETPARSER_H
#define __NEUROMORE_OSCPACKETPARSER_H

// include required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../Core/String.h"
#include "../Core/StringIterator.h"
#include "../Core/Array.h"
#include <oscpack/OscOutboundPacketStream.h>
#include <oscpack/OscReceivedElements.h>
#include <oscpack/OscTypes.h>
#include <oscpack/OscReceivedElements.h>
#include "OscMessageParser.h"

	
/**
 * The OSC parser wrapper class.
 */
class ENGINE_API OscPacketParser
{
	public:

		// special tokens for constructing the message
		struct BeginMessage
		{
			BeginMessage(const char* path) : mPath(path) {}
			const char* mPath;
		};
		struct EndMessage {};
		struct BeginPacket {};
		struct EndPacket {};

		// outgoing stream -> construct an osc package
		class OutStream
		{
		  public:

			// create an uninitialized OutStream
			OutStream() 
			{
				mBuffer = NULL;
				mBufferSize = 0;
				mBufferIsLocal = false;
				mOscPackStream = NULL;
				mInitialized = false;
			}

			// create an OutStream with initialized buffer of fixed size
			OutStream(uint32 numBytes) 
			{
				mBufferIsLocal = true;
				Init( new char[numBytes], numBytes );
			}

			virtual ~OutStream() 
			{
				if (mInitialized && mBufferIsLocal) 
					delete mOscPackStream;
			}

			void Init (char* buffer, uint32 buffersize)
			{
				mBuffer = buffer;
				mBufferSize = buffersize;
				mOscPackStream = new osc::OutboundPacketStream(mBuffer, buffersize);
				mInitialized = true;
			}
			
			virtual void Clear()		{ mOscPackStream->Clear(); }

			const char* GetData()		{ return mBuffer; }
			uint32		GetSize()		{ return (uint32)mOscPackStream->Size(); }
			
			// special message tokens
			OutStream& operator<<(const BeginMessage& token)	{ BeginMessage(token.mPath);  return *this; }
			OutStream& operator<<(const EndMessage& token)		{ EndMessage();	 return *this; }
			OutStream& operator<<(const BeginPacket& token)		{ BeginPacket(); return *this; }
			OutStream& operator<<(const EndPacket& token)		{ EndPacket();   return *this; }
			// values
			OutStream& operator<<(bool value)					{ WriteValue(value); return *this; }
			OutStream& operator<<(char value)					{ WriteValue(value); return *this; }
			OutStream& operator<<(int32 value)					{ WriteValue(value); return *this; }
			OutStream& operator<<(int64 value)					{ WriteValue(value); return *this; }
			OutStream& operator<<(double value)					{ WriteValue(value); return *this; }
			OutStream& operator<<(float value)					{ WriteValue(value); return *this; }
			OutStream& operator<<(const char* value)			{ WriteValue(value); return *this; }

			// special message tokens
			inline void BeginMessage (const char* path)			{ CORE_ASSERT(mInitialized); (*mOscPackStream) << osc::BeginMessage(path);		}
			inline void EndMessage()							{ CORE_ASSERT(mInitialized); (*mOscPackStream) << osc::MessageTerminator();	}
			inline void BeginPacket()							{ CORE_ASSERT(mInitialized); (*mOscPackStream) << osc::BeginBundleImmediate;	}
			inline void EndPacket()								{ CORE_ASSERT(mInitialized); (*mOscPackStream) << osc::EndBundle;				}
			// values
			inline void WriteValue(bool value)					{ CORE_ASSERT(mInitialized); (*mOscPackStream) << value; }
			inline void WriteValue(char value)					{ CORE_ASSERT(mInitialized); (*mOscPackStream) << value; }	
			inline void WriteValue(int32 value)					{ CORE_ASSERT(mInitialized); (*mOscPackStream) << (osc::int32)value; }
			inline void WriteValue(int64 value)					{ CORE_ASSERT(mInitialized); (*mOscPackStream) << (osc::int64)value; }
			inline void WriteValue(double value)				{ CORE_ASSERT(mInitialized); (*mOscPackStream) << value; }
			inline void WriteValue(float value)					{ CORE_ASSERT(mInitialized); (*mOscPackStream) << value; }
			inline void WriteValue(const char* value)			{ CORE_ASSERT(mInitialized); (*mOscPackStream) << value; }

			private:
				
				char*		mBuffer;
				uint32		mBufferSize; 
				bool		mBufferIsLocal;			// true if buffer is owned by this class

				osc::OutboundPacketStream* mOscPackStream;

				bool		mInitialized;
				
		};

		// constructor and destructor
		OscPacketParser()										{}	
		virtual ~OscPacketParser()								{ Clear(); }

		// clean and reserve
		void Clear();											// clear any stored messages 

		// main parse functions for processing incoming osc packages
		void ParsePacket(const char* data, uint32 size);		// provide the string with OscParser data

		// get and add messages
		inline uint32 GetNumMessages() const									{ return mMessages.Size(); }

#undef GetMessage
		OscMessageParser* GetMessage(uint32 index)								{ return &(mMessages[index]); }
//		void AddMessage(OscMessage& message)									{ mMessages.Add(message); }
		
	private:
		// the osc messages
		Core::Array<OscMessageParser> mMessages;
		
		// recursive parsing of osc packets
		void ProcessBundle( const osc::ReceivedBundle& b );
		void ProcessMessage( const osc::ReceivedMessage& m);

};


#endif
