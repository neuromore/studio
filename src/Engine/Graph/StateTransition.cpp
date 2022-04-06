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
#include "StateTransition.h"
#include "../EngineManager.h"
#include "Graph.h"
#include "StateMachine.h"
#include "State.h"
#include "StateTransitionCondition.h"
#include "StateTransitionTimeCondition.h"


using namespace Core;

// constructor
StateTransition::StateTransition(StateMachine* stateMachine) : Connection()
{
	mStateMachine			= stateMachine;
	mIsDone					= false;
	mIsActive				= false;
	mProgress			= 0.0;
	mTotalSeconds			= Time(0.0);
	mStartOffsetX			= 0;
	mStartOffsetY			= 0;
	mEndOffsetX				= 0;
	mEndOffsetY				= 0;
}


// destructor
StateTransition::~StateTransition()
{
	RemoveAllConditions(true);
}


// initialize
void StateTransition::Init()
{
	// register attributes

	// is the state disabled?
	AttributeSettings* param = RegisterAttribute("Disabled", "isDisabled", "Is disabled? If yes the transition will not be used by the state machine.", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	param->SetDefaultValue( AttributeBool::Create(false) );

	// the transition priority value
	param = RegisterAttribute("Priority", "priority", "The priority level of the transition.", ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	param->SetDefaultValue( AttributeInt32::Create(0) );
	param->SetMinValue( AttributeInt32::Create(0) );
	param->SetMaxValue( AttributeInt32::Create(INT_MAX) );

	// the transition blend time
	const double defaultTransitionTime = 0.25;
	param = RegisterAttribute("Transition Time", "transitionTime", "The transition time, in seconds.", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	param->SetDefaultValue( AttributeFloat::Create(defaultTransitionTime) );
	param->SetMinValue(	AttributeFloat::Create(0.0) );
	param->SetMaxValue( AttributeFloat::Create(FLT_MAX) );

	// pre-alloc all conditions
	const uint32 numConditions = mConditions.Size();
	for (uint32 i=0; i<numConditions; ++i)
	{
		StateTransitionCondition* condition = mConditions[i];
		condition->Init();
	}
}


void StateTransition::Reset()
{
	mIsDone			= false;
	mIsActive		= false;
	mProgress	= 0.0;
	mTotalSeconds	= Time(0.0);
	ResetConditions();
}


// update the transition
void StateTransition::Update(const Time& elapsed, const Time& delta)
{
	// get the blend time for the transition
	double blendTime = GetBlendTime();

	// update timers
	mTotalSeconds += delta;
	if (mTotalSeconds > blendTime)
	{
		mTotalSeconds = blendTime;
		mIsDone = true;
	}
	else
		mIsDone = false;

	// calculate the blend weight
	if (blendTime > Math::epsilon)
		mProgress = mTotalSeconds.InSeconds() / blendTime;
	else
		mProgress = 1.0;
}


// get source state
State* StateTransition::GetSourceState() const
{
	if (mSourceNode != NULL && mSourceNode->GetBaseType() == State::BASE_TYPE)
		return static_cast<State*>(mSourceNode);

	return NULL;
}


// get target state
State* StateTransition::GetTargetState() const
{
	if (mTargetNode != NULL && mTargetNode->GetBaseType() == State::BASE_TYPE)
		return static_cast<State*>(mTargetNode);

	return NULL;
}


// update data
void StateTransition::OnAttributesChanged()
{
	// get the number of conditions
	const uint32 numConditions = mConditions.Size();
	for (uint32 i=0; i<numConditions; ++i)
		mConditions[i]->OnAttributesChanged();
}


// does the attribute belong to this transition?
bool StateTransition::ContainsAttribute(Attribute* attribute) const
{
	// attribute belongs to graph object attribute set
	if (GraphObject::ContainsAttribute(attribute) == true)
		return true;

	// attrib belongs to one of the conditions
	const uint32 numConditions = mConditions.Size();
	for (uint32 i=0; i<numConditions; ++i)
		if (mConditions[i]->ContainsAttribute(attribute) == true)
			return true;

	return false;
}



// check if all conditions are tested positive
bool StateTransition::IsReady() const
{
	// get the number of conditions
	const uint32 numConditions = mConditions.Size();

	// state is ready by default if there are no conditions
	if (numConditions == 0)
		return true;

	// if one of the conditions is false, we evaluate to false
	bool isReady = true;

	for (uint32 i=0; i<numConditions; ++i)
	{
		// get the condition, test if this condition is fulfilled and update the previous test result
		StateTransitionCondition* condition = mConditions[i];
		const bool testResult = condition->TestCondition();
		condition->UpdatePreviousTestResult(testResult);

		// return directly in case one condition is not ready yet
		if (testResult == false)
			isReady = false;
	}

	// all conditions where true
	return isReady;
}


// the transition started
void StateTransition::OnStartTransition()
{
	mIsActive		= true;
	mProgress		= 0.0;
	mIsDone			= false;
	mTotalSeconds	= 0.0;
}


// end the transition
void StateTransition::OnEndTransition()
{
	mIsActive		= false;
	mProgress		= 1.0;
	mIsDone			= true;
}



// add the given condition to the transition
void StateTransition::AddCondition(StateTransitionCondition* condition)
{
	if (condition == NULL)
		return;

	// link the condition to the transition
	condition->mTransition = this;
	
	// add the condition
	mConditions.Add(condition);

	// reset the condition
	condition->Reset();
}


// remove all conditions
void StateTransition::RemoveAllConditions(bool delFromMem)
{
	// delete them all from memory
	if (delFromMem)
	{
		const uint32 numConditions = mConditions.Size();
		for (uint32 i=0; i<numConditions; ++i)
			delete mConditions[i];
	}

	// clear the conditions array
	mConditions.Clear();
}


// reset all transition conditions
void StateTransition::ResetConditions()
{
	// iterate through all conditions and reset them
	const uint32 numConditions = mConditions.Size();
	for (uint32 i=0; i<numConditions; ++i)
		mConditions[i]->Reset();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Serialization
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// save the given graph node
Json::Item StateTransition::Save(Json& json, Json::Item& item)
{
	// connection base class save
	Json::Item transitionItem = Connection::Save(json, item);
	if (transitionItem.IsNull() == true)
		return json.NullItem();

	// add additional attributes
	transitionItem.AddInt( "startOffsetX",	mStartOffsetX );
	transitionItem.AddInt( "startOffsetY",	mStartOffsetY );
	transitionItem.AddInt( "endOffsetX",	mEndOffsetX );
	transitionItem.AddInt( "endOffsetY",	mEndOffsetY );

	// save conditions
	if (SaveConditions(json, transitionItem) == false)
		return json.NullItem();

	return transitionItem;
}


// save all conditions
bool StateTransition::SaveConditions(Json& json, Json::Item& item)
{
	// get the number of conditions and check if we have to save any, if not skip directly
	const uint32 numConditions = mConditions.Size();
	if (numConditions == 0)
		return true;

	// add the conditions array item
	Json::Item conditionsItem = item.AddArray("conditions");

	// iterate through and save the conditions
	for (uint32 i=0; i<numConditions; ++i)
		mConditions[i]->Save( json, conditionsItem );

	return true;
}


bool StateTransition::Load(const Json& json, const Json::Item& item)
{
	// get the conditions item
	Json::Item conditionsItem = item.Find("conditions");
	if (conditionsItem.IsArray() == false)
	{
		//LogDetailedInfo("StateTransition::LoadConditions():  State transition does not contain any condition.");
		return true;
	}

	// get the number of conditions and iterate through them
	bool result = true;
	const uint32 numConditions = conditionsItem.Size();
	for (uint32 i=0; i<numConditions; ++i)
	{
		// get the condition item
		Json::Item conditionItem = conditionsItem[i];
		if (conditionItem.IsNull() == true)
			continue;

		// get the condition type
		Json::Item typeIdItem = conditionItem.Find("type");
		if (typeIdItem.IsString() == false)
			continue;

		// create the condition object
		GraphObject* object = GetGraphObjectFactory()->CreateObjectByTypeUuid(mStateMachine, typeIdItem.GetString());
		if (object == NULL || object->GetBaseType() != StateTransitionCondition::BASE_TYPE)
			continue;

		// down-cast the object to a condition
		StateTransitionCondition* condition = static_cast<StateTransitionCondition*>(object);

		// read the attributes
		if (condition->Read(json, conditionItem, true) == false)
		{
			delete condition;
			result = false;
			continue;
		}

		// add the condition
		AddCondition(condition);
	}

	return result;
}
