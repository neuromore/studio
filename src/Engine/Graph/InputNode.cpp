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
#include "InputNode.h"
#include "Graph.h"

using namespace Core;

// constructor
InputNode::InputNode(Graph* graph) : SPNode(graph)
{
}


// destructor
InputNode::~InputNode()
{
}


void InputNode::Reset()
{
	SPNode::Reset();

	// reset sensors
	const uint32 numSensors = GetNumSensors();
	for (uint32 i=0; i<numSensors; ++i)
		GetSensor(i)->Reset();
}


void InputNode::Init()
{
	// upload checkbox
	AttributeSettings* attributeUpload = RegisterAttribute("Upload", "upload", "Upload the data stream to neuromore Cloud after a successful session.", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeUpload->SetDefaultValue(AttributeBool::Create(false));
	
	// hide it by default
	attributeUpload->SetVisible(false);
}


void InputNode::ReInit(const Time& elapsed, const Time& delta)
{
	SPNode::ReInit(elapsed, delta);

	// stop node if nothing is connected
	if (mParentGraph->CalcNumOutputConnections(this) == 0)
		mIsInitialized = false;
}


void InputNode::Update(const Time& elapsed, const Time& delta)
{
	SPNode::Update(elapsed, delta);

	GenerateSamples();

	// feed queued samples into the channels
	const uint32 numSensors = GetNumSensors();
	for (uint32 i=0; i<numSensors; ++i)
		GetSensor(i)->Update(elapsed, delta);
}


// synchronize the the input channels (sensors) so the next sample that is added falls on this time
void InputNode::Sync(double syncTime)
{
	// sync all sensors
	//const uint32 numSensors = GetNumSensors();
	//for (uint32 i=0; i<numSensors; ++i)
	//{
	//	GetSensor(i)->Sync(syncTime);
	//}
}


// override Start() and align output channels to absolute time, not relative to input
void InputNode::Start(const Time& elapsed)
{
	// NOTE do not call SPNode::Start() here, because we override the output alignment code

	const uint32 numSensors = GetNumSensors();
	for (uint32 i = 0; i < numSensors; ++i)
	{
		ChannelBase* channel = GetSensor(i)->GetChannel();
		channel->SetStartTime(elapsed);
	}
}


double InputNode::FindMaximumInputLatency()
{
	double maxLatency = 0.0;

	const uint32 numSensors = GetNumSensors();
	for (uint32 i=0; i<numSensors; ++i)
		maxLatency = Max(maxLatency, GetSensor(i)->GetLatency());

	return maxLatency;
}


uint32 InputNode::CalculateInputMemoryUsed()
{
	uint32 numBytes = 0;

	const uint32 numSensors = GetNumSensors();
	for (uint32 i=0; i<numSensors; ++i)
	{
		Channel<double>* channel = GetSensor(i)->GetChannel();
		if (channel->IsBuffer() == false)
			numBytes += channel->CalculateMemoryUsed();
	}

	return numBytes;
}
