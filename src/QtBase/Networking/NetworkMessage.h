/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_NETWORKMESSAGE_H
#define __NEUROMORE_NETWORKMESSAGE_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/String.h>
#include <QByteArray>


#define NEUROCORE_MESSAGE_TYPE_SERVER_ANOUNCE		21401
#define NEUROCORE_MESSAGE_TYPE_REALTIME_UPDATE		21402
#define NEUROCORE_MESSAGE_TYPE_CLIENT_CONFIG		21403
#define NEUROCORE_MESSAGE_TYPE_JSON_EVENT			21404
#define NEUROCORE_MESSAGE_TYPE_DATA					21405


class QTBASE_API NetworkMessage
{
	public:
		// constructor & destructor
		NetworkMessage();								
		NetworkMessage(QByteArray* data);						// construct message from raw data
		virtual ~NetworkMessage();

		// accessor for the whole raw byte array and its size
		const char* GetRawData() const							{ return mData->data(); }
		uint32 GetSize() const									{ return mData->size(); }

		// access and resize the payload (begins after the last header)
		char* GetPayload() const								{ return mData->data() + GetHeaderSize(); }
		void SetPayloadSize(uint32 bytes)						{ mData->resize(bytes + GetHeaderSize()); }
		uint32 GetPayloadSize() const							{ return GetSize() - GetHeaderSize(); }

		virtual void		Read() = 0;							// raw data -> parsed data
		virtual void		Write() = 0;						// parsed data -> raw data

		virtual uint16 GetType() const = 0;
		virtual const char* GetTypeString() const = 0;

		// primary header
		struct Header
		{
			uint32 mMessageLength;
			uint32 mMessageType;
		};

		uint32 GetHeaderSize() const;
		const Header& GetHeader() const							{ return mHeader; }

		// get the pointer for parsing the extended header
		const char* GetExtendedHeaderPointer() const			{ return GetRawData() + sizeof(Header); }

		// header extension provided by derived classes)
		virtual const char* GetExtendedHeader() const			{ return 0; }
		virtual uint32 GetExtendedHeaderSize() const			{ return 0; }
		
		// set header fields 
		void FinalizeHeader();

		// write header to and read header from byte array
		void WriteHeader();
		void ReadHeader();
		
	private:
		Header		mHeader;
		QByteArray*	mData;
};


#endif
