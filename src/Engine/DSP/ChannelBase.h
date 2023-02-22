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

#ifndef __NEUROMORE_CHANNELBASE_H
#define __NEUROMORE_CHANNELBASE_H

// include required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../Core/LogManager.h"
#include "../Core/String.h"
#include "../Core/Color.h"
#include "../Core/Time.h"

template <class T>
class Channel;

// channel base class used for all template instances
class ENGINE_API ChannelBase
{
	CORE_LOGTRACE_DISABLE("ChannelBase", GetName(), this)
	CORE_LOGDEBUG_DISABLE("ChannelBase", GetName(), this)

	public:
		ChannelBase(uint32 bufferSize = 0);
		virtual ~ChannelBase();
		virtual uint32 GetType() const = 0;

		template<class T> Channel<T>* AsType()									{ CORE_ASSERT(GetType() == Channel<T>::TYPE_ID); return static_cast<Channel<T>*>(this); }
		template<class T> const Channel<T>* AsType() const						{ CORE_ASSERT(GetType() == Channel<T>::TYPE_ID); return static_cast<const Channel<T>*>(this); }

		void Reset();
		
		// sample counters
		uint64 GetSampleCounter() const											{ return mSampleCounter; }

		// lowest and highest valid sample index (e.g. valid index inside circular buffer)
		uint64 GetMinSampleIndex() const;
		uint64 GetMaxSampleIndex() const;
		bool IsValidSample(uint64 sampleIndex) const;

		// number of accessible samples in the channel
		uint64 GetNumSamples() const											{ return mNumSamples; }

		// number of samples added during the last update
		uint32 GetNumNewSamples() const											{ return mNumNewSamples; }
		void BeginAddSamples()													{ mNumNewSamples = 0; }

		// is the channel empty?
		bool IsEmpty() const													{ return mSampleCounter == 0; }

		// remove all samples so IsEmpty() is true
		virtual void Clear(bool deallocate = false) = 0;

		// remove one sample from the end of the channel (decrements indices and does not dealloc anything; used for drift correction)
		void RemoveLastSample();

		// length of the channel, in seconds
		Core::Time GetDuration() const;

		// timestamp of sample, where sample 0 falls on time 0
		Core::Time GetSampleTime(uint64 sampleIndex) const;
		Core::Time GetLastSampleTime() const;

		// get sample closest to a given absolute time
		uint64 FindIndexByTime(const Core::Time& time, bool roundToClosest = false);

		// TODO deprecate this
		uint32 FindIndexByTime(double time, bool roundToClosest = false, bool clampResult = true);
		
		// start and elapsed time of the channel
		void SetElapsedTime(const Core::Time& time)								{ mElapsedTime = time; }
		Core::Time GetElapsedTime() const										{ return mElapsedTime; }
		void SetStartTime(const Core::Time& time);
		Core::Time GetStartTime() const											{ return mStartTime; }
		
		// latency calculation
		double GetLatency() const												{ return mLatency; }
		void UpdateLatency();
		
		// set a maximum size of the channel (if > 0 it behaves like a circular buffer)
		bool IsBuffer() const													{ return (mBufferSize != 0); }
		
		virtual void SetBufferSize(uint32 numSamples, bool discard = true) = 0;
		uint32 GetBufferSize() const											{ return mBufferSize; }

		virtual uint64 CalculateMemoryAllocated(bool countBuffersOnly = false) const = 0;
		virtual uint64 CalculateMemoryUsed( bool countBuffersOnly = false) const = 0;

		// is channel active (are samples added regularly?)
		bool IsActive() const;
		void UpdateActivity(double timePassedInSeconds)							{ mTimeSinceLastAddSample += timePassedInSeconds; }
		void SetAsActive()														{ mTimeSinceLastAddSample = 0; }

		// is this an independent channel?
		bool IsIndependent() const												{ return mIsIndependent; }
		void SetIndependent(bool enable = true)									{ mIsIndependent = enable; }

		// channel name (also used for EEG electrode identification)
		void SetName(const char* name)											{ mName = name; }
		const char* GetName() const												{ return mName.AsChar(); }
		Core::String& GetNameString()											{ return mName; }

		// source name for rendering and as readable identification
		void SetSourceName(const char* sourceName)								{ mSourceName = sourceName; }
		const char* GetSourceName() const										{ return mSourceName; }
		Core::String& GetSourceNameString()										{ return mSourceName; }

		// identification
		uint32 GetID() const													{ return mID; }

		// sampling rate (use 0 for signals with varying sample rate)
		void SetSampleRate(double sampleRate)									{ mSampleRate = sampleRate; }
		double GetSampleRate() const											{ return mSampleRate; }

		// _typical_ min and max values for rendering
		void SetMinValue(double minValue)										{ mMinValue = minValue; }
		double GetMinValue() const												{ return mMinValue; }
		void SetMaxValue(double maxValue)										{ mMaxValue = maxValue; }
		double GetMaxValue() const												{ return mMaxValue; }

		// physical unit (e.g. mV, Points, n-Score, etc ...)
		void SetUnit(const char* unit)											{ mUnitString = unit; }
		const char* GetUnit() const												{ return mUnitString.AsChar(); }
		const Core::String& GetUnitString() const								{ return mUnitString; }
	
		void SetColorByID(uint32 colorID)										{ mColor.SetUniqueColor(colorID); }
		void SetColor(const Core::Color& color)									{ mColor = color; }
		Core::Color& GetColor()													{ return mColor; }

		// simple flag for highlighting a channel everywhere it is used
		void SetIsHighlighted(bool enabled)										{ mIsHighlighted = enabled; }
		bool IsHighlighted() const												{ return mIsHighlighted; }

	protected:
		double		mSampleRate;						// if > 0 we assume the channel's samples have fixed sample rate
		Core::Time	mStartTime;							// time of the first sample
		Core::Time	mElapsedTime;						// elapsed time of the channel (often differs from the time of the last sample! thats the point)
		double		mLatency;							// the average latency of the channel
		uint32		mNumSamples;						// the maximum number of accessible samples in the channel
		uint32		mNumNewSamples;						// number of samples added during the last update
		uint64		mSampleCounter;						// added samples since last call of Clear();
		uint32		mBufferSize;						// the maximum number of samples this channel holds; 0 in case circular buffer is disabled
		double		mTimeSinceLastAddSample;			// activity-detection

	private:
		// channel properties
		Core::String		mName;						// name of the channel (may be empty)
		uint32				mID;						// unique generated ID
		Core::String		mSourceName;				// readable name of the source (for graph labels, mostly just "NodeName.OutputName")
		double				mMinValue;					// minimum value used for rendering graphs
		double				mMaxValue;					// maximum value used for rendering graphs
		Core::String		mUnitString;				// string that contains the physical unit, e.g. "mV"
		Core::Color			mColor;						// the color of this channel
		bool				mIsHighlighted;				// highlight flag for visually highlighting the channel everywhere it is used
		bool				mIsIndependent;				// if channel is synced to engine or running independently
};




#endif
