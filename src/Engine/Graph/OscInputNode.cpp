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
#include "OscInputNode.h"
#include "../EngineManager.h"
#include "../Sensor.h"

using namespace Core;

// constructor
OscInputNode::OscInputNode(Graph* graph) : InputNode(graph), OscReceiver()
{
	// FIXME use member variable for sensor; this requires changes to the constructor (we cannot create a sensor with resampler using the default constructor)
	mSensor = NULL;

	// color the output channel same as the node
	UseChannelColoring();
}


// destructor
OscInputNode::~OscInputNode()
{
	GetOscMessageRouter()->UnregisterReceiver(this);

	delete mSensor;
}


// initialize the node
void OscInputNode::Init()
{
	// register with router
	GetOscMessageRouter()->RegisterReceiver(this);

	// defaults
	String defaultOscAdress = GenerateUniqueOscAddress();
	SetOscAddress(defaultOscAdress);
	const uint32 defaultSampleRate = 128;

	// configure sensor
	mSensor = new Sensor("OSC Sensor",  defaultSampleRate, 0.0);
	mSensor->SetDriftCorrectionEnabled(false);
	mSensor->GetChannel()->SetBufferSize(10);

	// PORTS

	InitOutputPorts(1);
	GetOutputPort(OUTPUTPORT).Setup(defaultOscAdress.AsChar(), "y1", AttributeChannels<double>::TYPE_ID, OUTPUTPORT);
	GetOutputPort(OUTPUTPORT).GetChannels()->AddChannel(mSensor->GetChannel());

	// ATTRIBUTES

	// baseclass attributes
	InputNode::Init();

	// path
	Core::AttributeSettings* attribute = RegisterAttribute( "OSC address", "oscAddress", "The OSC address that is listened to. The node will always output the last received float value.", Core::ATTRIBUTE_INTERFACETYPE_STRING);
	attribute->SetDefaultValue( Core::AttributeString::Create(defaultOscAdress.AsChar()));

	// sample rate
	attribute = RegisterAttribute( "Sample rate", "sampleRate", "Sample rate of the incoming data.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attribute->SetMinValue( Core::AttributeFloat::Create(FLT_MIN) );
	attribute->SetMaxValue( Core::AttributeFloat::Create(FLT_MAX) );
	attribute->SetDefaultValue( Core::AttributeFloat::Create(defaultSampleRate) );
}


void OscInputNode::Reset()
{
	InputNode::Reset();
}


// note : overrides InputNode::ReInit();
void OscInputNode::ReInit(const Time& elapsed, const Time& delta)
{
	InputNode::ReInit(elapsed, delta);

	// stop node if nothing is connected
	if (mParentGraph->CalcNumOutputConnections(this) == 0)
		mIsInitialized = false;

	if (IsOscAddressUnique(GetOscAddress()) == false)
	{
		SetError(ERROR_DUPLICATE_OSC_ADDRESS, "OSC address is not unique");
		mIsInitialized = false;
	}
	else
	{
		ClearError(ERROR_DUPLICATE_OSC_ADDRESS);
	}
	

	PostReInit(elapsed, delta);
}


// use the osc address attribute and set the channel and output port name accordingly
void OscInputNode::UpdateChannelAndPortNames()
{
	// construct channel name
	const char* oscPath = GetStringAttribute(ATTRIB_OSCADDRESS);
	mTempString.Format("osc:/%s", oscPath);
	mSensor->SetName(mTempString.AsChar());

	// set output port name
	OutputPort& outputPort = GetOutputPort(OUTPUTPORT);
	outputPort.SetName(oscPath);
}


// override Start() and align output channels to absolute time, not relative to input
void OscInputNode::Start(const Time& elapsed)
{
	// restart output 
	mSensor->Reset();
	mSensor->SetStartTime(elapsed);

	InputNode::Start(elapsed);

	// set the channel sample rates
	const float sampleRate = GetFloatAttribute(ATTRIB_SAMPLERATE);
	mSensor->SetSampleRate(sampleRate);

	// update channel and port name
	UpdateChannelAndPortNames();
}


// update the node
void OscInputNode::Update(const Time& elapsed, const Time& delta)
{	
	// shared base update helper
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update base class
	InputNode::Update(elapsed, delta);
}


void OscInputNode::OnAttributesChanged()
{
	// set the sample rate
	const double sampleRate = GetFloatAttribute(ATTRIB_SAMPLERATE);

	// update channel and port name
	UpdateChannelAndPortNames();

	// update address of osc receiver
	SetOscAddress(GetStringAttribute(ATTRIB_OSCADDRESS));

	// in case the sample rate changed, reset the node and channels
	if (mSensor->GetChannel()->GetSampleRate() != sampleRate)
		ResetAsync();
}



// generate a unique OSC address
Core::String OscInputNode::GenerateUniqueOscAddress()
{
	String result;
	for (uint32 i=0; i<CORE_INT16_MAX; ++i)
	{
		// set the OSC address for the current iteration
		result.Format("/in/%i", i);

		// found a unique address
		if (IsOscAddressUnique(result.AsChar()) == true)
			return result;
	}

	// if we arrive here, no valid osc address was found :(
	LogError("Could not create a unique OSC address!");
	CORE_ASSERT(false);
	result = "/could/not/find/a/unique/osc/address/wtf";
	return result;
}


bool OscInputNode::IsOscAddressUnique(const char* address)
{
	// make sure the feedback node is part of a valid classifier
	if (mParentGraph->GetType() != Classifier::TYPE_ID)
	{
		LogError("Cannot create unique feedback OSC address. Feedback node is not part of a valid classifier.");
		CORE_ASSERT(false);
		return false;
	}

	// down-cast graph to a classifier
	Classifier* classifier = static_cast<Classifier*>(mParentGraph);

	// get the number of osc output nodes
	const uint32 numNodes = classifier->GetNumNodes();
	for (uint32 i=0; i<numNodes; ++i)
	{
		Node* node = classifier->GetNode(i);

		if (node == this)
			continue;

		if (node->GetType() == OscInputNode::TYPE_ID)
		{
			OscInputNode* oscNode = static_cast<OscInputNode*>(node);
			const char* oscAddr = oscNode->GetOscAddress();

			// is address not unique?
			if (strcmp(oscAddr, address) == 0)
				return false;
		}
	}

	// no other address found, it is unique
	return true;
}


void OscInputNode::ProcessMessage(OscMessageParser* message)
{
	// address
	const char* messageAddress = message->GetAddress();
	
	// one sample value
	float value = 0;
	
	// match address of all channels 
	const char* channelAddress = GetStringAttribute(ATTRIB_OSCADDRESS);
	if (std::strcmp(messageAddress, channelAddress) == 0)
	{
		// process all float arguments // TODO check argument order! but should be correct
		const uint32 numArgs = message->GetNumArguments();
		for (uint32 a=0; a<numArgs; ++a)
		{
			(*message) >> value;
			mSensor->AddQueuedSample(value);
		}
	}
}
