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

// include required files
#include "Sensor.h"
#include "Core/Counter.h"
#include "Core/LogManager.h"
#include "EngineManager.h"
#include "Device.h"

using namespace Core;

// default constructor (no name, unknown sample rate, no upsampling)
Sensor::Sensor() : Sensor("", 0.0)
{
}


//// copy constructor
Sensor::Sensor(const Sensor& sensor) : Sensor(sensor.GetName(), sensor.GetSampleRate())
{
	// TODO: this is somehow needed by DLLs only
}


// create a sensor with fixed sample rate (the most common type)
Sensor::Sensor(const char* name, double sampleRate) : Sensor(name, sampleRate, sampleRate)
{
}

// create a sensor with different input/output samplerates. 0Hz input = irregular channel
Sensor::Sensor(const char* name, double sampleRateOut, double sampleRateIn)
{
	mID				 = CORE_COUNTER.Next();
	mIsEnabled		 = true;
	mHardwareChannel = -1;
	mLatency		 = 0;
	mExpectedJitter  = 0;
	mRealSampleRate  = 0;
	mNumLostSamples  = 0;
	mSampleRate		 = sampleRateOut;
	
	mUseDriftCorrection		= true;		// enabled drift correction by default // TODO check if the other way around makes more sense
	mNumDriftSamplesAdded	= 0;
	mNumDriftSamplesRemoved = 0;

	mContactQuality = CONTACTQUALITY_NOT_AVAILABLE;

	// reserve some memory for the sample queue
	mQueuedSamples.Reserve(2048);

	// for bursts, look at the last 200 updates (not great as it depends on the update rate.. but better than nothing)
	mBursts.Resize(200);
	mBursts.SetAll(0.0);

	// input channel
	GetInput()->SetBufferSize(2048);
	GetInput()->SetName(name);
	GetInput()->SetIndependent(true);  // input is by default independent (data stream from the hardware running on different clock)
	GetInput()->SetSampleRate(sampleRateIn);

	// create and configure resampler
	mResampler.SetInput(&mInputChannel);
	mResampler.SetResampleMode(ResampleProcessor::REALTIME);
	mResampler.SetOutputSampleRate(mSampleRate);
	mResampler.ReInit();

	// forward input channel specsconfigure specs of output channel the same as the input channel
	GetOutput()->SetMinValue(GetInput()->GetMinValue());
	GetOutput()->SetMaxValue(GetInput()->GetMaxValue());
	GetOutput()->SetUnit(GetInput()->GetUnit());
	GetOutput()->SetColor(GetInput()->GetColor());
	GetOutput()->SetBufferSize(2048);
	
	// set name of sensor / both channels
	SetName(name);
}


// destructor
Sensor::~Sensor()
{
}


// update the sensor data
void Sensor::Update(const Time& elapsed, const Time& delta)
{
	//
	// feed forward all queued samples
	//

	mQueuedSamplesLock.Lock();

	const uint32 numQueuedSamples = mQueuedSamples.Size();

	// always reset counter before adding the new samples
	GetInput()->BeginAddSamples();

	// add samples to raw sample channel
	for (uint32 i = 0; i < numQueuedSamples; ++i)
		GetInput()->AddSample(mQueuedSamples[i]);
	
	// clear the queued samples
	mQueuedSamples.Clear();

	mQueuedSamplesLock.Unlock();

	mResampler.Update(elapsed, delta);

	// increase elapsed time of the channels and update latency
	GetOutput()->SetElapsedTime(elapsed);
	GetInput()->SetElapsedTime(elapsed);
	GetOutput()->UpdateLatency();
	GetInput()->UpdateLatency();

	// shift all values one element towards index 0
	const uint32 numBursts = mBursts.Size();
	for (uint32 i=1; i<numBursts; ++i)
		mBursts[i-1] = mBursts[i];

	// add the new value at the end
	mBursts[numBursts-1] = numQueuedSamples;

	// calculate the real (hardware) sample rate of the sensor
	const uint32 numSamplesReceivedTotal = GetInput()->GetSampleCounter();
	const Time realElapsedTime = GetInput()->GetElapsedTime();
	if (realElapsedTime > 0)
		mRealSampleRate = numSamplesReceivedTotal / realElapsedTime.InSeconds();

	// correct hardware clock drift (iff enabled in sensor and in engine and input channel is of constant sample rate)
	if (mUseDriftCorrection == true && GetEngine()->GetDriftCorrectionSettings().mIsEnabled == true)
		CorrectForDrift();
}


