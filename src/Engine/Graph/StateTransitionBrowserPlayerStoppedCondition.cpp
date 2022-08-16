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
#include "StateTransitionBrowserPlayerStoppedCondition.h"
#include "../Core/AttributeSettings.h"
#include "../Core/EventManager.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
StateTransitionBrowserPlayerStoppedCondition::StateTransitionBrowserPlayerStoppedCondition(Graph* graph) : StateTransitionCondition(graph), mCondition(false)
{
	Reset();
}

// destructor
StateTransitionBrowserPlayerStoppedCondition::~StateTransitionBrowserPlayerStoppedCondition()
{
}

// register the attributes
void StateTransitionBrowserPlayerStoppedCondition::Init()
{
}

// update the passed time of the condition
void StateTransitionBrowserPlayerStoppedCondition::Update(const Time& elapsed, const Time& delta)
{
}

// reset the condition
void StateTransitionBrowserPlayerStoppedCondition::Reset()
{
	mCondition = false;
}


// test the condition
bool StateTransitionBrowserPlayerStoppedCondition::TestCondition()
{
	return mCondition;
}


// attributes have changed
void StateTransitionBrowserPlayerStoppedCondition::OnAttributesChanged()
{
	StateTransitionCondition::OnAttributesChanged();
}
