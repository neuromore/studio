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
#include "TimermacOS.h"


namespace Core
{

// only include this for OSX
#ifdef NEUROMORE_PLATFORM_OSX

// constructor
Timer::Timer()
{
	Reset();
}


// destructor
Timer::~Timer()
{
}


// reset the timer
void Timer::Reset()
{
	mStartTime = clock_gettime_nsec_np(CLOCK_UPTIME_RAW);	// get the systemtime
	mLastTime = mStartTime;
}


// get the time elapsed since the timer was reset
Time Timer::GetTime()
{ 
	uint64_t	currTime	= clock_gettime_nsec_np(CLOCK_UPTIME_RAW);	// get the systemtime
	uint64_t	timeDelta	= (currTime - mStartTime);		// calculate the time difference

	Time time;
	ToTime(&time, &timeDelta);

	return time;
}


// get the time elapsed since the last call of this function
Time Timer::GetTimeDelta()
{
	uint64_t	currTime	= clock_gettime_nsec_np(CLOCK_UPTIME_RAW);	// get the systemtime
	uint64_t	timeDelta	= currTime - mLastTime;			// calculate the time difference

	mLastTime = currTime;									// remember last time	

	Time time;
	ToTime(&time, &timeDelta);

	return time;
}


void Timer::ToTime(Time* out, uint64_t* nanoseconds)
{
	uint64_t seconds = *nanoseconds / 1000000000;

	// seconds
	out->mSeconds = seconds;

	// microseconds
	out->mNanoSeconds = (*nanoseconds - seconds * 1000000000);
}


#endif	// platform check

}	// namespace Core
