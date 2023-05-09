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
#include "StateTransitionUrlOpenedCondition.h"
#include "../Core/AttributeSettings.h"
#include "../Core/EventManager.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
StateTransitionUrlOpenedCondition::StateTransitionUrlOpenedCondition(Graph* graph) : StateTransitionCondition(graph), mOpened(false)
{
	Reset();
}


// destructor
StateTransitionUrlOpenedCondition::~StateTransitionUrlOpenedCondition()
{
}


// register the attributes
void StateTransitionUrlOpenedCondition::Init()
{
	// register attributes

	// register url attribute
	AttributeSettings* urlAttribute = RegisterAttribute("Opened URL", "url", "The URL that was opened in the browser.", ATTRIBUTE_INTERFACETYPE_STRING);
	urlAttribute->SetDefaultValue( AttributeString::Create("") );
}


// update the passed time of the condition
void StateTransitionUrlOpenedCondition::Update(const Time& elapsed, const Time& delta)
{
}


// reset the condition
void StateTransitionUrlOpenedCondition::Reset()
{
	mOpened = false;
}


// test the condition
bool StateTransitionUrlOpenedCondition::TestCondition()
{
	return mOpened;
}


// attributes have changed
void StateTransitionUrlOpenedCondition::OnAttributesChanged()
{
	StateTransitionCondition::OnAttributesChanged();

	// show/hide max number of loops attribute
	//const int32 function = GetInt32Attribute( ATTRIB_FUNCTION );
	//const bool isFunctionAttributeVisible = (function == FUNCTION_HASREACHEDMAXNUMLOOPS);
	//GetAttributeSettings(ATTRIB_NUMLOOPS)->SetVisible( isFunctionAttributeVisible );

	// fire event
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_URL)) );
}
