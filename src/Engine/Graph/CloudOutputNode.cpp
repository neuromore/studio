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
#include "CloudOutputNode.h"
#include "../CloudParameters.h"
#include "../Core/Math.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
CloudOutputNode::CloudOutputNode(Graph* graph) : SPNode(graph)
{
}


// destructor
CloudOutputNode::~CloudOutputNode()
{
}


// initialize the node
void CloudOutputNode::Init()
{
	// PORTS

	// register ports
	InitInputPorts(1);
	GetInputPort(INPUTPORT_VALUE).Setup("In", "x", AttributeChannels<double>::TYPE_ID, INPUTPORT_VALUE);

	// ATTRIBUTES

	// storage type
	Core::AttributeSettings* attribStorageType = RegisterAttribute("Type", "StorageType", "Parameter type.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attribStorageType->ResizeComboValues(2);
	attribStorageType->SetComboValue(0, "User Parameter");
	attribStorageType->SetComboValue(1, "Classifier Parameter");
	attribStorageType->SetDefaultValue(Core::AttributeInt32::Create(0));

	// save mode type
	Core::AttributeSettings* attribStorageMode = RegisterAttribute("Save Mode", "StorageMode", "Keep parmeter history or overwrite the parameter.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attribStorageMode->ResizeComboValues(2);
	attribStorageMode->SetComboValue(0, "Overwrite Value");
	attribStorageMode->SetComboValue(1, "Save History");
	attribStorageMode->SetDefaultValue(Core::AttributeInt32::Create(0));

	// baseclass attributes
	SPNode::Init();
}


void CloudOutputNode::Reset()
{
	SPNode::Reset();
}



void CloudOutputNode::ReInit(const Time& elapsed, const Time& delta)
{
	// shared base reinit helper
	if (BaseReInit(elapsed, delta) == false)
		return;

	SPNode::ReInit(elapsed, delta);

	if (mInputChannels.GetNumChannels() > 1)
	{
		SetError(ERROR_WRONG_INPUT, "Input has too many channels. Node requires a single channel.");
		mIsInitialized = false;
	}

	PostReInit(elapsed, delta);
}


void CloudOutputNode::Start(const Time& elapsed)
{
	// call baseclass start
	SPNode::Start(elapsed);
}


// update the node
void CloudOutputNode::Update(const Time& elapsed, const Time& delta)
{	
	// shared base update helper
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// show error if node name is not unique
	if (IsParameterNameUnique(GetName()) == false)
    {
        SetError(ERROR_DUPLICATE_NAME, "Name is not unique.");
    }
    else
		ClearError(ERROR_DUPLICATE_NAME);

	SPNode::Update(elapsed, delta);
}


// update the data
void CloudOutputNode::OnAttributesChanged()
{
}


bool CloudOutputNode::HasOutputValue()
{
	if (IsInitialized() == false)
		return false;

	if (mInputChannels.GetNumChannels() == 0)
		return false;

	// use only first channel
	ChannelBase* channel = mInputChannels.GetChannel(0);

	// no sampels in channel :(
	if (channel->GetNumSamples() == 0)
		return false;

	return true;
}


double CloudOutputNode::GetOutputValue()
{
	if (HasOutputValue() == false)
		return 0.0;

	// use only first channel
	Channel<double>* channel = mInputChannels.GetChannel(0)->AsType<double>();

	// return the last sample of input channel
	return channel->GetLastSample();
}


bool CloudOutputNode::IsParameterNameUnique(const char* name)
{
	// make sure the feedback node is part of a valid classifier
	if (mParentGraph->GetType() != Classifier::TYPE_ID)
	{
		LogError("Cannot check parameter name for uniqueness, parent is not a classifier");
		CORE_ASSERT(false);
		return "";
	}

	// down-cast graph to a classifier
	Classifier* classifier = static_cast<Classifier*>(mParentGraph);

	// get the number of custom feedback nodes in the classifier and iterate through them
	const uint32 numCloudNodes = classifier->GetNumCloudOutputNodes();
	for (uint32 i = 0; i<numCloudNodes; ++i)
	{
		// get the feedback node and check if it is another node than the current one
		CloudOutputNode* node = classifier->GetCloudOutputNode(i);
		if (node == this)
			continue;

		// match agains this node's name (= parameter name)
		if (node->GetNameString().Compare(name) == 0)
			return false;
	}

	return true;
}


uint32 CloudOutputNode::SaveCloudParameters(CloudParameters& parameters)
{
	// no value -> nothing to save
	if (HasOutputValue() == false)
		return 0;

	// save with item id
	const char* itemId = NULL;
	if (GetStorageType() == STORAGETYPE_CLASSIFIERPARAMETER)
		itemId = mParentGraph->GetUuid();

	// save with timestamp
	Core::Time timeStamp = 0;
	if (GetStorageMode() == CloudOutputNode::STORAGEMODE_SAVEHISTORY)
		timeStamp = GetSession()->GetStopTime();							// HACK HACK HACK ?? cloud output node accessing session for the stop time!

	parameters.Add<double>(GetName(), GetOutputValue(), timeStamp, itemId);

	return 1;
}
