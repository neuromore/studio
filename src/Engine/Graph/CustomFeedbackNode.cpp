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
#include "CustomFeedbackNode.h"
#include "Classifier.h"
#include "../EngineManager.h"
#include "Graph.h"


using namespace Core;

// constructor
CustomFeedbackNode::CustomFeedbackNode(Graph* parentGraph) : FeedbackNode(parentGraph)
{
}


// destructor
CustomFeedbackNode::~CustomFeedbackNode()
{
}


// initialize
void CustomFeedbackNode::Init()
{
	// TODO multi-channel support
	// setup channels
	AddOutput();
	Channel<double>* output = GetOutputChannel(0);
	output->SetUnit("Score");
	output->SetMinValue(0.0);
	output->SetMaxValue(1.0);

	// setup the input ports
	InitInputPorts(1);
	GetInputPort(INPUTPORT_VALUE).SetupAsChannels<double>("In", "x", INPUTPORT_VALUE);

	// init base class
	FeedbackNode::Init();

	// create user id attribute
	AttributeSettings* userID = RegisterAttribute("User ID", "id", "In case multiple people are connected simultaneously, use this ID to identify the user.", ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	userID->SetDefaultValue( AttributeInt32::Create(0) );
	userID->SetMinValue( AttributeInt32::Create(0) );
	userID->SetMaxValue( AttributeInt32::Create(CORE_INT32_MAX) );

	// is ranged attribute
	AttributeSettings* attributeIsRanged = RegisterAttribute("Is Ranged", "isRanged", "Set to true in case the value range of the feedback is known upfront. Input value can be any value in case of false.", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeIsRanged->SetDefaultValue( AttributeBool::Create(true) );

	// range min attribute
	AttributeSettings* attributeRangeMin = RegisterAttribute("Range Min", "rangeMin", "Minimum possible feedback value. Values smaller than this will be clamped", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeRangeMin->SetDefaultValue( AttributeFloat::Create(0.0) );
	attributeRangeMin->SetMinValue( AttributeFloat::Create(-FLT_MAX) );
	attributeRangeMin->SetMaxValue( AttributeFloat::Create(+FLT_MAX) );

	// range max attribute
	AttributeSettings* attributeRangeMax = RegisterAttribute("Range Max", "rangeMax", "Maximum possible feedback value. Values bigger than this will be clamped", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeRangeMax->SetDefaultValue( AttributeFloat::Create(1.0) );
	attributeRangeMax->SetMinValue( AttributeFloat::Create(-FLT_MAX) );
	attributeRangeMax->SetMaxValue( AttributeFloat::Create(+FLT_MAX) );

	// set new default OSC address (do not overwrite! its already set!)
	GetAttributeSettings(ATTRIB_OSCADDRESS)->GetDefaultValue()->InitFromString( GenerateUniqueOscAddress().AsChar() );
}


void CustomFeedbackNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	FeedbackNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


void CustomFeedbackNode::Reset()
{
	FeedbackNode::Reset();
}


// update the node
void CustomFeedbackNode::Update(const Time& elapsed, const Time& delta)
{
	// shared base update helper
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update OutputNode baseclass (calls resamplers)
	FeedbackNode::Update(elapsed, delta);

	UpdateResamplers(elapsed, delta);

	// TODO move this into ReInit!
	//////////////////////////////////////////////////////
	if (GetSendOscNetworkMessages() == true && IsOscAddressUnique(GetOscAddress()) == false)
	{
		SetError(ERROR_DUPLICATE_OSC_ADDRESS, "OSC address is not unique");
		return;
	}
	else
	{
		ClearError(ERROR_DUPLICATE_OSC_ADDRESS);
	}

	//////////////////////////////////////////////////////
	// check signal value range
	
	// feedbackscorenode attributes
	const bool	 isRanged = GetBoolAttribute(ATTRIB_ISRANGED);
	const double rangeMin = GetFloatAttribute(ATTRIB_RANGEMIN);
	const double rangeMax = GetFloatAttribute(ATTRIB_RANGEMAX);

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
		
		// check and clamp to range, if required
		if (isRanged == true)
		{
			// set error in case the the value is not in range
			if (value > rangeMax || value < rangeMin)
			{
				outOfRange = true;
				break;
			}
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

	if (GetNameString() == "Volume"
		|| GetNameString() == "MasterVolume") {
			String warningMsg;
			warningMsg.Format("Using custom feedback with name %s is deprecated, use the appropriate node", GetName());
			SetWarning(WARNING_DEPRECATED_NODE_NAME, warningMsg.AsChar());
	} else {
		ClearWarning(WARNING_DEPRECATED_NODE_NAME);
	}
}

// attributes have changed
void CustomFeedbackNode::OnAttributesChanged()
{
	FeedbackNode::OnAttributesChanged();

	// hide/show osc address attribute
	const bool showOscAddress = GetBoolAttribute(ATTRIB_SENDOSCNETWORKMESSAGES);
	GetAttributeSettings(ATTRIB_OSCADDRESS)->SetVisible(showOscAddress);

	// forward the min and max values to the channels
	const uint32 numChannels = mChannels.Size();
	for (uint32 i=0; i<numChannels; ++i)
	{
		Channel<double>* channel = mChannels[i];
		channel->SetMinValue( GetRangeMin() );
		channel->SetMaxValue( GetRangeMax() );
	}

	// hide/show range attributes depending on isRanged checkbox
	const bool			isRanged					= GetBoolAttribute(ATTRIB_ISRANGED);
	AttributeSettings*	rangeMinAttributeSettings	= FindAttributeSettingsByInternalName("rangeMin");
	AttributeSettings*	rangeMaxAttributeSettings	= FindAttributeSettingsByInternalName("rangeMax");
	rangeMinAttributeSettings->SetVisible( isRanged );
	rangeMaxAttributeSettings->SetVisible( isRanged );

	// fire event
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_RANGEMIN)) );
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_RANGEMIN)) );
}


