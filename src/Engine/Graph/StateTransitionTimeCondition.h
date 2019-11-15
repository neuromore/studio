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

#ifndef __NEUROMORE_STATETRANSITIONTIMECONDITION_H
#define __NEUROMORE_STATETRANSITIONTIMECONDITION_H

// include the required headers
#include "../Config.h"
#include "../Core/Time.h"
#include "StateTransitionCondition.h"


// forward declaration
class Node;
class Graph;
class StateMachine;
class StateTransition;

class ENGINE_API StateTransitionTimeCondition : public StateTransitionCondition
{
	public:
		enum { TYPE_ID = 0x00601 };
		static const char* Uuid () { return "6983b8a6-bb6c-11e4-8dfc-aa07a5b093db"; }

		enum
		{
			ATTRIB_USERANDOMIZATION	= 0,
			ATTRIB_COUNTDOWNTIME	= 1,
			ATTRIB_MINRANDOMTIME	= 2,
			ATTRIB_MAXRANDOMTIME	= 3
		};

		// constructor & destructor
		StateTransitionTimeCondition(Graph* graph);
		virtual ~StateTransitionTimeCondition();

		void Init() override;
		void Reset() override;
		void OnAttributesChanged() override;

		bool TestCondition() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		virtual const char* GetReadableType() const override final	{ return "Time Condition"; }
		uint32 GetType() const override								{ return TYPE_ID; }
		const char* GetTypeUuid() const override final				{ return Uuid(); }

		GraphObject* Clone(Graph* graph) override					{ StateTransitionTimeCondition* clone = new StateTransitionTimeCondition(graph); return clone; }

	private:
		Core::Time mElapsedTime;	/**< The elapsed time in seconds. */
		Core::Time mCountDownTime;	/**< The count down time in seconds. */
};


#endif
