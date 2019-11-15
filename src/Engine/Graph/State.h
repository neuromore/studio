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

#ifndef __NEUROMORE_STATE_H
#define __NEUROMORE_STATE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "Node.h"
#include "ActionSet.h"


// state base class
class ENGINE_API State : public Node
{
	CORE_LOGTRACE_DISABLE("State", GetReadableType(), this);
	CORE_LOGDEBUG_DISABLE("State", GetReadableType(), this);

	public:

		enum { BASE_TYPE = 0x006 };

		enum
		{
			ATTRIB_TRANSITIONMODE = 0,
			NUM_STATE_ATTRIBUTES
		};

		// port
		enum
		{
			PORTID_INPUT = 0,
			PORTID_OUTPUT = 0
		};

		// constructor & destructor
		State(Graph* parentGraph);
		virtual ~State();
		
		virtual uint32 GetBaseType() const override								{ return BASE_TYPE; }


		enum ECategory
		{
			CATEGORY_GENERAL = 0,
			NUM_CATEGORIES
		};

		static const char* GetCategoryName(ECategory category);
		static const char* GetCategoryDescription(ECategory category);

		void Init() override;
		void Reset() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		virtual void OnAttributesChanged() override;

		enum EState
		{
			STATE_DISABLED,
			STATE_INACTIVE,
			STATE_ENTERING,
			STATE_ACTIVE,
			STATE_EXITING
		};

		bool IsDisabled() const																	{ return mState == STATE_DISABLED; }

		// if state is currently active
		bool IsActive() const																	{ return mState == STATE_ACTIVE; }
		bool IsTransitioning() const															{ return IsEntering() || IsExiting(); }

		// state is transitioning in or out of a state
		bool IsEntering() const																	{ return mState == STATE_ENTERING; }
		bool IsExiting() const																	{ return mState == STATE_EXITING; }

		// get transition state of this state [0..1]
		double GetTransitionProgress() const;

		void ForceActivate();

		// serialization
		virtual Core::Json::Item Save(Core::Json& json, Core::Json::Item& item) override;
		virtual bool Load(const Core::Json& json, const Core::Json::Item& item) override;

	//private:
		// transition events
		//virtual void OnEnter(State* previousState, StateTransition* usedTransition)	{}		// TODO figure out how the atomic triggers should behave
		//virtual void OnEntered(State* previousState, StateTransition* usedTransition)	{}
		//virtual void OnExit(StateTransition* nextTransition)							{}
		//virtual void OnExited(StateTransition* nextTransition)						{}

		// activation events
		virtual void OnActivate()														{}
		virtual void OnDeactivate()														{}

	//protected:
		StateMachine*		mParentStateMachine;
		EState				mState;
		uint32				mActivationCount;
		EState				mToggleDirection;	// stores next toggle state so we know if to activate or deactivate state after incoming transition has finished

		// if the state is ready to enter/exit
		virtual bool CanEnter(State* previousState, StateTransition* usedTransition) const		{ return true; }
		virtual bool CanExit(StateTransition* nextTransition) const								{ return true; }

		// entry/exit events for base class
		virtual bool OnTryStateEnter(State* previousState, StateTransition* usedTransition);
		virtual bool OnTryStateEntered(State* previousState, StateTransition* usedTransition);
		virtual bool OnTryStateExit(StateTransition* nextTransition);
		virtual bool OnTryStateExited(StateTransition* nextTransition);

		// the primary transition
		bool HasPrimaryTransition() const			{ return mPrimaryTransition != NULL; }
		void ClearPrimaryTransition()				{ mPrimaryTransition = NULL; }
		StateTransition*	mPrimaryTransition;

		// TODO deprecate
		// transition modes
		enum ETransitionMode
		{
			TRANSITIONMODE_ALL,
			TRANSITIONMODE_PRIORITY,
			TRANSITIONMODE_RANDOM,
			NUM_TRANSMODES
		};

		// TODO deprecate this
		ETransitionMode GetTransitionMode() const { return (ETransitionMode)GetInt32Attribute(ATTRIB_TRANSITIONMODE); }

		// state activation behaviour
		enum EActivationMode
		{
			ACTIVATE_ALWAYS,				// OnActivate() / OnDeactivate() is always executed if an incoming / outgoing transition finishes
			ACTIVATE_FIRST,					// OnActivate() / OnDeactivate()  is executed once, when state goes/leaves ACTIVE mode
			ACTIVATE_TOGGLE					// all transitions toggle the state between active/inactive
		};

		virtual EActivationMode GetActivationMode() const { return ACTIVATE_ALWAYS; }

		// dead end: states that can't ever transition anywhere (all transitions disabled or none present)
		enum EDeadEndMode
		{
			DEADEND_STAYACTIVE,				// state stays active if 
			DEADEND_DEACTIVATE				// state goes inactive again immediately after activation
		};
		virtual EDeadEndMode GetDeadEndMode() const { return DEADEND_DEACTIVATE; }

		virtual Core::String& GetDebugString(Core::String& inout) override;
		Core::String mTempString;
};


#endif
