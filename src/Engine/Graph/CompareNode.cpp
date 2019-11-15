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
#include "CompareNode.h"
#include "../Core/Math.h"
#include "../Core/EventManager.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
CompareNode::CompareNode(Graph* graph) : ProcessorNode(graph, new CompareNode::Processor())
{
	// set the defaults
	mSettings.mFunction	= FUNCTION_EQUAL;
	mSettings.mCalculateFunc = CompareEqual;
	mSettings.mStaticValue = 0.0;
	mSettings.mTrueValue = 1.0;
	mSettings.mFalseValue = 0.0;
	mSettings.mTrueReturnMode = MODE_VALUE;
	mSettings.mFalseReturnMode = MODE_VALUE;
}


// destructor
CompareNode::~CompareNode()
{
}


// initialize the node
void CompareNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	// configure SPNode behaviour
	RequireMatchingSampleRates();
	
	// PORTS

	// setup the input ports
	GetInputPort(INPUTPORT_X).Setup( "x", "x1", AttributeChannels<double>::TYPE_ID, INPUTPORT_X);
	GetInputPort(INPUTPORT_Y).Setup( "y", "x2", AttributeChannels<double>::TYPE_ID, INPUTPORT_Y);

	// setup the output ports
	GetOutputPort(OUTPUTPORT_VALUE).Setup(GetFunctionString(mSettings.mFunction), "y1", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_VALUE);

	// ATTRIBUTES

	// create the compare function combobox
	Core::AttributeSettings* functionParam = RegisterAttribute("Function", "conditionFunction", "The copmarator function to use.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	functionParam->ResizeComboValues( (uint32)NUM_FUNCTIONS );
	functionParam->SetComboValue(FUNCTION_EQUAL,			"Equal to \t(x == y)");
	functionParam->SetComboValue(FUNCTION_GREATER,			"Greater than \t(x > y)");
	functionParam->SetComboValue(FUNCTION_LESS,				"Less than \t(x < y)");
	functionParam->SetComboValue(FUNCTION_GREATEROREQUAL,   "Grt. or Equal \t(x >= y)");
	functionParam->SetComboValue(FUNCTION_LESSOREQUAL,		"Less or Equal \t(x <= y)");
	functionParam->SetComboValue(FUNCTION_INEQUAL,			"Inequal to \t(x != y)");
	functionParam->SetDefaultValue(	Core::AttributeInt32::Create(mSettings.mFunction) );

	// create the static value float spinner
	Core::AttributeSettings* valueParam = RegisterAttribute("Second Value", "staticValue", "Value used for x or y when the input port has no connection.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	valueParam->SetDefaultValue( Core::AttributeFloat::Create(mSettings.mStaticValue) );
	valueParam->SetMinValue( Core::AttributeFloat::Create(-DBL_MAX) );
	valueParam->SetMaxValue( Core::AttributeFloat::Create(DBL_MAX) );

	// create true return mode dropdown
	Core::AttributeSettings* trueModeParam = RegisterAttribute("True Return Mode", "trueMode", "What to return when the result is true.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	trueModeParam->ResizeComboValues(3);
	trueModeParam->SetComboValue(MODE_VALUE, "Return True Value");
	trueModeParam->SetComboValue(MODE_X, "Return X");
	trueModeParam->SetComboValue(MODE_Y, "Return Y");
	trueModeParam->SetDefaultValue(Core::AttributeInt32::Create(mSettings.mTrueReturnMode));

	// create true return value spinner
	Core::AttributeSettings* trueValueParam = RegisterAttribute("Result when True", "trueResult", "The value returned when the expression is true.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	trueValueParam->SetDefaultValue( Core::AttributeFloat::Create(mSettings.mTrueValue) );
	trueValueParam->SetMinValue( Core::AttributeFloat::Create(-DBL_MAX) );
	trueValueParam->SetMaxValue( Core::AttributeFloat::Create(DBL_MAX) );

	// create false return mode dropdown
	Core::AttributeSettings* falseModeParam = RegisterAttribute("False Return Mode", "falseMode", "What to return when the result is false.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	falseModeParam->ResizeComboValues(3);
	falseModeParam->SetComboValue(MODE_VALUE, "Return False Value");
	falseModeParam->SetComboValue(MODE_X, "Return X");
	falseModeParam->SetComboValue(MODE_Y, "Return Y");
	falseModeParam->SetDefaultValue(Core::AttributeInt32::Create(mSettings.mFalseReturnMode));

	// create false return value spinner
	Core::AttributeSettings* falseValueParam = RegisterAttribute("Result when False", "falseResult", "The value returned when the expression is false.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	falseValueParam->SetDefaultValue( Core::AttributeFloat::Create(mSettings.mFalseValue) );
	falseValueParam->SetMinValue( Core::AttributeFloat::Create(-DBL_MAX) );
	falseValueParam->SetMaxValue( Core::AttributeFloat::Create(DBL_MAX) );
}


void CompareNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


void CompareNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::Update(elapsed, delta);
}


// update the selected function
void CompareNode::OnAttributesChanged()
{
	const ECompareFunction function = (ECompareFunction)GetInt32Attribute(ATTRIB_FUNCTION);
	if (mSettings.mFunction != function)
	{
		mSettings.mFunction = function;

		// set function
		switch (function)
		{
			case FUNCTION_EQUAL:			{ mSettings.mCalculateFunc = CompareEqual; break; }
			case FUNCTION_GREATER:			{ mSettings.mCalculateFunc = CompareGreater; break; }
			case FUNCTION_LESS:				{ mSettings.mCalculateFunc = CompareLess; break; }
			case FUNCTION_GREATEROREQUAL:	{ mSettings.mCalculateFunc = CompareGreaterOrEqual; break; }
			case FUNCTION_LESSOREQUAL:		{ mSettings.mCalculateFunc = CompareLessOrEqual; break; }
			case FUNCTION_INEQUAL:			{ mSettings.mCalculateFunc = CompareInequal; break; }
			default: CORE_ASSERT(false);	// function unknown
		};
		
		// set output port name
		GetOutputPort(OUTPUTPORT_VALUE).SetName(GetFunctionString(function));
	}

	mSettings.mStaticValue = GetFloatAttribute(ATTRIB_STATICVALUE);
	mSettings.mTrueValue = GetFloatAttribute(ATTRIB_TRUEVALUE);
	mSettings.mFalseValue = GetFloatAttribute(ATTRIB_FALSEVALUE);
	mSettings.mTrueReturnMode = (EReturnMode)GetInt32Attribute(ATTRIB_TRUEMODE);
	mSettings.mFalseReturnMode = (EReturnMode)GetInt32Attribute(ATTRIB_FALSEMODE);

	// show true/false value attrib only if the mode is selected
	GetAttributeSettings(ATTRIB_TRUEVALUE)->SetVisible(mSettings.mTrueReturnMode == EReturnMode::MODE_VALUE);
	GetAttributeSettings(ATTRIB_FALSEVALUE)->SetVisible(mSettings.mFalseReturnMode == EReturnMode::MODE_VALUE);

	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_TRUEVALUE)) );
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_FALSEVALUE)) );

	// reconfigure processors
	SetupProcessors();
}


