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
#include "SwitchNode.h"


using namespace Core;

// constructor
SwitchNode::SwitchNode(Graph* graph) : SPNode(graph)
{
}


// destructor
SwitchNode::~SwitchNode()
{
	DeleteOutputChannels();
}


// initialize node
void SwitchNode::Init()
{
	// init base class first
	SPNode::Init();

	// configure SPNode behaviour
	RequireMatchingSampleRates();
	RequireAllInputConnections();

	// PORTS
	// init only the control port
	InitInputPorts(1);
	GetInputPort(INPUTPORT_CONTROL).SetupAsChannels<double>("Control", "x1", INPUTPORT_CONTROL);

	InitOutputPorts(1);
	GetOutputPort(OUTPUTPORT).SetupAsChannels<double>("Out", "y1", OUTPUTPORT);
	
	// ATTRIBUTES

	// hidden port number attribute
	const uint32 numSwitchPortsDefault = 2; // two switch ports and one control portss
	Core::AttributeSettings* attribNumPorts = RegisterAttribute("Inputs", "numInputPorts", "", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	attribNumPorts->SetDefaultValue( Core::AttributeInt32::Create(numSwitchPortsDefault) );
	attribNumPorts->SetMinValue( Core::AttributeInt32::Create(2) );
	attribNumPorts->SetMaxValue( Core::AttributeInt32::Create(INT_MAX) );

}


void SwitchNode::Reset()
{
	SPNode::Reset();

	DeleteOutputChannels();
}


void SwitchNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;
	
	// reinit baseclass
	SPNode::ReInit(elapsed, delta);

	UpdateInputPorts();
		
	MultiChannel* controlChannels = GetInputPort(INPUTPORT_CONTROL).GetChannels();
	MultiChannel* switchPort0Channels = GetInputPort(INPUTPORT_SWITCHPORT0).GetChannels();
	const uint32 numControlChannels = (controlChannels == NULL ? 0 : controlChannels->GetNumChannels());
	CORE_ASSERT(GetNumInputPorts() > 1);
	const uint32 numSwitchPorts = GetNumInputPorts() - 1; 
	const uint32 numSignalChannels = (switchPort0Channels == NULL ? 0 : GetInputPort(INPUTPORT_SWITCHPORT0).GetChannels()->GetNumChannels());


	// must have either one control channel for all, or one for each channel in the switch ports
	if (mIsInitialized == true && numControlChannels != numSignalChannels && numControlChannels != 1 )
	{
		mIsInitialized = false;
		SetError(ERROR_CONTROLPORT_MISSMATCH, "Control channel is incompatible.");
	}
	else 
	{
		ClearError(ERROR_CONTROLPORT_MISSMATCH);
	}


	// check the multichannel size of the other switch ports against the first one, they must match
	if (mIsInitialized == true)
	{
		bool sizeMatches = true;
		for (uint32 i=1; i<numSwitchPorts && sizeMatches; ++i)
		{
			MultiChannel* switchPortChannels = GetInputPort(INPUTPORT_SWITCHPORT0 + i).GetChannels();
			CORE_ASSERT(switchPortChannels != NULL);

			if (switchPortChannels->GetNumChannels() != numSignalChannels)
				sizeMatches = false;
		}

		if (sizeMatches == false)
		{
			mIsInitialized = false;
			SetError(ERROR_SWITCHPORT_MISSMATCH, "Input channels are not compatible.");
		}
		else 
		{
			ClearError(ERROR_SWITCHPORT_MISSMATCH);
		}
	}

	PostReInit(elapsed, delta);
}


