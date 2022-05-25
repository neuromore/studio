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
#include "SampleGateNode.h"
#include "../Core/AttributeSettings.h"
#include "Graph.h"
#include "../DSP/ChannelReader.h"


using namespace Core;

// constructor
SampleGateNode::SampleGateNode(Graph* graph) : SPNode(graph)
{	
	// Note: no channel coloring, we forward everything from the inputs
}


// destructor
SampleGateNode::~SampleGateNode()
{
	// remove all output channels
	const uint32 numChannels = mChannels.Size();
	for (uint32 i = 0; i < numChannels; ++i)
		delete mChannels[i];
	mChannels.Clear();
}


// initialize the node
void SampleGateNode::Init()
{
	// init base class first
	SPNode::Init();

	RequireInputConnection();
	UseChannelNamePropagation();

	// PORTS
	InitInputPorts(2);
	GetInputPort(INPUTPORT_VALUE).SetupAsChannels<double>("In", "x1", INPUTPORT_VALUE);
	GetInputPort(INPUTPORT_CONTROL).SetupAsChannels<double>("Control", "x2", INPUTPORT_CONTROL);
	InitOutputPorts(1);
	GetOutputPort(OUTPUTPORT_VALUE).SetupAsChannels<double>("Out", "y1", OUTPUTPORT_VALUE);

	// ATTRIBUTES

	// Channel Name
	Core::AttributeSettings* defaultState = RegisterAttribute("Default State", "defaultState", "The default state of the STATE, used in case the control input has no data or is not connected.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	defaultState->ResizeComboValues(2);
	defaultState->SetComboValue(STATE_CLOSED, "Closed");
	defaultState->SetComboValue(STATE_OPEN, "Open");
	defaultState->SetDefaultValue(Core::AttributeInt32::Create(0));
}


void SampleGateNode::Reset()
{
	SPNode::Reset();

	// remove all output channels
	const uint32 numChannels = mChannels.Size();
	for (uint32 i = 0; i < numChannels; ++i)
		delete mChannels[i];
	mChannels.Clear();

	GetOutputPort(OUTPUTPORT_VALUE).GetChannels()->Clear();
}


void SampleGateNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	SPNode::ReInit(elapsed, delta);

	// require connection on value port
	if (GetInputPort(INPUTPORT_VALUE).HasConnection() == false)
		mIsInitialized = false;
	
	// check number of control channels (must be either 1 or match the size of the value channels)
	if (mIsInitialized == true && GetInputPort(INPUTPORT_CONTROL).HasConnection() == true)
	{
		const uint32 numControlChannels = GetInputPort(INPUTPORT_CONTROL).GetChannels()->GetNumChannels() ;
		if (numControlChannels!= 1 && numControlChannels != GetInputPort(INPUTPORT_VALUE).GetChannels()->GetNumChannels() )
		{
			mIsInitialized = false;
			SetError(ERROR_INPUT_INCOMPATIBLE, "Control port has wrong number of channels.");
		}
		else
			ClearError(ERROR_INPUT_INCOMPATIBLE);
	}
	else
	{
		ClearError(ERROR_INPUT_INCOMPATIBLE);
	}

	PostReInit(elapsed, delta);
}


// start node
void SampleGateNode::Start(const Time& elapsed)
{
	CORE_ASSERT(mChannels.Size() == 0);
	CORE_ASSERT(GetOutputPort(OUTPUTPORT_VALUE).GetChannels()->GetNumChannels() == 0);

	// create output channels and set them up
	MultiChannel* inChannels = GetInputPort(INPUTPORT_VALUE).GetChannels();

	const uint32 numChannels = inChannels->GetNumChannels();
	for (uint32 i = 0; i < numChannels; ++i)
	{
		Channel<double>* outChannel = new Channel<double>();
		Channel<double>* inChannel = inChannels->GetChannel(i)->AsType<double>();

		// set any buffersize > 0
		outChannel->SetBufferSize(10);

		// add and connect to outputport
		mChannels.Add(outChannel);
		GetOutputPort(OUTPUTPORT_VALUE).GetChannels()->AddChannel(outChannel);

		// STATE output has varying sample rate
		outChannel->SetSampleRate(0.0);

		// forward properties
		outChannel->SetUnit(inChannel->GetUnit());
		outChannel->SetColor(inChannel->GetColor());
		outChannel->SetMinValue(inChannel->GetMinValue());
		outChannel->SetMaxValue(inChannel->GetMaxValue());
	}

	// call baseclass start last
	SPNode::Start(elapsed);
}


