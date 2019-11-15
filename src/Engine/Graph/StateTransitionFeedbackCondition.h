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

#ifndef __NEUROMORE_STATETRANSITIONFEEDBACKCONDITION_H
#define __NEUROMORE_STATETRANSITIONFEEDBACKCONDITION_H

// include the required headers
#include "../Config.h"
#include "../Core/Time.h"
#include "StateTransitionCondition.h"


// forward declaration
class Node;
class Graph;
class StateMachine;
class StateTransition;

class ENGINE_API StateTransitionFeedbackCondition : public StateTransitionCondition
{
	public:
		enum { TYPE_ID = 0x00602 };
		static const char* Uuid () { return "08c9f398-263a-11e5-b345-feff819cdc9f"; }

		enum
		{
			ATTRIB_FEEDBACK			= 0,
			ATTRIB_TESTVALUE		= 1,
			ATTRIB_RANGEVALUE		= 2,
			ATTRIB_FUNCTION			= 3
		};

		enum EFunction
		{
			FUNCTION_GREATER		= 0,
			FUNCTION_GREATEREQUAL	= 1,
			FUNCTION_LESS			= 2,
			FUNCTION_LESSEQUAL		= 3,
			FUNCTION_NOTEQUAL		= 4,
			FUNCTION_EQUAL			= 5,
			FUNCTION_INRANGE		= 6,
			FUNCTION_NOTINRANGE		= 7,
			FUNCTION_NUMFUNCTIONS	= 8
		};

		// constructor & destructor
		StateTransitionFeedbackCondition(Graph* graph);
		virtual ~StateTransitionFeedbackCondition();

		void Init() override;
		void Reset() override;
		void OnAttributesChanged() override;

		bool TestCondition() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		virtual const char* GetReadableType() const override final	{ return "Feedback Condition"; }
		uint32 GetType() const override								{ return TYPE_ID; }
		const char* GetTypeUuid() const override final				{ return Uuid(); }

		GraphObject* Clone(Graph* graph) override					{ StateTransitionFeedbackCondition* clone = new StateTransitionFeedbackCondition(graph); return clone; }

		// test function
		void SetFunction(EFunction func);
		const char* GetTestFunctionString();

	private:
		// test function types
		typedef bool (CORE_CDECL *FeedbackConditionFunction)(double paramValue, double testValue, double rangeValue);

		// test function
		EFunction							mFunction;
		FeedbackConditionFunction			mTestFunction;

		// double test functions
		static bool CORE_CDECL TestGreater(double paramValue, double testValue, double rangeValue);
		static bool CORE_CDECL TestGreaterEqual(double paramValue, double testValue, double rangeValue);
		static bool CORE_CDECL TestLess(double paramValue, double testValue, double rangeValue);
		static bool CORE_CDECL TestLessEqual(double paramValue, double testValue, double rangeValue);
		static bool CORE_CDECL TestEqual(double paramValue, double testValue, double rangeValue);
		static bool CORE_CDECL TestNotEqual(double paramValue, double testValue, double rangeValue);
		static bool CORE_CDECL TestInRange(double paramValue, double testValue, double rangeValue);
		static bool CORE_CDECL TestNotInRange(double paramValue, double testValue, double rangeValue);
};


#endif
