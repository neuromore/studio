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

#ifndef __NEUROMORE_TIMERSTATE_H
#define __NEUROMORE_TIMERSTATE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../DSP/ClockGenerator.h"
#include "State.h"


// state class
class ENGINE_API TimerState : public State
{
	public:
		enum { TYPE_ID = 0x1007 };
		static const char* Uuid () { return "fa08a456-a115-11e5-8994-feff819cdc9f"; }
		
		enum
		{
			ATTRIB_INTERVAL = NUM_STATE_ATTRIBUTES,
			ATTRIB_STARTACTIVE,
			ATTRIB_ENTRYMODE,
			ATTRIB_LOOPMODE, 
			ATTRIB_NUMLOOPS,
			ATTRIB_FIRSTTICK,
		};

		enum EEntryMode
		{
			ENTRYMODE_START,
			ENTRYMODE_RESTART,
			ENTRYMODE_TOGGLE,
			NUM_ENTRYMODES
		};

		enum ELoopMode
		{
			LOOPMODE_NONE,
			LOOPMODE_COUNT,
			LOOPMODE_FOREVER,
			NUM_LOOPMODES
		};

		// constructor & destructor
		TimerState(Graph* parentGraph);
		virtual ~TimerState();
		
		void Init() override final;
		void Reset() override final;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override final;
		void OnAttributesChanged() override final;

		Core::Color GetColor() const override									{ return Core::RGBA(33, 191, 234); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Timer"; }
		const char* GetRuleName() const override final							{ return "STATE_Timer"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_GENERAL; }
		GraphObject* Clone(Graph* parentGraph) override							{ State* clone = new TimerState(parentGraph); return clone; }

		EActivationMode GetActivationMode() const override final;
		EDeadEndMode GetDeadEndMode() const override final						{ return DEADEND_STAYACTIVE; }

		bool CanExit(StateTransition* transition) const override final;
		bool CanEnter(State* previousState, StateTransition* usedTransition) const override final;

		virtual void OnActivate() override final;
		virtual void OnDeactivate() override final;

	private:

		ClockGenerator	mClock;

		uint32			mNumLoopsPassed;
		bool			mFirstTickCompleted;		// for handling the first tick in CanExit

};


#endif
