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
#include "ChannelMergerNode.h"


using namespace Core;

// constructor
ChannelMergerNode::ChannelMergerNode(Graph* graph) : SPNode(graph)
{
}


// destructor
ChannelMergerNode::~ChannelMergerNode()
{
	DeleteOutputChannels();
}


// initialize node
void ChannelMergerNode::Init()
{
	// init base class first
	SPNode::Init();

	// configure SPNode behaviour
	RequireMatchingSampleRates();
	RequireInputConnection();
	UseChannelMetadataPropagation(false); // Hack: disable metadata propagation, it interfers with the function of merger node


	// PORTS
	const uint32 numPortsDefault = 1;
	InitInputPorts(numPortsDefault);
	UpdateInputPorts();

	InitOutputPorts(1);
	GetOutputPort(OUTPUTPORT_MERGED).SetupAsChannels<double>("Out", "y1", OUTPUTPORT_MERGED);
	
	// ATTRIBUTES

	// hidden port number attribute
	Core::AttributeSettings* attribNumPorts = RegisterAttribute("", "numInputPorts", "", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	attribNumPorts->SetDefaultValue( Core::AttributeInt32::Create(numPortsDefault) );
	attribNumPorts->SetMinValue( Core::AttributeInt32::Create(1) );
	attribNumPorts->SetMaxValue( Core::AttributeInt32::Create(INT_MAX) );
	attribNumPorts->SetVisible(false);
}


void ChannelMergerNode::Reset()
{
	SPNode::Reset();

	DeleteOutputChannels();
}


void ChannelMergerNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// dynamic number of input ports
	EnsureFreeInputPort();

	// reinit baseclass
	SPNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


void ChannelMergerNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	SPNode::Update(elapsed, delta);

	// nothing to do if node is not initialized
	if (mIsInitialized == false)
		return;

	Port& outPort = GetOutputPort(OUTPUTPORT_MERGED); 
	MultiChannel* outputSet = outPort.GetChannels();

	const uint32 numChannels = mInputReader.GetNumChannels();
	
	// make sure Start() worked correctly
	CORE_ASSERT(outputSet->GetNumChannels() == numChannels);

	// forward all samples to the outputs
	for (uint32 i = 0; i < numChannels; ++i)
	{
		Channel<double>* outputChannel = outputSet->GetChannel(i)->AsType<double>();

		// read from the inputs and copy samples to output channels
		ChannelReader* reader = mInputReader.GetReader(i);
		const uint32 numSamples = reader->GetNumNewSamples();
		for (uint32 s = 0; s < numSamples; ++s)
		{
			double sample = reader->PopOldestSample<double>();
			outputChannel->AddSample(sample);
		}
	}
}


void ChannelMergerNode::Start(const Time& elapsed)
{
	MultiChannel* outputSet = GetOutputPort(OUTPUTPORT_MERGED).GetChannels();

	// just in case
	CORE_ASSERT(outputSet->GetNumChannels() == 0);

	// create output channels
	const uint32 numChannels = mInputReader.GetNumChannels();
	for (uint32 i = 0; i < numChannels; ++i)
	{
		// create channel if there isnt one already
		Channel<double>* channel = new Channel<double>();
		outputSet->AddChannel(channel);
		
		// configure output channel to same parameters as input channel
		Channel<double>* inputChannel = mInputReader.GetChannel(i)->AsType<double>();
		channel->SetBufferSize(10);		// set any buffersize > 0
		channel->SetName(inputChannel->GetName());
		channel->SetSampleRate(inputChannel->GetSampleRate());
		channel->SetMinValue(inputChannel->GetMinValue());
		channel->SetMaxValue(inputChannel->GetMaxValue());
		channel->SetColor(inputChannel->GetColor());
	}

	SPNode::Start(elapsed);
}


// update the data
void ChannelMergerNode::OnAttributesChanged()
{
	// init if number if ports has changed (only relevant for node loading right now)
	const uint32 numPorts = GetInt32Attribute(ATTRIB_NUMINPUTPORTS);
	if (GetNumInputPorts() != numPorts)
	{
		InitInputPorts(numPorts);
		UpdateInputPorts();
	}
}


// NOTE: same as ChannelMathNode::UpdateInputPorts()
// overridden init input ports
void ChannelMergerNode::UpdateInputPorts()
{
	// set all port names
	const uint32 numPorts = GetNumInputPorts();
	for (uint32 i=0; i<numPorts; ++i)
	{
		mTempString.Format("x%i", i+1);		// is 1-indexed
		GetInputPort(i).SetupAsChannels<double>("In", mTempString.AsChar(), i);
	}
}


// NOTE: same as ChannelMathNode::EnsureFreeInputPorts()
// check if there are free input ports; if not, add one more
void ChannelMergerNode::EnsureFreeInputPort()
{
	bool haveFreePort = false;
	const uint32 numPorts = GetNumInputPorts();
	for (uint32 i=0; i<numPorts; ++i)
	{
		if (GetInputPort(i).HasConnection() == false)
		{
			haveFreePort = true;

			// if the free port is the last one, we can exit immediately
			if (i == numPorts - 1)
				return;

			break;
		}
	}

	// calculate new number of ports
	uint32 newNumPorts = numPorts;
	
	// add one
	if (haveFreePort == false)
	{
		newNumPorts++;
	}
	else   // remove all free ports at the end, except one
	{
		while (newNumPorts > 1)
		{
			if (GetInputPort(newNumPorts-1).HasConnection() == false && 
				GetInputPort(newNumPorts-2).HasConnection() == false)
			{
				newNumPorts--;
			}
			else
				break;
		}
	}
	
	// update number of ports
	if (newNumPorts != numPorts)
	{
		SetInt32Attribute("numInputPorts", newNumPorts);
		InitInputPorts(newNumPorts);
	}

	UpdateInputPorts();
}


void ChannelMergerNode::DeleteOutputChannels()
{
	// delete all output channels
	const uint32 numOutPorts = GetNumOutputPorts();
	for (uint32 i = 0; i < numOutPorts; ++i)
	{
		Port& outPort = GetOutputPort(i);
		MultiChannel* outputSet = outPort.GetChannels();

		const uint32 numChannels = outputSet->GetNumChannels();
		for (uint32 c = 0; c < numChannels; ++c)
			delete outputSet->GetChannel(c);

		outputSet->Clear();
	}

}

