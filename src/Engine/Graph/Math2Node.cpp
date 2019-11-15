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
#include "Math2Node.h"
#include "../Core/Math.h"


using namespace Core;

// constructor
Math2Node::Math2Node(Graph* graph) : ProcessorNode(graph, new Math2Node::Processor())
{
	// default settings
	mSettings.mMathFunction	 = MATHFUNCTION_ADD;
	mSettings.mCalculateFunc = CalculateAdd;
	mSettings.mDefaultValue = 1;
}


// destructor
Math2Node::~Math2Node()
{
}


// initialize the node
void Math2Node::Init()
{
	// init base class first
	ProcessorNode::Init();
	
	// SETUP PORTS

	// setup the input ports
	GetInputPort(INPUTPORT_X).Setup("x", "x1", AttributeChannels<double>::TYPE_ID, PORTID_INPUTPORT_X);	
	GetInputPort(INPUTPORT_Y).Setup("y", "x2", AttributeChannels<double>::TYPE_ID, PORTID_INPUTPORT_Y);	

	// setup the output ports
	GetOutputPort(OUTPUTPORT_RESULT).Setup(GetFunctionString(mSettings.mMathFunction), "y",AttributeChannels<double>::TYPE_ID, PORTID_OUTPUTPORT_RESULT);

	// SETUP ATTRIBUTES

	// create the math function combobox
	Core::AttributeSettings* functionParam = RegisterAttribute("Math Function", "mathFunction", "The math function to use.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	functionParam->ResizeComboValues( (uint32)MATHFUNCTION_NUMFUNCTIONS );
	functionParam->SetComboValue(MATHFUNCTION_ADD,		"Add");
	functionParam->SetComboValue(MATHFUNCTION_SUBTRACT,	"Subtract");
	functionParam->SetComboValue(MATHFUNCTION_MULTIPLY,	"Multiply");
	functionParam->SetComboValue(MATHFUNCTION_DIVIDE,	"Divide");
	functionParam->SetComboValue(MATHFUNCTION_AVERAGE,	"Average");
	functionParam->SetComboValue(MATHFUNCTION_RANDOMFLOAT,"Random Float");
	functionParam->SetComboValue(MATHFUNCTION_MOD,		"Mod");
	functionParam->SetComboValue(MATHFUNCTION_MIN,		"Minimum");
	functionParam->SetComboValue(MATHFUNCTION_MAX,		"Maximum");
	functionParam->SetComboValue(MATHFUNCTION_POW,		"Power");
	functionParam->SetDefaultValue( Core::AttributeInt32::Create(mSettings.mMathFunction) );

	// create the static value float spinner
	Core::AttributeSettings* valueParam = RegisterAttribute("Second Value", "staticValue", "Value used for x or y when the input port has no connection.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	valueParam->SetDefaultValue( Core::AttributeFloat::Create(mSettings.mDefaultValue) );
	valueParam->SetMinValue( Core::AttributeFloat::Create(-DBL_MAX) );
	valueParam->SetMaxValue( Core::AttributeFloat::Create(+DBL_MAX) );
}


void Math2Node::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);

	// determine if the inputs are compatible
	bool isCompatible = false;
	
	// FIXME why is this so ugly
	// HACK: set node error after reset ;)

	MultiChannel* channelsX = GetInputPort(INPUTPORT_X).GetChannels();
	MultiChannel* channelsY = GetInputPort(INPUTPORT_Y).GetChannels();

	// two channels: either one is Nonuniform, or both have the same samperate
	if (channelsX != NULL && channelsY != NULL)
	{
		const double sampleRateX = channelsX->GetSampleRate();
		const double sampleRateY = channelsY->GetSampleRate();

		if ( (sampleRateX == 0 && sampleRateY > 0) ||
			 (sampleRateX >  0 && sampleRateY == 0) || 
			 (sampleRateX == sampleRateY && sampleRateX != 0) )
		{
			isCompatible = true;	
		}

		
		if (isCompatible == false)
		{
			mIsInitialized = false;
			SetError(ERROR_INPUT_MATCHING_SAMPLERATES, "Input sample rates are incompatible.");
			return;
		}
		
	}
	
	ClearError(ERROR_INPUT_MATCHING_SAMPLERATES);
}


