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
#include "LoretaNode.h"
#include "../Core/Math.h"
#include "../EngineManager.h"
#include "../DSP/Spectrum.h"


using namespace Core;

// constructor
LoretaNode::LoretaNode(Graph* graph) : SPNode(graph)
{
}


// destructor
LoretaNode::~LoretaNode()
{
}


// initialize the node
void LoretaNode::Init()
{

	// SETUP PORTS
	
	// setup the input ports
	InitInputPorts(1);
	GetInputPort(INPUTPORT).Setup("EEG", "x", AttributeChannels<double>::TYPE_ID, INPUTPORT);

	// ATTRIBUTES

	//Core::AttributeSettings* functionParam = RegisterAttribute("Math Function", "mathFunction", "The math function to use.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	//functionParam->ResizeComboValues( (uint32)MATHFUNCTION_NUMFUNCTIONS );
	//for (uint32 i=0; i<MATHFUNCTION_NUMFUNCTIONS; ++i)
	//	functionParam->SetComboValue( i, GetFunctionString((EMathFunction)i) );
	//functionParam->SetDefaultValue( Core::AttributeInt32::Create(mSettings.mMathFunction) );
}

// reset everything
void LoretaNode::Reset()
{ 
	SPNode::Reset();

	mChannels.Clear();
	mElectrodes.Clear();
}

void LoretaNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	SPNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


void LoretaNode::Start(const Time& elapsed)
{
	SPNode::Start(elapsed);

	CollectElectrodeChannels();
}


// update the node
void LoretaNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update the baseclass
	SPNode::Update(elapsed, delta);
}



// update the data
void LoretaNode::OnAttributesChanged()
{
	// nothing yet
}


// map input channels to EEG electrodes by name and store them in the arrays
void LoretaNode::CollectElectrodeChannels()
{
	mChannels.Clear();
	mElectrodes.Clear();

	// 0) handle special cases
	Port& port = GetInputPort(INPUTPORT);
	if (port.HasConnection() == false)
		return;

	MultiChannel* channels = port.GetChannels();
	if (channels == NULL)
		return;

	bool electrodeNotFound = false;

	// go through all channels and check if they are named like our electrodes
	const uint32 numChannels = channels->GetNumChannels();
	for (uint32 i = 0; i < numChannels; ++i)
	{
		ChannelBase* channel = channels->GetChannel(i);

		// try to find electrode by channel name
		const String& name = channel->GetNameString();
		if (GetEngine()->GetEEGElectrodes()->IsValidElectrodeID(name))
		{
			EEGElectrodes::Electrode electrode = GetEngine()->GetEEGElectrodes()->GetElectrodeByID(name);

			// add channel and the electrode to our list
			mChannels.AddChannel(channel);
			mElectrodes.Add(electrode);
		}
		else
		{
			electrodeNotFound = true;
		}
	}

	// show node error message if there were some unknown IDs
	if (electrodeNotFound == true)
		SetError(ERROR_ELECTRODE_NAMES, "Input channels are not named like electrodes (case-sensitive).");
	else
		ClearError(ERROR_ELECTRODE_NAMES);
}


Core::String& LoretaNode::GetDebugString(Core::String& inout)
{
	SPNode::GetDebugString(inout);

	const uint32 numChannels = GetNumChannels();
	mTempString.Format("Num Channels: %i\n", numChannels);
	inout += mTempString;
		
	return inout;
}
