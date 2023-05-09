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
#include "SessionTimeNode.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
SessionTimeNode::SessionTimeNode(Graph* graph) : InputNode(graph)
{
	// configure the sensor
	mSensor.GetChannel()->SetSampleRate(128);
	mSensor.GetChannel()->SetBufferSize(10);
	mSensor.SetDriftCorrectionEnabled(false);

	// color the output channel same as the node
	UseChannelColoring();

	// configure and start clockgenerator
	mClock.SetFrequency(128);
	mClock.Start();
}


// destructor
SessionTimeNode::~SessionTimeNode()
{
}


// initialize the node
void SessionTimeNode::Init()
{
	// PORTS

	// register ports
	InitOutputPorts(1);
	GetOutputPort(OUTPUTPORT_VALUE).SetupAsChannels<double>(GetTimeUnitString(UNIT_SECONDS), "y", OUTPUTPORT_VALUE );
	GetOutputPort(OUTPUTPORT_VALUE).GetChannels()->AddChannel(mSensor.GetChannel());

	// ATTRIBUTES

	// baseclass attributes
	InputNode::Init();

	// register attributes
	Core::AttributeSettings* timeUnit = RegisterAttribute("Time Unit", "timeUnit", "The time unit to convert the session time to.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	timeUnit->ResizeComboValues( (uint32)UNIT_NUM );
	timeUnit->SetComboValue(UNIT_MILLISECONDS, GetTimeUnitString(UNIT_MILLISECONDS));
	timeUnit->SetComboValue(UNIT_SECONDS, GetTimeUnitString(UNIT_SECONDS));
	timeUnit->SetComboValue(UNIT_MINUTES, GetTimeUnitString(UNIT_MINUTES));
	timeUnit->SetComboValue(UNIT_HOURS, GetTimeUnitString(UNIT_HOURS));
	timeUnit->SetDefaultValue( Core::AttributeInt32::Create(UNIT_SECONDS) );

	// sample rate
	const double defaultSampleRate	= mSensor.GetChannel()->GetSampleRate();
	Core::AttributeSettings* attributeSettings = RegisterAttribute("Sample Rate", "sampleRate", "Sample rate of the generated signal.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeSettings->SetDefaultValue( Core::AttributeFloat::Create(defaultSampleRate) );
	attributeSettings->SetMinValue( Core::AttributeFloat::Create(1.0) );
	attributeSettings->SetMaxValue( Core::AttributeFloat::Create(FLT_MAX) );

}


// get time unit string
const char* SessionTimeNode::GetTimeUnitString(ETimeUnit unit)
{
	switch (unit)
	{
		case UNIT_MILLISECONDS:	{ return "Milliseconds"; }
		case UNIT_SECONDS:		{ return "Seconds"; }
		case UNIT_MINUTES:		{ return "Minutes"; }
		case UNIT_HOURS:		{ return "Hours"; }
        default:                { return ""; }
	}
}


// get short time unit string 
const char* SessionTimeNode::GetTimeUnitStringShort (ETimeUnit unit)
{
	switch (unit)
	{
		case UNIT_MILLISECONDS:	{ return "ms"; }
		case UNIT_SECONDS:		{ return "s"; }
		case UNIT_MINUTES:		{ return "m"; }
		case UNIT_HOURS:		{ return "h"; }
		default:                { return ""; }
	}
}


void SessionTimeNode::Reset()
{
	InputNode::Reset();

	mClock.Stop();
	mClock.Reset();
}


void SessionTimeNode::ReInit(const Time& elapsed, const Time& delta)
{
	InputNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


void SessionTimeNode::Start(const Time& elapsed)
{
	// call baseclass start
	InputNode::Start(elapsed);

	// set output port name
	const ETimeUnit unit = (ETimeUnit)GetInt32Attribute(ATTRIB_UNIT);
	OutputPort& outputPort = GetOutputPort(OUTPUTPORT_VALUE);
	outputPort.SetName(GetTimeUnitString(unit));

	// configure output channel
	const double sampleRate = GetFloatAttribute(ATTRIB_SAMPLERATE);
	mSensor.SetSampleRate(sampleRate);

	// construct a channel name
	mTempString.Format("Session Time in %s", GetTimeUnitString(unit));
	mSensor.SetName(mTempString.AsChar());

	// set channel time unit
	mSensor.GetChannel()->SetUnit(GetTimeUnitStringShort(unit));

	// configure clock and start it at current elapsed time
	mClock.Reset();
	mClock.SetFrequency(sampleRate);
	mClock.SetStartTime(elapsed);
	mClock.Start();
}


// update the node
void SessionTimeNode::Update(const Time& elapsed, const Time& delta)
{	
	// shared base update helper
	if (BaseUpdate(elapsed, delta) == false)
		return;

	mClock.Update(elapsed, delta);

	InputNode::Update(elapsed, delta);
}


void SessionTimeNode::OnAttributesChanged()
{
	// in case the sample rate changed, reset the node and channels
	const double sampleRate = GetFloatAttribute(ATTRIB_SAMPLERATE);
	if (mSensor.GetChannel()->GetSampleRate() != sampleRate)
		ResetAsync();
}


// the function that fills the sensors with samples
void SessionTimeNode::GenerateSamples()
{
	const double sampleTimeDelta	= 1.0 / mSensor.GetChannel()->GetSampleRate();	// sample spacing
	const uint32 numNewSamples		= mClock.GetNumNewTicks();						// number of samples we have to add
	mClock.ClearNewTicks();

	const ETimeUnit timeUnit 		= (ETimeUnit)GetInt32Attribute(ATTRIB_UNIT);

	// get the session time in seconds
	double factor = 0.0;		// multiply by seconds to get the result
	switch (timeUnit)
	{
		case UNIT_MILLISECONDS:	{ factor = 1000.0;		 break; }
		case UNIT_SECONDS:		{ factor = 1.0;			 break; }
		case UNIT_MINUTES:		{ factor = 1.0 / 60.0;	 break; }
		case UNIT_HOURS:		{ factor = 1.0 / 3600.0; break; }
        default:                { break; }
	}

	uint32 sampleCounter = mSensor.GetChannel()->GetSampleCounter();

	// create samples
	for (uint32 i=0; i<numNewSamples; ++i)
	{
		const double time = sampleCounter * sampleTimeDelta;

		// calculate the sin of the current time value
		const double value = time * factor;

		// add the sample
		mSensor.AddQueuedSample(value);
		
		sampleCounter++;
	}
}
