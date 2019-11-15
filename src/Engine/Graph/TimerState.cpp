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
#include "TimerState.h"
#include "../Core/EventManager.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
TimerState::TimerState(Graph* parentGraph) : State(parentGraph)
{
	mNumLoopsPassed = 0;
	mFirstTickCompleted = false;
}


// destructor
TimerState::~TimerState()
{
}


// register the attributes
void TimerState::Init()
{
	// init base class 
	State::Init();
	
	// interval
	const double defaultInterval = 1.0;
	AttributeSettings* intervalAttribute = RegisterAttribute("Interval (seconds)", "interval", "Time between timer ticks.", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	intervalAttribute->SetMinValue(AttributeFloat::Create(0.0));
	intervalAttribute->SetMaxValue(AttributeFloat::Create(FLT_MAX));
	intervalAttribute->SetDefaultValue(AttributeFloat::Create(defaultInterval));


	const bool defaultStartup = true;
	AttributeSettings* startupAttribute = RegisterAttribute("Start Active", "startActive", "The default state of the timer after it is initialized.", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	startupAttribute->SetDefaultValue(AttributeBool::Create(defaultStartup));

	// entry mode
	const int32 defaultEntryMode = ENTRYMODE_START;
	AttributeSettings* entryModeAttribute = RegisterAttribute("Entry Mode", "entryMode", "How the timer should behave if a transition triggers it.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	entryModeAttribute->ResizeComboValues(NUM_ENTRYMODES);
	entryModeAttribute->SetComboValue(ENTRYMODE_START, "Start Timer");
	entryModeAttribute->SetComboValue(ENTRYMODE_RESTART, "Restart Timer");
	entryModeAttribute->SetComboValue(ENTRYMODE_TOGGLE, "Toggle Timer");
	entryModeAttribute->SetDefaultValue(AttributeInt32::Create(defaultEntryMode));

	// loop mode
	AttributeSettings* loopModeAttribute = RegisterAttribute("Loop Mode", "loopMode", "The loop behavior of the timer.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	loopModeAttribute->ResizeComboValues(NUM_LOOPMODES);
	loopModeAttribute->SetComboValue(LOOPMODE_NONE, "Don't loop");
	loopModeAttribute->SetComboValue(LOOPMODE_COUNT, "Loop x times");
	loopModeAttribute->SetComboValue(LOOPMODE_FOREVER, "Loop forever");
	loopModeAttribute->SetDefaultValue(AttributeInt32::Create(LOOPMODE_FOREVER));

	// num loops
	AttributeSettings* numLoopAttribute = RegisterAttribute("Tick Count", "tickCount", "The number of times the timer will tick.", ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	numLoopAttribute->SetDefaultValue(AttributeInt32::Create(3));
	numLoopAttribute->SetMinValue(AttributeInt32::Create(1));
	numLoopAttribute->SetMaxValue(AttributeInt32::Create(CORE_INT32_MAX));

	// tick on activation
	AttributeSettings* tickOnActivationAttribute = RegisterAttribute("First Tick", "firstTick", "If true, the timer will not ommit the first tick at t=0, right after activation.", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	tickOnActivationAttribute->SetDefaultValue(AttributeBool::Create(false));
}


// update the state
void TimerState::Update(const Time& elapsed, const Time& delta)
{
	// base-update
	State::Update(elapsed, delta);

	// clear ticks from last update
	mClock.ClearNewTicks();
	mClock.Update(elapsed, delta);
}


// reset
void TimerState::Reset()
{
	// base-class reset
	State::Reset();

	mClock.Reset();	
	mClock.Stop(); // Note: reset does not stop the clock

	mNumLoopsPassed = 0;
	mFirstTickCompleted = false;

	const double frequency = 1.0 / GetFloatAttribute(ATTRIB_INTERVAL);
	mClock.SetFrequency(frequency);

	// begine active if attrib is set
	if (GetBoolAttribute(ATTRIB_STARTACTIVE) == true)
	{
		mState = STATE_ACTIVE;
		mClock.Start();
	}
}


// called when any of the attributes changed
void TimerState::OnAttributesChanged()
{
	// base-class call
	State::OnAttributesChanged();

	// activate state if the attribute was set to true
	const bool startActive = GetBoolAttribute(ATTRIB_STARTACTIVE);
	const double frequency = 1.0 / GetFloatAttribute(ATTRIB_INTERVAL);

	// frequency changed
	if (mClock.GetFrequency() != frequency)
	{
		mClock.Reset();
		mClock.SetFrequency(frequency);

		if (startActive == true)
			mClock.Start();
	}

	// startActive state changed
	if (startActive == true && mState != STATE_ACTIVE)
		OnActivate();

	const bool showNumLoops = ((ELoopMode)GetInt32Attribute(ATTRIB_LOOPMODE) == LOOPMODE_COUNT);
	GetAttributeSettings(ATTRIB_NUMLOOPS)->SetVisible(showNumLoops);
	
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_NUMLOOPS)) );
}


// activation mode depends on toggle attribute
State::EActivationMode TimerState::GetActivationMode() const
{
	EEntryMode entryMode = (EEntryMode)GetInt32Attribute(ATTRIB_ENTRYMODE);
	switch (entryMode)
	{
		case ENTRYMODE_START:   return ACTIVATE_ALWAYS; // return ACTIVATE_FIRST 
		case ENTRYMODE_RESTART:	return ACTIVATE_ALWAYS;
		case ENTRYMODE_TOGGLE:	return ACTIVATE_TOGGLE;
			
		default:				return ACTIVATE_ALWAYS;
	}
}


// always allow entry for now, may change later
bool TimerState::CanEnter(State* previousState, StateTransition* usedTransition) const
{
	return true;
}


// timer state has to exit the moment the clock ticks (or at t=0)
bool TimerState::CanExit(StateTransition* transition) const
{
	// clock ticked at least once? Note: ticks are cleared in update()
	if (mClock.GetNumNewTicks() > 0)
		return true;

	// timer has to fire at t=0?
	// TODO clock should support a mode that makes it tick on t=0, it would make this firstTick code obsolete
	else if (GetBoolAttribute(ATTRIB_FIRSTTICK) == true && mFirstTickCompleted == false)
		return true;

	// timer not firing
	return false;
}


// start or toggle clock, depending on settings
void TimerState::OnActivate()
{
	EEntryMode entryMode = (EEntryMode)GetInt32Attribute(ATTRIB_ENTRYMODE);

	// restart or just start clock
	if (entryMode == ENTRYMODE_RESTART)
		mClock.Reset();
	else
		mClock.Start();
}


// deactivate is called by base class after state has transitioned out
void TimerState::OnDeactivate()
{
	// complete first tick
	if (mFirstTickCompleted  == false)
		mFirstTickCompleted = true;

	// keep state active if repeat and not in toggle mode
	//if ((EEntryMode)GetInt32Attribute(ATTRIB_ENTRYMODE) != ENTRYMODE_TOGGLE)
	{
		switch (GetInt32Attribute(ATTRIB_LOOPMODE))
		{
			case LOOPMODE_FOREVER:
				mState = STATE_ACTIVE;
				return;	// skips clock reset, loop

			case LOOPMODE_NONE:
				break;	// don't skip clock reset

			case LOOPMODE_COUNT:

				// check if we have to loop another time, if not: stop the clock
				mNumLoopsPassed++;
				if ((int32)mNumLoopsPassed < GetInt32Attribute(ATTRIB_NUMLOOPS))
				{
					mState = STATE_ACTIVE;
					return;	// skips clock reset, loop
				}
				else
				{
					mNumLoopsPassed = 0;
					break;	// stop the clock
				}
		}
	}

	// stop clock in all other cases
	mClock.Stop();
	mClock.Reset();
}
