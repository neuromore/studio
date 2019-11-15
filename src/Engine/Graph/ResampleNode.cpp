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
#include "ResampleNode.h"
#include "../Core/EventManager.h"


using namespace Core;

// constructor
ResampleNode::ResampleNode(Graph* graph) : ProcessorNode(graph, new ResampleProcessor())
{
	mSettings.mTargetSampleRate = 128;
	mSettings.mResampleAlgo = ResampleProcessor::FORWARD;
	mSettings.mResampleType = ResampleProcessor::NO_RESAMPLING;
	mSettings.mResampleMode = ResampleProcessor::REALTIME;
}


// destructor
ResampleNode::~ResampleNode()
{
}


// initialize the node
void ResampleNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	// SETUP PORTS

	GetInputPort(INPUTPORT_CHANNEL).Setup("In", "x", AttributeChannels<double>::TYPE_ID, PORTID_INPUT);
	GetOutputPort(OUTPUTPORT_CHANNEL).Setup("Out", "y", AttributeChannels<double>::TYPE_ID, PORTID_OUTPUT);
	
	// ATTRIBUTES

	// initialize with default values from settings
	
	// Sample Rate
	const uint32 defaultSampleRate = mSettings.mTargetSampleRate;
	AttributeSettings* attr = RegisterAttribute("Sample Rate", "SampleRate", "The target sample rate.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attr->SetDefaultValue(Core::AttributeFloat::Create(defaultSampleRate));
	attr->SetMinValue(Core::AttributeFloat::Create(FLT_EPSILON));
	attr->SetMaxValue(Core::AttributeFloat::Create(FLT_MAX));

	// Resample Mode (disabled for now)
	const uint32 defaultMode = (uint32)mSettings.mResampleMode;
	attr = RegisterAttribute("Mode", "ResampleMode", "Select Realtime for applications where a zero delay is most important. Use 'Best Quality' if the signal's frequency spectrum is more important.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attr->AddComboValue("Realtime");
	attr->AddComboValue("Good Quality");
	//attr->AddComboValue("Best Quality");
	attr->SetDefaultValue(Core::AttributeInt32::Create(defaultMode));
	attr->SetIsEnabled(false);
}


void ResampleNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass -> reinit processors
	ProcessorNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);

	// FIXME: dirty solution; output channels are synced to engine; we cant set this in the processor because it gets overwritten by PropagateChannelMetaData() in SPNode::ReInit(); find a nice solution 
	//			Idea: splitting the propagate method into individual methods and add flags similar to the ones used for channel color propagation

	if (mIsInitialized == false)
		return;

	MultiChannel* channels = GetOutputPort(0).GetChannels();
	if (channels != NULL)
	{
		const uint32 numChannels = channels->GetNumChannels();
		for (uint32 i = 0; i < numChannels; ++i)
		{
			ChannelBase* channel = channels->GetChannel(i);
			if (channel != NULL)
				channel->SetIndependent(false);
		}
	}
}


void ResampleNode::Start(const Time& elapsed)
{
	// create and reinit processors etc
	ProcessorNode::Start(elapsed);

	// set resampler start time
	const uint32 numProcessors = mProcessors.Size();
	for (uint32 i = 0; i < numProcessors; ++i)
	{
		// note: no type check required for casting
		ResampleProcessor* processor = static_cast<ResampleProcessor*>(mProcessors[i]);
		processor->SetStartTime(elapsed);

		// reinit (again!)
		processor->ReInit();
	}
}



// update the node
void ResampleNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update baseclass
	ProcessorNode::Update(elapsed, delta);
}


// attributes have changed
void ResampleNode::OnAttributesChanged()
{
	// check if attributes have changed
	const double sampleRate =  GetFloatAttribute(ATTRIB_SAMPLERATE);
	const uint32 resampleMode = GetInt32Attribute(ATTRIB_MODE);
	
	// nothing changed?
	if (mSettings.mTargetSampleRate == sampleRate && 
		mSettings.mResampleMode == (ResampleProcessor::EResampleMode)resampleMode)
		return;
	
	mSettings.mTargetSampleRate = sampleRate;
	mSettings.mResampleMode = (ResampleProcessor::EResampleMode)resampleMode;
	
	ResetAsync();
}
