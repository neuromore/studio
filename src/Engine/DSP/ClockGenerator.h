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

#ifndef __NEUROMORE_CLOCKGENERATOR_H
#define __NEUROMORE_CLOCKGENERATOR_H

// include required headers
#include "../Config.h"
#include "Channel.h"
#include "ChannelReader.h"
#include "../Core/Time.h"


// the sensor class
class ENGINE_API ClockGenerator
{
	public:
		// constructor & destructor
		ClockGenerator(double frequency = 0);
		virtual ~ClockGenerator();

		// call this regularly to update the clock
		void Reset();	
		void Update(const Core::Time& elapsed, const Core::Time& delta);

		// control the  clock
		void Start()												{ mIsRunning = true; }
		void Stop()													{ mIsRunning = false; }
		bool IsRunning() const										{ return mIsRunning; }

		// set clock frequency
		void SetFrequency(double frequency);
		double GetFrequency() const									{ return mFrequency; }

		// set the initial elapsed time
		void SetStartTime(Core::Time elapsed)						{ mStartTime = elapsed; mElapsedTime = elapsed; }

		enum EClockMode
		{
			INDEPENDENT,		// clock is independently and has no reference channel
			SYNCED,				// clock is driven only by the input channel (cannot advance further than the input)
			SYNCED_AHEAD		// clock is driven independently and stays synchronized to the input channel (can advance one sample in the future)
		};

		// in manual mode the number of new ticks is not cleared on update
		void SetMode(EClockMode mode)								{ mClockMode= mode; }

		// total number of ticks since reset
		uint64 GetElapsedTicks() const								{ return mElapsedTicks; }
		
		// get or decrement the number of new ticks 
		uint32 GetNumNewTicks() const								{ return mNewTicks; }
		void DecrementNewTicks(uint32 numTicks);
		void ClearNewTicks()										{ DecrementNewTicks(GetNumNewTicks()); }

		// get new ticks (where 0 is the oldest one)
		uint64 GetTick(uint32 newTickIndex);

		// return index of the oldest tick, and remove it from the new ticks list
		uint64 PopOldestTick();

		// get the time of a tick
		Core::Time GetTickTime(uint64 tickIndex) const				{ return mStartTime + Core::Time( (tickIndex+1) / mFrequency); }

		// get the time of the last tick (does not equal elapsed time)
		Core::Time GetLastTickTime() const							{ return mElapsedTime; }

		// set clock reference (required for synced modes)
		void SetReferenceChannel(ChannelBase* channel = NULL)		{ mReferenceChannel = channel; }
		bool HasReferenceChannel() const							{ return mReferenceChannel != NULL; }
		ChannelBase* GetReferenceChannel()							{ return mReferenceChannel; }
		
	private:
		bool			mIsRunning;				// for starting/stopping the clock
		double			mFrequency;				// frequency of this clock (equals sample rate)
		EClockMode 		mClockMode;				// the mode the clock is running in

		Core::Time		mStartTime;				// the initial time of the clock
		Core::Time		mElapsedTime;			// elapsed time of the clock
		uint64			mElapsedTicks;			// the number of ticks the clock has outputted since reset (may be less than elapsed time)
		uint32			mNewTicks;				// this tracks the number of new ticks

		ChannelBase*	mReferenceChannel;		// the reference channel the clock syncs to
		
        // unused variable
        //bool			mIsInSync;				// true after the clock has synced itself to the reference channel
};


#endif