// update the node
void SampleGateNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update base class
	SPNode::Update(elapsed, delta);

	// do nothing if node is not fully initialized
	if (mIsInitialized == false)
		return;


	// input channels
	const uint32 numValueChannels = GetInputPort(INPUTPORT_VALUE).GetChannels()->GetNumChannels();

	MultiChannel* controlChannels = GetInputPort(INPUTPORT_CONTROL).GetChannels();
	const uint32 numControlChannels = (controlChannels == NULL ? 0 : controlChannels->GetNumChannels());
	const uint32 controlReaderIndexStart = numValueChannels;

	// check if we have the same samplerate on every channel
	const bool hasUniformSamplerate = mInputReader.HasUniformSampleRate();

	//
	// Mode 1: uniform samplerates accross channels -> we have exactly one control sample for each value sample
	//
	if (hasUniformSamplerate == true && mInputReader.GetSampleRate() != 0)
	{

		//
		// Mode 1a: single ctrl channel to control all value channels
		//
		if (numControlChannels == 1)
		{
			// control value input reader (last index)
			ChannelReader* controlReader = mInputReader.GetReader(controlReaderIndexStart);

			// forward individual samples if the control value is >= 1.0
			const uint32 numSamples = mInputReader.GetMinNumNewSamples(); 
			for (uint32 i = 0; i < numSamples; ++i)
			{
				// get control value
				const double controlValue = controlReader->PopOldestSample<double>();
				const bool stateOpen = (controlValue >= 1.0);

				// forward or discard the sample
				if (stateOpen == true)
					for (uint32 c = 0; c < numValueChannels; ++c)
						mChannels[c]->AddSample( mInputReader.GetReader(c)->PopOldestSample<double>() );
				else  // discard
					for (uint32 c = 0; c < numValueChannels; ++c)
						mInputReader.GetReader(c)->PopOldestSample<double>();
			}
		}
		// Mode 1b: single ctrl channel for each value input
		else if (numControlChannels == numValueChannels)
		{
			// forward individual samples if the control value is >= 1.0
			const uint32 numSamples = mInputReader.GetMinNumNewSamples(); 
			for (uint32 i = 0; i < numSamples; ++i)
			{
				for (uint32 c = 0; c < numValueChannels; ++c)
				{
					// get control value
					ChannelReader* controlReader = mInputReader.GetReader( controlReaderIndexStart+c );
					const double controlValue = controlReader->PopOldestSample<double>();
					const bool stateOpen = (controlValue >= 1.0);

					// forward or discard value
					if (stateOpen == true)
						mChannels[c]->AddSample( mInputReader.GetReader(c)->PopOldestSample<double>() );
					else
						mInputReader.GetReader(c)->PopOldestSample<double>();
				}
			}
		
		} // Mode 1c: no control channel; just use the default value
		else if (numControlChannels == 0)
		{
			const double controlValue = (double)GetInt32Attribute(ATTRIB_DEFAULTSTATE);
			const bool stateOpen = (controlValue >= 1.0);

			// process all channels individually
			for (uint32 c = 0; c < numValueChannels; ++c)
			{
				ChannelReader* channelReader = mInputReader.GetReader(c);
				const uint32 numSamples = channelReader->GetNumNewSamples();

				// forward or discard all samples
				if (stateOpen == true)
				{
					for (uint32 i=0; i<numSamples; ++i)
						mChannels[c]->AddSample( channelReader->PopOldestSample<double>() );
				}
				else
					channelReader->Flush();
			}
		}
		else
		{
			CORE_ASSERT(false);
		}
	}
	
	//
	// Mode 2: control channel samplerate are different from the value channels -> only read the newest value and use it as control value
	//
	else
	{
		// Mode 2a: single ctrl channel to control all value channels
		if (numControlChannels == 1)
		{
			// get the control value and clear the reader
			ChannelReader* controlReader = mInputReader.GetReader(controlReaderIndexStart);
			double controlValue = (double)GetInt32Attribute(ATTRIB_DEFAULTSTATE);
			if (controlReader->GetNumNewSamples() > 0)
			{
				controlValue = controlReader->GetNewestSample<double>();
				controlReader->Flush();
			}
			
			const bool stateOpen = (controlValue >= 1.0);

			// process all channels individually
			for (uint32 c = 0; c < numValueChannels; ++c)
			{
				ChannelReader* channelReader = mInputReader.GetReader(c);
				const uint32 numSamples = channelReader->GetNumNewSamples();

				// forward or discard all samples
				if (stateOpen == true)
				{
					for (uint32 i=0; i<numSamples; ++i)
						mChannels[c]->AddSample( channelReader->PopOldestSample<double>() );
				}
				else
					channelReader->Flush();
			}
		}

		// Mode 2b: separate ctrl channel for each value input
		else if (numControlChannels == numValueChannels)
		{
			// process all channels individually
			for (uint32 c = 0; c < numValueChannels; ++c)
			{
				// get the control value and clear the reader
				ChannelReader* controlReader = mInputReader.GetReader(controlReaderIndexStart + c);
				double controlValue = GetFloatAttribute(ATTRIB_DEFAULTSTATE);
				if (controlReader->GetNumNewSamples() > 0)
				{
					controlValue = controlReader->GetNewestSample<double>();
					controlReader->Flush();
				}
			
				const bool stateOpen = (controlValue >= 1.0);

				ChannelReader* channelReader = mInputReader.GetReader(c);
				//const uint32 numSamples = channelReader->GetNumNewSamples();

				// forward or discard all samples
				if (stateOpen == true)
					mChannels[c]->AddSample( channelReader->PopOldestSample<double>() );
				else
					channelReader->Flush();
			}
		}
		else if (numControlChannels == 0) // Mode 2c: no control channel -> use only the default value
		{
			const double controlValue = (double)GetInt32Attribute(ATTRIB_DEFAULTSTATE);
			const bool stateOpen = (controlValue >= 1.0);

			// process all channels individually
			for (uint32 c = 0; c < numValueChannels; ++c)
			{
				ChannelReader* channelReader = mInputReader.GetReader(c);
				const uint32 numSamples = channelReader->GetNumNewSamples();

				// forward or discard all samples
				if (stateOpen == true)
				{
					for (uint32 i=0; i<numSamples; ++i)
						mChannels[c]->AddSample( channelReader->PopOldestSample<double>() );
				}
				else
					channelReader->Flush();
			}
		}
		else
		{
			CORE_ASSERT(false);
		}
	}

}


// an attribute has changed
void SampleGateNode::OnAttributesChanged()
{
}

