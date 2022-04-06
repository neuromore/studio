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

// include required headers
#include "SmoothNode.h"
#include "../Core/Math.h"


using namespace Core;

// constructor
SmoothNode::SmoothNode(Graph* graph) : ProcessorNode(graph, new SmoothNode::Processor())
{
	// default settings
	mSettings.mInterpolationSpeed	= 0.75;
	mSettings.mStartValue			= 0.0;
}


// destructor
SmoothNode::~SmoothNode()
{
}


// initialize the node
void SmoothNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	// PORTS

	GetInputPort(INPUTPORT).Setup("In", "x",  AttributeChannels<double>::TYPE_ID, PORTID_INPUT);
	GetOutputPort(OUTPUTPORT).Setup("Out", "y", AttributeChannels<double>::TYPE_ID, PORTID_OUTPUT);

	// ATTRIBUTES

	// create the interpolation speed attribute
	Core::AttributeSettings* speedAttribute = RegisterAttribute("Interpolation Speed", "interpolationSpeed", "The interpolation speed where 0.0 means the value won't change at all and 1.0 means the input value will directly be mapped to the output value.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSLIDER);
	speedAttribute->SetDefaultValue( Core::AttributeFloat::Create(mSettings.mInterpolationSpeed) );
	speedAttribute->SetMinValue( Core::AttributeFloat::Create(0.0f) );
	speedAttribute->SetMaxValue( Core::AttributeFloat::Create(1.0f) );

	// start value
	Core::AttributeSettings* startValueAttribute = RegisterAttribute("Start Value", "startValue", "Start value for interpolation.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	startValueAttribute->SetDefaultValue( Core::AttributeFloat::Create(mSettings.mStartValue) );
	startValueAttribute->SetMinValue( Core::AttributeFloat::Create(-FLT_MAX) );
	startValueAttribute->SetMaxValue( Core::AttributeFloat::Create(+FLT_MAX) );
}


void SmoothNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


void SmoothNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::Update(elapsed, delta);
}


// update the settings
void SmoothNode::OnAttributesChanged()
{
	const double interpolationSpeed = GetFloatAttributeByName("interpolationSpeed");
	const double startValue = GetFloatAttributeByName("startValue");

	mSettings.mInterpolationSpeed = interpolationSpeed;
	mSettings.mStartValue = startValue;
	
	// update settings
	SetupProcessors();
}


//-----------------------------------------------
// the node's processor implementation
//-----------------------------------------------

void SmoothNode::Processor::ReInit()					
{ 
	// ReInit baseclass
	ChannelProcessor::ReInit();

	mIsInitialized = false;
				
	ChannelBase* input = GetInput();
	ChannelBase* output = GetOutput();

	// nothing to do until a channel is connected
	if (input == NULL || output == NULL)
		return;

	double outputSampleRate = input->GetSampleRate();
	output->SetSampleRate(outputSampleRate);

	// reset the current value to the interpolation start value
	mCurrentValue = mSettings.mStartValue;
    
	mIsFirstUpdate = false;
	mIsInitialized = true;
}


void SmoothNode::Processor::Update()
{
	if (mIsInitialized == false)
		return;

	// update base
	ChannelProcessor::Update();

	// channel input reader
	ChannelReader* input = GetInputReader(0);

	// input/output channels
	ChannelBase* channel = GetInput();
	Channel<double>* output = GetOutput()->AsType<double>();

	// we need at least one connected channel
	if (channel == NULL)
		return;

	const double mFrameDeltaTime = 1.0 / channel->GetSampleRate();
	const double interpolationSpeed = mSettings.mInterpolationSpeed * mFrameDeltaTime;
				
	// get number of samples
	uint32 numNewSamples = input->GetNumNewSamples();
	
	// process input samples
	for (uint32 i=0; i<numNewSamples; ++i)
	{
		// perform interpolation
		const double sourceValue		= input->PopOldestSample<double>();
		const double interpolationResult= ( mSettings.mInterpolationSpeed < 0.99999f) ? Core::LinearInterpolate<double>( mCurrentValue, sourceValue, interpolationSpeed ) : sourceValue;

		// pass the interpolated result to the output port and the current value of the unique data
		mCurrentValue = interpolationResult;

		// add result to output
		output->AddSample(interpolationResult);
	}
}


