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
#include "SwitchState.h"
#include "../EngineManager.h"
#include "GraphManager.h"
#include "Classifier.h"


using namespace Core;

// constructor
SwitchState::SwitchState(Graph* parentGraph) : State(parentGraph)
{
}


// destructor
SwitchState::~SwitchState()
{
}


// register the attributes
void SwitchState::Init()
{
	// init base class 
	State::Init();

}


// update the state
void SwitchState::Update(const Time& elapsed, const Time& delta)
{
	// base-update
	State::Update(elapsed, delta);
}


// reset
void SwitchState::Reset()
{
	// base-class reset
	State::Reset();
}


// called when any of the attributes changed
void SwitchState::OnAttributesChanged()
{
	// base-class call
	State::OnAttributesChanged();

}

