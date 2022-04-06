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
#include "TimerLinux.h"


namespace Core
{

// only include this for Linux
#if defined(NEUROMORE_PLATFORM_LINUX)

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
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);

	// set start time
	mStartTime.mSeconds = now.tv_sec;
	mStartTime.mNanoSeconds = now.tv_nsec;
	mLastTime = mStartTime;
}


// get the current time
Time Timer::GetTime()
{ 
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	
	// return difference to start time
	return Time (now.tv_sec, now.tv_nsec) - mStartTime;
}


// get the time since start ot the last call to GetTimeDelta()
Time Timer::GetTimeDelta()
{
	// current time
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	const Time currentTime = Time(now.tv_sec, now.tv_nsec);

	// delta since last GetTimeDelta()
	const Time deltaTime = currentTime - mLastTime;

	// set last time
	mLastTime = currentTime;

	return deltaTime;
}

#endif	// platform check

}	// namespace Core
