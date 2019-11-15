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
#include "State.h"
#include "../EngineManager.h"
#include "GraphManager.h"
#include "Classifier.h"


using namespace Core;

// constructor
State::State(Graph* parentGraph) : Node(parentGraph)
{
	if (parentGraph != NULL)
	{
		CORE_ASSERT(parentGraph != NULL && parentGraph->GetType() == StateMachine::TYPE_ID);
		mParentStateMachine = static_cast<StateMachine*>(parentGraph);
	}
	else
		mParentStateMachine = NULL;
}


// destructor
State::~State()
{
}

// get the name for the given category
const char* State::GetCategoryName(ECategory category)
{
	switch (category)
	{
		case CATEGORY_GENERAL:		return "General";
		default:                    return "INVALID CATEGORY";
	}

}

// get a short description for the given category
const char* State::GetCategoryDescription(ECategory category)
{
	switch (category)
	{
		case CATEGORY_GENERAL:		return "General";
		default:                    return "INVALID CATEGORY";
	}
}


// register the attributes	
void State::Init()
{
	LogTrace("Init");

	// init base class 
	Node::Init();

	// setup the input ports
	InitInputPorts(1);
	GetInputPort(PORTID_INPUT).Setup("Input", "x", AttributeFloat::TYPE_ID, PORTID_INPUT);

	// setup the output ports
	InitOutputPorts(1);
	GetOutputPort(PORTID_OUTPUT).Setup("Output", "y", AttributeFloat::TYPE_ID, PORTID_OUTPUT);

	// DEPRECATED this is deprecated and will be removed completely later. Default mode for all states will be TRANSITIONMODE_PRIORITY, only special states can have different transition modes
	AttributeSettings* transitionModeAttribute = RegisterAttribute("Transition Mode", "transitionMode", "", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	transitionModeAttribute->ResizeComboValues(NUM_TRANSMODES);
	transitionModeAttribute->SetComboValue(TRANSITIONMODE_ALL, "Use All Transitions");
	transitionModeAttribute->SetComboValue(TRANSITIONMODE_PRIORITY, "Select Highest Priority");
	transitionModeAttribute->SetComboValue(TRANSITIONMODE_RANDOM, "Select Randomly");
	transitionModeAttribute->SetDefaultValue(AttributeInt32::Create(TRANSITIONMODE_PRIORITY));	// DEPRECATED: changed default mode to priority
}


// update the state
void State::Update(const Time& elapsed, const Time& delta)
{
	LogTraceRT("Update");
}


// reset
void State::Reset()
{
	LogTrace("Reset");

	// base-class reset
	Node::Reset();

	LogDebug("Reset %s", GetName());

	mState = STATE_INACTIVE;
	mToggleDirection = STATE_ACTIVE;
	mActivationCount = 0;

	ClearPrimaryTransition();
}


// called when any of the attributes changed
void State::OnAttributesChanged()
{
	LogTrace("OnAttributesChanged");

	// base-class call
	Node::OnAttributesChanged();
}


// transition progress of a single state (where 1=active 0=inactive)
double State::GetTransitionProgress() const
{
	switch (mState)
	{
		// transitioning in/out
		case STATE_ENTERING:
			if (HasPrimaryTransition() == false)
				return 0.0;
			return mPrimaryTransition->GetProgress();

		case STATE_EXITING:
			if (HasPrimaryTransition() == false)
				return 0.0;
			return 1.0 - mPrimaryTransition->GetProgress();
	
		// active
		case STATE_ACTIVE:
			return 1.0;

		// inactive
		default:
		case STATE_INACTIVE:
		case STATE_DISABLED:
			return 0.0;
	}
}


void State::ForceActivate()
{
	mState = STATE_ACTIVE;
	mActivationCount++;
	OnActivate();
}


// called when an incoming transition begins; returns true if transition is allowed
bool State::OnTryStateEnter(State* previousState, StateTransition* usedTransition)
{
	LogTraceRT("OnTryStateEnter");

	// state disabled
	if (mState == STATE_DISABLED)
		return false;

	// state cannot be entered via the given transition
	if (CanEnter(previousState, usedTransition) == false)
		return false;

	switch (mState)
	{
		default:
		// dont allow transitions if the state is already exiting
		case STATE_EXITING:
			return false;

		case STATE_INACTIVE:			// state is not active yet -> the used transition will become the primary transition and the state will start entering
		case STATE_ENTERING:			// another transition is already active
		case STATE_ACTIVE:				// state is already active (but must not have a primary trans)
		{
			// no primary transition yet, the current one is the first to trigger
			if (HasPrimaryTransition() == false)
			{
				// update primary transition
				mPrimaryTransition = usedTransition;

				// go into entering phase
				mState = STATE_ENTERING;

				// reset all outgoing transitions the moment we enter the state (its easier and safer than doing it when state is exited because it has to be done after the last transition finished; we cant reset them earlier)
				mParentStateMachine->ResetOutputConnections(this);
			}

			// remember toggle direction for OnEntered
			if (GetActivationMode() == ACTIVATE_TOGGLE)
			{
				if (mState == STATE_INACTIVE)
					mToggleDirection = STATE_ACTIVE;	// toggle it on
				else if (mState == STATE_ACTIVE)
					mToggleDirection = STATE_INACTIVE;	// toggle it off
			}
		} break;
	}
	
	// Note: not used right now
	//OnEnter(previousState, usedTransition);



	return true;
}


// called when the incoming transition ended;
bool State::OnTryStateEntered(State* previousState, StateTransition* usedTransition)
{
	LogTraceRT("OnTryStateEntered");

	// state disabled
	if (mState == STATE_DISABLED)
		return false;

	// state cannot be entered via the given transition
	if (CanEnter(previousState, usedTransition) == false)
		return false;

	switch (mState)
	{
		// cannot enter
		default:
		case STATE_EXITING:
		case STATE_INACTIVE:	
			return false;

		// can enter, do stuff
		case STATE_ENTERING:
		case STATE_ACTIVE:

			// is this the primary transition?
			if (HasPrimaryTransition() && usedTransition == mPrimaryTransition)
			{
				// activate state
				mState = STATE_ACTIVE;
				LogDebug("Activate State %s", GetName());
				mActivationCount++;
				OnActivate();

				ClearPrimaryTransition();
			}
			else // it's not the primary trans
			{
				if (GetActivationMode() == ACTIVATE_ALWAYS)
				{
					LogDebug("Activate State %s", GetName());
					mActivationCount++;
					OnActivate();
				}
				else if (GetActivationMode() == ACTIVATE_TOGGLE)
				{
					mState = mToggleDirection;
					LogDebug("Toggle State %s %s", GetName(), (mToggleDirection == STATE_ACTIVE ? "On" : "Off"));
					if (mToggleDirection == STATE_ACTIVE)
					{
						mActivationCount++;
						OnActivate();
					}
					else if (mState == STATE_INACTIVE)
					{
						OnDeactivate();
						// apprupt state change from active to inactive -> clear all transitions
						ClearPrimaryTransition();
					}
				}

			}
		break;
	}

	// Dead End behaviour
	if (mParentStateMachine->FindNumOutTransitions(this, true) == 0)		// TODO optimize this, state should have access to a static list of all outgoing transitions!
	{
		if (GetDeadEndMode() == DEADEND_DEACTIVATE)
		{
			mState = STATE_INACTIVE;
			OnDeactivate();
		}
	}

	// always execute enter code
	//OnEntered(previousState, usedTransition);

	return true;
}


// called when we start transitioning to another state
bool State::OnTryStateExit(StateTransition* nextTransition)
{
	LogTraceRT("OnTryStateExit");

	// state disabled
	if (mState == STATE_DISABLED)
		return false;

	// state cannot be exited via the given transition
	if (CanExit(nextTransition) == false)
		return false;

	switch (mState)
	{
		// cannot exit
		default:
		case STATE_INACTIVE:
		case STATE_ENTERING:
			return false;

		// can exit
		case STATE_EXITING:	
		case STATE_ACTIVE:	
		{
			// no primary transition yet, the current one is the first to trigger
			if (HasPrimaryTransition() == false)
			{
				// update primary transition
				mPrimaryTransition = nextTransition;

				if (GetActivationMode() != ACTIVATE_TOGGLE)
				{
					// go into exiting phase
					mState = STATE_EXITING;

					LogDebug("Deactivate State %s", GetName());
					OnDeactivate();
				}
			}
			else // it's not the primary trans
			{
				if (GetActivationMode() == ACTIVATE_ALWAYS)
				{
					LogDebug("Deactivate State %s", GetName());
					OnDeactivate();
				}

			}
		} break;

	}

	//OnExit(nextTransition);

	return true;
}


// called when we fully left the state and now the new state is active
bool State::OnTryStateExited(StateTransition* nextTransition)
{
	LogTraceRT("OnTryStateExited");

	// state disabled
	if (mState == STATE_DISABLED)
		return false;

	// state cannot exit via the given transition
	if (CanExit(nextTransition) == false)
		return false;

	switch (mState)
	{
		// cannot exit
		default:
		case STATE_INACTIVE:	
		case STATE_ENTERING:	// should never happen?
		case STATE_ACTIVE:		// should never happen?
			return false;

		case STATE_EXITING:
			if (HasPrimaryTransition() == true && mPrimaryTransition == nextTransition)
			{
				if (GetActivationMode() != ACTIVATE_TOGGLE)
				{
					mState = STATE_INACTIVE;
					LogDebug("Fully Transitioned out State %s", GetName());
				}
				ClearPrimaryTransition();
			}
		
		break;
	}

//		// reset all outgoing transition conditions
//		GetParentStateMachine()->ResetOutputConnections(this);


	
	// execute exit code
	//OnExited(nextTransition);

	return true;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Serialization
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Json::Item State::Save(Json& json, Json::Item& item)
{
	// base class save
	return Node::Save( json, item );
}


bool State::Load(const Json& json, const Json::Item& item)
{
	bool success = true;

	// base class load
	if (Node::Load(json, item) == false)
		success = false;

	// reset the dirty flag
	SetIsDirty(false);
	
	return success;
}


String& State::GetDebugString(String& inout)
{
	Node::GetDebugString(inout);

	//mTempString.Format("Initialized: %s\n", (mIsInitialized ? "True" : "False") );
	//inout += mTempString;


	inout += "State: ";
	switch (mState)
	{
		case STATE_DISABLED: inout += "Disabled"; break;
		case STATE_INACTIVE: inout += "Inactive"; break;
		case STATE_ENTERING: inout += "Entering"; break;
		case STATE_ACTIVE:   inout += "Active";   break;
		case STATE_EXITING:  inout += "Exiting";  break;
	}
	inout += "\n";

	inout += "Activation: ";
	switch (GetActivationMode())
	{
		case ACTIVATE_ALWAYS:  inout += "Always"; break;
		case ACTIVATE_FIRST:   inout += "First";  break;
		case ACTIVATE_TOGGLE:  inout += "Toggle"; break;
	}
	inout += "\n";

	inout += "DeadEnd: ";
	switch (GetDeadEndMode())
	{
		case DEADEND_STAYACTIVE:  inout += "StayActive"; break;
		case DEADEND_DEACTIVATE:   inout += "Deactivate";  break;
	}
	inout += "\n";

	mTempString.Format("Progress: %.2f\n", GetTransitionProgress());
	inout += mTempString.AsChar();
	
	mTempString.Format("Primary Trans: %x\n", mPrimaryTransition);
	inout += mTempString.AsChar();

	mTempString.Format("Counter: %i\n", mActivationCount);
	inout += mTempString.AsChar();

	return inout;
}
