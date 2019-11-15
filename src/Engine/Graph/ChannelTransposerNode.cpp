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
#include "ChannelTransposerNode.h"
#include "../Core/AttributeSettings.h"
#include "Graph.h"


using namespace Core;

// constructor
ChannelTransposerNode::ChannelTransposerNode(Graph* graph) : SPNode(graph)
{
	UseChannelMetadataPropagation(false);
	UseChannelColoring(false);
}


// destructor
ChannelTransposerNode::~ChannelTransposerNode()
{
	DeleteOutputChannels();
}


// initialize the node
void ChannelTransposerNode::Init()
{
	// init base class first
	SPNode::Init();

	// configure SPNode behaviour
	RequireMatchingSampleRates();
	RequireInputConnection();
	
	// PORTS
	const uint32 numInPortsDefault = 1;
	InitInputPorts(numInPortsDefault);
	GetInputPort(0).SetupAsChannels<double>("In", "x1", 0);

	const uint32 numOutPortsDefault = 0;
	InitOutputPorts(numOutPortsDefault);

	// ATTRIBUTES

	// hidden input port count attribute
	Core::AttributeSettings* attribNumInPorts = RegisterAttribute("", "numInputPorts", "", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	attribNumInPorts->SetDefaultValue( Core::AttributeInt32::Create(numInPortsDefault) );
	attribNumInPorts->SetMinValue( Core::AttributeInt32::Create(1) );
	attribNumInPorts->SetMaxValue( Core::AttributeInt32::Create(INT_MAX) );
	attribNumInPorts->SetVisible(false);
		
	// hidden port number attribute
	Core::AttributeSettings* attribNumOutPorts = RegisterAttribute("", "numOutputPorts", "", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	attribNumOutPorts->SetDefaultValue( Core::AttributeInt32::Create(numOutPortsDefault) );
	attribNumOutPorts->SetMinValue( Core::AttributeInt32::Create(0) );
	attribNumOutPorts->SetMaxValue( Core::AttributeInt32::Create(INT_MAX) );
	attribNumOutPorts->SetVisible(false);

}


// important: a reset must not delete the output ports or sever the connections in any way
void ChannelTransposerNode::Reset()
{
	SPNode::Reset();

	// delete all output channels and disconnect them from the outputs, but leave the output ports intact
	DeleteOutputChannels();
}


void ChannelTransposerNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;
	
	// dynamic number of input ports
	EnsureFreeInputPort();

	// reinit baseclass
	SPNode::ReInit(elapsed, delta);

	//
	// stop node if input is not uniform (same number of channels on all ports)
	//

	if (CheckAndCollectInputChannels() == false && mInputReader.GetNumChannels() > 0)
	{
		SetError(ERROR_INPUT_INCOMPATIBLE_MULTICHANNELS, "All inputs must have equal number of channels.");
		mIsInitialized = false;
	}
	else
	{
		ClearError(ERROR_INPUT_INCOMPATIBLE_MULTICHANNELS);
	}

	PostReInit(elapsed, delta);
}


void ChannelTransposerNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update baseclass
	SPNode::Update(elapsed, delta);

	// nothing to do if node is not initialized
	if (mIsInitialized == false)
		return;

	CORE_ASSERT(mOutputChannels.Size() == mSizeOut);

	// forward all samples to the outputs transposed
	for (uint32 i=0; i<mSizeIn; ++i)
	{
		CORE_ASSERT(i < mInputReader.GetNumChannels());
		for (uint32 j=0; j<mSizeOut; ++j)
		{
			ChannelReader* reader = mInputReader.GetReader( mTransposedInputs[j][i] );
			ChannelBase* output = mOutputChannels[j][i];

			// TODO make forward loop independent from sample type
			const uint32 numSamples = reader->GetNumNewSamples();
			for (uint32 s = 0; s < numSamples; ++s)
			{
				double sample = reader->PopOldestSample<double>();	
				output->AsType<double>()->AddSample(sample);
			}
		}
	}
	
}


// an attribute has changed
void ChannelTransposerNode::OnAttributesChanged()
{
	// A little hacky? This codepath is only used to create output ports during loading of the classifier - the attribute is the only way we know how 
	// many ports there should be created before loading the connection.
	if (GetNumOutputPorts() == 0)
	{
		mSizeOut = GetInt32Attribute(ATTRIB_NUMOUTPUTPORTS);
		ReInitOutputPorts();
	}

	// create ports if number of input ports has changed (only relevant for node loading right now)
	const uint32 numInPorts = GetInt32Attribute(ATTRIB_NUMINPUTPORTS);
	if (GetNumInputPorts() != numInPorts)
	{
		InitInputPorts(numInPorts);
		UpdateInputPorts();
	}
}


// start the node: create output ports and channels
void ChannelTransposerNode::Start(const Time& elapsed)
{
	// save output port sizes
	SetInt32AttributeByIndex(ATTRIB_NUMOUTPUTPORTS, mSizeOut);

	ReInitOutputPorts();				// init ports (empty, without channels)
	ReInitOutputChannels();				// init output channels and connect them to the ports; also name the ports

	// Note: we have to call SPNode::Start after we created the channels
	SPNode::Start(elapsed);
}


