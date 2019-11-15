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

#ifndef __NEUROMORE_OSCMESSAGEQUEUE_H
#define __NEUROMORE_OSCMESSAGEQUEUE_H

// include required headers
#include "../Config.h"
#include "../Core/Array.h"
#include "../Core/Mutex.h"
#include "OscMessageParser.h"


// the new OscMessageQueue class (OSC based)
class ENGINE_API OscMessageQueue
{
	public:
		// constructor & destructor
		OscMessageQueue();
		virtual ~OscMessageQueue();

		void Push(OscMessageParser* message);
		OscMessageParser* Pop();
		inline bool IsEmpty() const								{ return mQueue.IsEmpty(); }
		void ClearQueue();

	private:
		Core::Array<OscMessageParser*>	mQueue;
		Core::Mutex						mLock;
};


#endif