void Math2Node::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::Update(elapsed, delta);
}


// update the data
void Math2Node::OnAttributesChanged()
{
	EMathFunction function = (EMathFunction)GetInt32Attribute(ATTRIB_MATHFUNCTION);
	const double defaultValue = GetFloatAttribute(ATTRIB_STATICVALUE);

	// if it didn't change, don't update anything
	if (function == mSettings.mMathFunction && mSettings.mDefaultValue == defaultValue)
		return;
	
	mSettings.mMathFunction = function;
	switch (mSettings.mMathFunction)
	{
		case MATHFUNCTION_ADD:			mSettings.mCalculateFunc = CalculateAdd;			break;
		case MATHFUNCTION_SUBTRACT:		mSettings.mCalculateFunc = CalculateSubtract;		break;
		case MATHFUNCTION_MULTIPLY:		mSettings.mCalculateFunc = CalculateMultiply;		break;
		case MATHFUNCTION_DIVIDE:		mSettings.mCalculateFunc = CalculateDivide;			break;
		case MATHFUNCTION_AVERAGE:		mSettings.mCalculateFunc = CalculateAverage;		break;
		case MATHFUNCTION_RANDOMFLOAT:	mSettings.mCalculateFunc = CalculateRandomFloat;	break;
		case MATHFUNCTION_MOD:			mSettings.mCalculateFunc = CalculateMod;			break;
		case MATHFUNCTION_MIN:			mSettings.mCalculateFunc = CalculateMin;			break;
		case MATHFUNCTION_MAX:			mSettings.mCalculateFunc = CalculateMax;			break;
		case MATHFUNCTION_POW:			mSettings.mCalculateFunc = CalculatePow;			break;
		default: CORE_ASSERT(1==0);	// function unknown
	};

	mSettings.mDefaultValue = defaultValue;

	// output port name
	OutputPort& outputPort = GetOutputPort(OUTPUTPORT_RESULT);
	outputPort.SetName( GetFunctionString(function) );
	
	// reconfigure processors
	SetupProcessors();
}


const char* Math2Node::GetFunctionString(EMathFunction function)
{
	switch (function)
	{
		case MATHFUNCTION_ADD:			{ return "x + y"; }
		case MATHFUNCTION_SUBTRACT:		{ return "x - y"; }
		case MATHFUNCTION_MULTIPLY:		{ return "x * y"; }
		case MATHFUNCTION_DIVIDE:		{ return "x / y"; }
		case MATHFUNCTION_AVERAGE:		{ return "Average"; }
		case MATHFUNCTION_RANDOMFLOAT:	{ return "Random[x..y]"; }
		case MATHFUNCTION_MOD:			{ return "x MOD y"; }
		case MATHFUNCTION_MIN:			{ return "Min(x, y)"; }
		case MATHFUNCTION_MAX:			{ return "Max(x, y)"; }
		case MATHFUNCTION_POW:			{ return "Pow(x, y)"; }
        default:                        { return ""; }
	}
}

//-----------------------------------------------
// the math functions
//-----------------------------------------------
double Math2Node::CalculateAdd(double x, double y)				{ return x + y; }
double Math2Node::CalculateSubtract(double x, double y)			{ return x - y; }
double Math2Node::CalculateMultiply(double x, double y)			{ return x * y; }
double Math2Node::CalculateDivide(double x, double y)			{ if (Core::IsClose<double>(y, 0.0, Core::Math::epsilon) == false) return x / y; return 0.0f; }
double Math2Node::CalculateAverage(double x, double y)			{ return (x+y)*0.5f; }
double Math2Node::CalculateRandomFloat(double x, double y)		{ return Core::Math::RandF(x, y); }
double Math2Node::CalculateMod(double x, double y)				{ if (Core::IsClose<double>(y, 0.0, Core::Math::epsilon) == false) return Core::Math::FModD(x, y); return 0.0f; }
double Math2Node::CalculateMin(double x, double y)				{ return Core::Min<double>(x, y); }
double Math2Node::CalculateMax(double x, double y)				{ return Core::Max<double>(x, y); }
double Math2Node::CalculatePow(double x, double y)				{ if (Core::IsClose<double>(x, 0.0, Core::Math::epsilon) == true && y < 0.0f) return 0.0f; return Core::Math::PowD(x, y); }


