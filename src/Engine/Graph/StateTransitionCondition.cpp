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
#include "StateTransitionCondition.h"
#include "StateMachine.h"
#include "../Core/EventManager.h"


using namespace Core;

// constructor
StateTransitionCondition::StateTransitionCondition(Graph* graph) : GraphObject()
{
	mStateMachine	= NULL;
	mTransition		= NULL;

	if (graph != NULL && graph->GetType() == StateMachine::TYPE_ID)
	{
		StateMachine* stateMachine = static_cast<StateMachine*>(graph);
		mStateMachine = stateMachine;
	}
}


// destructor
StateTransitionCondition::~StateTransitionCondition()
{
}


// update the test result from the last test condition call and call the corresponding event
void StateTransitionCondition::UpdatePreviousTestResult(bool newTestResult)
{
	//// call the event in case the condition got triggered
	//if (newTestResult != mPreviousTestResult)
	//	CORE_EVENTMANAGER.OnConditionTriggered(this);

	// update the previous test result
	mPreviousTestResult = newTestResult;
}


// save the condition
Json::Item StateTransitionCondition::Save(Json& json, Json::Item& item)
{
	// add the condition item
	Json::Item conditionItem = item.AddObject();

	// add the attributes
	conditionItem.AddString( "type", GetTypeUuid() );

	// add attributes from the graph object attribute set
	Write( json, conditionItem, true );

	return conditionItem;
}