// reset sensor
void Sensor::Reset()
{
	// reset sample channel and sample queue
	GetOutput()->Reset();
	ClearQueuedSamples();
	
	GetInput()->Reset();

	// clear burst sizes
	mBursts.SetAll(0);
	
	// reset statistic values
	mNumDriftSamplesAdded = 0;
	mNumDriftSamplesRemoved = 0;
	mNumLostSamples = 0;

	mResampler.ReInit();
}


void Sensor::SetStartTime(const Time& seconds)
{
	// set channel start time
	GetOutput()->SetStartTime(seconds);
	GetInput()->SetStartTime(seconds);
}


// set the sensor sample rate
void Sensor::SetSampleRate(double sampleRate)
{
	mSampleRate = sampleRate;

	// set sample rate of resampler
	mResampler.SetOutputSampleRate(mSampleRate);
	mResampler.ReInit();
	
	GetOutput()->SetSampleRate(mSampleRate);
}


void Sensor::AddQueuedSample(double value)
{
	mQueuedSamplesLock.Lock();
	mQueuedSamples.Add(value);
	mQueuedSamplesLock.Unlock();
}


void Sensor::ClearQueuedSamples()
{ 
	mQueuedSamplesLock.Lock();
	mQueuedSamples.Clear(); 
	mQueuedSamplesLock.Unlock();
}


// compensate for lost samples by adding zero values and increase lostsample counter
void Sensor::HandleLostSamples(uint32 numLostSamples)
{
	for (uint32 i=0; i<numLostSamples; ++i)
		AddQueuedSample(0.0);

	mNumLostSamples += numLostSamples;
}


void Sensor::Sync(const Core::Time& time, bool usePadding)
{
	// Do the Sync Padding
	if (usePadding == true)
	{
		// calculate the number of padding samples we need
		const Time paddingDuration = time - Time(GetLatency());

		// clamp to zero
		if (paddingDuration > 0)
		{
			const uint32 numPaddingSamples = (uint32)(paddingDuration.InSeconds() * mSampleRate + 0.5);		// round to closest integer

			// add dummy samples
			for (uint32 i = 0; i < numPaddingSamples; ++i)
				GetOutput()->AddSample(0.0);
		}
	}
	else // Set start time
	{
		SetStartTime(time);
	}
}


double Sensor::GetExpectedJitter() const
{
	// if no jitter is set (=0), we use the burst rate
	if (mExpectedJitter == 0)
		return FindMaxBurstSize() * mSampleRate;
	else
		return mExpectedJitter;
}


//
// Burst Calculations
//

// return the burst size of the last Update()
uint32 Sensor::GetLastBurstSize()
{
	if (mBursts.Size() == 0)
		return 0;

	return mBursts.GetLast();
}


// find the maximum burst size within the last few updates
uint32 Sensor::FindMaxBurstSize() const
{
	if (mBursts.Size() == 0)
		return 0;

	uint32 maxVal = 0;
	const uint32 numBursts = mBursts.Size();
	for (uint32 i=0; i < numBursts; i++)
		maxVal = Core::Max<uint32>(maxVal, mBursts[i]);

	return maxVal;
}


// calculate the average burst size within the last few seconds
double Sensor::CalculateAverageBurstSize() const
{
	if (mBursts.Size() == 0)
		return 0;

	uint32 sum = 0;
	uint32 num = 0;
	const uint32 numBursts = mBursts.Size();
	for (uint32 i=0; i < numBursts; i++)
	{
		const uint32 burstSize = mBursts[i];
		
		// skip zero bursts!
		if (burstSize == 0)
			continue;

		sum += burstSize;
		num++;
	}

	return (double)sum / (double)num;
}


