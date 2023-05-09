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
#include "FreezeNode.h"
#include "../Core/EventManager.h"
#include "../Core/Math.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
FreezeNode::FreezeNode(Graph* graph) : ProcessorNode(graph, new FreezeNode::Processor())
{
	// defaults
	mSettings.mSetByTime = true;
	mSettings.mDelayInSeconds = 1;
	mSettings.mDelayInSamples = 100;
	mSettings.mPassThrough = true;
}


// destructor
FreezeNode::~FreezeNode()
{
}


// initialize the node
void FreezeNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	// CONFIG SPNODE
	RequireConstantSampleRate(mSettings.mSetByTime == true);

	// SETUP PORTS

	// setup the input ports
	GetInputPort(INPUTPORT_VALUE).Setup("In", "x", AttributeChannels<double>::TYPE_ID, INPUTPORT_VALUE);

	// setup the output ports
	GetOutputPort(OUTPUTPORT_RESULT).Setup("Out", "y", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_RESULT);

	// ATTRIBUTES


	// delay type combo box
	const uint32 defaultType = (mSettings.mSetByTime ? DELAYTYPE_DURATION : DELAYTPE_NUMSAMPLES);
	Core::AttributeSettings* intervalTypeAttr = RegisterAttribute("Delay Type", "delayType", "How the delay should be determined", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	intervalTypeAttr->ResizeComboValues(2);
	intervalTypeAttr->SetComboValue(DELAYTYPE_DURATION, "Duration");
	intervalTypeAttr->SetComboValue(DELAYTPE_NUMSAMPLES, "Sample Count");
	intervalTypeAttr->SetDefaultValue(Core::AttributeInt32::Create(defaultType));

	// set by seconds														//  TODO: that vvv is a legacy name
	Core::AttributeSettings* delaySecondsParam = RegisterAttribute("Delay (Seconds)", "freezeTime", "Freeze after receiving this many seconds of data. Note: The resolution is limited by the sample rate of the signal.", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	delaySecondsParam->SetMinValue(Core::AttributeFloat::Create(0));
	delaySecondsParam->SetMaxValue(Core::AttributeFloat::Create(FLT_MAX));
	delaySecondsParam->SetDefaultValue(Core::AttributeFloat::Create(mSettings.mDelayInSeconds));
	delaySecondsParam->SetVisible(mSettings.mSetByTime == true);

	// set by samples
	Core::AttributeSettings* delaySamplesParam = RegisterAttribute("Delay (Samples)", "delayInSamples", "Freeze after x samples were received.", ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	delaySamplesParam->SetMinValue(Core::AttributeInt32::Create(0));
	delaySamplesParam->SetMaxValue(Core::AttributeInt32::Create(INT_MAX));
	delaySamplesParam->SetDefaultValue(Core::AttributeInt32::Create(mSettings.mDelayInSamples));
	delaySamplesParam->SetVisible(mSettings.mSetByTime == false);

	// set by samples
	Core::AttributeSettings* delayModeParam = RegisterAttribute("Pass Through", "passThrough", "If enabled, the input is forwarded until the freeze is triggered.", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	delayModeParam->SetDefaultValue( Core::AttributeBool::Create(mSettings.mPassThrough) );
}


void FreezeNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


void FreezeNode::Start(const Time& elapsed)
{
	// update settings right before start (depends on sample rate)
	if (mSettings.mSetByTime == true)
		SetDelayByTime(mSettings.mDelayInSeconds);
	else
		SetDelayBySamples(mSettings.mDelayInSamples);

	ProcessorNode::Start(elapsed);
}


// update the node
void FreezeNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update the baseclass
	ProcessorNode::Update(elapsed, delta);
}


void FreezeNode::OnAttributesChanged()
{
	const bool setDelayByTime = (GetInt32Attribute(ATTRIB_DELAY_TYPE) == DELAYTYPE_DURATION);
	const uint32 delayInSamples = GetInt32Attribute(ATTRIB_DELAY_NUMSAMPLES);
	const double delayInSeconds = GetFloatAttribute(ATTRIB_DELAY_DURATION);
	const bool passThrough = GetBoolAttribute(ATTRIB_PASSTHROUGH);

	mSettings.mPassThrough = passThrough;	// doesnt require reset

	// no changes
	if (setDelayByTime == mSettings.mSetByTime &&
		delayInSeconds == mSettings.mDelayInSeconds &&
		delayInSamples == mSettings.mDelayInSamples)
	{
		return;
	}

	mSettings.mSetByTime = setDelayByTime;
	mSettings.mDelayInSeconds = delayInSeconds;
	mSettings.mDelayInSamples = delayInSamples;
	
	if (mSettings.mSetByTime == true)
		SetDelayByTime(mSettings.mDelayInSeconds);
	else
		SetDelayBySamples(mSettings.mDelayInSamples);

	// update spnode settings
	RequireConstantSampleRate(mSettings.mSetByTime == true);

	ResetAsync();
}



// attribute change helpers: set by num samples
void FreezeNode::SetDelayBySamples(uint32 delayInSamples)
{
	mSettings.mDelayInSamples = delayInSamples;
	mSettings.mSetByTime = false;

	// we only can recalc delay time if we have a connection
	if (HasIncomingConnections() == true && GetInputPort(INPUTPORT_VALUE).GetChannels()->GetSampleRate() > 0)
	{
		const double sampleRate = GetInputPort(INPUTPORT_VALUE).GetChannels()->GetSampleRate();
		if (sampleRate > 0)
			mSettings.mDelayInSeconds = mSettings.mDelayInSamples / sampleRate;
		else
			mSettings.mDelayInSeconds = 0.0;

		mTempString.Format("%.2fs", mSettings.mDelayInSeconds);
	}
	else
	{
		mSettings.mDelayInSeconds = 0.0;
	}

	// update seconds attribute
	SetFloatAttribute("freezeTime", mSettings.mDelayInSeconds);
	SetInt32Attribute("delayType", (mSettings.mSetByTime ? DELAYTYPE_DURATION : DELAYTPE_NUMSAMPLES));

	GetAttributeSettings(ATTRIB_DELAY_DURATION)->SetVisible(false);
	GetAttributeSettings(ATTRIB_DELAY_NUMSAMPLES)->SetVisible(true);

	// update port name
	mTempString.Format("%i smpl", mSettings.mDelayInSamples);
	GetOutputPort(OUTPUTPORT_RESULT).SetName(mTempString.AsChar());

	// fire event
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_DELAY_DURATION)) );
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_DELAY_NUMSAMPLES)) );
}


