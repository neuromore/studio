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
#include "RemapNode.h"
#include "../Core/Math.h"


using namespace Core;

// constructor
RemapNode::RemapNode(Graph* graph) : ProcessorNode(graph, new RemapNode::Processor())
{
	mSettings.mMinInput = 0.0;
	mSettings.mMaxInput = 1.0;
	mSettings.mMinOutput = 0.0;
	mSettings.mMaxOutput = 1.0;
	mSettings.mClampOutput = false;
}


// destructor
RemapNode::~RemapNode()
{
}


// initialize the node
void RemapNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	// configure SPNode behaviour
	RequireMatchingSampleRates();
	
	// PORTS

	// setup the input ports
	GetInputPort(INPUTPORT_X).Setup( "x", "x1", AttributeChannels<double>::TYPE_ID, INPUTPORT_MAX_IN);	
	GetInputPort(INPUTPORT_MAX_IN).Setup( "In Max",  "x2", AttributeChannels<double>::TYPE_ID, INPUTPORT_MAX_IN);
	GetInputPort(INPUTPORT_MIN_IN).Setup( "In Min",  "x3", AttributeChannels<double>::TYPE_ID, INPUTPORT_MIN_IN);
	GetInputPort(INPUTPORT_MAX_OUT).Setup("Out Max", "x4", AttributeChannels<double>::TYPE_ID, INPUTPORT_MAX_OUT);
	GetInputPort(INPUTPORT_MIN_OUT).Setup("Out Min", "x5", AttributeChannels<double>::TYPE_ID, INPUTPORT_MIN_OUT);

	// setup the output ports
	GetOutputPort(OUTPUTPORT_RESULT).Setup("Out", "y", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_RESULT);

	// ATTRIBUTES

	// input min
	Core::AttributeSettings* param = RegisterAttribute("Input Min", "inputMin", "The minimum incoming value. Values smaller than this will be clipped.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	param->SetDefaultValue(	Core::AttributeFloat::Create(mSettings.mMinInput) );
	param->SetMinValue(	Core::AttributeFloat::Create(-FLT_MAX) );
	param->SetMaxValue( Core::AttributeFloat::Create(+FLT_MAX) );

	// input max
	param = RegisterAttribute("Input Max", "inputMax", "The maximum incoming value. Values bigger than this will be clipped.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	param->SetDefaultValue(	Core::AttributeFloat::Create(mSettings.mMaxInput) );
	param->SetMinValue( Core::AttributeFloat::Create(-FLT_MAX) );
	param->SetMaxValue( Core::AttributeFloat::Create(+FLT_MAX) );

	// output min
	param = RegisterAttribute("Output Min", "outputMin", "The minimum outcoming value. The minimum incoming value will be mapped to the minimum outcoming value. The output port can't hold a smaller value than this.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	param->SetDefaultValue(	Core::AttributeFloat::Create(mSettings.mMinOutput) );
	param->SetMinValue( Core::AttributeFloat::Create(-FLT_MAX) );
	param->SetMaxValue( Core::AttributeFloat::Create(+FLT_MAX) );

	// output max
	param = RegisterAttribute("Output Max", "outputMax", "The maximum outcoming value. The maximum incoming value will be mapped to the maximum outcoming value. The output port can't hold a bigger value than this.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	param->SetDefaultValue(	Core::AttributeFloat::Create(mSettings.mMaxOutput) );
	param->SetMinValue( Core::AttributeFloat::Create(-FLT_MAX) );
	param->SetMaxValue(	Core::AttributeFloat::Create(+FLT_MAX) );

	// clamp checkbox
	param = RegisterAttribute("Clamp Output", "clampOutput", "Clamp the output between the minimum and maximum value.", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	param->SetDefaultValue(	Core::AttributeBool::Create(mSettings.mClampOutput) );
}


void RemapNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	// require connection on the value input
	if (GetInputPort(INPUTPORT_X).GetChannels() == NULL)
		mIsInitialized = false;
	else if (GetInputPort(INPUTPORT_X).GetChannels()->GetNumChannels() == 0)
		mIsInitialized = false;


	PostReInit(elapsed, delta);
}


void RemapNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::Update(elapsed, delta);
}


void RemapNode::OnAttributesChanged()
{
	const double inputMin = GetFloatAttribute (ATTRIB_INPUTMIN);
	const double inputMax = GetFloatAttribute (ATTRIB_INPUTMAX);
	const double outputMin = GetFloatAttribute(ATTRIB_OUTPUTMIN);
	const double outputMax = GetFloatAttribute(ATTRIB_OUTPUTMAX);
	const bool clampOutput = GetBoolAttribute (ATTRIB_CLAMP);

	mSettings.mMinInput = inputMin;
	mSettings.mMaxInput = inputMax;
	mSettings.mMinOutput = outputMin;
	mSettings.mMaxOutput = outputMax;
	mSettings.mClampOutput  = clampOutput;

	// port names
	mTempString.Format("[%g, %g]", inputMin, inputMax);
	GetInputPort(INPUTPORT_X).SetName(mTempString.AsChar());

	mTempString.Format("[%g, %g]", outputMin, outputMax);
	GetOutputPort(OUTPUTPORT_RESULT).SetName(mTempString.AsChar());

	// update settings
	SetupProcessors();
}


void RemapNode::Processor::ReInit()					
{ 
	// ReInit baseclass
	ChannelProcessor::ReInit();

	mIsInitialized = false;
				
	ChannelBase* input  = GetInput();
	ChannelBase* output = GetOutput();

	// nothing to do until both channels are connected
	if (input == NULL || output == NULL)
		return;

	double outputSampleRate = input->GetSampleRate();
	output->SetSampleRate(outputSampleRate);
				
	mIsInitialized = true;
}


void RemapNode::Processor::Update()
{
	if (mIsInitialized == false)
		return;
			
	// update base
	ChannelProcessor::Update();
		
	// channel input reader
	ChannelReader* input = GetInputReader(INPUTPORT_X);

	// input/output channels
	ChannelBase* channel = GetInput();
	Channel<double>* output = GetOutput()->AsType<double>();

	// we need at least one connected channel
	if (channel == NULL)
		return;

	// get the current min/max values
	const bool haveMaxInChannel  = (GetInput(INPUTPORT_MAX_IN) != NULL);
	const bool haveMinInChannel  = (GetInput(INPUTPORT_MIN_IN) != NULL);
	const bool haveMaxOutChannel = (GetInput(INPUTPORT_MAX_OUT) != NULL);
	const bool haveMinOutChannel = (GetInput(INPUTPORT_MIN_OUT) != NULL);
	
	ChannelReader* inputMaxIn = GetInputReader(INPUTPORT_MAX_IN);
	ChannelReader* inputMinIn = GetInputReader(INPUTPORT_MIN_IN);
	ChannelReader* inputMaxOut = GetInputReader(INPUTPORT_MAX_OUT);
	ChannelReader* inputMinOut = GetInputReader(INPUTPORT_MIN_OUT);

	// number of new samples we can read
	const uint32 numSamples = input->GetNumNewSamples();
	const uint32 numSamplesMaxIn  = (haveMaxInChannel  ? inputMaxIn->GetNumNewSamples()  : numSamples);
	const uint32 numSamplesMinIn  = (haveMinInChannel  ? inputMinIn->GetNumNewSamples()  : numSamples);
	const uint32 numSamplesMaxOut = (haveMaxOutChannel ? inputMaxOut->GetNumNewSamples() : numSamples);
	const uint32 numSamplesMinOut = (haveMinOutChannel ? inputMinOut->GetNumNewSamples() : numSamples);
	
	const uint32 minNumSamples = Core::Min3( Core::Min(numSamplesMaxIn,  numSamplesMinIn), 
											 Core::Min(numSamplesMaxOut, numSamplesMinOut),
											 numSamples);

	// process input samples
	double value = 0;
	for (uint32 i=0; i<minNumSamples; ++i)
	{
		const double x = input->PopOldestSample<double>();
		const double maxInput  = (haveMaxInChannel  ? inputMaxIn->PopOldestSample<double>()  : mSettings.mMaxInput);
		const double minInput  = (haveMinInChannel  ? inputMinIn->PopOldestSample<double>()  : mSettings.mMinInput);
		const double maxOutput = (haveMaxOutChannel ? inputMaxOut->PopOldestSample<double>() : mSettings.mMaxOutput);
		const double minOutput = (haveMinOutChannel ? inputMinOut->PopOldestSample<double>() : mSettings.mMinOutput);

		// remap
		if (mSettings.mClampOutput == true)
			value = ClampedRemapRange( x, minInput, maxInput, minOutput, maxOutput );
		else
			value = RemapRange( x, minInput, maxInput, minOutput, maxOutput );

		// add result to output
		output->AddSample(value);
	}
}
