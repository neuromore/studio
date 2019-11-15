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

#include "Time.h"
#include "LogManager.h"
#include <chrono>
#include <ctime>


using namespace std;
using namespace std::chrono;

namespace Core
{

// default constructor
Time::Time()
{
	mSeconds		= 0;
	mNanoSeconds	= 0;
}


// copy constructor
Time::Time(const Time& time)
{
	*this = time;
}


// constructor
Time::Time(double seconds)
{
	InitFrom(seconds);
}


// constructor
Time::Time(uint64 seconds, uint32 nanoseconds)
{
	mSeconds		= seconds;
	mNanoSeconds = nanoseconds;
}


// destructor
Time::~Time()
{
}


// current local epoch time
Time Time::Now()
{
	// get the high resolution time time stamp and the milliseconds that passed till then
#if defined(NEUROMORE_PLATFORM_WINDOWS) || defined(NEUROMORE_PLATFORM_ANDROID)

	const uint64 numSeconds = duration_cast<seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	const uint32 numMilliseconds = duration_cast<milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - numSeconds * 1000;

	// return time object, including milliseconds if available
	return Time(numSeconds, numMilliseconds * 1000 * 1000);
#else
	// don't use millisecond precission
	const time_t systemClockTime = system_clock::to_time_t(std::chrono::system_clock::now());
	return Time((uint64)systemClockTime);
#endif
}


time_t Time::ToStdTime() const
{
	return (time_t)mSeconds;
}


Time Time::FromStdTime(time_t stdTime)
{
	return Time ((uint64)stdTime, 0);
}


// to make sure we have a valid struct (not all value combinations are valid)
bool Time::IsValid() const
{
	// only thing that can be invalid is the nanoseconds (it doesn't utilize the full range of the datatype)
	return mNanoSeconds < 1E9;
}


// convert to ISO 8601 string representation
String Time::AsUtcString() const
{
	// Format is: "yyyy-MM-ddThh:mm:ssZ"
	// example:    2015-10-31T09:15:32Z 

	// use gmtime for this
	time_t time = ToStdTime();
	tm utcDateTime = *gmtime(&time);

	// read out all date time values
	const uint32 year	= utcDateTime.tm_year + 1900; // tm_year = years since 1900
	const uint32 month	= utcDateTime.tm_mon + 1; // tm_mon = months since january [0, 11]
	const uint32 day	= utcDateTime.tm_mday;

	const uint32 hour	= utcDateTime.tm_hour;
	const uint32 minute	= utcDateTime.tm_min;
	const uint32 second	= utcDateTime.tm_sec;

	// construct the string
	String result;
#if defined(NEUROMORE_PLATFORM_WINDOWS) || defined(NEUROMORE_PLATFORM_ANDROID)
	// TODO ask a time-zone wizard to check this for correctness:
	const uint32 ms		= mNanoSeconds / 1E6;		// gmtime doesn't support milliseconds precision so we take it directly from the timestamp, assuming the seconds lign up in both representations

    result.Format( "%i-%02i-%02iT%02i:%02i:%02iZ%04i", year, month, day, hour, minute, second, ms );
#else
    //LogWarning("DateTime::NowUTC: iOS doesnt support high_resolution_clock. Date time stamp will be created at second basis only.");
    result.Format( "%i-%02i-%02iT%02i:%02i:%02iZ", year, month, day, hour, minute, second );
#endif
	return result;
}


// time formating using std::put_time
// replaces %S/%M/%H/%d/%A/%a/%w/%m/%B/%b/%Y and %c
String Time::Format(const Core::String& format) const
{
	String tmp;
	Format(format, tmp);
	return tmp;
}


// time formating using std::put_time
// replaces %S/%M/%H/%d/%A/%a/%w/%m/%B/%b/%Y and %c
void Time::Format(const Core::String& format, Core::String& result) const
{
	result = format;

	// format string doesn't contain any literals
	if (format.Contains("%") == false)
		return;

	// strftime literals
	const uint32 numLiterals = 15;
	const char* literals[numLiterals] = { "%S", "%M", "%H", "%d", "%A", "%a", "%w", "%m", "%B", "%b", "%Y", "%c"};

	// start with format string
	
	// holds the replacement strings
	String tempString;

	// convert to std::time_t (has 1 second resolution)
	time_t stdTime = ToStdTime();
	
	for (uint32 i = 0; i < numLiterals; ++i)
	{
		// check if literal is present first
		if (result.Contains(literals[i]) == false)
			continue;

		// evaluate literal with strftime
		tempString.Resize(256);
		size_t length = std::strftime(tempString.AsChar(), tempString.GetMaxLength(), literals[i], std::localtime(&stdTime));
		tempString.Resize((uint32)length);

		// replace text
		result.Replace(literals[i], tempString.AsChar());
	}

	// special microsecond literal (not part of strftime)
	const char* msLiteral = "%f";
	if (result.Contains(msLiteral) == true)
	{
		const uint32 ms = (*this - FromStdTime(stdTime)).mNanoSeconds / 1000 / 1000;
		tempString.Format("%03i", ms);
		result.Replace(msLiteral, tempString.AsChar());
	}

}


Time Time::FromUtcString(const Core::String& utcString)
{
	// Format is: "yyyy-MM-ddThh:mm:ssZ"
	// example:    2015-10-31T09:15:32Z 

	// parse string with sscanf
	tm time;
	int year;
	const int numParsed = sscanf(utcString.AsChar(), "%4i-%2i-%2iT%2i:%2i:%2i", &year, &time.tm_mon, &time.tm_mday, &time.tm_hour, &time.tm_min, &time.tm_sec);
	if (numParsed == 6)
	{
		// calculate year
		time.tm_year = year - 1900;

		return Time::FromStdTime(std::mktime(&time));
	}

	return Time(0);
}


void Time::InitFrom(double seconds)
{
	mSeconds = (uint64)seconds;											// cast to integer rounds down
	mNanoSeconds = (uint32)( (seconds - (double)mSeconds) * 1E9);		// convert remainder to microseconds
}


// assignment operator
Time& Time::operator=(const Time& time)
{
	mSeconds		= time.mSeconds;
	mNanoSeconds	= time.mNanoSeconds;

	return *this;
}


// int 
Time& Time::operator=(int seconds)
{
	mSeconds = seconds;
	mNanoSeconds = 0;

	return *this;
}


// uint64
Time& Time::operator=(uint64 seconds)
{
	mSeconds = seconds;
	mNanoSeconds = 0;

	return *this;
}


// set from double
Time& Time::operator=(double seconds)
{
	InitFrom(seconds);
	return *this;
}


// add assignment
Time& Time::operator+=(const Time& time)
{
	mSeconds += time.mSeconds;
	mNanoSeconds += time.mNanoSeconds;

	// handle the overflow in microseconds
	if (mNanoSeconds >= 1E9)
	{
		mSeconds++;
		mNanoSeconds -= 1E9;
	}

	return *this;
}


// subtract assignment
Time& Time::operator-=(const Time& time)
{
	// make sure we do not not underflow
	if (time >= *this)
	{
		mSeconds = 0;
		mNanoSeconds = 0;
	}
	else
	{
		mSeconds -= time.mSeconds;

		// again, make sure we do not underflow
		if (time.mNanoSeconds > mNanoSeconds)
		{
			// note: the underflow can occur only once per operation
			const uint32 nanoSecondsToSubtract = time.mNanoSeconds - mNanoSeconds;
			
			// clamp to 0
			if (mSeconds == 0)
			{
				mNanoSeconds = 0;
			} 
			else
			{
				mSeconds--;
				mNanoSeconds = 1E9 - nanoSecondsToSubtract;
			}
		}
		else
		{
			mNanoSeconds -= time.mNanoSeconds;
		}
	}

	return *this;
}


// multiply assignment
Time& Time::operator*=(uint32 factor)
{
	// naive way, just to be safe
	while (factor > 1)
	{
		factor--;
		*this += *this;
	}

	return *this;
}


// add
Time Time::operator+(const Time& time) const
{
	Time result = *this;
	result += time;
	return result;
}


// subtract
Time Time::operator-(const Time& time) const
{
	Time result = *this;
	result -= time;
	return result;
}


// multiply by integer
Time Time::operator*(uint32 factor) const
{
	Time result = *this;
	result *= factor;
	return result;
}


/////////////////////////////////////

// equality operator 
bool Time::operator==(const Time& time) const
{
	return ( (mSeconds == time.mSeconds) && (mNanoSeconds == time.mNanoSeconds) );
}


// inequality operator
bool Time::operator!=(const Time& time) const
{
	return ( (mSeconds != time.mSeconds) || (mNanoSeconds != time.mNanoSeconds) );
}


// comparison operator
bool Time::operator<=(const Time& time) const
{
	return ( (*this < time) || (*this == time) );
}


// comparison operator
bool Time::operator>=(const Time& time) const
{
	return ( (*this > time) || (*this == time) );
}


// comparison operator
bool Time::operator<(const Time& time) const
{
	if (mSeconds < time.mSeconds)
		return true;

	if (mSeconds == time.mSeconds)
		if (mNanoSeconds < time.mNanoSeconds)
			return true;

	return false;
}


// comparison operator
bool Time::operator>(const Time& time) const
{
	if (mSeconds > time.mSeconds)
		return true;

	if (mSeconds == time.mSeconds)
		if (mNanoSeconds > time.mNanoSeconds)
			return true;

	return false;
}

/////////////////////////////////////


// equality operator 
bool Time::operator==(double time) const
{
	return (InSeconds() == time);
}


// inequality operator
bool Time::operator!=(double time) const
{
	return (InSeconds() != time);
}


// comparison operator
bool Time::operator<=(double time) const
{
	return (InSeconds() <= time);
}


// comparison operator
bool Time::operator>=(double time) const
{
	return (InSeconds() >= time);
}


// comparison operator
bool Time::operator<(double time) const
{
	return (InSeconds() < time);
}


// comparison operator
bool Time::operator>(double time) const
{
	return (InSeconds() > time);
}

//////////////////////////////////////////


// convert to microseconds
double Time::InMicroseconds() const
{
	return ((double)mSeconds * (double)1E6) + ((double)mNanoSeconds / (double)1E3);
}


// convert to milliseconds
double Time::InMilliseconds() const
{
	return ((double)mSeconds * (double)1E3) + ((double)mNanoSeconds / (double)1E6);
}


// convert to seconds (is also the standard double conversion)
double Time::InSeconds() const
{
	return (double)mSeconds + (double)(mNanoSeconds / 1E9);
}


// convert to minutes
double Time::InMinutes() const
{
	// FIXME implement a more precise version that operates directly on integer?
	return InSeconds() / 60.0;
}


// convert to hours
double Time::InHours() const
{
	// FIXME implement a more precise version that operates directly on integer?
	return InSeconds() / 3600.0;
}


// convert to days
double Time::InDays() const
{
	// FIXME implement a more precise version that operates directly on integer?
	return InSeconds() / (3600.0 * 24.0);
}

//////////////////////////////////////////


// clamp
Time Time::Clamp(const Time& min, const Time& max) const
{
	if (*this < min)	
		return min;

	else if (*this > max)
		return max;

	return *this;
}


} // namespace Core
