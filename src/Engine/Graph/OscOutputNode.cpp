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
#include "OscOutputNode.h"
#include "Classifier.h"
#include "../EngineManager.h"
#include "Graph.h"


using namespace Core;

// constructor
OscOutputNode::OscOutputNode(Graph* parentGraph) : OutputNode(parentGraph)
{
}


// destructor
OscOutputNode::~OscOutputNode()
{
}


// initialize
void OscOutputNode::Init()
{
	String uniqueOscAddress = GenerateUniqueOscAddress();

	// setup channels
	AddOutput();
	Channel<double>* output = GetOutputChannel(0);
	output->SetUnit("");
	output->SetMinValue(0.0);
	output->SetMaxValue(1.0);

	// setup the input ports
	InitInputPorts(1);
	GetInputPort(INPUTPORT_VALUE).SetupAsChannels<double>(uniqueOscAddress.AsChar(), "x", INPUTPORT_VALUE);

	// init base class
	OutputNode::Init();

	// create the OSC address
	AttributeSettings* attributeOscAddress = RegisterAttribute("OSC Address", "oscAddress", "e.g. /out/1.", ATTRIBUTE_INTERFACETYPE_STRING);
	attributeOscAddress->SetDefaultValue( AttributeString::Create( uniqueOscAddress.AsChar() ) );
	
	// hide upload attribute
	GetAttributeSettings(ATTRIB_UPLOAD)->SetVisible(false);
}


void OscOutputNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	OutputNode::ReInit(elapsed, delta);

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


void OscOutputNode::Reset()
{
	OutputNode::Reset();
}


// update the node
void OscOutputNode::Update(const Time& elapsed, const Time& delta)
{
	// shared base update helper
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update OutputNode baseclass (calls resamplers)
	OutputNode::Update(elapsed, delta);

	UpdateResamplers(elapsed, delta);

	// queue a packet for each new sample
	Channel<double >* channel = GetOutputChannel(INPUTPORT_VALUE);
	const uint32 numNewSamples = channel->GetNumNewSamples();
	for (uint32 i = 0; i < numNewSamples; ++i)
	{
		// get free packet from server
		OscPacket* packet = GetOscMessageRouter()->GetOscPacketFromPool();
		packet->Clear();

		// write message into packet
		WriteOscMessage(packet);

		// push into queue
		GetOscMessageRouter()->QueueOutputPacket(packet);
	}
}


void OscOutputNode::SetName(const char* name)
{
	OutputNode::SetName(name);
	mChannels[0]->SetName(name);
}


// attributes have changed
void OscOutputNode::OnAttributesChanged()
{
	OutputNode::OnAttributesChanged();

	// update input port name when the OSC adr changes
	InputPort& inputPort = GetInputPort(INPUTPORT_VALUE);
	if (inputPort.GetNameString().IsEqual(GetOscAddress()) == false)
	{
		inputPort.SetName(GetOscAddress());
		ResetAsync();
	}
}


// write the OSC message
void OscOutputNode::WriteOscMessage(OscPacketParser::OutStream* outStream)
{
	// only send out the OSC message in case we have samples inside the channel already
	if (IsValidInput(INPUTPORT_VALUE) == false)
		return;

	outStream->BeginMessage( GetOscAddress() );

		// output a float value and not the double (more OSC conform)
		const float value = GetCurrentValue();
		outStream->WriteValue( value );

	outStream->EndMessage();
}


// generate a unique OSC address
Core::String OscOutputNode::GenerateUniqueOscAddress()
{
	String result;
	for (uint32 i=0; i<CORE_INT16_MAX; ++i)
	{
		// set the OSC address for the current iteration
		result.Format("/out/%i", i);

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


bool OscOutputNode::IsOscAddressUnique(const char* address)
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

		if (node->GetType() == OscOutputNode::TYPE_ID)
		{
			OscOutputNode* oscNode = static_cast<OscOutputNode*>(node);
			const char* oscAddr = oscNode->GetOscAddress();

			// is address not unique?
			if (strcmp(oscAddr, address) == 0)
				return false;
		}
	}

	// no other address found, it is unique
	return true;
}