//
// Drift Correction
//

void Sensor::CorrectForDrift()
{
	// skip channels with 0 (undefined) sample rate
	const double targetSampleRate = GetOutput()->GetSampleRate();
	if (targetSampleRate <= 0.0)
		return;

	// recalculate real sample rate
	const uint32 numSamplesTotal = GetOutput()->GetSampleCounter();
	const Time realElapsedTime = GetOutput()->GetElapsedTime();

	// improved device startup: do not correct for drift until we have 2 second of data and more than 10 samples (time constraint is for high sample rates, sample constraint is for low sample rates)
	if (realElapsedTime < 2.0 || numSamplesTotal < 10)
		return;

	// calc the clock drift in number of samples
	const int drift = CalculateDrift();
	const uint32 driftAbsolute = (uint32)abs(drift);
	const double driftInSeconds = drift / targetSampleRate;
    const double driftInSecondsAbsolute = Math::AbsD(driftInSeconds);

	// drift correction settings
	EngineManager::DriftCorrectionSettings settings = GetEngine()->GetDriftCorrectionSettings();
	
    // unused variable
    //const double numBurstsTolerance = 3;

	// automatic engine sync of drift exeeds maximum
	if (driftInSecondsAbsolute > settings.mMaxDriftUntilSync && GetEngine()->GetAutoSyncSetting() == true)
	{
		LogDebug("Drift Correction: Sensor %s has drifted %.2f seconds, which is more than the absolute allowed %.2f; scheduling a sync to correct this.", GetName(), driftInSeconds, settings.mMaxDriftUntilSync);
		GetEngine()->SyncAsync();
		return;
	}

	// important: correction can't work if drift is smaller than sample interval (1/samplerate)
	if (driftInSecondsAbsolute < 1.0 / targetSampleRate)
		return;
	
	//
	// now add/remove the samples
	//

	// late signals / backward drift
	if (driftInSeconds > settings.mMaxBackwardDrift)
	{
		// max number of samples drift correction should add at once
		const double driftToCorrect = driftInSeconds - settings.mMaxBackwardDrift;
		uint32 maxNumSamplesToAdd = driftToCorrect * targetSampleRate;
		const uint32 numSampelsToAdd = Min(driftAbsolute, maxNumSamplesToAdd);
		
		//LogDebug("Drift Correction: Sensor %s is running too slow (currently %i samples) - adding %i samples.", GetName(), drift, numSampelsToAdd);
		// LogDebug("Real Sample Rate is %.2f", GetRealSampleRate());
		
		mNumDriftSamplesAdded += numSampelsToAdd;

		// copy last sample 
		double dummyValue = 0.0;
		if (numSamplesTotal > 0)
			dummyValue = GetInput()->GetLastSample();

		for (uint32 i = 0; i<numSampelsToAdd; ++i)
			GetInput()->AddSample(dummyValue);
	}

	// early signals / forward drift
	else if (driftInSeconds < -settings.mMaxForwardDrift)
	{
		// max number of samples drift correction should renove at once
		const double driftToCorrect = -driftInSeconds - settings.mMaxForwardDrift;
		uint32 maxNumSamplesToRemove = driftToCorrect * targetSampleRate;

		// do NEVER EVER remove more samples than we added in this update (last burst size), otherwise the samples counter may run backward from the viewpoint of the classifier
		const uint32 numNewSamples = mBursts.GetLast();
		maxNumSamplesToRemove = Min(maxNumSamplesToRemove, numNewSamples);

		const uint32 numSamplesToRemove = Min(maxNumSamplesToRemove, driftAbsolute);

		//LogDebug("Drift Correction: Sensor %s is running too fast (currently %i samples) - removing %i samples.", GetName(), -drift, numToRemove);
		
		mNumDriftSamplesRemoved += numSamplesToRemove;

		for (uint32 i = 0; i<numSamplesToRemove; ++i)
			GetInput()->RemoveLastSample();
	}
}

