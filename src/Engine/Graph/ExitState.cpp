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
#include "ExitState.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
ExitState::ExitState(Graph* parentGraph) : State(parentGraph)
{
}


// destructor
ExitState::~ExitState()
{
}


// register the attributes
void ExitState::Init()
{
	// init base class 
	State::Init();

	// integer exit status 
	AttributeSettings* exitStatusAttribute = RegisterAttribute("Exit Status", "exitStatus", "", ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	exitStatusAttribute->SetDefaultValue(AttributeInt32::Create(0));
	exitStatusAttribute->SetMinValue(AttributeInt32::Create(0));
	exitStatusAttribute->SetMaxValue(AttributeInt32::Create(CORE_INT32_MAX));
}


// update the state
void ExitState::Update(const Time& elapsed, const Time& delta)
{
	// base-update
	State::Update(elapsed, delta);
}


// reset
void ExitState::Reset()
{
	// base-class reset
	State::Reset();
}


// called when any of the attributes changed
void ExitState::OnAttributesChanged()
{
	// base-class call
	State::OnAttributesChanged();
}
