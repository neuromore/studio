/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_NETWORKMESSAGEEVENT_H
#define __NEUROMORE_NETWORKMESSAGEEVENT_H

// include required headers
#include "../QtBaseConfig.h"
#include "NetworkMessage.h"
#include <QByteArray>



class QTBASE_API NetworkMessageEvent : public NetworkMessage
{
	public:
		enum { TYPE_ID = NEUROCORE_MESSAGE_TYPE_JSON_EVENT };

		// constructor & destructor
		NetworkMessageEvent(Core::String string);				// message from json event string
		NetworkMessageEvent(QByteArray* data);					// message from raw data
		virtual ~NetworkMessageEvent();

        uint16 GetType() const override                         { return TYPE_ID; }
		const char*  GetTypeString() const override				{ return "JSON Event"; }

		void Read() override;
		void Write() override;

		const Core::String& GetString() const					{ return mMessage; }
        void SetString(const Core::String& string)              { mMessage = string; Write(); }

	protected:
		Core::String mMessage;
		
};


#endif
