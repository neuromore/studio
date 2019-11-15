/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_NETWORKMESSAGECONFIG_H
#define __NEUROMORE_NETWORKMESSAGECONFIG_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/Json.h>
#include "NetworkMessage.h"
#include <QByteArray>
#include <QHostAddress>



class QTBASE_API NetworkMessageConfig: public NetworkMessage
{
	public:
		enum { TYPE_ID = NEUROCORE_MESSAGE_TYPE_CLIENT_CONFIG };

		// constructor & destructor
		NetworkMessageConfig(int clientType, Core::String name);
		NetworkMessageConfig(QByteArray* data);					// construct message from raw data
		virtual ~NetworkMessageConfig();
    
        uint16 GetType() const override                         { return TYPE_ID; }
		const char*  GetTypeString() const override				{ return "Client Configuration"; }

		void Read() override;
		void Write() override;

		const Core::String& GetString() const					{ return mMessage; }
        void SetString(const Core::String& string)              { mMessage = string; Write(); }

        int GetClientType() const                               { return mClientType; }
		const Core::String GetName() const						{ return mName; }

	protected:
		Core::String		mMessage;

		Core::String		mName;
		int					mClientType;
};


#endif
