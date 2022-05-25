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

// include required files
#include "ChannelBase.h"
#include "../EngineManager.h"
#include "../Core/Counter.h"
#include "../Core/Time.h"

using namespace Core;

// explicit constructor
ChannelBase::ChannelBase(uint32 bufferSize)
{
	mID						= CORE_COUNTER.Next();
	mBufferSize				= bufferSize;

	// ------------------------
	
	mSampleRate		= 0;
	mIsIndependent  = false;
	mName			= "";
	mSourceName		= "";
	mMinValue		= 0;
	mMaxValue		= 0;
	mUnitString		= "";
	mIsHighlighted  = false;
	mNumSamples	= 0;
	mSampleCounter = 0;

	mElapsedTime = 0;
	mStartTime = 0;
	mLatency = 0;
	mTimeSinceLastAddSample = 100; // marks channel as inactive
	mIsHighlighted = false;
}


// destructor
ChannelBase::~ChannelBase()
{
}

void ChannelBase::Reset()
{ 
	LogTrace("Reset");

	Clear();
	mNumSamples	= 0;
	mNumNewSamples = 0;
	mSampleCounter = 0;
	mElapsedTime = 0;
	mStartTime = 0;
	mLatency = 0;
	mTimeSinceLastAddSample = 100;
}



uint64 ChannelBase::GetMinSampleIndex() const 				
{ 
	if (mSampleCounter == 0)
		return CORE_INVALIDINDEX64;

	return mSampleCounter - mNumSamples; 
}


uint64 ChannelBase::GetMaxSampleIndex() const
{ 
	if (mSampleCounter == 0)
		return CORE_INVALIDINDEX64;

	return mSampleCounter - 1; 
}

bool ChannelBase::IsValidSample(uint64 sampleIndex) const
{
	if (sampleIndex == CORE_INVALIDINDEX64)
		return false;

	// no samples at all
	if (mSampleCounter == 0)
		return false;

	// calc valid index range and check index
	const uint64 maxIndex = mSampleCounter - 1;
	const uint64 minIndex = mSampleCounter - mNumSamples;			
	const bool isValid = sampleIndex >= minIndex && sampleIndex <= maxIndex;

	return isValid;
}


// is channel active (are samples added regularly?)
bool ChannelBase::IsActive() const
{
	const double channelTimeoutInSeconds = 2.0;
	if (mTimeSinceLastAddSample > channelTimeoutInSeconds)
		return false;

	return true;
}


// remove one sample from the end of the channel (decrements indices and does not dealloc anything; used for drift correction)
void ChannelBase::RemoveLastSample()
{
	LogTrace("RemoveLastSample");

	// just to be safe
	CORE_ASSERT(mNumSamples > 0);
	CORE_ASSERT(mSampleCounter > 0);
	

	// decrement counters
	mNumSamples--; 
	mSampleCounter--;

	// decrement only if there are new samples
	if (mNumNewSamples > 0)
		mNumNewSamples--; 
}


// get the length of the channel in seconds
Time ChannelBase::GetDuration() const
{
	if (mSampleRate == 0.0)
		return Time(0.0);

	return Time(mSampleCounter / mSampleRate);
}


// timestamp of sample, where sample 0 falls on time mStartTime+1.0/samplerate (no sample at t=0)
Time ChannelBase::GetSampleTime(uint64 sampleIndex) const
{ 
	if (mSampleRate == 0.0)
		return Time(0.0);

	return mStartTime + Time((sampleIndex+1) / mSampleRate);
}


// the timestamp of the last sample in the channel
Time ChannelBase::GetLastSampleTime() const
{ 
	if (mSampleRate == 0.0)
		return Time(0.0);

	return mStartTime + Time(mSampleCounter / mSampleRate);
}


// calculate the current latency and calculate a smooth average by using smoothstep
void ChannelBase::UpdateLatency()
{
	if (IsEmpty() == true)
		return;

	// calculate signed latency with high precision
	double currentLatency = 0;
	Time elapsedTime = GetElapsedTime();
	Time lastSampleTime = GetLastSampleTime();
	if (elapsedTime > lastSampleTime)
		currentLatency = (elapsedTime - lastSampleTime).InSeconds();
	else
		currentLatency = (lastSampleTime - elapsedTime).InSeconds();

	// use current latency as start value (assume it will never be 0.0, except right after reset)
	if (mLatency == 0.0)
		mLatency = currentLatency;
	else
		mLatency = (mLatency * 5 + currentLatency) / 6.0;
}


void ChannelBase::SetStartTime(const Core::Time& time)								
{
	LogTrace("SetStartTime");
	LogDebug("Channel::SetStartTime: set mStartTime = %f", time.InSeconds());

	mStartTime = time;
}

// get sample closest to a given absolute time
uint64 ChannelBase::FindIndexByTime(const Time& time, bool roundToClosest)
{
	uint64 index = CORE_INVALIDINDEX64;

	// handle the negative index case first
	if (time < mStartTime)
		return index;


	// FIXME the float index calculationg will be rounding errors for large sample indices!
	//  -> This can not be fixed as long as we want to support rational sample rates during resampling.
	//  -> The only alternative is to implement integer-ratio resampling, which does not use this method

	// calculate sample index in the channel
	const Time relativeTime = time - mStartTime;
	double floatIndex = relativeTime.InSeconds() * mSampleRate - 1;

	if (roundToClosest == true)
	{
		// calc uint32 index if it is not out  of range
		if (floatIndex >= 0.0 && floatIndex <= mSampleCounter)
			index = (uint64)(floatIndex + 0.5);					// round to closest			
	}
	else
	{
		// calc uint32 index if it is not out  of range
		if (floatIndex >= 0.0 && floatIndex <= mSampleCounter)
			index = (uint64)(floatIndex);						// round down
	}

	LogDebugRT("found sample index %i for time %f where start is %f", index, time.InSeconds(), mStartTime.InSeconds());

	return index;
}


// DEPRECATED
// get sample closest to a given absolute time (this one supports negative time)
uint32 ChannelBase::FindIndexByTime(double time, bool roundToClosest, bool clampResult)
{
	uint32 index = CORE_INVALIDINDEX32;

	const double relativeTime = time - mStartTime.InSeconds();
	double floatIndex = relativeTime * mSampleRate - 1;

	// clamp, if wanted
	if (clampResult == true)
		floatIndex = Clamp<double>(floatIndex, GetMinSampleIndex(), GetMaxSampleIndex());

	if (roundToClosest == true)
	{
		// calc uint32 index if it is not out  of range
		if (floatIndex >= 0.0 && floatIndex <= mSampleCounter)
			index = (uint32)(floatIndex + 0.5);					// round to closest			
	}
	else
	{
		// calc uint32 index if it is not out  of range
		if (floatIndex >= 0.0 && floatIndex <= mSampleCounter)
			index = (uint32)(floatIndex);						// round down
	}

	LogDebugRT("found sample index %i for time %f where start is %f", index, time, mStartTime.InSeconds());

	return index;
}
