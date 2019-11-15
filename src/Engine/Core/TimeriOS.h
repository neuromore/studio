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

#ifndef __CORE_TIMERIOS_H
#define __CORE_TIMERIOS_H

// only include this for iOS
#ifdef NEUROMORE_PLATFORM_IOS

// include required headers
#include "StandardHeaders.h"
#include "Time.h"
#include <mach/mach_time.h>


namespace Core
{

class Timer
{
	public:
		// constructor & destructor
		Timer();
		~Timer();
		
		Time GetTime();
		Time GetTimeDelta();

		void Reset();
		
	private:
		uint64_t	mLastTime;	/**< The last time value recorded, in nanoseconds. */

		// convert nanoseconds to time
		void ToTime(Time* out, uint64_t* nanoseconds);
};
	
} // namespace Core


#endif	// platform check


#endif
