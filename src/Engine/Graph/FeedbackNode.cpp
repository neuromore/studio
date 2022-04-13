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
#include "FeedbackNode.h"
#include "Graph.h"


using namespace Core;

// constructor
FeedbackNode::FeedbackNode(Graph* parentGraph) : OutputNode(parentGraph)
{
}


// destructor
FeedbackNode::~FeedbackNode()
{
}


// initialize the node
void FeedbackNode::Init()
{
	OutputNode::Init();

	// create output signal quality
	const bool defaultSendOSCFlag = false;
	AttributeSettings* attributeSendOsc = RegisterAttribute("Send OSC Msgs", "sendOscMessages", "Send the values via OSC to all connected network clients.", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeSendOsc->SetDefaultValue(AttributeBool::Create(defaultSendOSCFlag));

	// create the OSC address
	AttributeSettings* attributeOscAddress = RegisterAttribute("OSC Address", "oscAddress", "OSC address, e.g. /feedback/1.", ATTRIBUTE_INTERFACETYPE_STRING);
	attributeOscAddress->SetDefaultValue(AttributeString::Create("/invalid/address"));
}


void FeedbackNode::OnAttributesChanged()
{
	OutputNode::OnAttributesChanged();
}

