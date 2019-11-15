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

// include the required headers
#include "EventManager.h"
#include "EventHandler.h"
#include "LogManager.h"
#include "EventLogger.h"


namespace Core
{

EventManager::EventManager()
{
	mEventLogger = NULL;

#ifdef CORE_DEBUG
	// enable event logging
	mEventLogger = new EventLogger();
	AddEventHandler(mEventLogger);
#endif
}


EventManager::~EventManager()
{
	mEventHandlers.Clear();
	delete mEventLogger;
}


void EventManager::AddEventHandler(EventHandler* eventHandler)
{
	CORE_ASSERT(FindEventHandlerIndex(eventHandler) == CORE_INVALIDINDEX32);

	mEventHandlers.Add(eventHandler);
}


uint32 EventManager::FindEventHandlerIndex(EventHandler* eventHandler) const
{
	const uint32 numEventHandlers = mEventHandlers.Size();
	for (uint32 i=0; i<numEventHandlers; ++i)
	{
		if (eventHandler == mEventHandlers[i])
			return i;
	}

	return CORE_INVALIDINDEX32;
}


bool EventManager::RemoveEventHandler(EventHandler* eventHandler, bool delFromMem)
{
	const uint32 index = FindEventHandlerIndex(eventHandler);
	if (index == CORE_INVALIDINDEX32)
		return false;

	RemoveEventHandler(index, delFromMem);
	return true;
}


void EventManager::RemoveEventHandler(uint32 index, bool delFromMem)
{
	if (delFromMem == true)
		delete mEventHandlers[index];

	mEventHandlers.Remove(index);
}

} // namespace Core
