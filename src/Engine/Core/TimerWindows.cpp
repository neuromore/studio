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
#include "TimerWindows.h"
#include "Time.h"


namespace Core
{

// if we're on the windows platform
#ifdef NEUROMORE_PLATFORM_WINDOWS

// constructor
Timer::Timer()
{
	QueryPerformanceFrequency(&mTimerFreq);
	
	// reset the timer
	Reset();
}


// destructor
Timer::~Timer()
{
}


// reset the timer
void Timer::Reset()
{
	QueryPerformanceCounter(&mStartTicks);
	
	mLastTicks = mStartTicks;
}



// get the current time
Time Timer::GetTime()
{ 
	QueryPerformanceCounter(&mCurrentTicks);

	// elapsed ticks
	LARGE_INTEGER ticksSinceReset;
	ticksSinceReset.QuadPart = mCurrentTicks.QuadPart - mStartTicks.QuadPart;

	// convert ticks to time
	Time currentTime;
	ToTime(&currentTime, &ticksSinceReset, &mTimerFreq);

	return currentTime;
}


// get the time since the last Reset
Time Timer::GetTimeDelta()
{
	QueryPerformanceCounter(&mCurrentTicks);

	// elapsed time in ticks
	LARGE_INTEGER elapsedTicks;
	elapsedTicks.QuadPart = mCurrentTicks.QuadPart - mLastTicks.QuadPart;

	// convert to our format
	Time elapsedTime;
    ToTime(&elapsedTime, &elapsedTicks, &mTimerFreq);

	// remember for next delta calc
	mLastTicks = mCurrentTicks;

	return elapsedTime;
}


// convert QPC ticks to our time format
void Timer::ToTime(Time* out, LARGE_INTEGER* ticks, LARGE_INTEGER* frequency)
{
	// seconds
	out->mSeconds = ticks->QuadPart / frequency->QuadPart;

	// microseconds
	out->mNanoSeconds = (ticks->QuadPart * (LONGLONG)1000000000) / frequency->QuadPart - (LONGLONG)out->mSeconds * (LONGLONG)1000000000;
}



#endif	// #ifdef NEUROMORE_PLATFORM_WINDOWS

}	// namespace Core
