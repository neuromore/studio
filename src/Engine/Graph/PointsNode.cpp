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
#include "PointsNode.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
PointsNode::PointsNode(Graph* parentGraph) : OutputNode(parentGraph)
{
}


// destructor
PointsNode::~PointsNode()
{
}


// init
void PointsNode::Init()
{	
	// setup the input ports
	InitInputPorts(1);
	GetInputPort(INPUTPORT_VALUE).SetupAsChannels<double>("In", "x", INPUTPORT_VALUE);

	// setup output
	AddOutput();
	Channel<double>* output = GetOutputChannel(0);
	output->SetName("Points");
	output->SetUnit("Points");
	output->SetMinValue(0.0);
	output->SetMaxValue(0.0);

	// init base class
	OutputNode::Init();

	// create the points multiplier attribute
	Core::AttributeSettings* valueParam = RegisterAttribute("Points Multiplier", "points", ".", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	valueParam->SetDefaultValue( Core::AttributeInt32::Create(0) );
	valueParam->SetMinValue( Core::AttributeInt32::Create(0) );
	valueParam->SetMaxValue( Core::AttributeInt32::Create(10000) );

	// hide unused base class attributes
	GetAttributeSettings(ATTRIB_SIGNALRESOLUTION)->SetVisible(false);
	GetAttributeSettings(ATTRIB_UPLOAD)->SetVisible(false);
}


// reset the node
void PointsNode::Reset()
{
	OutputNode::Reset();

	mPoints = 0; 
	mIntegratedInput.Reset();
}


// reinitialize
void PointsNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	OutputNode::ReInit(elapsed, delta);

	// configure points integrating channel with the same parameters as the input channel
	if (HasIncomingConnections() == true && mInputReader.GetNumChannels() > 0)
	{
		ChannelBase* input = mInputReader.GetChannel(0);
		mIntegratedInput.SetSampleRate(input->GetSampleRate());
		mIntegratedInput.SetBufferSize(input->GetBufferSize());
	}

	PostReInit(elapsed, delta);
}


// node startup
void PointsNode::Start(const Time& elapsed)
{
	OutputNode::Start(elapsed);

	// set min/max value of output channel so it scales correctly in reports
	mChannels[0]->SetMinValue(0.0);
	mChannels[0]->SetMaxValue(0.0);
}


// update the node
void PointsNode::Update(const Time& elapsed, const Time& delta)
{
	// shared base update helper
	if (BaseUpdate(elapsed, delta) == false)
		return;

	OutputNode::Update(elapsed, delta);

	// nothing is connected
	if (HasIncomingConnections() == false)
		return;

	// require at least one channel
	MultiChannel* input = GetInputPort(INPUTPORT_VALUE).GetChannels();
	if (input->GetNumChannels() == 0)
		return;

	const double pointsMultiplier = (double)GetInt32Attribute(ATTRIB_POINTSMULTIPLIER);

	// use zero as special value: enables new behaviour and skips the integration code
	// TODO make this the default
	if (pointsMultiplier == 0)
	{
		// there must be at least one sample
		if (input->GetChannel(0)->IsEmpty() == true)
			mPoints = 0;
		else
			mPoints = input->GetChannel(0)->AsType<double>()->GetLastSample();
	}
	else
	{
		DEPRECATED_Update(elapsed, delta);
	}
}


void PointsNode::DEPRECATED_Update(const Time& elapsed, const Time& delta)
{
	// reset new samples counter
	mIntegratedInput.BeginAddSamples();

	ChannelReader* channelReader = mInputReader.GetReader(0);
	
	// node attribute
	const double pointsMultiplier = (double)GetInt32Attribute(ATTRIB_POINTSMULTIPLIER);

	// time delta for integration
	MultiChannel* input = GetInputPort(INPUTPORT_VALUE).GetChannels();
	const double timeDelta = 1.0 / input->GetSampleRate();

	// for checking out-of-range
	//bool outOfRange = false;

	// accumulate points
	const uint32 numNewSamples = channelReader->GetNumNewSamples();
	for (uint32 i=0; i<numNewSamples; ++i)
	{
		double value = channelReader->PopOldestSample<double>();

		// set error in case the n-score value is not normalized
		//if (value > 1.0 || value < 0.0)
		//	outOfRange = true;

		// clamp the score value
		value = Clamp<double>(value, 0.0, 1.0);

		// calculate the game points based on the integrated area over time
		const double pointsToAdd = value * pointsMultiplier * timeDelta;
		mPoints += pointsToAdd;
		
		mIntegratedInput.AddSample(mPoints);
	}

	// call output update
	UpdateResamplers(elapsed, delta);

	// set min/max value of output channel (changes every update)
	mChannels[0]->SetMaxValue(mPoints);
}