void SwitchNode::Start(const Time& elapsed)
{
	// create output channels
	MultiChannel* outputChannels = GetOutputPort(OUTPUTPORT).GetChannels();
	MultiChannel* switchPort0Channels = GetInputPort(INPUTPORT_SWITCHPORT0).GetChannels();
	const uint32 numSignalChannels = (switchPort0Channels == NULL ? 0 : GetInputPort(INPUTPORT_SWITCHPORT0).GetChannels()->GetNumChannels());

	for (uint32 i = 0; i < numSignalChannels; ++i)
	{
		// create channel if there isnt one already
		Channel<double>* channel = new Channel<double>();
		outputChannels->AddChannel(channel);
		
		// configure output channel to same parameters as input channel
		Channel<double>* inputChannel = switchPort0Channels->GetChannel(i)->AsType<double>();
		channel->SetBufferSize(10);		// set any buffersize > 0
		channel->SetName(inputChannel->GetName());
		channel->SetSampleRate(inputChannel->GetSampleRate());
		channel->SetMinValue(inputChannel->GetMinValue());
		channel->SetMaxValue(inputChannel->GetMaxValue());
		//channel->SetColor(inputChannel->GetColor());
	}

	SPNode::Start(elapsed);
}


// update the data
void SwitchNode::OnAttributesChanged()
{
	// init input ports
	const uint32 numSwitchPorts = GetInt32Attribute(ATTRIB_NUMINPUTPORTS);
	if (GetNumInputPorts() != numSwitchPorts + 1)
		UpdateInputPorts();
}


void SwitchNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	SPNode::Update(elapsed, delta);

	// nothing to do if node is not initialized
	if (mIsInitialized == false)
		return;

	MultiChannel* outputChannels = GetOutputPort(OUTPUTPORT).GetChannels();

	MultiChannel* controlChannels = GetInputPort(INPUTPORT_CONTROL).GetChannels();
	MultiChannel* switchPort0Channels = GetInputPort(INPUTPORT_SWITCHPORT0).GetChannels();
	const uint32 numControlChannels = (controlChannels == NULL ? 0 : controlChannels->GetNumChannels());
	CORE_ASSERT(GetNumInputPorts() > 1);
	const uint32 numSwitchPorts = GetNumInputPorts() - 1; 
	const uint32 numSignalChannels = (switchPort0Channels == NULL ? 0 : GetInputPort(INPUTPORT_SWITCHPORT0).GetChannels()->GetNumChannels());
	const uint32 numControlChannelReaders = (numControlChannels > 1 ? numSignalChannels : 1);

	const uint32 numSamples = mInputReader.GetMinNumNewSamples();

	for (uint32 i=0; i<numSamples; ++i)
	{
		for (uint32 j=0; j<numSignalChannels; ++j)
		{
			const uint32 controlChannelReaderIndex = (numControlChannels > 1 ? j : 0);
			const double controlValue = mInputReader.GetReader(controlChannelReaderIndex)->GetSample<double>(i);

			// calculate which input the switch selects and get the value from it
			const uint32 selectedInputIndex = Clamp<uint32>( (uint32)controlValue, 0, numSwitchPorts-1);
			const uint32 inputChannelReaderIndex = numControlChannelReaders + selectedInputIndex * numSignalChannels + j;
			const double outputValue = mInputReader.GetReader(inputChannelReaderIndex)->GetSample<double>(i);

			// output the selected value
			outputChannels->GetChannel(j)->AsType<double>()->AddSample(outputValue);
		}
	}
	
	mInputReader.Flush();
}


// overridden init input ports
void SwitchNode::UpdateInputPorts()
{
	const uint32 numSwitchPorts = GetInt32Attribute(ATTRIB_NUMINPUTPORTS);
	if (GetNumInputPorts() != numSwitchPorts + 1)
		InitInputPorts(numSwitchPorts + 1);

	// set all switch port names
	for (uint32 i=0; i<numSwitchPorts; ++i)
	{
		const uint32 portIndex = i+1;

		String internalName = mTempString.Format("x%i", portIndex+1); // Note: name is 1-indexed
		String displayName = mTempString.Format("%i .. %i", i, i+1);
		
		GetInputPort(portIndex).SetupAsChannels<double>(displayName, internalName, portIndex);
	}
}


void SwitchNode::DeleteOutputChannels()
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

