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

#ifndef __NEUROMORE_OSCRECEIVER_H
#define __NEUROMORE_OSCRECEIVER_H

// include required headers
#include "../Config.h"
#include "OscMessageQueue.h"


// osc receiver
class ENGINE_API OscReceiver
{
	public:
		OscReceiver() : mOscAddress("")								{}
		OscReceiver(const char* address) : mOscAddress(address)		{}
		virtual ~OscReceiver() {}
		
		inline const char* GetOscAddress() const					{ return mOscAddress.AsChar(); }
		void SetOscAddress(const char* address)						{ mOscAddress = address; }
		
		virtual void ProcessMessage(OscMessageParser* message)		{ /* TODO implement default OSC handler so every device can be adressed by OSC (adress sensors by index or name)*/}

		void ProcessData(OscMessageQueue* messageQueue)
		{
			while (messageQueue->IsEmpty() == false)
			{
				OscMessageParser* message = messageQueue->Pop();
		
				// NOTE: this must not happen right now -> always process messages!
				// was message already processed (e.g. two receivers registered the same path)
				//if (message->mIsReady == false)
				//{
					ProcessMessage(message);
					message->mIsReady = true;
				//}
			}
		}

	// wildcarded address for this receiver (e.g. /muse/0/*)
	Core::String		mOscAddress;	
};

#endif
