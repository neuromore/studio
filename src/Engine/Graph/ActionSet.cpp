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

// include precompiled header
#include <Engine/Precompiled.h>

// include the required headers
#include "ActionSet.h"
#include "../Core/EventManager.h"


using namespace Core;

// constructor
ActionSet::ActionSet()
{
}


// constructor
ActionSet::ActionSet(const char* internalName)
{
	mInternalName = internalName;
}


// destructor
ActionSet::~ActionSet()
{
	Clear();
}


// find the given action by pointer
uint32 ActionSet::FindIndexByPointer(Action* action)
{
	const uint32 numActions = mActions.Size();
	for (uint32 i=0; i<numActions; ++i)
	{
		if (mActions[i] == action)
			return i;
	}

	return CORE_INVALIDINDEX32;
}


// remove the given action by pointer
void ActionSet::RemoveByPointer(Action* action)
{
	uint32 index = FindIndexByPointer(action);
	if (index == CORE_INVALIDINDEX32)
		return;

	RemoveByIndex(index);
}


// remove the given action by index
void ActionSet::RemoveByIndex(uint32 index)
{
	delete mActions[index];
	mActions.Remove(index);
}


// get rid of all actions
void ActionSet::Clear()
{
	const uint32 numActions = mActions.Size();
	for (uint32 i=0; i<numActions; ++i)
		delete mActions[i];

	mActions.Clear();
}


// save all actions to JSON
Json::Item ActionSet::Save(Json& json, Json::Item& item)
{
	// get the number of actions and check if we have to save any, if not skip directly
	const uint32 numActions = mActions.Size();
	if (numActions == 0)
		return json.NullItem();

	// add the actions array item
	Json::Item actionsItem = item.AddArray( mInternalName.AsChar() );

	// iterate through and save the actions
	for (uint32 i=0; i<numActions; ++i)
	{
		Action* action = mActions[i];
		action->Save( json, actionsItem );
	}

	return actionsItem;
}