//-----------------------------------------------
// the node's processor implementation
//-----------------------------------------------

void Math2Node::Processor::ReInit()					
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

	// determine output samplerate
	// Note: the output SR is zero if there is no channel with SR > 0
	double outputSampleRate = 0;
	if (inputX != NULL && inputX->GetSampleRate() > 0)
		outputSampleRate = inputX->GetSampleRate();
	else if (inputY != NULL && inputY->GetSampleRate() > 0)
		outputSampleRate = inputY->GetSampleRate();

	output->SetSampleRate(outputSampleRate);
				
	mIsInitialized = true;
}

void Math2Node::Processor::Update()
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

	// we need at least one connected channel
	if (channelX == NULL && channelY == NULL)
		return;

	// check what types of channels are available
	const bool haveChannelX = (channelX != NULL && channelX->GetSampleRate() > 0);
	const bool haveChannelY = (channelY != NULL && channelY->GetSampleRate() > 0);
	const bool haveNonuniformChannelX = (channelX != NULL && channelX->GetSampleRate() == 0);
	const bool haveNonuniformChannelY = (channelY != NULL && channelY->GetSampleRate() == 0);
	

	//
	// 1) get number of samples we can process first
	//

	uint32 numSamplesX = 0;
	uint32 numSamplesY = 0;
	uint32 numSamplesOut = 0;

	if (channelX != NULL) 
		numSamplesX = inputX->GetNumNewSamples();

	if (channelY != NULL) 
		numSamplesY = inputY->GetNumNewSamples();

	// Case 1.1: two channels connected, both with fixed samplerate -> process as much sample-pairs as possible
	if (haveChannelX && haveChannelY)
	{
		numSamplesOut = Core::Min<uint32>(numSamplesX, numSamplesY);
	}
	// Case 1.2: two channels connected, but only one has a fixed samplerate
	else if (haveChannelX && haveNonuniformChannelY)
	{
		numSamplesOut = numSamplesX;
	}
	else if (haveNonuniformChannelX && haveChannelY)
	{
		numSamplesOut = numSamplesY;
	}
	// Case 1.3: only one channel connected
	else if ( (haveChannelX || haveNonuniformChannelX) && !(haveChannelY || haveNonuniformChannelY) )
	{
		numSamplesOut = numSamplesX;
	}
	else if ( (haveChannelY || haveNonuniformChannelY) && !(haveChannelX || haveNonuniformChannelX) )
	{
		numSamplesOut = numSamplesY;
	}
	else {CORE_ASSERT(false);}


	//
	// 2) determine the default parameters (either from the processor settings, or the Non-Uniform channel)
	//

	double defaultValueX = mSettings.mDefaultValue;
	double defaultValueY = mSettings.mDefaultValue;

	if (haveNonuniformChannelX && channelX->GetNumSamples())
	{
		defaultValueX = channelX->AsType<double>()->GetLastSample();
		
		// if the other channel has constant samplerate we must clear the input reader here, because we don't pop the samples in the loop below
		if (haveChannelY)
			inputX->Flush();
	}

	if (haveNonuniformChannelY && channelY->GetNumSamples())
	{
		defaultValueY = channelY->AsType<double>()->GetLastSample();
		
		// if the other channel has constant samplerate we must clear the input reader here, because we don't pop the samples in the loop below
		if (haveChannelX)
			inputY->Flush();
	}


	//
	// 3) produce the output samples
	//

	for (uint32 i=0; i<numSamplesOut; ++i)
	{
		double x, y;
		
		// channel x
		if (haveChannelX || (haveNonuniformChannelX && !haveChannelY) )
			x = inputX->PopOldestSample<double>();
		else
			x = defaultValueX;

		// channel y
		if (haveChannelY || (!haveChannelX && haveNonuniformChannelY) )
			y = inputY->PopOldestSample<double>();
		else
			y = defaultValueY;

		// apply math function
		const double result = mSettings.mCalculateFunc(x,y);

		// add result to output
		output->AsType<double>()->AddSample(result);
	}
}
