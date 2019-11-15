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

#ifndef __CORE_TIME_H
#define __CORE_TIME_H

// include required headers
#include "StandardHeaders.h"
#include "String.h"

namespace Core
{

// the time class
class ENGINE_API Time
{
	public:
		// constructors
		Time();
		Time(const Time& time);
		Time(double seconds);
		Time(uint64 seconds, uint32 nanoseconds);

		// destructor
		virtual ~Time();

		// the timestamp data
		uint64	mSeconds;
		uint32	mNanoSeconds;

		//
		// Epoch Time
		//

		// get current local epoch time
		static Time Now();

		// converter epoch time to/from ISO 8601 string representation
		String AsUtcString() const;
		static Time FromUtcString(const Core::String& utcString);

		// absolute time formating
		// replaces %S/%M/%H/%d/%A/%a/%w/%m/%B/%b/%Y and %c // TODO fixme this doesn't support enough replacements to be useful!
		String Format(const Core::String& format) const;

		//
		// Operators
		//

		// assignment operators
		Time& operator=(const Time& time);
		Time& operator=(int seconds);
		Time& operator=(uint64 seconds);
		Time& operator=(double seconds);

		// mathematical assignment operations
		Time& operator+=(const Time& time);
		Time& operator-=(const Time& time);
		Time& operator*=(uint32 factor);

		// mathematical operations
		Time operator+(const Time& time) const;
		Time operator-(const Time& time) const;
		Time operator*(uint32 factor) const;

		// compare operators
		bool operator==(const Time& time) const;
		bool operator!=(const Time& time) const;
		bool operator<=(const Time& time) const;
		bool operator>=(const Time& time) const;
		bool operator<(const Time& time) const;
		bool operator>(const Time& time) const;

		bool operator==(double time) const;
		bool operator!=(double time) const;
		bool operator<=(double time) const;
		bool operator>=(double time) const;
		bool operator<(double time) const;
		bool operator>(double time) const;

		// other methods
		Time Clamp(const Time& min, const Time& max) const;

		// time base conversions
		double InMicroseconds() const;
		double InMilliseconds() const;
		double InSeconds() const;
		double InMinutes() const;
		double InHours() const;
		double InDays() const;

	private:
		void Format(const Core::String& format, Core::String& result) const;
		bool IsValid() const;

		void InitFrom(double seconds);

		// std time helpers
		time_t ToStdTime() const;
		static Time FromStdTime(time_t stdTime);
};

} // namespace Core


#endif
