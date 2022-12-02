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
#include "VolumeControlNode.h"
#include "Classifier.h"
#include "../EngineManager.h"
#include "Graph.h"


using namespace Core;

// constructor
VolumeControlNode::VolumeControlNode(Graph* parentGraph) : FeedbackNode(parentGraph)
{
}


// destructor
VolumeControlNode::~VolumeControlNode()
{
}


// initialize
void VolumeControlNode::Init()
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

	const Mode defaultMode = STUDIO_VOLUME;

    // mode type
	AttributeSettings* modeParam = RegisterAttribute("Mode", "Mode", "Select Volume Node mode.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	modeParam->ResizeComboValues( (uint32)MODE_NUM );
	for (uint32 i=0; i<MODE_NUM; ++i)
		modeParam->SetComboValue( i, GetModeString((Mode)i) );
	modeParam->SetDefaultValue( AttributeInt32::Create(defaultMode) );

	// file url
	AttributeSettings* fileUrl = RegisterAttribute("File", "File", "The local filename of the media file.", ATTRIBUTE_INTERFACETYPE_STRING);
	fileUrl->SetDefaultValue( Core::AttributeString::Create() );

	// node name
	AttributeSettings* nodeName = RegisterAttribute("Node", "Node", "The name of the Tone Generator Node.", ATTRIBUTE_INTERFACETYPE_STRING);
	nodeName->SetDefaultValue( Core::AttributeString::Create() );

	ShowAttributesForSignalType(defaultMode);

}


void VolumeControlNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	FeedbackNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


void VolumeControlNode::Reset()
{
	FeedbackNode::Reset();
}


// update the node
void VolumeControlNode::Update(const Time& elapsed, const Time& delta)
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
void VolumeControlNode::OnAttributesChanged()
{
	Mode mode = GetMode();

	if (mode != mCurrentMode)
	{
		mCurrentMode = mode;
		ResetAsync();
	}

	// show/hide attributes for the selected mode
	const uint32 signalType = GetInt32Attribute(ATTRIB_MODE);
	ShowAttributesForSignalType((Mode)signalType);
}


double VolumeControlNode::GetCurrentValue(uint32 channelIndex) const
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
bool VolumeControlNode::IsEmpty(uint32 channelIndex) const
{
	CORE_ASSERT(channelIndex <= mInputReader.GetNumChannels());

	if (channelIndex >= mInputReader.GetNumChannels())
		return true;

	if (mInputReader.GetChannel(channelIndex)->GetNumSamples() == 0)
		return true;

	return false;
}

const char* VolumeControlNode::GetModeString(Mode mode) const
{
	switch (mode)
	{
		case Mode::STUDIO_VOLUME:	        return "Studio Volume";
		case Mode::OVERALL_SYSTEM_VOLUME:	return "Overall System Volume";
		case Mode::SINGLE_MEDIA_FILE:	    return "Single Media File";
		case Mode::TONE:	                return "Tone";
        default:                            return "Unkonwn";
	}
}

// show only the attributes required for this signal type
void VolumeControlNode::ShowAttributesForSignalType(Mode type)
{
	GetAttributeSettings(ATTRIB_SIGNALRESOLUTION)->SetVisible(false);
	GetAttributeSettings(ATTRIB_UPLOAD)->SetVisible(false);
	GetAttributeSettings(ATTRIB_SENDOSCNETWORKMESSAGES)->SetVisible(false);
	GetAttributeSettings(ATTRIB_OSCADDRESS)->SetVisible(false);
	switch (type)
	{
		// amplitude, frequency, dcoffset
		case STUDIO_VOLUME:
		case OVERALL_SYSTEM_VOLUME:
			GetAttributeSettings(ATTRIB_MODE)->SetVisible(true);
			GetAttributeSettings(ATTRIB_FILENAME)->SetVisible(false);
			GetAttributeSettings(ATTRIB_NODENAME)->SetVisible(false);
			break;
		case TONE:
			GetAttributeSettings(ATTRIB_MODE)->SetVisible(true);
			GetAttributeSettings(ATTRIB_FILENAME)->SetVisible(false);
			GetAttributeSettings(ATTRIB_NODENAME)->SetVisible(true);
			break;

		// squarewave has also dutycycle
		case SINGLE_MEDIA_FILE:
			GetAttributeSettings(ATTRIB_MODE)->SetVisible(true);
			GetAttributeSettings(ATTRIB_FILENAME)->SetVisible(true);
			GetAttributeSettings(ATTRIB_NODENAME)->SetVisible(false);
			break;
		default:
			break;
	}
}