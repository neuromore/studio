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

// include precompiled header
#include <Engine/Precompiled.h>

// include the required headers
#include "StateTransitionAudioCondition.h"
#include "../Core/AttributeSettings.h"
#include "../Core/EventManager.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
StateTransitionAudioCondition::StateTransitionAudioCondition(Graph* graph) : StateTransitionCondition(graph)
{
	Reset();
}


// destructor
StateTransitionAudioCondition::~StateTransitionAudioCondition()
{
}


// register the attributes
void StateTransitionAudioCondition::Init()
{
	// register attributes

	// register audio url attribute
	AttributeSettings* urlAttribute = RegisterAttribute("Audio URL", "url", "The URL or local filename of the audio file.", ATTRIBUTE_INTERFACETYPE_STRING);
	urlAttribute->SetDefaultValue( AttributeString::Create("") );

	// create the function combobox
	AttributeSettings* functionAttribute = RegisterAttribute("Test Function", "testFunction", "The type of test function or condition.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	functionAttribute->ResizeComboValues( FUNCTION_NUMFUNCTIONS );
	functionAttribute->SetComboValue(FUNCTION_REACHEDEND,			"Reached End");
	functionAttribute->SetComboValue(FUNCTION_HASREACHEDMAXNUMLOOPS,"Has Reached Max Num Loops");
	//functionAttribute->SetComboValue(FUNCTION_PLAYTIME,			"Has Reached Specified Play Time");
	//functionAttribute->SetComboValue(FUNCTION_PLAYTIMELEFT,		"Has Less Play Time Left");
	functionAttribute->SetDefaultValue( AttributeInt32::Create(FUNCTION_REACHEDEND) );

	// create the num loops
	AttributeSettings* numLoopsAttribute = RegisterAttribute("Num Loops", "numLoops", "The number of loops the audio needs to play before the condition switches.", ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	numLoopsAttribute->SetDefaultValue( AttributeInt32::Create(1) );
	numLoopsAttribute->SetMinValue( AttributeInt32::Create(0) );
	numLoopsAttribute->SetMaxValue( AttributeInt32::Create(CORE_INT32_MAX) );
	numLoopsAttribute->SetVisible( false );

	// create the maximum play time
	//AttributeSettings* playTimeAttribute = RegisterAttribute("Play Time", "playTime", "...", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	//playTimeAttribute->SetDefaultValue( AttributeFloat::Create(1.0) );
	//playTimeAttribute->SetMinValue( AttributeFloat::Create(0) );
	//playTimeAttribute->SetMaxValue( AttributeFloat::Create(FLT_MAX) );
}


// update the passed time of the condition
void StateTransitionAudioCondition::Update(const Time& elapsed, const Time& delta)
{
}


// reset the condition
void StateTransitionAudioCondition::Reset()
{
	mLoops = 0;
}


// test the condition
bool StateTransitionAudioCondition::TestCondition()
{
	const int32 function = GetInt32Attribute( ATTRIB_FUNCTION );
	switch (function)
	{
		case FUNCTION_HASREACHEDMAXNUMLOOPS:
		{
			const int32 maxNumLoops = GetInt32Attribute( ATTRIB_NUMLOOPS );
			return (mLoops >= maxNumLoops);
		}

		default:
		{
			return (mLoops > 0);
		}
	}
}


// attributes have changed
void StateTransitionAudioCondition::OnAttributesChanged()
{
	StateTransitionCondition::OnAttributesChanged();

	// show/hide max number of loops attribute
	const int32 function = GetInt32Attribute( ATTRIB_FUNCTION );
	const bool isFunctionAttributeVisible = (function == FUNCTION_HASREACHEDMAXNUMLOOPS);
	GetAttributeSettings(ATTRIB_NUMLOOPS)->SetVisible( isFunctionAttributeVisible );

	// fire event
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_NUMLOOPS)) );
}
