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
#include "BodyFeedbackNode.h"
#include "../EngineManager.h"
#include "Graph.h"


using namespace Core;

// constructor
BodyFeedbackNode::BodyFeedbackNode(Graph* parentGraph) : FeedbackNode(parentGraph)
{
}


// destructor
BodyFeedbackNode::~BodyFeedbackNode()
{
}


// initialize
void BodyFeedbackNode::Init()
{
	const uint32 numChannels = 3;

	// setup channels
	for (uint32 i=0; i<numChannels; ++i)
		AddOutput();

	GetOutputChannel(0)->SetName("Heart Rate");		GetOutputChannel(0)->SetUnit("BPM");	GetOutputChannel(0)->SetMinValue(30.0);		GetOutputChannel(0)->SetMaxValue(220.0);
	GetOutputChannel(1)->SetName("HRV");			GetOutputChannel(1)->SetUnit("ms");		GetOutputChannel(1)->SetMinValue(0.0);		GetOutputChannel(1)->SetMaxValue(200.0);
	GetOutputChannel(2)->SetName("Breathing Rate");	GetOutputChannel(2)->SetUnit("BPM");	GetOutputChannel(2)->SetMinValue(0.0);		GetOutputChannel(2)->SetMaxValue(1.0);

	// setup the input ports
	InitInputPorts(numChannels);
	GetInputPort(INPUTPORT_HEARTRATE).SetupAsChannels<double>("Heart Rate", "heartRate", INPUTPORT_HEARTRATE);
	GetInputPort(INPUTPORT_HRV).SetupAsChannels<double>("HRV", "hrv", INPUTPORT_HRV);
	GetInputPort(INPUTPORT_BREATHINGRATE).SetupAsChannels<double>("Breathing Rate", "breathingRate", INPUTPORT_BREATHINGRATE);

	// init base class
	FeedbackNode::Init();

	// create user id attribute
	AttributeSettings* userID = RegisterAttribute("User ID", "id", "In case multiple people are connected simultaneously, use this ID to identify the user.", ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	userID->SetDefaultValue( AttributeInt32::Create(0) );
	userID->SetMinValue( AttributeInt32::Create(0) );
	userID->SetMaxValue( AttributeInt32::Create(CORE_INT32_MAX) );

	// hide  OSC address
	GetAttributeSettings(ATTRIB_OSCADDRESS)->SetVisible(false);
}


void BodyFeedbackNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// base class reinitialization
	FeedbackNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


void BodyFeedbackNode::Reset()
{
	FeedbackNode::Reset();
}


// update the node
void BodyFeedbackNode::Update(const Time& elapsed, const Time& delta)
{
	// shared base update helper
	if (BaseUpdate(elapsed, delta) == false)
		return;

	FeedbackNode::Update(elapsed, delta);

	UpdateResamplers(elapsed, delta);
}


// write the OSC message
void BodyFeedbackNode::WriteOscMessage(OscPacketParser::OutStream* outStream)
{
	// heart rate
	if (IsValidInput(INPUTPORT_HEARTRATE) == true)
	{
		outStream->BeginMessage( "/heartrate" );
			const float heartRate = GetHeartRate();
			outStream->WriteValue(heartRate);
		outStream->EndMessage();
	}

	// HRV
	if (IsValidInput(INPUTPORT_HRV) == true)
	{
		outStream->BeginMessage( "/hrv" );
			const float hrv = GetHRV();
			outStream->WriteValue(hrv);
		outStream->EndMessage();
	}

	// breathing rate
	if (IsValidInput(INPUTPORT_BREATHINGRATE) == true)
	{
		outStream->BeginMessage( "/breathingrate" );
			const float breathingRate = GetBreathingRate();
			outStream->WriteValue(breathingRate);
		outStream->EndMessage();
	}
}
