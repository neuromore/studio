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
#include "StateTransitionTimeCondition.h"
#include "../Core/AttributeSettings.h"
#include "../Core/EventManager.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
StateTransitionTimeCondition::StateTransitionTimeCondition(Graph* graph) : StateTransitionCondition(graph)
{
	mElapsedTime	= Time(0.0);
	mCountDownTime	= Time(0.0);
}


// destructor
StateTransitionTimeCondition::~StateTransitionTimeCondition()
{
}


// register the attributes
void StateTransitionTimeCondition::Init()
{
	// register attributes

	// create the randomization flag checkbox
	AttributeSettings* attribInfo = RegisterAttribute("Use Randomization", "randomization", "When randomization is enabled the count down time will be a random one between the min and max random time.", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attribInfo->SetDefaultValue( AttributeBool::Create(false) );

	// create the count down time value float spinner
	attribInfo = RegisterAttribute("Count Down Time In Seconds", "countDownTime", "The amount of seconds the condition will count down until the condition will trigger.", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attribInfo->SetDefaultValue( AttributeFloat::Create(1.0f) );
	attribInfo->SetMinValue( AttributeFloat::Create(0.0f) );
	attribInfo->SetMaxValue( AttributeFloat::Create(FLT_MAX) );

	// create the min random time value float spinner
	attribInfo = RegisterAttribute("Min Random Time In Seconds", "minRandomTime", "The minimum randomized count down time. This will be only used then randomization is enabled.", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attribInfo->SetDefaultValue( AttributeFloat::Create(0.0f) );
	attribInfo->SetMinValue( AttributeFloat::Create(0.0f) );
	attribInfo->SetMaxValue( AttributeFloat::Create(FLT_MAX) );
	attribInfo->SetVisible( false );

	// create the max random time value float spinner
	attribInfo = RegisterAttribute("Max Random Time In Seconds", "maxRandomTime", "The maximum randomized count down time. This will be only used then randomization is enabled.", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attribInfo->SetDefaultValue( AttributeFloat::Create(1.0f) );
	attribInfo->SetMinValue( AttributeFloat::Create(0.0f) );
	attribInfo->SetMaxValue( AttributeFloat::Create(FLT_MAX) );
	attribInfo->SetVisible( false );
}


// attributes have changed
void StateTransitionTimeCondition::OnAttributesChanged()
{
	StateTransitionCondition::OnAttributesChanged();

	// show/hide the randomization time attributes
	const bool isRandom = GetBoolAttribute( ATTRIB_USERANDOMIZATION );
	GetAttributeSettings(ATTRIB_COUNTDOWNTIME)->SetVisible( !isRandom );
	GetAttributeSettings(ATTRIB_MINRANDOMTIME)->SetVisible( isRandom );
	GetAttributeSettings(ATTRIB_MAXRANDOMTIME)->SetVisible( isRandom );

	// fire events
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_COUNTDOWNTIME)) );
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_MINRANDOMTIME)) );
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_MAXRANDOMTIME)) );
}


// update the passed time of the condition
void StateTransitionTimeCondition::Update(const Time& elapsed, const Time& delta)
{
	// remember elapsed time
	mElapsedTime += delta;
}


// reset the condition
void StateTransitionTimeCondition::Reset()
{
	// reset the elapsed time
	mElapsedTime = Time(0.0);

	// use randomized count downs?
	if (GetBoolAttribute(ATTRIB_USERANDOMIZATION) == true)
	{
		const float minValue = GetFloatAttribute(ATTRIB_MINRANDOMTIME);
		const float maxValue = GetFloatAttribute(ATTRIB_MAXRANDOMTIME);

		// create a randomized count down value
		mCountDownTime = Time( Math::RandF(minValue, maxValue) );
	}
	else
	{
		// get the fixed count down value from the attribute
		mCountDownTime = Time( GetFloatAttribute(ATTRIB_COUNTDOWNTIME) );
	}
}


// test the condition
bool StateTransitionTimeCondition::TestCondition()
{
	// in case the elapsed time is bigger than the count down time, we can trigger the condition
	if (mElapsedTime >= mCountDownTime)
		return true;

	// we have not reached the count down time yet, don't trigger yet
	return false;
}