// NOTE: same as ChannelMathNode::EnsureFreeInputPorts(), and ChannelSelectorNode::EnsureFreeInputPorts()
// check if there are free input ports; if not, add one more
void ChannelTransposerNode::EnsureFreeInputPort()
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


// NOTE: same as ChannelMathNode::UpdateInputPorts()
// overridden init input ports
void ChannelTransposerNode::UpdateInputPorts()
{
	// set all port names
	const uint32 numPorts = GetNumInputPorts();
	for (uint32 i=0; i<numPorts; ++i)
	{
		mTempString.Format("x%i", i+1);		// is 1-indexed
		GetInputPort(i).SetupAsChannels<double>("In", mTempString.AsChar(), i);
	}
}


// collect all input channels into the mInputChannels array (return false if inputs are not homogenous)
bool ChannelTransposerNode::CheckAndCollectInputChannels()
{
	// reset sizes;
	mSizeIn = 0;
	mSizeOut = 0;	

	// Step 1: collect all non-empty input multichannels
	Array<MultiChannel*> multiChannels;

	const uint32 numInputPorts = GetNumInputPorts();
	for (uint32 i=0; i<numInputPorts; ++i)
	{
		Port& port = GetInputPort(i);
		if (port.HasConnection() == true && port.GetChannels() != NULL)
			multiChannels.Add(port.GetChannels());
	}

	// Step 2: analyze multichannel sizes
	const uint32 numMultiChannels = multiChannels.Size();
	
	// nothing to transpose
	if (numMultiChannels == 0)
		return false;

	const uint32 size = multiChannels[0]->GetNumChannels();
	
	// check first multichannels against all others (if there is only one it will also work)
	for (uint32 i=1; i<numMultiChannels; ++i)
	{
		if (multiChannels[i]->GetNumChannels() != size )
			return false;
	}

	// everything ok, we now know both dimensions for the transpose action
	mSizeIn = numMultiChannels;
	mSizeOut = size;

	// collect the inputs into the 2d array so the update loop just has to copy them 1:1
	mTransposedInputs.Clear();
	mTransposedInputs.Resize(mSizeOut);
	uint32 index = 0; // index of channel within flat inputchannel array
	for (uint32 i=0; i<mSizeIn; ++i)
	{
		for (uint32 j=0; j<mSizeOut; ++j)
		{
			mTransposedInputs[j].Add(index);
			index++;
		}
	}

	return true;
}


// create the output ports (with empty multi channels)
void ChannelTransposerNode::ReInitOutputPorts()
{
	// create empty ports
	InitOutputPorts(mSizeOut);

	for (uint32 i = 0; i < mSizeOut; ++i)
	{
		mTempString.Format("y%i", i + 1);
		GetOutputPort(i).SetupAsChannels<double>("-", mTempString.AsChar(), i);
	}
}


// create the output channels (one for each selected input channel) and connect them to the outputs
void ChannelTransposerNode::ReInitOutputChannels()
{
	DeleteOutputChannels();

	mOutputChannels.Resize(mSizeOut);

	for (uint32 i=0; i<mSizeIn; ++i)
	{
		for (uint32 j=0; j<mSizeOut; ++j)
		{
			Channel<double>* channel = new Channel<double>();		// TODO clone channel here so its independent of type
			Channel<double>* inputChannel = mInputReader.GetChannel( mTransposedInputs[j][i] )->AsType<double>();
			
			// create new channel name using the source node name
			String newName = inputChannel->GetSourceNameString() + " " + inputChannel->GetName();

			// configure output channel to same parameters as input channel
			channel->SetBufferSize(10);		// set any buffersize > 0
			channel->SetName(newName);
			channel->SetSampleRate(inputChannel->GetSampleRate());
			channel->SetMinValue(inputChannel->GetMinValue());
			channel->SetMaxValue(inputChannel->GetMaxValue());
			channel->SetColor(inputChannel->GetColor());

			mOutputChannels[j].Add(channel);

			// attach output channel to output port
			MultiChannel* multichannel = GetOutputPort(j).GetChannels();
			multichannel->AddChannel(mOutputChannels[j][i]);
		}
	}

	// update the port names (using name of first channel in each port)
	for (uint32 j=0; j<mSizeOut; ++j)
	{
		Channel<double>* inputChannel = mInputReader.GetChannel( mTransposedInputs[j][0] )->AsType<double>();
		GetOutputPort(j).SetName(inputChannel->GetName());
	}
}


// delete all output channels and remove them from ports
void ChannelTransposerNode::DeleteOutputChannels()
{
	//const uint32 numChannels = mOutputChannels.Size();
	const uint32 numOutputPorts = GetNumOutputPorts();

	// clear output ports first
	for (uint32 i = 0; i < numOutputPorts; ++i)
	{
		GetOutputPort(i).GetChannels()->Clear();
		GetOutputPort(i).SetName("-");
	}
	
	// delete the output channels
	const uint32 numOutputArrays = mOutputChannels.Size();
	for (uint32 i = 0; i < numOutputArrays; ++i)
		DestructArray(mOutputChannels[i]);
	
	mOutputChannels.Clear();
		
}
