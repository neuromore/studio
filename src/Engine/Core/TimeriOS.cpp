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
#include "TimeriOS.h"


namespace Core
{

// only include this for iOS
#ifdef NEUROMORE_PLATFORM_IOS
	
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
		// get timebase info (iOS uses another timebase than OS X)
		mach_timebase_info_data_t info;
		mach_timebase_info(&info);
		
		uint64_t systemTime = mach_absolute_time();
		
		// Convert into nanoseconds
		systemTime *= info.numer;
		systemTime /= info.denom;
		
		mLastTime = systemTime;
	}
	
	
	// get the current time
	Time Timer::GetTime()
	{ 
		// get timebase info (iOS uses another timebase than OS X)
		mach_timebase_info_data_t info;
		mach_timebase_info(&info);
		
		uint64_t systemTime = mach_absolute_time();

		// Convert into nanoseconds
		systemTime *= info.numer;
		systemTime /= info.denom;
		
		uint64_t currTime = systemTime;
		uint64_t timeDelta = (currTime - mLastTime);
		
		Time time;
		ToTime(&time, &timeDelta);

		return time;
	}
	
	
	// get the time since the last Reset
	Time Timer::GetTimeDelta()
	{
		// get timebase info (iOS uses another timebase than OS X)
		mach_timebase_info_data_t info;
		mach_timebase_info(&info);
		
		uint64_t systemTime = mach_absolute_time();
		
		// Convert into nanoseconds
		systemTime *= info.numer;
		systemTime /= info.denom;
		
		uint64_t currTime = systemTime;
		uint64_t timeDelta = (currTime - mLastTime);
		
		// set new lasttime
		mLastTime = currTime;
		
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
