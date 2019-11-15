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
#include "AVEColorNode.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
AVEColorNode::AVEColorNode(Graph* graph) : SPNode(graph)
{
}


// destructor
AVEColorNode::~AVEColorNode()
{
}


// initialize the node
void AVEColorNode::Init()
{
	// PORTS

	// register ports
	InitInputPorts(3);
	GetInputPort(INPUTPORT_RED).Setup("Red", "x1", AttributeChannels<double>::TYPE_ID, INPUTPORT_RED);
	GetInputPort(INPUTPORT_GREEN).Setup("Green", "x2", AttributeChannels<double>::TYPE_ID, INPUTPORT_GREEN);
	GetInputPort(INPUTPORT_BLUE).Setup("Blue", "x3", AttributeChannels<double>::TYPE_ID, INPUTPORT_BLUE);

	// baseclass attributes
	SPNode::Init();

	Core::AttributeSettings* rangeFunctionParam = RegisterAttribute("Output Type", "OutputType", "Type of color output (determines number of input channels and what they are used for)", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	rangeFunctionParam->ResizeComboValues((uint32)NUM_OUTPUTTYPES);
	rangeFunctionParam->SetComboValue(OUTPUTTYPE_EXPERIENCE_BACKGROUND, "Experience Background Color");
	rangeFunctionParam->SetComboValue(OUTPUTTYPE_EXPERIENCE_AVE4, "AVE Experience 4 Zones");
	rangeFunctionParam->SetDefaultValue(Core::AttributeInt32::Create(OUTPUTTYPE_EXPERIENCE_BACKGROUND));

}


void AVEColorNode::Reset()
{
	SPNode::Reset();
}



void AVEColorNode::ReInit(const Time& elapsed, const Time& delta)
{
	// shared base reinit helper
	if (BaseReInit(elapsed, delta) == false)
		return;

	SPNode::ReInit(elapsed, delta);

	const uint32 numColorChannels = GetNumColorChannels();

	// check input connection sizes
	for (uint32 i = 0; i < 3; ++i)
	{
		if (GetInputPort(i).HasConnection() == false || GetInputPort(i).GetChannels()->GetNumChannels() != numColorChannels)
		{
			mTempString.Format("Must have multichannel of size %i on all ports", numColorChannels);
			SetError(ERROR_WRONG_INPUT, mTempString.AsChar());
			mIsInitialized = false;
			break;
		}
	}

	// make sure node color array has right size
	mColors.Resize(numColorChannels);
	mRed.Resize(numColorChannels);
	mGreen.Resize(numColorChannels);
	mBlue.Resize(numColorChannels);
	mAlpha.Resize(numColorChannels);
	
	PostReInit(elapsed, delta);
}


void AVEColorNode::Start(const Time& elapsed)
{
	// call baseclass start
	SPNode::Start(elapsed);
}


// update the node
void AVEColorNode::Update(const Time& elapsed, const Time& delta)
{	
	// shared base update helper
	if (BaseUpdate(elapsed, delta) == false)
		return;

	SPNode::Update(elapsed, delta);

	if (mIsInitialized == false)
		return;

	// no samples
	if (mInputReader.GetMinNumNewSamples() == 0)
		return;

	const uint32 numColorChannels = GetNumColorChannels();
	for (uint32 i = 0; i < numColorChannels; ++i)
	{
		ChannelBase* redChannel = GetInputPort(INPUTPORT_RED).GetChannels()->GetChannel(i);
		ChannelBase* greenChannel = GetInputPort(INPUTPORT_GREEN).GetChannels()->GetChannel(i);
		ChannelBase* blueChannel = GetInputPort(INPUTPORT_BLUE).GetChannels()->GetChannel(i);

		CORE_ASSERT(redChannel != NULL && greenChannel != NULL && blueChannel != NULL);

		// get current color
		const double red = Clamp<double>( redChannel->AsType<double>()->GetLastSample(), 0.0, 1.0 );
		const double green = Clamp<double>( greenChannel->AsType<double>()->GetLastSample(), 0.0, 1.0 );
		const double blue = Clamp<double>( blueChannel->AsType<double>()->GetLastSample(), 0.0, 1.0 );

		mColors[i].r = red;
		mColors[i].g = green;
		mColors[i].b = blue;

		mRed[i]	  = red;
		mGreen[i] = green;
		mBlue[i] = blue;
		mAlpha[i] = 1.0;
	}

	// emit experience events and do other things if required for the set output type
	const EOutputType type = GetOutputType();
	switch (type)
	{
		case OUTPUTTYPE_EXPERIENCE_BACKGROUND:
        {
			CORE_EVENTMANAGER.OnSetBackgroundColor(mColors[0]);
			break;
        }
            
		case OUTPUTTYPE_EXPERIENCE_AVE4:
        {
			CORE_EVENTMANAGER.OnSetFourZoneAVEColors(mRed.GetPtr(), mGreen.GetPtr(), mBlue.GetPtr(), mAlpha.GetPtr());
			break;
        }
            
        default: break;
	}
	// emit all the color events
	// TODO add more events
}


// update the data
void AVEColorNode::OnAttributesChanged()
{
}

uint32 AVEColorNode::GetNumColorChannels() const
{
	EOutputType type = GetOutputType();
	switch (type)
	{
		case OUTPUTTYPE_EXPERIENCE_BACKGROUND:	return 1;
		case OUTPUTTYPE_EXPERIENCE_AVE4:		return 4;
		default: return 0;
	}
}