const char* CompareNode::GetFunctionString(ECompareFunction function)
{
	switch (function)
	{
		case FUNCTION_EQUAL:			{ return "x == y"; }
		case FUNCTION_GREATER:			{ return "x > y"; }
		case FUNCTION_LESS:				{ return "x < y"; }
		case FUNCTION_GREATEROREQUAL:	{ return "x >= y"; }
		case FUNCTION_LESSOREQUAL:		{ return "x <= y"; }
		case FUNCTION_INEQUAL:			{ return "x != y"; }
        default:                        { return ""; }
	}
}


//-----------------------------------------------
// the comparator functions
//-----------------------------------------------
bool CompareNode::CompareEqual(double x, double y)				{ return Core::IsClose<double>(x, y, Core::Math::epsilon); }
bool CompareNode::CompareGreater(double x, double y)			{ return (x > y); }
bool CompareNode::CompareLess(double x, double y)				{ return (x < y); }
bool CompareNode::CompareGreaterOrEqual(double x, double y)		{ return (x >= y); }
bool CompareNode::CompareLessOrEqual(double x, double y)		{ return (x <= y); }
bool CompareNode::CompareInequal(double x, double y)			{ return !CompareEqual(x,y); }
 

//-----------------------------------------------
// the node's processor implementation
//-----------------------------------------------

