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
#include "StateTransitionFeedbackCondition.h"
#include "../EngineManager.h"
#include "../Core/AttributeSettings.h"
#include "../Core/Math.h"


using namespace Core;

// constructor
StateTransitionFeedbackCondition::StateTransitionFeedbackCondition(Graph* graph) : StateTransitionCondition(graph)
{
	// test function
	mTestFunction		= TestGreater;
	mFunction			= FUNCTION_GREATER;
}


// destructor
StateTransitionFeedbackCondition::~StateTransitionFeedbackCondition()
{
}


// register the attributes
void StateTransitionFeedbackCondition::Init()
{
	// register attributes
	AttributeSettings* attribInfo;

	// register the source motion file name
	attribInfo = RegisterAttribute("Feedback Node Name", "feedback", "The name of the feedback node to apply the condition on.", ATTRIBUTE_INTERFACETYPE_STRING);
	attribInfo->SetDefaultValue( AttributeString::Create() );

	// create the test value float spinner
	attribInfo = RegisterAttribute("Test Value", "testValue", "The float value to test against the feedback value.", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attribInfo->SetDefaultValue( AttributeFloat::Create(0.0) );
	attribInfo->SetMinValue( AttributeFloat::Create(-FLT_MAX) );
	attribInfo->SetMaxValue( AttributeFloat::Create(FLT_MAX) );

	// create the range value
	attribInfo = RegisterAttribute("Range Value", "rangeValue", "The range high or low bound value, only used when the function is set to 'In Range' or 'Not in Range'.", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attribInfo->SetDefaultValue( AttributeFloat::Create(0.0) );
	attribInfo->SetMinValue( AttributeFloat::Create(-FLT_MAX) );
	attribInfo->SetMaxValue( AttributeFloat::Create(FLT_MAX) );
	attribInfo->SetVisible( false );

	// create the function combobox
	attribInfo = RegisterAttribute("Test Function", "testFunction", "The type of test function or condition.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attribInfo->ResizeComboValues( FUNCTION_NUMFUNCTIONS );
	attribInfo->SetComboValue(FUNCTION_GREATER,		"feedback > testValue");
	attribInfo->SetComboValue(FUNCTION_GREATEREQUAL,"feedback >= testValue");
	attribInfo->SetComboValue(FUNCTION_LESS,		"feedback < testValue");
	attribInfo->SetComboValue(FUNCTION_LESSEQUAL,	"feedback <= testValue");
	attribInfo->SetComboValue(FUNCTION_NOTEQUAL,	"feedback != testValue");
	attribInfo->SetComboValue(FUNCTION_EQUAL,		"feedback == testValue");
	attribInfo->SetComboValue(FUNCTION_INRANGE,		"feedback INRANGE [testValue..rangeValue]");
	attribInfo->SetComboValue(FUNCTION_NOTINRANGE,	"feedback NOT INRANGE [testValue..rangeValue]");
	attribInfo->SetDefaultValue( AttributeInt32::Create(mFunction) );
}


// update the passed time of the condition
void StateTransitionFeedbackCondition::Update(const Time& elapsed, const Time& delta)
{
	// nothing to do
}


// reset the condition
void StateTransitionFeedbackCondition::Reset()
{
	// nothing to do
}


// test the condition
bool StateTransitionFeedbackCondition::TestCondition()
{
	// get access to the active classifier
	Classifier* activeClassifier = GetEngine()->GetActiveClassifier();
	if (activeClassifier == NULL)
		return false;

	// get access to the feedback node
	CustomFeedbackNode* feedbackNode = NULL;
	const uint32 numCustomFeedbackNodes = activeClassifier->GetNumCustomFeedbackNodes();
	for (uint32 i=0; i<numCustomFeedbackNodes; ++i)
	{
		if (activeClassifier->GetCustomFeedbackNode(i)->GetNameString().IsEqual( GetStringAttribute(ATTRIB_FEEDBACK) ) == true)
		{
			feedbackNode = activeClassifier->GetCustomFeedbackNode(i);
			break;
		}
	}

	if (feedbackNode == NULL || feedbackNode->IsEmpty() == true)
		return false;

	const double feedbackValue	= feedbackNode->GetCurrentValue();
	const double testValue		= GetFloatAttribute( ATTRIB_TESTVALUE );
	const double rangeValue		= GetFloatAttribute( ATTRIB_RANGEVALUE );

	// now apply the function
	return mTestFunction( feedbackValue, testValue, rangeValue );
}


// set the math function to use
void StateTransitionFeedbackCondition::SetFunction(EFunction func)
{
	// if it didn't change, don't update anything
	if (func == mFunction)
		return;

	mFunction = func;
	switch (mFunction)
	{
		case FUNCTION_GREATER:			mTestFunction = TestGreater;		return;
		case FUNCTION_GREATEREQUAL:		mTestFunction = TestGreaterEqual;	return;
		case FUNCTION_LESS:				mTestFunction = TestLess;			return;
		case FUNCTION_LESSEQUAL:		mTestFunction = TestLessEqual;		return;
		case FUNCTION_NOTEQUAL:			mTestFunction = TestNotEqual;		return;
		case FUNCTION_EQUAL:			mTestFunction = TestEqual;			return;
		case FUNCTION_INRANGE:			mTestFunction = TestInRange;		return;
		case FUNCTION_NOTINRANGE:		mTestFunction = TestNotInRange;		return;
		default:						CORE_ASSERT(1==0);					return; // function unknown
	};
}


// update the data
void StateTransitionFeedbackCondition::OnAttributesChanged()
{
	StateTransitionCondition::OnAttributesChanged();

	// update the test function pointer
	const int32 function = GetInt32Attribute(ATTRIB_FUNCTION);
	SetFunction( (EFunction)function );

	// disable the range value if we're not using a function that needs the range
	AttributeSettings* rangeAttributeSettings = GetAttributeSettings( ATTRIB_RANGEVALUE );
	if (function == FUNCTION_INRANGE || function == FUNCTION_NOTINRANGE)
		rangeAttributeSettings->SetVisible( true );
	else
		rangeAttributeSettings->SetVisible( false );


	// fire event
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_RANGEVALUE)) );
}


