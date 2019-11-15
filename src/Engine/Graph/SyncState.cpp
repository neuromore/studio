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

// include required headers
#include "SyncState.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
SyncState::SyncState(Graph* parentGraph) : State(parentGraph)
{
}


// destructor
SyncState::~SyncState()
{
}


// register the attributes
void SyncState::Init()
{
	// init base class 
	State::Init();

	AttributeSettings* syncModeAttribute = RegisterAttribute("Sync Mode", "syncMode", "", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	syncModeAttribute->ResizeComboValues(NUM_WAITMODES);
	syncModeAttribute->SetComboValue(SYNCMODE_ALLTRANSITIONS, "Wait for all incoming transitions");
	syncModeAttribute->SetComboValue(SYNCMODE_TRANSITIONCOUNT, "Wait for x incoming transitions");
	syncModeAttribute->SetDefaultValue(AttributeInt32::Create(SYNCMODE_ALLTRANSITIONS));

	AttributeSettings* countAttribute = RegisterAttribute("Transition Count", "transitionCount", "", ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	countAttribute->SetDefaultValue(AttributeInt32::Create(2));
	countAttribute->SetMinValue(AttributeInt32::Create(1));
	countAttribute->SetMaxValue(AttributeInt32::Create(CORE_INT32_MAX));
	countAttribute->SetVisible(false);

	AttributeSettings* probabilityAttribute = RegisterAttribute("Probability", "probability", "", ATTRIBUTE_INTERFACETYPE_FLOATSLIDER);
	probabilityAttribute->SetDefaultValue(AttributeFloat::Create(0.5));
	probabilityAttribute->SetMinValue(AttributeFloat::Create(0.0));
	probabilityAttribute->SetMaxValue(AttributeFloat::Create(1.0));
	probabilityAttribute->SetVisible(false);

}


// update the state
void SyncState::Update(const Time& elapsed, const Time& delta)
{
	// base-update
	State::Update(elapsed, delta);

	UpdateAllowedTransitionList();
}



// reset
void SyncState::Reset()
{
	// base-class reset
	State::Reset();

	// check which transitions can enter
	UpdateAllowedTransitionList();
}



// called when any of the attributes changed
void SyncState::OnAttributesChanged()
{
	// base-class call
	State::OnAttributesChanged();

	// update attribute visibility
	const uint32 syncMode = GetInt32Attribute(ATTRIB_SYNCMODE);
	GetAttributeSettings(ATTRIB_TRANSITIONCOUNT)->SetVisible(syncMode == SYNCMODE_TRANSITIONCOUNT);

	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_TRANSITIONCOUNT)) );
}


bool SyncState::CanEnter(State* previousState, StateTransition* usedTransition) const 
{
	return mAllowedTransitions.Contains(usedTransition);
}


void SyncState::OnDeactivate() 
{
	Reset();
}


bool SyncState::UpdateAllowedTransitionList()
{
	mAllowedTransitions.Clear();
	
	// find which incoming transition is allowed to enter
	// collect them first and then decide if we accept or deny depending on sync mode
	const uint32 numIncoming = mParentStateMachine->FindNumInTransitions(this, true);
	for (uint32 i = 0; i < numIncoming; ++i)
	{
		StateTransition* transition = mParentStateMachine->FindInTransition(this, i, true);
		State* sourceState = transition->GetSourceState();

		// is state active and transition ready to exit?
		if (sourceState->IsActive() == false || sourceState->CanExit(transition) == false)
			continue;

		mAllowedTransitions.Add(transition);
	}

	// decide if state can activate, modify list accordingly
	const uint32 syncMode = GetInt32Attribute(ATTRIB_SYNCMODE);
	switch (syncMode)
	{
		case SYNCMODE_ALLTRANSITIONS:
		{
			// accept only if all transitions are ready
			if (mAllowedTransitions.Size() == numIncoming)
				return true;
		} break;
	
		case SYNCMODE_TRANSITIONCOUNT:
		{
			const uint32 count = GetInt32Attribute(ATTRIB_TRANSITIONCOUNT);
			if (mAllowedTransitions.Size() == count)
				return true;

		} break;
		default: break;
	}

	// default: not ready, don't allow any transition
	mAllowedTransitions.Clear();
	return false;
}
