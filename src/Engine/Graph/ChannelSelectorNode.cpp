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
#include "ChannelSelectorNode.h"
#include "../Core/AttributeSettings.h"
#include "Graph.h"


using namespace Core;

// constructor
ChannelSelectorNode::ChannelSelectorNode(Graph* graph) : SPNode(graph)
{
}


// destructor
ChannelSelectorNode::~ChannelSelectorNode()
{
	DeleteOutputChannels();
}


// initialize the node
void ChannelSelectorNode::Init()
{
	// init base class first
	SPNode::Init();

	// configure SPNode behaviour
	RequireMatchingSampleRates();
	UseChannelMetadataPropagation(false); // Hack: disable metadata propagation, it interfers with the function of selector node
	
	// PORTS
	InitInputPorts(1);
	GetInputPort(INPUTPORT_SET).SetupAsChannels<double>("In", "x1", INPUTPORT_SET);

	const uint32 numPortsDefault = 0;
	InitOutputPorts(numPortsDefault);

	// ATTRIBUTES
		
	// hidden port number attribute
	Core::AttributeSettings* attribNumPorts = RegisterAttribute("", "numOutputPorts", "", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	attribNumPorts->SetDefaultValue( Core::AttributeInt32::Create(numPortsDefault) );
	attribNumPorts->SetMinValue( Core::AttributeInt32::Create(0) );
	attribNumPorts->SetMaxValue( Core::AttributeInt32::Create(INT_MAX) );
	attribNumPorts->SetVisible(false);

	// channel names 
	Core::AttributeSettings* attribChannelNames = RegisterAttribute("Channel Names", "channels", "", Core::ATTRIBUTE_INTERFACETYPE_STRINGARRAY);
	attribChannelNames->SetDefaultValue( Core::AttributeStringArray::Create("*") );

	// single output attribute
	Core::AttributeSettings* attribSingleOutput = RegisterAttribute("Single Output", "singleOutput", "", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attribSingleOutput->SetDefaultValue( Core::AttributeBool::Create(false) );

	// quick configuration attribute
	Core::AttributeSettings* attribQuickConfig = RegisterAttribute("Quick Config", "quickconfig", "Configurable in Experience Wizard", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attribQuickConfig->SetDefaultValue(Core::AttributeBool::Create(false));
}


// important: a reset must not delete the output ports or sever the connections in any way
void ChannelSelectorNode::Reset()
{
	SPNode::Reset();

	// delete all output channels and disconnect them from the outputs, but leave the output ports intact
	DeleteOutputChannels();
}


void ChannelSelectorNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	SPNode::ReInit(elapsed, delta);

	// stop node if it doesnt have any connection or no input channels
	Port& inPort = GetInputPort(INPUTPORT_SET);
	if (inPort.HasConnection() == true && inPort.GetChannels() != NULL)
	{
		if (inPort.GetChannels() != NULL && inPort.GetChannels()->GetNumChannels() == 0)
		{
			mIsInitialized = false;
		}
	}

	PostReInit(elapsed, delta);
}


void ChannelSelectorNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update baseclass
	SPNode::Update(elapsed, delta);

	// nothing to do if node is not initialized
	if (mIsInitialized == false)
		return;

	CORE_ASSERT(mOutputChannels.Size() == mSelectedInputs.Size());

	// forward all samples to the outputs
	const uint32 numChannels = mOutputChannels.Size();
	for (uint32 i=0; i<numChannels; ++i)
	{
		CORE_ASSERT(mSelectedInputs[i] < mInputReader.GetNumChannels());
		
		ChannelReader* reader = mInputReader.GetReader(mSelectedInputs[i]);	
		ChannelBase* output = mOutputChannels[i];

		// TODO make forward loop independent from sample type
		const uint32 numSamples = reader->GetNumNewSamples();
		for (uint32 s = 0; s < numSamples; ++s)
		{
			double sample = reader->GetSample<double>(s);		// Note: don't pop samples here, just read them (as we might forward one channel multiple times to different outputs)
			output->AsType<double>()->AddSample(sample);
		}
	}

	// advance all input reader at once after samples have been processed
	mInputReader.Flush(true);
}


// an attribute has changed
void ChannelSelectorNode::OnAttributesChanged()
{
	// A little hacky? This codepath is only used to create output ports during loading of the classifier - the attribute is the only way we know how 
	// many ports there should be created before loading the connection.
	if (GetNumOutputPorts() == 0)
		ReInitOutputPorts();

	// always reinit if a node attribute was changed
	ResetAsync();
}


// start the node: create output ports and channels
void ChannelSelectorNode::Start(const Time& elapsed)
{
	// remember number of ports in attribute
	SetInt32Attribute("numOutputPorts", mInputReader.GetNumChannels());

	CollectSelectedInputChannels();		// collect channels first
	ReInitOutputPorts();				// init ports (empty, without channels)
	ReInitOutputChannels();				// init output channels and connect them to the ports; also name the ports

	// Note: we have to call SPNode::Start after we created the channels
	SPNode::Start(elapsed);
}


// filter input channels by name
void ChannelSelectorNode::CollectSelectedInputChannels()
{
	// specified channel names
	const Array<String>& names = GetStringArrayAttribute(ATTRIB_CHANNELNAMES, Array<String>());
	const bool hasNames = !names.IsEmpty();
	const bool useSingleWildCardFeature = (hasNames && names[0].GetLength() == 1 && names[0].GetFirst() == StringCharacter::asterisk);
	
	mSelectedInputs.Clear();

	const uint32 numInputChannels = mInputReader.GetNumChannels();

	// no channel names specified: don't output any channel
	if (hasNames == false)
		return;
	
	// a single asterisk matches all channels
	if (useSingleWildCardFeature == true)
	{
		// forward all channel
		for (uint32 i=0; i<numInputChannels; ++i)
			mSelectedInputs.Add(i);
	
		return;
	}
	
	// iterate through the channel names and search for the next input channel that matches the name specification
	const uint32 numNames = names.Size();
	for (uint32 i=0; i<numNames; ++i)
	{
		String name = names[i];
		name.Trim();

		// handle strings like *FOO* special (search for substring instead of exact compare)
		const bool useWildCardFeature = (name.GetLength() >=3 && 
										 name.GetFirst() == StringCharacter::asterisk && 
										 name.GetLast()  == StringCharacter::asterisk);

		bool haveChannel = false;

		// normal mode: exact name matching
		if (useWildCardFeature == false && useSingleWildCardFeature == false)
		{
			uint32 channelIndex = CORE_INVALIDINDEX32;

			// find first channel that matches the name exactly
			for (uint32 j=0; j<numInputChannels; ++j)
			{
				ChannelBase* channel = mInputReader.GetChannel(j);
				if (channel->GetNameString().IsEqual(name) == true)
				{
					// found it
					channelIndex = j;
					break;
				}
			}
			
			// add the channel
			if (channelIndex != CORE_INVALIDINDEX32)
			{
				mSelectedInputs.Add(channelIndex);
				haveChannel = true;
			}

			// node error when channel was not found (not in wild card mode, only in exact match mode)
			if (haveChannel == false)
			{
				String tmp; tmp.Format("Channel '%s' not found", name.AsChar());
				SetError(ERROR_CHANNEL_NOT_FOUND, tmp.AsChar());
			}

		}
		else // wildcard modes: use substring matching
		{
			// get wildcard string by stripping the first/last char (an asterisk)
			name.Trim(StringCharacter::asterisk);

			// find _ALL_ channels that contain the name and add them to the list
			for (uint32 j=0; j<numInputChannels; ++j)
			{
				ChannelBase* channel = mInputReader.GetChannel(j);
				if (channel->GetNameString().Contains(name) == true)
				{
					// found one, add it (don't break, keep processing all input channels)
					mSelectedInputs.Add(j);
					haveChannel = true;
				}
			}
		}

	
	}
}


// use this to check if an input channel is forwarded by the node
bool ChannelSelectorNode::IsChannelSelected(const ChannelBase* channel) const
{
	// compare pointer against all selected input channels
	const uint32 numSelectedChannels = mSelectedInputs.Size();
	for (uint32 i=0; i<numSelectedChannels; ++i)
	{
		const uint32 index = mSelectedInputs[i];
		if (const_cast<ChannelSelectorNode*>(this)->mInputReader.GetChannel(index) == channel)
			return true;
	}

	return false;
}


// create the output ports (with empty multi channels)
void ChannelSelectorNode::ReInitOutputPorts()
{
	const bool singleOutput = GetBoolAttribute(ATTRIB_SINGLE_OUTPUT);
	uint32 numPortsAttr = GetInt32Attribute(ATTRIB_NUMOUTPUTPORTS);		// previously remembered number of output ports

	// get number of required output ports
	if (singleOutput == true)
	{
		InitOutputPorts(1);
		GetOutputPort(0).SetupAsChannels<double>("-", "y1", 0);

		// update channel references
		GetOutputPort(0).GetChannels()->Clear();

		// remember number of outputs (if it has changed)
		if (numPortsAttr != 1)
			SetInt32AttributeByIndex(ATTRIB_NUMOUTPUTPORTS, 1);
	}
	else
	{
		// get number of output channels we have to output
		const uint32 numChannels = mSelectedInputs.Size();
		uint32 numPorts;

		// use last port count only if there are no input channel present
		if (mInputReader.GetNumChannels() == 0 && GetNumOutputPorts() == 0)
			numPorts = numPortsAttr;
		else
			numPorts = numChannels;

		SetInt32AttributeByIndex(ATTRIB_NUMOUTPUTPORTS, numPorts);

		// create empty ports
		InitOutputPorts(numPorts);
		for (uint32 i = 0; i < numPorts; ++i)
		{
			mTempString.Format("y%i", i + 1);
			GetOutputPort(i).SetupAsChannels<double>("-", mTempString.AsChar(), i);
		}
	}
}


// create the output channels (one for each selected input channel) and connect them to the outputs
void ChannelSelectorNode::ReInitOutputChannels()
{
	DeleteOutputChannels();

	// create output channel array, for the selected channels given
	const uint32 numChannels = mSelectedInputs.Size();
	
	for (uint32 i = 0; i < numChannels; ++i)
	{
		CORE_ASSERT(mSelectedInputs[i] < mInputReader.GetNumChannels());

		Channel<double>* channel = new Channel<double>();		// TODO clone channel here so its independent of type
		Channel<double>* inputChannel = mInputReader.GetChannel(mSelectedInputs[i])->AsType<double>();

		// configure output channel to same parameters as input channel
		channel->SetBufferSize(10);		// set any buffersize > 0
		channel->SetName(inputChannel->GetName());
		channel->SetSampleRate(inputChannel->GetSampleRate());
		channel->SetMinValue(inputChannel->GetMinValue());
		channel->SetMaxValue(inputChannel->GetMaxValue());
		channel->SetColor(inputChannel->GetColor());

		mOutputChannels.Add(channel);
	}

	// now add connections to output ports

	const bool singleOutput = GetBoolAttribute(ATTRIB_SINGLE_OUTPUT);
	if (singleOutput == true)
	{
		CORE_ASSERT(GetNumOutputPorts() == 1);

		// add all channels into one multichannel
		MultiChannel* multichannel = GetOutputPort(0).GetChannels();
		multichannel->Clear();
		for (uint32 i = 0; i < numChannels; ++i)
			multichannel->AddChannel(mOutputChannels[i]);

		// set the port name
		GetOutputPort(0).SetName("Out");
	}
	else
	{
		CORE_ASSERT(GetNumOutputPorts() == numChannels);
		
		// add each channel to its individual port
		for (uint32 i = 0; i < numChannels; ++i)
		{
			MultiChannel* multichannel = GetOutputPort(i).GetChannels();
			multichannel->Clear();
			multichannel->AddChannel(mOutputChannels[i]);

			// set the port name
			GetOutputPort(i).SetName(mOutputChannels[i]->GetName());
		}
	}
}


// delete all output channels and remove them from ports
void ChannelSelectorNode::DeleteOutputChannels()
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
	DestructArray(mOutputChannels);
}
