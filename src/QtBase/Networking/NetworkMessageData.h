/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_NETWORKMESSAGEDATA_H
#define __NEUROMORE_NETWORKMESSAGEDATA_H

// include required headers
#include "../QtBaseConfig.h"
#include "NetworkMessage.h"
#include <QByteArray>
#include <QHostAddress>


// simple message for transmitting chunks of raw data over UDP
class QTBASE_API NetworkMessageData: public NetworkMessage
{
	public:
		enum { TYPE_ID = NEUROCORE_MESSAGE_TYPE_DATA };

		// constructor & destructor
		NetworkMessageData(uint32 data);
		NetworkMessageData(QByteArray* data);					// construct message from raw data
		virtual ~NetworkMessageData();

        uint16 GetType() const override                         { return TYPE_ID; }
		const char*  GetTypeString() const override				{ return "Client Configuration"; }

		void Read() override;
		void Write() override;

		// extended message header
		struct Header
		{
			uint32 mSequenceNumber;
		};
		
		virtual uint32 GetExtendedHeaderSize() const override	{ return sizeof(Header); }
		virtual const char* GetExtendedHeader() const override	{ return (const char*)&mHeader; }

		// access header fields
		uint32 GetSequenceNumber() const						{ return mHeader.mSequenceNumber; }
		void SetSequenceNumber(uint32 number)					{ mHeader.mSequenceNumber = number; }

	private:
		Header	mHeader;

};


#endif
