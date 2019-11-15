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
#include "ClockGenerator.h"

using namespace Core;

// constructor
ClockGenerator::ClockGenerator(double frequency)
{
	mFrequency = frequency;
	mClockMode = INDEPENDENT;
	mReferenceChannel = NULL;

	// clock is started by default
	mIsRunning = true;
	Reset();
}


// destructor
ClockGenerator::~ClockGenerator()
{
}



// reset clock
void ClockGenerator::Reset()
{
	// reset counters
	mStartTime = 0;
	mElapsedTime = 0;
	mElapsedTicks = 0;
	mNewTicks = 0;
}


// update the clock
void ClockGenerator::Update(const Time& elapsed, const Time& delta)
{
	// clock is stopped
	if (mIsRunning == false)
		return;
	
	// zero frequency means the clock is not ticking
	if (mFrequency == 0)
		return;

	// clock begins ticking as soon as start time is reached
	if (elapsed < mStartTime)
		return;
	
	// do not run clock if the mode requires a reference channel
	if (mClockMode != INDEPENDENT && HasReferenceChannel() == false)
		return;

	// calculate how far the clock can advance
	Time maxElapsedTime;
	if (mClockMode == INDEPENDENT)
	{
		maxElapsedTime = elapsed;
	}
	else if (mClockMode == SYNCED)
	{
		maxElapsedTime = Min<Time>(elapsed, mReferenceChannel->GetLastSampleTime());
	}
	else if (mClockMode == SYNCED_AHEAD)
	{
		maxElapsedTime = Min<Time>(elapsed, mReferenceChannel->GetLastSampleTime() + 1.0 / mReferenceChannel->GetSampleRate());
	}

	//LogDebug("time difference = -%f +%f", (elapsed - maxElapsedTime).InSeconds(), (maxElapsedTime - elapsed).InSeconds());

	// input reference channel must never run ahead of clock
	if (maxElapsedTime > elapsed)
	{
		LogDebug("Clock reference channel is running ahead of global elapsed time (%.5fs > %.5fs)!", maxElapsedTime.InSeconds(), elapsed.InSeconds());
		maxElapsedTime = elapsed;
	}

	// calculate the number of new ticks that occured
	const double samplePeriod = 1.0 / mFrequency;
	Time intervalLength = maxElapsedTime - mElapsedTime;
	uint32 numTicks = intervalLength.InSeconds() / samplePeriod;
	
	// advance clock
	mNewTicks += numTicks;
	mElapsedTicks += numTicks;
	mElapsedTime = mStartTime + mElapsedTicks * samplePeriod;
}



void ClockGenerator::SetFrequency(double frequency)
{
	if (mFrequency == frequency)
		return;

	mFrequency = frequency;
}


// manually decrement the number of new ticks (used in manual mode)
void ClockGenerator::DecrementNewTicks(uint32 numTicks)
{
	CORE_ASSERT(mNewTicks >= numTicks);
	
	mNewTicks -= numTicks;
}


// get new ticks (where 0 is the oldest one)
uint64 ClockGenerator::GetTick(uint32 newTickIndex)
{
	const uint64 tick = mElapsedTicks - mNewTicks + newTickIndex;
	CORE_ASSERT(tick < mElapsedTicks);
	
	return tick;
}


// return index of the oldest tick, and remove it from the new ticks list
uint64 ClockGenerator::PopOldestTick()
{
	uint64 tick = GetTick(0);
	mNewTicks--;
	return tick;
}
