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
#include "DelayNode.h"
#include "../Core/EventManager.h"
#include "../Core/Math.h"
#include "../EngineManager.h"

using namespace Core;

// constructor
DelayNode::DelayNode(Graph* graph) : ProcessorNode(graph, new DelayNode::Processor())
{
	// defaults
	mSettings.mSetByTime = true;
	mSettings.mDelayInSamples = 0;
	mSettings.mDelayInSeconds = 0;
}


// destructor
DelayNode::~DelayNode()
{
}


// initialize the node
void DelayNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	// CONFIG SPNODE
	RequireConstantSampleRate();

	// SETUP PORTS

	// setup the input ports
	GetInputPort(INPUTPORT_VALUE).Setup("In", "x", AttributeChannels<double>::TYPE_ID, INPUTPORT_VALUE);

	// setup the output ports
	GetOutputPort(OUTPUTPORT_RESULT).Setup("Out", "y", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_RESULT);

	// ATTRIBUTES

	// set by samples
	Core::AttributeSettings* delayModeParam = RegisterAttribute("Set by Time", "setByTime", "If true the delay stays the same if the sample rate changes.", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	delayModeParam->SetDefaultValue( Core::AttributeBool::Create(mSettings.mSetByTime) );

	// set by seconds
	Core::AttributeSettings* delaySecondsParam = RegisterAttribute("Delay (Seconds)", "delayInSeconds", "The time the signal should be delayed. Note: The resolution is limited by the sample rate of the signal.", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	delaySecondsParam->SetMinValue(Core::AttributeFloat::Create(0));
	delaySecondsParam->SetMaxValue(Core::AttributeFloat::Create(FLT_MAX));
	delaySecondsParam->SetDefaultValue(Core::AttributeFloat::Create(mSettings.mDelayInSeconds));
	delaySecondsParam->SetVisible(mSettings.mSetByTime == true);

	// set by samples
	Core::AttributeSettings* delaySamplesParam = RegisterAttribute("Delay (Samples)", "delayInSamples", "The number of samples the signal is delayed.", ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	delaySamplesParam->SetMinValue( Core::AttributeInt32::Create(0) );
	delaySamplesParam->SetMaxValue( Core::AttributeInt32::Create(INT_MAX) );
	delaySamplesParam->SetDefaultValue( Core::AttributeInt32::Create(mSettings.mDelayInSamples) );
	delaySamplesParam->SetVisible(mSettings.mSetByTime == false);

}


void DelayNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


// update the node
void DelayNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update the baseclass
	ProcessorNode::Update(elapsed, delta);
}


// start the node
void DelayNode::Start(const Time& elapsed)
{
	if (mSettings.mSetByTime == true)
		SetDelayByTime(mSettings.mDelayInSeconds);
	else
		SetDelayBySamples(mSettings.mDelayInSamples);

	// start the baseclass
	ProcessorNode::Start(elapsed);
}


void DelayNode::OnAttributesChanged()
{
	const bool setDelayByTime = GetBoolAttribute(ATTRIB_DELAY_SETBYTIME);
	const uint32 delayInSamples = GetInt32Attribute(ATTRIB_DELAY_NUMSAMPLES);
	const double delayInSeconds = GetFloatAttribute(ATTRIB_DELAY_DURATION);

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
void DelayNode::SetDelayBySamples(uint32 delayInSamples)
{
	mSettings.mDelayInSamples = delayInSamples;
	mSettings.mSetByTime = false;
	
	// we only can recalc delay time if we have a connection! 
	if (HasIncomingConnections() == true)
	{
		const double sampleRate = GetInputPort(INPUTPORT_VALUE).GetChannels()->GetSampleRate();

		if (sampleRate > 0)
			mSettings.mDelayInSeconds = mSettings.mDelayInSamples / sampleRate;
		else
			mSettings.mDelayInSeconds = 0.0;
	}
	else
		mSettings.mDelayInSeconds = 0.0;

	// update seconds attribute
	SetFloatAttribute("delayInSeconds", mSettings.mDelayInSeconds); 
	SetBoolAttribute("setByTime", false); 

	// hide/show attributes
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
void DelayNode::SetDelayByTime(double delayInSeconds)
{
	mSettings.mDelayInSeconds = delayInSeconds;
	mSettings.mSetByTime = true;

	// calculate num sampels and round (if connection is present) and then recalculate the delay time
	if (HasIncomingConnections() == true)
	{
		double sampleRate = GetInputPort(INPUTPORT_VALUE).GetChannels()->GetSampleRate();
		mSettings.mDelayInSamples = (uint32)(mSettings.mDelayInSeconds * sampleRate + 0.5);

		if (sampleRate > 0.0)
			mSettings.mDelayInSeconds = mSettings.mDelayInSamples / sampleRate;
		else
			mSettings.mDelayInSeconds = delayInSeconds;
	}
	else
	{
		// just use the preset time
		mSettings.mDelayInSeconds = delayInSeconds;
	}

	// update all attribute
	SetFloatAttribute("delayInSeconds", mSettings.mDelayInSeconds); 
	SetInt32Attribute("delayInSamples", mSettings.mDelayInSamples); 
	SetBoolAttribute("setByTime", true); 

	// hide/show attributes
	GetAttributeSettings(ATTRIB_DELAY_DURATION)->SetVisible(true);
	GetAttributeSettings(ATTRIB_DELAY_NUMSAMPLES)->SetVisible(false);

	// update port name
	mTempString.Format("%.2fs", mSettings.mDelayInSeconds);
	GetOutputPort(OUTPUTPORT_RESULT).SetName(mTempString.AsChar());

	// fire events
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_DELAY_DURATION)) );
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_DELAY_NUMSAMPLES)) );
}

//-----------------------------------------------
// the node's processor implementation
//-----------------------------------------------

void DelayNode::Processor::ReInit()					
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

	mIsInitialized = true;
}


void DelayNode::Processor::Update()
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
				
	// number of outputted samples in total
	uint64 numSamplesOut = output->GetSampleCounter();

	// iterate over new input samples that arrived 
	uint32 numNewSamples = channel->GetNumNewSamples();

	for (uint32 i = 0; i < numNewSamples; ++i)
	{
		double value = 0.0;
		
		// start popping samples as soon as the delay time is reached
		if (numSamplesOut >= mSettings.mDelayInSamples)
			value = input->PopOldestSample<double>();

		// output sample
		output->AddSample(value);
		numSamplesOut++;
	}
}
