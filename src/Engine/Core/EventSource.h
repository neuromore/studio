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

#ifndef __CORE_EVENTSOURCE_H
#define __CORE_EVENTSOURCE_H

// include the required headers
#include "Config.h"
//#include "EventManager.h"

	
/**
 * Following macro should be used for emitting events.
 */
#define EMIT_EVENT(FUNCTIONCALL)	\
	if (GetEmitEvents() == true)  	\
		CORE_EVENTMANAGER.FUNCTIONCALL;

namespace Core
{

/**
 * The event source extends a class, so it is able to emit events
 */
class ENGINE_API EventSource
{
	public:
		EventSource() : mEventSystemEmitEvents(true) {}
		virtual ~EventSource() {}

		void SetEmitEvents(bool enabled = true)	{ mEventSystemEmitEvents = enabled; }
		bool GetEmitEvents() const				{ return mEventSystemEmitEvents; }	

	private:

		// enable/disable emitting of events
		bool mEventSystemEmitEvents;
		
};

} // namespace Core


#endif