// attribute change helpers: set by time
void FreezeNode::SetDelayByTime(double delayInSeconds)
{
	mSettings.mDelayInSeconds = delayInSeconds;
	mSettings.mSetByTime = true;

	// calculate num sampels and round (if connection is present) and then recalculate the delay time
	if (HasIncomingConnections() == true)
	{
		double sampleRate = GetInputPort(INPUTPORT_VALUE).GetChannels()->GetSampleRate();
		if (sampleRate > 0)
		{
			mSettings.mDelayInSamples = (uint32)(mSettings.mDelayInSeconds * sampleRate + 0.5);
			mSettings.mDelayInSeconds = mSettings.mDelayInSamples / sampleRate;
		}
		else
		{
			mSettings.mDelayInSamples = 0.0;
		}
	}

	// update all attribute
	SetFloatAttribute("freezeTime", mSettings.mDelayInSeconds);
	SetInt32Attribute("delayInSamples", mSettings.mDelayInSamples);
	SetInt32Attribute("delayType", (mSettings.mSetByTime ? DELAYTYPE_DURATION : DELAYTPE_NUMSAMPLES));

	// update port name
	mTempString.Format("%.2fs", mSettings.mDelayInSeconds);
	GetOutputPort(OUTPUTPORT_RESULT).SetName(mTempString.AsChar());

	GetAttributeSettings(ATTRIB_DELAY_DURATION)->SetVisible(true);
	GetAttributeSettings(ATTRIB_DELAY_NUMSAMPLES)->SetVisible(false);

	// fire events
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_DELAY_DURATION)) );
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_DELAY_NUMSAMPLES)) );
}

//-----------------------------------------------
// the node's processor implementation
//-----------------------------------------------

void FreezeNode::Processor::ReInit()					
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

	mFreezeSampleCounter = 0;
	
	mIsInitialized = true;
}


void FreezeNode::Processor::Update()
{
	if (mIsInitialized == false)
		return;

	// update base
	ChannelProcessor::Update();
				
	// channel input reader
	ChannelReader* input = GetInputReader(0);

	// input/output channels
	Channel<double>* channel = GetInput()->AsType<double>();
	Channel<double>* output = GetOutput()->AsType<double>();

	// we need at least one connected channel
	if (channel == NULL)
		return;

	// number of outputted samples in total
	const bool outputZeros = channel->GetSampleRate() > 0;

	// iterate over new input samples
	uint32 numNewSamples = input->GetNumNewSamples();
	for (uint32 i = 0; i < numNewSamples; ++i)
	{
		// is the freeze triggert at this sample? If so, remember value
		const bool isReached = (mFreezeSampleCounter == mSettings.mDelayInSamples);
		if (isReached == true)
			mFrozenValue = input->PopOldestSample<double>();
		
		// is the node in frozen state?
		const bool isFrozen = (mFreezeSampleCounter >= mSettings.mDelayInSamples);
	
		// three different states
		if (isFrozen == true)
		{
			// output frozen value
			output->AddSample(mFrozenValue);
			
			// pop sample (only if we did not trigger in this loop iteration
			if (isReached == false)
				input->PopOldestSample<double>();
		}
		else if (mSettings.mPassThrough == true)
		{
			// freeze not triggered + pass through enabled : forward samples
			const double value = input->PopOldestSample<double>();
			output->AddSample(value);
		}
		else 
		{
			// freeze not triggered  and passthrough disable : swallow input sample, output zero value if input is constant samplerate
			input->PopOldestSample<double>();
			if (outputZeros == true)
				output->AddSample(0.0);
		}

		// increment sample counter
		mFreezeSampleCounter++;
	}

}
