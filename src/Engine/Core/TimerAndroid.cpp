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
#include "TimerAndroid.h"


namespace Core
{

// only include this for Android
#if defined(NEUROMORE_PLATFORM_ANDROID)

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
    struct timeval now;
    gettimeofday(&now, NULL);

	// set start time
	mStartTime.mSeconds = now.tv_sec;
	mStartTime.mNanoSeconds = now.tv_usec * 1000;
	mLastTime = mStartTime;
}


// get the current time
Time Timer::GetTime()
{ 
	struct timeval now;
	gettimeofday(&now, NULL);

	// return difference to start time
	return Time (now.tv_sec, now.tv_usec * 1000) - mStartTime;
}


// get the time since the last Reset
Time Timer::GetTimeDelta()
{
    struct timeval now;
	gettimeofday(&now, NULL);

	const Time currentTime = Time(now.tv_sec, now.tv_usec * 1000);

	// delta since last GetTimeDelta()
	const Time deltaTime = currentTime - mLastTime;

	// set last time
	mLastTime = currentTime;

	return deltaTime;
}

#endif	// platform check

}	// namespace Core