// use the n-score color for the graph node color
Color CustomFeedbackNode::GetColor() const
{
	if (mParentGraph->GetType() == Classifier::TYPE_ID)
	{
		Classifier* classifier = static_cast<Classifier*>(mParentGraph);

		// try to find the node construction order index so that the color sticks along with the node rather than changing when moving it
		uint32 customFeedbackNodeIndex = 0;
		const uint32 numNodes = classifier->GetNumNodes();
		for (uint32 i=0; i<numNodes; ++i)
		{
			Node* node = classifier->GetNode(i);

			if (node == this)
				break;

			if (node->GetType() == TYPE_ID)
				customFeedbackNodeIndex++;
		}

		return GetColor(customFeedbackNodeIndex);
	}

	return Color(0.0,0.0,0.0);
}


// return the color for the score value
Color CustomFeedbackNode::GetColor(uint32 index)
{
	switch (index)
	{
		// NOTE: remember to change the color in the Unity integration along with chaning one of these
		case 0:		return RGBA( 0, 159, 227 );
		case 1:		return RGBA( 191, 234, 33 );
		case 2:		return RGBA( 112, 52, 255 );
		case 3:		return RGBA( 12, 92, 232 );
		case 4:		return RGBA( 110, 255, 13 );
		case 5:		return RGBA( 255, 131, 13 );
		case 6:		return RGBA( 175, 186, 232 );
		case 7:		return RGBA( 255, 250, 26 );
		case 8:		return RGBA( 232, 12, 208 );
		
		default:
		{
			Color uniqueColor;
			uniqueColor.SetUniqueColor(index);
			return uniqueColor;
		}
	}
}


// write the OSC message
void CustomFeedbackNode::WriteOscMessage(OscPacketParser::OutStream* outStream)
{
	// only send out the OSC message in case we have samples inside the channel already
	if (IsValidInput(INPUTPORT_VALUE) == false)
		return;

	outStream->BeginMessage( GetOscAddress() );

		// 1. feedback value
		// output a float value and not the double (more OSC conform)
		const float feedbackValue = GetCurrentValue();
		outStream->WriteValue(feedbackValue);

		// 2. is ranged?
		const bool isRanged = IsRanged();
		outStream->WriteValue(isRanged);

		// 3. range min
		const float rangeMin = GetRangeMin();
		outStream->WriteValue(rangeMin);

		// 4. range min
		const float rangeMax = GetRangeMax();
		outStream->WriteValue(rangeMax);

	outStream->EndMessage();
}


// generate unique OSC address
Core::String CustomFeedbackNode::GenerateUniqueOscAddress()
{
	// generate OSC addresses
	String result;
	for (uint32 i=0; i<CORE_INT16_MAX; i++)
	{
		// set the OSC address for the current iteration
		result.Format("/feedback/%i", i);

		// found a unique address
		if (IsOscAddressUnique(result.AsChar()) == true)
			return result;
	}

	// if we arrive here, no valid osc address was found :(
	LogError("Could not create a unique OSC address!");
	CORE_ASSERT(false);
	result = "/could/not/find/a/unique/osc/address/";
	return result;
}


bool CustomFeedbackNode::IsOscAddressUnique(const char* address)
{
	// make sure the feedback node is part of a valid classifier
	if (mParentGraph->GetType() != Classifier::TYPE_ID)
	{
		LogError("Cannot create unique feedback OSC address. Feedback node is not part of a valid classifier.");
		CORE_ASSERT(false);
		return "";
	}

	// down-cast graph to a classifier
	Classifier* classifier = static_cast<Classifier*>(mParentGraph);

	// get the number of custom feedback nodes in the classifier and iterate through them
	const uint32 numCustomFeedbackNodes = classifier->GetNumCustomFeedbackNodes();
	for (uint32 i = 0; i<numCustomFeedbackNodes; ++i)
	{
		// get the feedback node and check if it is another node than the current one
		CustomFeedbackNode* feedbackNode = classifier->GetCustomFeedbackNode(i);
		if (feedbackNode == this)
			continue;

		// make sure the node's OSC message is active
		if (feedbackNode->GetSendOscNetworkMessages() == false)
			continue;

		// check the score index of the other score node, in case they are both equal they are not unique anymore
		// TODO: maybe use something from our String class here?
		if (strcmp(address, feedbackNode->GetOscAddress()) == 0)
			return false;
	}

	return true;
}
