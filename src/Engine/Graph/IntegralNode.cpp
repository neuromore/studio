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
#include "IntegralNode.h"
#include "../Core/EventManager.h"
#include "../Core/Math.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
IntegralNode::IntegralNode(Graph* graph) : ProcessorNode(graph, new IntegralNode::Processor())
{
	// TODO: init future settings here
}


// destructor
IntegralNode::~IntegralNode()
{
}


// initialize the node
void IntegralNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	// SETUP PORTS
	RequireConstantSampleRate();

	// setup the input ports
	GetInputPort(INPUTPORT_VALUE).Setup("In", "x", AttributeChannels<double>::TYPE_ID, INPUTPORT_VALUE);

	// setup the output ports
	GetOutputPort(OUTPUTPORT_RESULT).Setup("Out", "y", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_RESULT);

	// ATTRIBUTES

	// TODO add attributes for integral order etc
}


void IntegralNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


void IntegralNode::Start(const Time& elapsed)
{
	ProcessorNode::Start(elapsed);
}


// update the node
void IntegralNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update the baseclass
	ProcessorNode::Update(elapsed, delta);
}


void IntegralNode::OnAttributesChanged()
{
	// TODO react on attribute changes here, change settings and reinit node or processor if required
}


//-----------------------------------------------
// the node's processor implementation
//-----------------------------------------------

void IntegralNode::Processor::ReInit()					
{ 
	// ReInit baseclass
	ChannelProcessor::ReInit();

	mIsInitialized = false;
				
	ChannelBase* input = GetInput();
	ChannelBase* output = GetOutput();

	// nothing to do until a channel is connected
	if (input == NULL || output == NULL)
		return;

	// forward sample rate
	double outputSampleRate = input->GetSampleRate();
	output->SetSampleRate(outputSampleRate);

	mIntegral = 0;
	
	mIsInitialized = true;
}


void IntegralNode::Processor::Update()
{
	if (mIsInitialized == false)
		return;

	// update base
	ChannelProcessor::Update();
				
	// channel input reader
	ChannelReader* reader = GetInputReader(0);

	// input/output channels
	Channel<double>* input = GetInput()->AsType<double>();
	Channel<double>* output = GetOutput()->AsType<double>();

	// we need at least one connected channel
	if (input == NULL)
		return;

	const double sampleRate = input->GetSampleRate();

	// integrate samples, considering sample time
	uint32 numNewSamples = reader->GetNumNewSamples();
	for (uint32 i = 0; i < numNewSamples; ++i)
	{
		mIntegral += reader->PopOldestSample<double>() / sampleRate;
		output->AddSample(mIntegral);
	}

}
