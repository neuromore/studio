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
#include "LogicNode.h"


using namespace Core;

// constructor
LogicNode::LogicNode(Graph* graph) : ProcessorNode(graph, new LogicNode::Processor())
{
	// default on logic AND
	mSettings.mFunction = FUNCTION_AND;
	mSettings.mCalculateFunc = BoolLogicAND;
	mSettings.mStaticValue = 0.0;
	mSettings.mTrueValue = 1.0;
	mSettings.mFalseValue = 0.0;
}


// destructor
LogicNode::~LogicNode()
{
}


// initialize node
void LogicNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	// configure SPNode behaviour
	RequireMatchingSampleRates();
	
	// PORTS

	// setup the input ports
	GetInputPort(INPUTPORT_X).Setup("x", "x1", AttributeChannels<double>::TYPE_ID, PORTID_INPUT_X);
	GetInputPort(INPUTPORT_Y).Setup("y", "x2", AttributeChannels<double>::TYPE_ID, PORTID_INPUT_Y);

	// setup the output ports
	GetOutputPort(OUTPUTPORT_VALUE).Setup(GetFunctionString(mSettings.mFunction), "y1", AttributeChannels<double>::TYPE_ID, PORTID_OUTPUT_VALUE);

	// ATTRIBUTES

	// create the function combobox
	Core::AttributeSettings* functionParam = RegisterAttribute("Logic Function", "logicFunction", "The logic function to use.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	functionParam->ResizeComboValues( (uint32)NUM_FUNCTIONS );
	functionParam->SetComboValue(FUNCTION_AND,	"AND");
	functionParam->SetComboValue(FUNCTION_OR,	"OR");
	functionParam->SetComboValue(FUNCTION_XOR,	"XOR");
	functionParam->SetDefaultValue( Core::AttributeInt32::Create(mSettings.mFunction) );

	// create the static value float spinner
	Core::AttributeSettings* valueParam = RegisterAttribute("Second Value", "staticValue", "Value used for x or y when the input port has no connection.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	valueParam->ResizeComboValues( 2 );
	valueParam->SetComboValue(0, "False");
	valueParam->SetComboValue(1, "True");
	valueParam->SetDefaultValue( Core::AttributeInt32::Create(mSettings.mStaticValue) );

	Core::AttributeSettings* trueValueParam = RegisterAttribute("Float Result when True", "trueResult", "The float value returned when the expression is true.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	trueValueParam->SetDefaultValue( Core::AttributeFloat::Create(mSettings.mTrueValue) );
	trueValueParam->SetMinValue( Core::AttributeFloat::Create(-DBL_MAX) );
	trueValueParam->SetMaxValue( Core::AttributeFloat::Create(+DBL_MAX) );

	Core::AttributeSettings* falseValueParam = RegisterAttribute("Float Result when False", "falseResult", "The float value returned when the expression is false.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	falseValueParam->SetDefaultValue( Core::AttributeFloat::Create(mSettings.mFalseValue) );
	falseValueParam->SetMinValue( Core::AttributeFloat::Create(-DBL_MAX) );
	falseValueParam->SetMaxValue( Core::AttributeFloat::Create(+DBL_MAX) );
}


void LogicNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


void LogicNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::Update(elapsed, delta);
}


// update the data
void LogicNode::OnAttributesChanged()
{
	const ELogicFunction function =(ELogicFunction)GetInt32Attribute(ATTRIB_FUNCTION);
	
	if (mSettings.mFunction != function)
	{
		// set function
		mSettings.mFunction = function;
		switch (function)
		{
			case FUNCTION_AND:	{ mSettings.mCalculateFunc = BoolLogicAND; break; }
			case FUNCTION_OR:	{ mSettings.mCalculateFunc = BoolLogicOR; break; }
			case FUNCTION_XOR:	{ mSettings.mCalculateFunc = BoolLogicXOR; break; }
			default: CORE_ASSERT(1==0);	// function unknown
		};

		// set output port name
		GetOutputPort(OUTPUTPORT_VALUE).SetName(GetFunctionString(function));
	}

	const double trueValue = GetFloatAttribute(ATTRIB_TRUEVALUE);
	const double falseValue = GetFloatAttribute(ATTRIB_FALSEVALUE);

	mSettings.mTrueValue = trueValue;
	mSettings.mFalseValue = falseValue;

	// reconfigure processors
	SetupProcessors();
}


const char* LogicNode::GetFunctionString(ELogicFunction function)
{
	switch (function)
	{
		case FUNCTION_AND:		{ return "x AND y"; }
		case FUNCTION_OR:		{ return "x OR y"; }
		case FUNCTION_XOR:		{ return "x XOR y"; }
        default:                { return ""; }
	}
}


//-----------------------------------------------
// the condition functions
//-----------------------------------------------
bool LogicNode::BoolLogicAND(bool x, bool y)		{ return (x && y); }
bool LogicNode::BoolLogicOR(bool x, bool y)			{ return (x || y); }
bool LogicNode::BoolLogicXOR(bool x, bool y)		{ return (x ^ y); }


//-----------------------------------------------
// the node's processor implementation
//-----------------------------------------------

void LogicNode::Processor::ReInit()					
{ 
	// ReInit baseclass
	ChannelProcessor::ReInit();

	mIsInitialized = false;
				
	ChannelBase* inputX = GetInput(0);
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


void LogicNode::Processor::Update()
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
	Channel<double>* output = GetOutput()->AsType<double>();

	// check what channels are available
	const bool haveChannelX = (channelX != NULL);
	const bool haveChannelY = (channelY != NULL);

	// we need at least one connected channel
	if (haveChannelX == false && haveChannelY == false)
		return;

	uint32 numSamplesX = 0;
	uint32 numSamplesY = 0;
	uint32 numSamples = 0;

	// 1) channel x is connected
	if (haveChannelX == true) 
	{
		// get number of samples
		numSamplesX = inputX->GetNumNewSamples();

		// in case only this channel is connected
		if (haveChannelY == false)
			numSamples = numSamplesX;
	}

	// 2) channel y is connected
	if (haveChannelY == true) 
	{
		// get number of samples
		numSamplesY = inputY->GetNumNewSamples();

		// in case only this channel is connected
		if (haveChannelX == false)
			numSamples = numSamplesY;
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
		// float input values
		double xf, yf;

		// channel x value and time
		if (haveChannelX == true)
			xf = inputX->PopOldestSample<double>();
		else
			xf = mSettings.mStaticValue;

		// channel y value and time
		if (haveChannelY == true)
			yf = inputY->PopOldestSample<double>();
		else
			yf = mSettings.mStaticValue;

		// IMHO: binary compare seems more correct than epsilon thresholding
		// value is true if it is not exactly zero
		const bool x = (xf != 0.0);
		const bool y = (yf != 0.0);

		// compare both values
		const bool boolResult = mSettings.mCalculateFunc(x,y);
		double result = 0.0;

		// output value depends on settings
		if (boolResult == true)
			result = mSettings.mTrueValue;
		else
			result = mSettings.mFalseValue;

		// add output sample
		output->AddSample(result);
	}
}
