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

// include required headers
#include "ActionState.h"
#include "../EngineManager.h"
#include "GraphManager.h"
#include "Classifier.h"
#include "StateTransitionButtonCondition.h"


using namespace Core;

// constructor
ActionState::ActionState(Graph* parentGraph) : State(parentGraph)
{
	mOnEnterActions.SetInternalName("onEnterActions");
	mOnExitActions.SetInternalName("onExitActions");
}


// destructor
ActionState::~ActionState()
{
}


// register the attributes
void ActionState::Init()
{
	// init base class 
	State::Init();

}


// update the state
void ActionState::Update(const Time& elapsed, const Time& delta)
{
	// base-update
	State::Update(elapsed, delta);
}


// called when we fully transitioned into the state
void ActionState::OnActivate()
{
	// execute all on-enter actions
	const uint32 numActions = mOnEnterActions.GetNumActions();
	for (uint32 i = 0; i<numActions; ++i)
	{
		Action* action = mOnEnterActions.GetAction(i);
		action->Execute();
	}
}


// called when we fully left the state and now the new state is active
void ActionState::OnDeactivate()
{
	// execute all on-exit actions
	const uint32 numActions = mOnExitActions.GetNumActions();
	for (uint32 i=0; i<numActions; ++i)
	{
		Action* action = mOnExitActions.GetAction(i);
		action->Execute();
	}

	// TODO optimize me by remembering if the state has a button conditions
	// if there are button conditions then fire the ClearButtons event

	bool hasButtonCondition = false;
	const uint32 numOutTransitions = mParentStateMachine->FindNumOutTransitions(this, true);
	for (uint32 i = 0; i < numOutTransitions && hasButtonCondition == false; ++i)
	{
		StateTransition* transition = mParentStateMachine->FindOutTransition(this, i, false);
		const uint32 numConditions = transition->GetNumConditions();
		for (uint32 j = 0; j < numConditions; ++j)
		{
			StateTransitionCondition* condition = transition->GetCondition(j);
			if (condition->GetType() == StateTransitionButtonCondition::TYPE_ID)
			{
				hasButtonCondition = true;
				break;
			}
		}
	}

	// clear buttons if there were button conditions
	if (hasButtonCondition == true)
		EMIT_EVENT(OnClearButtons());
}


// reset
void ActionState::Reset()
{
	// base-class reset
	State::Reset();

	// on-enter actions
	const uint32 numOnEnterActions = mOnEnterActions.GetNumActions();
	for (uint32 i=0; i<numOnEnterActions; ++i)
		mOnEnterActions.GetAction(i)->Reset();

	// on-exit actions
	const uint32 numOnExitActions = mOnExitActions.GetNumActions();
	for (uint32 i=0; i<numOnExitActions; ++i)
		mOnExitActions.GetAction(i)->Reset();
}


// called when any of the attributes changed
void ActionState::OnAttributesChanged()
{
	// base-class call
	State::OnAttributesChanged();

	// on-enter actions
	const uint32 numOnEnterActions = mOnEnterActions.GetNumActions();
	for (uint32 i=0; i<numOnEnterActions; ++i)
		mOnEnterActions.GetAction(i)->OnAttributesChanged();

	// on-exit actions
	const uint32 numOnExitActions = mOnExitActions.GetNumActions();
	for (uint32 i=0; i<numOnExitActions; ++i)
		mOnExitActions.GetAction(i)->OnAttributesChanged();
}


// does the attribute belong to this action state?
bool ActionState::ContainsAttribute(Attribute* attribute) const 
{
	// attribute belongs to graph object attribute set
	if (GraphObject::ContainsAttribute(attribute) == true)
		return true;

	// on-enter actions
	const uint32 numOnEnterActions = mOnEnterActions.GetNumActions();
	for (uint32 i=0; i<numOnEnterActions; ++i)
		if (mOnEnterActions.GetAction(i)->ContainsAttribute(attribute) == true)
			return true;

	// on-exit actions
	const uint32 numOnExitActions = mOnExitActions.GetNumActions();
	for (uint32 i=0; i<numOnExitActions; ++i)
		if (mOnExitActions.GetAction(i)->ContainsAttribute(attribute) == true)
			return true;

	return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Serialization
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Json::Item ActionState::Save(Json& json, Json::Item& item)
{
	// base class save
	Json::Item stateItem = State::Save( json, item );

	// save the actions
	mOnEnterActions.Save( json, stateItem );
	mOnExitActions.Save( json, stateItem );

	return stateItem;
}


bool ActionState::Load(const Json& json, const Json::Item& item)
{
	// base class load
	State::Load( json, item );

	// load actions
	LoadActionSet( json, item, &mOnEnterActions );
	LoadActionSet( json, item, &mOnExitActions );

	// reset the dirty flag
	SetIsDirty(false);
	
	return true;
}


// load actions
bool ActionState::LoadActionSet(const Json& json, const Json::Item& item, ActionSet* actionSet)
{
	bool result = true;

	// find the actions item
	Json::Item actionsItem = item.Find( actionSet->GetInternalName() );
	if (actionsItem.IsArray() == false)
		return false;

	// get the number of actions and iterate through them
	const uint32 numActions = actionsItem.Size();
	for (uint32 i=0; i<numActions; ++i)
	{
		Json::Item actionItem = actionsItem[i];

		// get the action type
		Json::Item typeIdItem = actionItem.Find("type");
		if (typeIdItem.IsString() == false)
			continue;

		// create the condition object
		GraphObject* object = GetGraphObjectFactory()->CreateObjectByTypeUuid( mParentGraph, typeIdItem.GetString() );
		
		if (object == NULL || object->GetBaseType() != Action::BASE_TYPE)
			continue;

		// down-cast the object to an action
		Action* action = static_cast<Action*>(object);

		// read the attributes
		if (action->Read(json, actionItem, true) == false)
		{
			delete action;
			result = false;
			continue;
		}

		// add the action
		actionSet->Add(action);
	}

	return result;
}
