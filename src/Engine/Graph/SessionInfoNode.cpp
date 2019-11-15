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
#include "SessionInfoNode.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
SessionInfoNode::SessionInfoNode(Graph* graph) : InputNode(graph)
{
	// configure the sensors

	mTimeSensor.GetChannel()->SetSampleRate(128);
	mTimeSensor.GetChannel()->SetBufferSize(10);
	mTimeSensor.SetDriftCorrectionEnabled(false);
	mSensors.Add(&mTimeSensor);

	mPauseSensor.SetName("Session Paused");
	mPauseSensor.GetChannel()->SetSampleRate(128);
	mPauseSensor.GetChannel()->SetBufferSize(10);
	mPauseSensor.SetDriftCorrectionEnabled(false);
	mSensors.Add(&mPauseSensor);

	mPointsSensor.SetName("Session Points");
	mPointsSensor.GetChannel()->SetUnit("Points");
	mPointsSensor.GetChannel()->SetSampleRate(128);
	mPointsSensor.GetChannel()->SetBufferSize(10);
	mPointsSensor.SetDriftCorrectionEnabled(false);
	mSensors.Add(&mPointsSensor);

	// color the output channel same as the node
	UseChannelColoring();

	// configure and start clockgenerator
	mClock.SetFrequency(128);
	mClock.Start();
}


// destructor
SessionInfoNode::~SessionInfoNode()
{
}


// initialize the node
void SessionInfoNode::Init()
{
	// PORTS

	// register ports
	InitOutputPorts(3);

	GetOutputPort(OUTPUTPORT_TIME).SetupAsChannels<double>("Elapsed Time", "time", OUTPUTPORT_TIME );
	GetOutputPort(OUTPUTPORT_TIME).GetChannels()->AddChannel(mTimeSensor.GetChannel());

	GetOutputPort(OUTPUTPORT_PAUSED).SetupAsChannels<double>("Paused", "pause", OUTPUTPORT_PAUSED );
	GetOutputPort(OUTPUTPORT_PAUSED).GetChannels()->AddChannel(mPauseSensor.GetChannel());
	
	GetOutputPort(OUTPUTPORT_POINTS).SetupAsChannels<double>("Points", "points", OUTPUTPORT_POINTS );
	GetOutputPort(OUTPUTPORT_POINTS).GetChannels()->AddChannel(mPointsSensor.GetChannel());

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
	const double defaultSampleRate	= mTimeSensor.GetChannel()->GetSampleRate();
	Core::AttributeSettings* attributeSettings = RegisterAttribute("Sample Rate", "sampleRate", "Sample rate of the generated signal.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeSettings->SetDefaultValue( Core::AttributeFloat::Create(defaultSampleRate) );
	attributeSettings->SetMinValue( Core::AttributeFloat::Create(1.0) );
	attributeSettings->SetMaxValue( Core::AttributeFloat::Create(FLT_MAX) );

}


// get time unit string
const char* SessionInfoNode::GetTimeUnitString(ETimeUnit unit)
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
const char* SessionInfoNode::GetTimeUnitStringShort (ETimeUnit unit)
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


void SessionInfoNode::Reset()
{
	InputNode::Reset();

	mClock.Stop();
	mClock.Reset();
}


void SessionInfoNode::ReInit(const Time& elapsed, const Time& delta)
{
	InputNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


void SessionInfoNode::Start(const Time& elapsed)
{
	// call baseclass start
	InputNode::Start(elapsed);

	// configure output
	const ETimeUnit unit = (ETimeUnit)GetInt32Attribute(ATTRIB_TIMEUNIT);
	const double sampleRate = GetFloatAttribute(ATTRIB_SAMPLERATE);
	mTimeSensor.SetSampleRate(sampleRate);
	mTempString.Format("Session Time in %s", GetTimeUnitString(unit));
	mTimeSensor.SetName(mTempString.AsChar());
	mTimeSensor.GetChannel()->SetUnit(GetTimeUnitStringShort(unit));

	// configure other channels
	mPauseSensor.SetSampleRate(sampleRate);
	mPointsSensor.SetSampleRate(sampleRate);

	// configure clock and start it at current elapsed time
	mClock.Reset();
	mClock.SetFrequency(sampleRate);
	mClock.SetStartTime(elapsed);
	mClock.Start();
}


// update the node
void SessionInfoNode::Update(const Time& elapsed, const Time& delta)
{	
	// shared base update helper
	if (BaseUpdate(elapsed, delta) == false)
		return;

	mClock.Update(elapsed, delta);

	InputNode::Update(elapsed, delta);
}


void SessionInfoNode::OnAttributesChanged()
{
	// in case the sample rate changed, reset the node and channels
	const double sampleRate = GetFloatAttribute(ATTRIB_SAMPLERATE);
	if (mTimeSensor.GetChannel()->GetSampleRate() != sampleRate)
		ResetAsync();
}


// the function that fills the sensors with samples
void SessionInfoNode::GenerateSamples()
{
	//const double sampleTimeDelta	= 1.0 / mTimeSensor.GetChannel()->GetSampleRate();	// sample spacing
	const uint32 numNewSamples		= mClock.GetNumNewTicks();						// number of samples we have to add
	mClock.ClearNewTicks();

	
	// get the session time in seconds
	Core::Time time = 0.0;
	if (GetSession()->IsRunning() == true)
		time = GetSession()->GetElapsedTime();

	// convert to unit
	double convertedTime = 0.0;
	const ETimeUnit timeUnit 		= (ETimeUnit)GetInt32Attribute(ATTRIB_TIMEUNIT);
	switch (timeUnit)
	{
		case UNIT_MILLISECONDS:	convertedTime = time.InMilliseconds();	break;
		default:
		case UNIT_SECONDS:		convertedTime = time.InSeconds();		break;
		case UNIT_MINUTES:		convertedTime = time.InMinutes();		break;
		case UNIT_HOURS:		convertedTime = time.InHours();			break;
	}
	
	// paused value
	const bool isPaused = GetSession()->IsRunning() && GetSession()->IsPaused();
	const double isPausedVal = (isPaused ? 1.0 : 0.0);

	// current points
	double pointsVal = 0.0;
	if (GetSession()->IsRunning() == true)
		pointsVal = GetSession()->GetPoints();
	
	// output samples
	for (uint32 i=0; i<numNewSamples; ++i)
	{
		// TODO interpolate between updates, the current way produces steps because it outputs only the current session time at the time of the update
		mTimeSensor.AddQueuedSample(convertedTime);
		mPauseSensor.AddQueuedSample(isPausedVal);
		mPointsSensor.AddQueuedSample(pointsVal);
	}
}
