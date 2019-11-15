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
#include "ColorWheelNode.h"
#include "../Core/EventManager.h"
#include "../Core/Math.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
ColorWheelNode::ColorWheelNode(Graph* graph) : ProcessorNode(graph, new ColorWheelNode::Processor())
{
	// TODO: init future settings here
	mColor = Color(1.0, 1.0, 1.0, 1.0);
}


// destructor
ColorWheelNode::~ColorWheelNode()
{
}


// initialize the node
void ColorWheelNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	RequireMatchingSampleRates();

	// SETUP PORTS

	// setup the input ports
	GetInputPort(INPUTPORT_HUE).Setup("Hue", "hue", AttributeChannels<double>::TYPE_ID, INPUTPORT_HUE);
	GetInputPort(INPUTPORT_SAT).Setup("Saturation", "sat", AttributeChannels<double>::TYPE_ID, INPUTPORT_SAT);
	GetInputPort(INPUTPORT_VAL).Setup("Value", "val", AttributeChannels<double>::TYPE_ID, INPUTPORT_VAL);

	// setup the output ports
	GetOutputPort(OUTPUTPORT_RED).Setup("R", "red", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_RED);
	GetOutputPort(OUTPUTPORT_GREEN).Setup("G", "green", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_GREEN);
	GetOutputPort(OUTPUTPORT_BLUE).Setup("B", "blue", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_BLUE);

}


void ColorWheelNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	// huse must be connected
	const bool hasHue = (GetInputPort(INPUTPORT_HUE).HasConnection() && GetInputPort(INPUTPORT_HUE).GetChannels()->GetNumChannels() > 0);
	if (hasHue == false)
		mIsInitialized = false;

	PostReInit(elapsed, delta);
}


void ColorWheelNode::Start(const Time& elapsed)
{
	ProcessorNode::Start(elapsed);
}


// update the node
void ColorWheelNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update the baseclass
	ProcessorNode::Update(elapsed, delta);

	// update node color to the output color of the first processor :)
	if (mProcessors.IsEmpty() == false)
		mColor = static_cast<Processor*>(mProcessors[0])->GetColor();
}


void ColorWheelNode::OnAttributesChanged()
{
	// TODO react on attribute changes here, change settings and reinit node or processor if required
}


//-----------------------------------------------
// the node's processor implementation
//-----------------------------------------------

void ColorWheelNode::Processor::ReInit()					
{ 
	// ReInit baseclass
	ChannelProcessor::ReInit();

	mIsInitialized = false;
				
	ChannelBase* input = GetInput();

	// nothing to do until a channel is connected
	if (input == NULL)
		return;

	// forward sample rate
	double outputSampleRate = input->GetSampleRate();
	GetOutput(OUTPUTPORT_RED)->SetSampleRate(outputSampleRate);
	GetOutput(OUTPUTPORT_GREEN)->SetSampleRate(outputSampleRate);
	GetOutput(OUTPUTPORT_BLUE)->SetSampleRate(outputSampleRate);

	mIsInitialized = true;
}


void ColorWheelNode::Processor::Update()
{
	if (mIsInitialized == false)
		return;

	// update base
	ChannelProcessor::Update();
				
	// channel input reader
	ChannelReader* hueReader = GetInputReader(INPUTPORT_HUE);
	ChannelReader* satReader = GetInputReader(INPUTPORT_SAT);
	ChannelReader* valReader = GetInputReader(INPUTPORT_VAL);

	// input/output channels
	Channel<double>* redChannel = GetOutput(OUTPUTPORT_RED)->AsType<double>();
	Channel<double>* greenChannel = GetOutput(OUTPUTPORT_GREEN)->AsType<double>();
	Channel<double>* blueChannel = GetOutput(OUTPUTPORT_BLUE)->AsType<double>();

	// no hue: no output
	if (hueReader->HasChannel() == false)
		return;

	// convert colors to
	uint32 numNewSamples = Min3(hueReader->GetNumNewSamples(), satReader->HasChannel() ? satReader->GetNumNewSamples() : CORE_INT32_MAX, valReader->HasChannel() ? valReader->GetNumNewSamples() : CORE_INT32_MAX);
	for (uint32 i = 0; i < numNewSamples; ++i)
	{
		const double hue = hueReader->PopOldestSample<double>();
		const double saturation = (satReader->HasChannel() ? satReader->PopOldestSample<double>() : 1.0 );		// default value is saturation = 1.0
		const double value = (valReader->HasChannel() ? valReader->PopOldestSample<double>() : 1.0 );			// default value is value = 1.0
		
		// convert to RGB
		mColor.FromHSV( Clamp(hue,0.0,0.999), Clamp(saturation, 0.0, 1.0), Clamp(value, 0.0, 1.0));

		// output color
		redChannel->AddSample(mColor.r);
		greenChannel->AddSample(mColor.g);
		blueChannel->AddSample(mColor.b);
	}

}
