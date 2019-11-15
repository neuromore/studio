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

#ifndef __NEUROMORE_STATEMACHINETRANSITIONCONDITION_H
#define __NEUROMORE_STATEMACHINETRANSITIONCONDITION_H

// include the required headers
#include "../Config.h"
#include "../Core/String.h"
#include "../Core/EventSource.h"
#include "GraphObject.h"


// forward declaration
class Graph;
class StateMachine;
class StateTransition;

//
class ENGINE_API StateTransitionCondition : public GraphObject
{
	friend StateTransition;
	public:
		enum { BASE_TYPE = 0x004 };

		// constructor & destructor
		StateTransitionCondition(Graph* graph);
		virtual ~StateTransitionCondition();

		virtual bool TestCondition() = 0;

		uint32 GetBaseType() const override final								{ return BASE_TYPE; }

		void UpdatePreviousTestResult(bool newTestResult);

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// Serialization
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual Core::Json::Item Save(Core::Json& json, Core::Json::Item& item) override;

	private:
		StateMachine*		mStateMachine;
		StateTransition*	mTransition;
		bool				mPreviousTestResult; /**< Result of the last TestCondition() call. */
};


#endif