void CompareNode::Processor::ReInit()					
{ 
	// ReInit baseclass
	ChannelProcessor::ReInit();

	mIsInitialized = false;
				
	ChannelBase* inputX = GetInput();
	ChannelBase* inputY = GetInput(1);
	ChannelBase* output = GetOutput();

	// nothing to do until both channels are connected
	if ( (inputX == NULL && inputY == NULL) || output == NULL)
		return;

	// just use sample rate of first channel, assume they are identical
	double outputSampleRate = 0;
	if (inputX != NULL)
		outputSampleRate = inputX->GetSampleRate();
	else if (inputY != NULL)
		outputSampleRate = inputY->GetSampleRate();

	output->SetSampleRate(outputSampleRate);
				
	mIsInitialized = true;
}


void CompareNode::Processor::Update()
{
	if (mIsInitialized == false)
		return;
	
	// update base
	ChannelProcessor::Update();
				
	// channel input reader
	ChannelReader* inputX = GetInputReader(0);
	ChannelReader* inputY = GetInputReader(1);

	// input/output channels
	ChannelBase* channelX = GetInput(0);
	ChannelBase* channelY = GetInput(1);
	ChannelBase* output = GetOutput();

	// check what channels are available
	const bool haveChannelX = (channelX != NULL);
	const bool haveChannelY = (channelY != NULL);

	// we need at least one connected channel
	if (haveChannelX == false && haveChannelY == false)
		return;

	uint32 numSamplesX = 0;
	uint32 numSamplesY = 0;
	uint32 numSamples = 0;
	//double sampleRate = 0;

	// 1) channel x is connected
	if (haveChannelX == true) 
	{
		// get number of samples
		numSamplesX = inputX->GetNumNewSamples();

		// in case only this channel is connected
		if (haveChannelY == false)
		{
			numSamples = numSamplesX;
			//sampleRate = channelX->GetSampleRate();
		}
	}

	// 2) channel y is connected
	if (haveChannelY == true) 
	{
		// get number of samples
		numSamplesY = inputY->GetNumNewSamples();

		// in case only this channel is connected
		if (haveChannelX == false)
		{
			numSamples = numSamplesY;
			//sampleRate = channelY->GetSampleRate();
		}
	}

	// 3) both channels are connected
	if (haveChannelX == true && haveChannelY == true)
	{
		// get the number of processable samples
		numSamples = Core::Min<uint32>(numSamplesX, numSamplesY);
	}
				
	// process input samples
	for (uint32 i=0; i<numSamples; ++i)
	{
		double x, y;
		
		// channel x value and time
		if (haveChannelX == true)
			x = inputX->PopOldestSample<double>();
		else
			x = mSettings.mStaticValue;

		// channel y value and time
		if (haveChannelY == true)
			y = inputY->PopOldestSample<double>();
		else
			y = mSettings.mStaticValue;

		// compare both values
		const bool compareResult = mSettings.mCalculateFunc(x,y);
		double result = 0.0;

		// output value depends on settings
		if (compareResult == true)
		{
			// TRUE
			if (mSettings.mTrueReturnMode == CompareNode::MODE_VALUE)
				result = mSettings.mTrueValue;
			else if (mSettings.mTrueReturnMode == CompareNode::MODE_X)
				result = x;
			else 
				result = y;
		}
		else
		{
			// FALSE
			if (mSettings.mFalseReturnMode == CompareNode::MODE_VALUE)
				result = mSettings.mFalseValue;
			else if (mSettings.mFalseReturnMode == CompareNode::MODE_X)
				result = x;
			else 
				result = y;
		}

		// add output sample
		output->AsType<double>()->AddSample(result);
	}
}
