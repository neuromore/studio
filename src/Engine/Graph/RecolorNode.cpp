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
#include "RecolorNode.h"
#include "../Core/AttributeSettings.h"
#include "../EngineManager.h"
#include "Graph.h"


using namespace Core;

// constructor
RecolorNode::RecolorNode(Graph* graph) : SPNode(graph)
{	
	mMainColor = Color(0, 159.0f/255.0f, 227.0f/255.0f);
	mLoadedAttributes = false;
}


// destructor
RecolorNode::~RecolorNode()
{
	// remove all output channels
	const uint32 numChannels = mChannels.Size();
	for (uint32 i = 0; i < numChannels; ++i)
		delete mChannels[i];
	mChannels.Clear();
}


// initialize the node
void RecolorNode::Init()
{
	// init base class first
	SPNode::Init();

	// doesn't hurt to check all channels in multichannel for correct sample rate (not sure if this can happen)
	RequireMatchingSampleRates();	
	RequireInputConnection();

	// PORTS
	InitInputPorts(1);
	GetInputPort(INPUTPORT).SetupAsChannels<double>("In", "x", INPUTPORT);
	InitOutputPorts(1);
	GetOutputPort(OUTPUTPORT).SetupAsChannels<double>("Out", "y", OUTPUTPORT);

	// ATTRIBUTES

	// Channel Name
	Core::AttributeSettings* colorList = RegisterAttribute("Channel Colors", "channelColors", "The list of colors that should be applied to the input channels.", ATTRIBUTE_INTERFACETYPE_STRINGARRAY);
	colorList->SetDefaultValue( Core::AttributeStringArray::Create("") );

	// color picker for adding colors to the list
	Core::AttributeSettings* colorPicker = RegisterAttribute("Add Color", "colorPick", "Use the colorpick to add a color", ATTRIBUTE_INTERFACETYPE_COLOR);
	colorPicker->SetDefaultValue(  Core::AttributeColor::Create(mMainColor) );
}


void RecolorNode::Reset()
{
	SPNode::Reset();

	// remove all output channels
	const uint32 numChannels = mChannels.Size();
	for (uint32 i = 0; i < numChannels; ++i)
		delete mChannels[i];
	mChannels.Clear();

	GetOutputPort(OUTPUTPORT).GetChannels()->Clear();
}


void RecolorNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	SPNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


// update the node
void RecolorNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update base class
	SPNode::Update(elapsed, delta);

	// do nothing if node is not fully initialized
	if (mIsInitialized == false)
		return;


	// simply forward all samples
	const uint32 numSamples = mInputReader.GetMinNumNewSamples();
	const uint32 numChannels = mInputReader.GetNumChannels();
	for (uint32 c = 0; c < numChannels; ++c)
	{
		for (uint32 i = 0; i < numSamples; ++i)
		{
			mChannels[c]->AddSample(mInputReader.GetReader(c)->PopOldestSample<double>());
		}
	}
}


void RecolorNode::Start(const Time& elapsed)
{
	CORE_ASSERT(mChannels.Size() == 0);
	CORE_ASSERT(GetOutputPort(OUTPUTPORT).GetChannels()->GetNumChannels() == 0);

	// create output channels and set them up
	MultiChannel* inChannels = GetInputPort(INPUTPORT).GetChannels();
	
	const uint32 numChannels = inChannels->GetNumChannels();
	for (uint32 i = 0; i < numChannels; ++i)
	{
		Channel<double>* outChannel = new Channel<double>();
		Channel<double>* inChannel = inChannels->GetChannel(i)->AsType<double>();
		
		// set any buffersize > 0
		outChannel->SetBufferSize(10);		

		// add and connect to outputport
		mChannels.Add(outChannel);
		GetOutputPort(OUTPUTPORT).GetChannels()->AddChannel(outChannel);

		// forward properties
		outChannel->SetName(inChannel->GetName());
		outChannel->SetSampleRate(inChannel->GetSampleRate());
		outChannel->SetMinValue(inChannel->GetMinValue());
		outChannel->SetMaxValue(inChannel->GetMaxValue());
		outChannel->SetUnit(inChannel->GetUnit());

		// set main color first
		outChannel->SetColor(mMainColor);
	}

	UpdateOutputChannelColors();

	// call baseclass start last
	SPNode::Start(elapsed);
}


// an attribute has changed
void RecolorNode::OnAttributesChanged()
{
	Color newMainColor = GetColorAttribute(ATTRIB_COLORPICKER);;

	// add color to the list if user picked a new one; we have to detect the call that happens while loading, we must not append the attribute then.
	if (mLoadedAttributes == true && mMainColor != newMainColor)
	{

		// add color to the attribute
		Array<String> colors = GetStringArrayAttribute(ATTRIB_CHANNELCOLORS, Array<String>());
		colors.Add( newMainColor.ToHexString() );
		SetStringArrayAttributeByIndex(ATTRIB_CHANNELCOLORS, colors);

		// emit event
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_CHANNELCOLORS)) );
	}
	else
	{
		mLoadedAttributes = true;
	}
	
	mMainColor = newMainColor;

	// apply new output names
	if (mChannels.Size() > 0)
		UpdateOutputChannelColors();

	ResetAsync();
}


// update output channel names from the attribute
void RecolorNode::UpdateOutputChannelColors()
{
	// nothing todo (also required so this can be called from OnAttributesChanged() during load
	MultiChannel* inChannels = GetInputPort(INPUTPORT).GetChannels();
	const uint32 numChannels = mChannels.Size();
	CORE_ASSERT(numChannels == inChannels->GetNumChannels());

	const Array<String>& colors = GetStringArrayAttribute(ATTRIB_CHANNELCOLORS, Array<String>());
	const uint32 numColors = colors.Size();

	// iterate through all output channels and determine + set its color
	for (uint32 i = 0; i < numChannels; ++i)
	{
		Channel<double>* outChannel = mChannels[i];
		Channel<double>* inChannel = inChannels->GetChannel(i)->AsType<double>();
	
		// is there a color for this channel?
		Color color = mMainColor;

		if (i < numColors)
		{
			// try to parse it, if it fails it will default to the inChannel Color
			color = inChannel->GetColor();
			color.FromHexString(colors[i]);
		}
		
		outChannel->SetColor(color);
	}
}
