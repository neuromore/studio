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
#include "RenameNode.h"
#include "../Core/AttributeSettings.h"
#include "Graph.h"


using namespace Core;

// constructor
RenameNode::RenameNode(Graph* graph) : SPNode(graph)
{	
	// Note: no channel coloring, we forward everything from the inputs
}


// destructor
RenameNode::~RenameNode()
{
	// remove all output channels
	const uint32 numChannels = mChannels.Size();
	for (uint32 i = 0; i < numChannels; ++i)
		delete mChannels[i];
	mChannels.Clear();
}


// initialize the node
void RenameNode::Init()
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
	Core::AttributeSettings* namesParam = RegisterAttribute("Channel Names", "channelNames", "The name of the channels. Separate with ',' to rename single channels within a multi channel.", ATTRIBUTE_INTERFACETYPE_STRING);
	namesParam->SetDefaultValue( Core::AttributeString::Create("") );

	// "prefix/postfix" option
	Core::AttributeSettings* modeParam = RegisterAttribute("Mode", "renameMode", "How to rename the channels.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	modeParam->ResizeComboValues( 3 );
	modeParam->SetComboValue(MODE_RENAME, "Rename" );
	modeParam->SetComboValue(MODE_PREFIX, "Prefix" );
	modeParam->SetComboValue(MODE_POSTFIX, "Postfix" );
	modeParam->SetDefaultValue( Core::AttributeInt32::Create(0) );
}


void RenameNode::Reset()
{
	SPNode::Reset();

	// remove all output channels
	const uint32 numChannels = mChannels.Size();
	for (uint32 i = 0; i < numChannels; ++i)
		delete mChannels[i];
	mChannels.Clear();

	GetOutputPort(OUTPUTPORT).GetChannels()->Clear();
}


void RenameNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	SPNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


// update the node
void RenameNode::Update(const Time& elapsed, const Time& delta)
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


void RenameNode::Start(const Time& elapsed)
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
		outChannel->SetSampleRate(inChannel->GetSampleRate());
		outChannel->SetMinValue(inChannel->GetMinValue());
		outChannel->SetMaxValue(inChannel->GetMaxValue());
		outChannel->SetUnit(inChannel->GetUnit());
		outChannel->SetColor(inChannel->GetColor());
	}

	UpdateOutputChannelNames();

	// call baseclass start last
	SPNode::Start(elapsed);
}


// an attribute has changed
void RenameNode::OnAttributesChanged()
{
	// apply new output names
	if (mChannels.Size() > 0)
		UpdateOutputChannelNames();

	ResetAsync();
}


// update output channel names from the attribute
void RenameNode::UpdateOutputChannelNames()
{
	String nameString = GetStringAttribute(ATTRIB_CHANNELNAMES);

	// we accept comma separated list for multichannels
	Array<String> names = nameString.Split(StringCharacter::comma);
	const uint32 numNamesGiven = names.Size();

	// don't trim in prefix/postfix mode, so we can add spaces
	if (GetInt32Attribute(ATTRIB_MODE) == MODE_RENAME)
	{
		for (uint32 i=0; i<numNamesGiven; ++i)
			names[i].Trim();
	}

	// check if only a single name was given, then we don't match the names 1:1 as a list, but rather to that single name
	const bool hasSingleName = names.Size() == 1 && names[0].ContainsVisibleCharacter() == true;

	// nothing todo (also required so this can be called from OnAttributesChanged() during load
	MultiChannel* inChannels = GetInputPort(INPUTPORT).GetChannels();
	CORE_ASSERT(mChannels.Size() == inChannels->GetNumChannels());

	const uint32 numChannels = mChannels.Size();
	for (uint32 i = 0; i < numChannels; ++i)
	{
		Channel<double>* outChannel = mChannels[i];
		Channel<double>* inChannel = inChannels->GetChannel(i)->AsType<double>();
		String inName = inChannel->GetName();
		
		// set to in-name first
		outChannel->SetName(inName);

		// act depending on the mode that is set 
		switch (GetInt32Attribute(ATTRIB_MODE))
		{
			// rename each channel; if there is only one name set, name all the channels the same
			case MODE_RENAME:
				// rename every channel the same
				if (hasSingleName)
				{
					outChannel->SetName(names[0]);
				}
				else if (i < numNamesGiven)
				{
					// rename channel if new name is not empty
					if (names[i].ContainsVisibleCharacter() == true)
						outChannel->SetName(names[i]);
				}

				break;

			// prepend name
			case MODE_PREFIX:
				if (hasSingleName)
					outChannel->SetName(names[0] + inName);
				else if (i < numNamesGiven)
					outChannel->SetName(names[i] + inName);

				break;

			// append name
			case MODE_POSTFIX:

				if (hasSingleName)
					outChannel->SetName(inName + names[0]);
				else if (i < numNamesGiven)
					outChannel->SetName(inName + names[i]);

				break;
		}


	}
}
