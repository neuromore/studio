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
#include "MultiParameterNode.h"
#include "../Core/Math.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
MultiParameterNode::MultiParameterNode(Graph* graph) : InputNode(graph)
{
 	mSampleRate = 128;
}


// destructor
MultiParameterNode::~MultiParameterNode()
{
}


// initialize the node
void MultiParameterNode::Init()
{
	// color the output channel same as the node
	UseChannelColoring();

	// PORTS

	// register ports
	InitOutputPorts(1);
	GetOutputPort(OUTPUTPORT_VALUE).Setup("Out", "y", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_VALUE);

	// ATTRIBUTES

	// baseclass attributes
	InputNode::Init();

	// sample rate
	Core::AttributeSettings* attributeSettings = RegisterAttribute("Sample Rate", "sampleRate", "Sample rate of the output channel.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeSettings->SetDefaultValue( Core::AttributeFloat::Create(mSampleRate) );
	attributeSettings->SetMinValue( Core::AttributeFloat::Create(0) );
	attributeSettings->SetMaxValue( Core::AttributeFloat::Create(DBL_MAX) );

	Core::AttributeSettings* functionParam = RegisterAttribute("Values", "ValueString", "", ATTRIBUTE_INTERFACETYPE_STRING);
        functionParam->SetDefaultValue( Core::AttributeString::Create("") );
}


void MultiParameterNode::Reset()
{
	GetOutputPort(OUTPUTPORT_VALUE).GetChannels()->Clear();

	InputNode::Reset();

	mClock.Stop();
	mClock.Reset();
}


void MultiParameterNode::ReInit(const Time& elapsed, const Time& delta)
{
	InputNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


void MultiParameterNode::Start(const Time& elapsed)
{
	CreateSensors();

	// call baseclass start
	InputNode::Start(elapsed);
	
	// configure clock and start it at current elapsed time
	mClock.Reset();
	mClock.SetFrequency(mSampleRate);
	mClock.SetStartTime(elapsed);
	mClock.Start();
}


// update the node
void MultiParameterNode::Update(const Time& elapsed, const Time& delta)
{	
	// shared base update helper
	if (BaseUpdate(elapsed, delta) == false)
		return;

	mClock.Update(elapsed, delta);

	InputNode::Update(elapsed, delta);
}


// update the data
void MultiParameterNode::OnAttributesChanged()
{
	// check if default value was changed
	const char* valueString = GetStringAttribute(ATTRIB_VALUES);
	const double sampleRate = GetFloatAttribute(ATTRIB_SAMPLERATE);
        
	Array<String> values = String(valueString).Split(StringCharacter::comma);
        const uint32 numValues = values.Size();

	bool valueChanged = false;	
	if (numValues == mValues.Size())
	{
		for (uint32 i=0; i<numValues; ++i)
		{
			values[i].Trim();
			const double value = values[i].ToFloat();
			if (value != mValues[i])
				valueChanged = true;
		}
	}
	else
	{
		valueChanged = true;
	}

	if (mSampleRate != sampleRate)
	{
		valueChanged = true;
		mSampleRate = sampleRate;
	}

	if (valueChanged == true ||  mSampleRate != sampleRate)
	{
		mSampleRate = sampleRate;
		ResetAsync();
	}

}


// the function that fills the sensors with samples
void MultiParameterNode::GenerateSamples()
{
	const uint32 numNewSamples = mClock.GetNumNewTicks();			// number of samples in this interval
	mClock.ClearNewTicks();

	// add the samples
	const uint32 numSensors = mSensors.Size();
	for (uint32 i = 0; i<numNewSamples; ++i)
		for (uint32 j = 0; j<numSensors; ++j)
			mSensors[j].AddQueuedSample(mValues[j]);

//	// if clock is not running (samplerate = 0), output a sample only if the parameter was changed
//	ChannelBase* channel = mSensor.GetChannel();
//	if (channel->GetSampleRate() == 0)
//	{
//		if (channel->GetNumSamples() == 0)
//			mSensor.AddQueuedSample(currentValue);
//		else if (channel->AsType<double>()->GetLastSample() != currentValue)
//			mSensor.AddQueuedSample(currentValue);
//	}
}


void MultiParameterNode::CreateSensors()
{
    // we accept comma separated list for multichannels
    Array<String> values = String(GetStringAttribute(ATTRIB_VALUES)).Split(StringCharacter::comma);
    const uint32 numValues = values.Size();

	mValues.Resize(numValues);
	mSensors.Resize(numValues);
	GetOutputPort(OUTPUTPORT_VALUE).GetChannels()->Clear();

	for (uint32 i=0; i<numValues; ++i)
	{
		values[i].Trim();
		const double value = values[i].ToFloat();
		mValues[i] = value;
		
		mSensors[i].Reset();
		mSensors[i].GetChannel()->SetBufferSize(10);
		mSensors[i].SetDriftCorrectionEnabled(false);
		mSensors[i].GetChannel()->SetSampleRate(mSampleRate);
		mTempString.Format("%.2f", value);
		mSensors[i].GetChannel()->SetName(mTempString.AsChar());
		GetOutputPort(OUTPUTPORT_VALUE).GetChannels()->AddChannel(mSensors[i].GetChannel());
	}
}

