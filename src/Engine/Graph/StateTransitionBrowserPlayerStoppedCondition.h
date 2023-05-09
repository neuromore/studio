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

#ifndef __NEUROMORE_STATETRANSITIONBROWSERPLAYERSTOPPEDCONDITION_H
#define __NEUROMORE_STATETRANSITIONBROWSERPLAYERSTOPPEDCONDITION_H

// include the required headers
#include "../Config.h"
#include "../Core/Time.h"
#include "StateTransitionCondition.h"


// forward declaration
class Node;
class Graph;
class StateMachine;
class StateTransition;

class ENGINE_API StateTransitionBrowserPlayerStoppedCondition : public StateTransitionCondition
{
	public:
		enum { TYPE_ID = 0x00609 };
		static const char* Uuid () { return "5f5ef3c0-a5ce-410f-b197-3041c1e31ad2"; }

		// constructor & destructor
		StateTransitionBrowserPlayerStoppedCondition(Graph* graph);
		virtual ~StateTransitionBrowserPlayerStoppedCondition();

		inline void SetCondition(bool v) { mCondition = v; }

		void Init() override;
		void Reset() override;
		void OnAttributesChanged() override;

		bool TestCondition() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		virtual const char* GetReadableType() const override final	{ return "Browser Player Stopped Condition"; }
		uint32 GetType() const override								{ return TYPE_ID; }
		const char* GetTypeUuid() const override final				{ return Uuid(); }

		GraphObject* Clone(Graph* graph) override					{ StateTransitionBrowserPlayerStoppedCondition* clone = new StateTransitionBrowserPlayerStoppedCondition(graph); return clone; }

	private:
		bool mCondition;
};


#endif