// calculates how many samples the sensor is ahead in time (negative means it lags behind)
int32 Sensor::CalculateDrift() const
{
	// FIXME the follow signed integers wastes half of the index range, samples higher the 2^63 cannot be handled by this code
	const int64 numSamplesTheoretical = GetOutput()->GetElapsedTime().InSeconds() * mSampleRate;		// rounds down
	const int64 numSamplesReal = GetOutput()->GetSampleCounter();
	const int32 distance = numSamplesTheoretical - numSamplesReal;

	return distance;
}

//
// Contact Quality
//

// get an appropriate text description for a contact quality
const char* Sensor::GetContactQualityDescription(EContactQuality quality)
{
	switch (quality)
	{
		case Sensor::CONTACTQUALITY_NOT_AVAILABLE:	return "n/a";
		case Sensor::CONTACTQUALITY_VERY_BAD:		return "Very Bad";
		case Sensor::CONTACTQUALITY_POOR:			return "Poor";
		case Sensor::CONTACTQUALITY_FAIR:			return "Fair";
		case Sensor::CONTACTQUALITY_GOOD:			return "Good";
		default:									return "No Signal"; 
	}
}

// contact quality of this sensor in text form
const char* Sensor::GetContactQualityAsString() const
{
	return GetContactQualityDescription(mContactQuality);
}


// get the corresponding color for the current contact quality
Color Sensor::GetContactQualityColor() const
{
	switch (mContactQuality)
	{
		// old colors
		//case Sensor::CONTACTQUALITY_NO_SIGNAL:	{ return Color( 0.0f, 0.0f, 0.0f ); break; }	// black
		//case Sensor::CONTACTQUALITY_VERY_BAD:	{ return Color( 1.0f, 0.0f, 0.0f ); break; }	// red
		//case Sensor::CONTACTQUALITY_POOR:		{ return Color( 1.0f, 0.4f, 0.0f ); break; }	// orange
		//case Sensor::CONTACTQUALITY_FAIR:		{ return Color( 1.0f, 1.0f, 0.0f ); break; }	// yellow
		//case Sensor::CONTACTQUALITY_GOOD:		{ return Color( 0.0f, 1.0f, 0.0f ); break; }	// green

		// TODO we don't have CI colors in engine, so how do we color sensors? should we rather move signal quality coloring into the EEG widget, or move CI colors into the core?
		// CI colors
		case Sensor::CONTACTQUALITY_NO_SIGNAL:		{ return Color(100.0f / 255.0f, 100.0f / 255.0f, 99.0f / 255.0f); }// medium light-grey
		case Sensor::CONTACTQUALITY_VERY_BAD:		{ return Color(237.0f / 255.0f,	0.0f,			46.0f / 255.0f); }	// red
		case Sensor::CONTACTQUALITY_POOR:			{ return Color(235.0f / 255.0f, 114.0f / 255.0f, 5.0f / 255.0f); }	// orange
		case Sensor::CONTACTQUALITY_FAIR:			{ return Color(255.0f / 255.0f, 171.0f / 255.0f, 7.0f / 255.0f); }	// yellow
		
		// Note: our CI color 'green' looks like yellow on most screens! -> using other green instead
		//case Sensor::CONTACTQUALITY_GOOD:			{ return Color(210.0f / 255.0f, 255.0f / 255.0f, 36.0f / 255.0f); }	// green
		case Sensor::CONTACTQUALITY_GOOD:			{ return Color(50.0f / 255.0f, 234.0f / 255.0f, 33.0f / 255.0f); }	// green

		default:
		case Sensor::CONTACTQUALITY_NOT_AVAILABLE:	{ return Color(0.0f, 159.0f / 255.0f, 227.0f / 255.0f); }			// NM blue

	}
}
