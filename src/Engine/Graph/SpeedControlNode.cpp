/****************************************************************************
**
** Copyright 2022 neuromore co
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
#include "SpeedControlNode.h"
#include "Classifier.h"
#include "../EngineManager.h"
#include "Graph.h"


using namespace Core;

// constructor
SpeedControlNode::SpeedControlNode(Graph* parentGraph) : CustomFeedbackNode(parentGraph)
{
}


// destructor
SpeedControlNode::~SpeedControlNode()
{
}


// initialize
void SpeedControlNode::Init()
{
	AddOutput();
	Channel<double>* output = GetOutputChannel(0);
	output->SetUnit("Score");
	output->SetMinValue(0.0);
	output->SetMaxValue(1.0);
    // init base class
	FeedbackNode::Init();

	// setup the input ports
	InitInputPorts(1);
	GetInputPort(INPUTPORT_VALUE).SetupAsChannels<double>("In", "x", INPUTPORT_VALUE);

    HideAttributes();
}


void SpeedControlNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	FeedbackNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


void SpeedControlNode::Reset()
{
	FeedbackNode::Reset();
}


// update the node
void SpeedControlNode::Update(const Time& elapsed, const Time& delta)
{
	// shared base update helper
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update OutputNode baseclass (calls resamplers)
	FeedbackNode::Update(elapsed, delta);

	UpdateResamplers(elapsed, delta);

	Channel<double>* channel = mChannels[0]->AsType<double>();
	
	// nothing to do
	if (channel->IsEmpty() == true)
	{
		return;
	}

	const uint32 lastSampleIndex = channel->GetMaxSampleIndex();
	const uint32 numSamples = GetLastBurstSize(0);

	// iterate over the last added samples and check the range
	bool outOfRange = false;
	for (uint32 i=0; i<numSamples; ++i)
	{
		const uint32 sampleIndex = lastSampleIndex - i;
		if (i>lastSampleIndex)
			break;

		double value = channel->GetSample(sampleIndex); 
		
		// set error in case the the value is not in range
		if (value > GetRangeMax() || value < GetRangeMin())
		{
			outOfRange = true;
			break;
		}
	}

	// we do not want to reset the error message if no samples were received (because the out-of-range would not be detected)
	if (numSamples > 0)
	{
		if (outOfRange == true)
			SetError(ERROR_VALUE_RANGE, "Value not in range");
		else
			ClearError(ERROR_VALUE_RANGE);
	}
}


// attributes have changed
void SpeedControlNode::OnAttributesChanged()
{
}


double SpeedControlNode::GetCurrentValue(uint32 channelIndex) const
{
	CORE_ASSERT(channelIndex <= mInputReader.GetNumChannels());

	if (mIsInitialized == false)
		return 0.0;

	if (channelIndex >= mInputReader.GetNumChannels())
		return 0.0;

	if (mInputReader.GetChannel(channelIndex)->GetNumSamples() == 0)
		return 0.0;

	return mInputReader.GetChannel(channelIndex)->AsType<double>()->GetLastSample();
}

// check if feedback node has a current value
bool SpeedControlNode::IsEmpty(uint32 channelIndex) const
{
	CORE_ASSERT(channelIndex <= mInputReader.GetNumChannels());

	if (channelIndex >= mInputReader.GetNumChannels())
		return true;

	if (mInputReader.GetChannel(channelIndex)->GetNumSamples() == 0)
		return true;

	return false;
}

void SpeedControlNode::HideAttributes()
{
	GetAttributeSettings(ATTRIB_SENDOSCNETWORKMESSAGES)->SetVisible(false);
    GetAttributeSettings(ATTRIB_SENDOSCNETWORKMESSAGES)->SetDefaultValue(AttributeBool::Create(true));
	GetAttributeSettings(ATTRIB_OSCADDRESS)->SetVisible(false);
}

// write the OSC message
void SpeedControlNode::WriteOscMessage(OscPacketParser::OutStream* outStream)
{
	// only send out the OSC message in case we have samples inside the channel already
	if (IsValidInput(INPUTPORT_VALUE) == false)
		return;

	outStream->BeginMessage( "/movement-speed" );

		// 1. feedback value
		// output a float value and not the double (more OSC conform)
		const float feedbackValue = GetCurrentValue();
		outStream->WriteValue(feedbackValue);

		// 2. range
		outStream->WriteValue(true);

		// 3. range min
		const float rangeMin = GetRangeMin();
		outStream->WriteValue(rangeMin);

		// 4. range min
		const float rangeMax = GetRangeMax();
		outStream->WriteValue(rangeMax);

	outStream->EndMessage();
}

void SpeedControlNode::SetName(const char* name)
{
	FeedbackNode::SetName(name);
	mChannels[0]->SetName(name);
}