//------------------------------------------------------------------------------------------
// Test Functions
//------------------------------------------------------------------------------------------
bool StateTransitionFeedbackCondition::TestGreater(double paramValue, double testValue, double rangeValue)				{ return (paramValue > testValue);	}
bool StateTransitionFeedbackCondition::TestGreaterEqual(double paramValue, double testValue, double rangeValue)			{ return (paramValue >= testValue);	}
bool StateTransitionFeedbackCondition::TestLess(double paramValue, double testValue, double rangeValue)					{ return (paramValue < testValue);	}
bool StateTransitionFeedbackCondition::TestLessEqual(double paramValue, double testValue, double rangeValue)			{ return (paramValue <= testValue); }
bool StateTransitionFeedbackCondition::TestEqual(double paramValue, double testValue, double rangeValue)				{ return IsClose<double>(paramValue, testValue, Math::epsilon); }
bool StateTransitionFeedbackCondition::TestNotEqual(double paramValue, double testValue, double rangeValue)				{ return (IsClose<double>(paramValue, testValue, Math::epsilon) == false); }

bool StateTransitionFeedbackCondition::TestInRange(double paramValue, double testValue, double rangeValue)
{
	if (testValue <= rangeValue)
		return (InRange<double>( paramValue, testValue, rangeValue ) == true);
	else
		return (InRange<double>( paramValue, rangeValue, testValue ) == true);
}

bool StateTransitionFeedbackCondition::TestNotInRange(double paramValue, double testValue, double rangeValue)	
{
	if (testValue <= rangeValue)
		return (InRange<double>( paramValue, testValue, rangeValue ) == false);
	else
		return (InRange<double>( paramValue, rangeValue, testValue ) == false);
}
