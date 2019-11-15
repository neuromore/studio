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

#ifndef __CORE_FPSCOUNTER_H
#define __CORE_FPSCOUNTER_H

// include required headers
#include "StandardHeaders.h"
#include "Timer.h"
#include "String.h"


namespace Core
{

// performance statistics
struct ENGINE_API PerformanceStatistics
{
	// constructors
	PerformanceStatistics();
	PerformanceStatistics(double fps, double theoreticalFps, double averagedAlgorithmTime, double bestCaseTiming, double wordCaseTiming);

	void Reset();

	double	mFps;
	double	mTheoreticalFps;
	double	mAveragedAlgorithmTime;
	double	mBestCaseTiming;
	double	mWorstCaseTiming;
};


class ENGINE_API FpsCounter
{
	public:
		// constructor & destructor
		FpsCounter(double updateEvery=1.0); // default: update fps statistics every 1.0 seconds
		~FpsCounter();

		// call this before starting your algorithm
		void BeginTiming();

		void Pause();
		void UnPause();

		// call this directly after your algorithm finished
		void StopTiming();

		// fps statistics
		inline PerformanceStatistics GetStatistics() const							{ return mStatistics; }
		inline double GetFps() const												{ return mStatistics.mFps; }
		inline double GetTheoreticalFps() const										{ return mStatistics.mTheoreticalFps; }
		inline double GetAveragedTimeDelta() const									{ return mStatistics.mAveragedAlgorithmTime; }
		inline double GetBestCaseTiming() const										{ return mStatistics.mBestCaseTiming; }
		inline double GetWorstCaseTiming() const									{ return mStatistics.mWorstCaseTiming; }
		inline const char* GetText() const											{ return mText.AsChar(); }
		inline const String& GetTextString() const									{ return mText; }

	private:
		// update the fps counter and feed in the time in seconds that our timed algorithm needed
		void Update();

		void Reset();

		bool	mInitialized;
		bool	mIsPaused;
		double	mUpdateEvery;					// inverse update frequency
		String	mText;

		Timer	mTimer;
		double	mTimeElapsed;					// the total time elapsed since the last fps calculation

		Timer	mAlgorithmTimer;
		double	mAccumulatedAlgorithmTime;		// the accumulated time in seconds of the measured algorithm or calculation
		uint32	mNumAlgorithmCalls;

		double	mCurrentBestCase;
		double	mCurrentWorstCase;

		PerformanceStatistics mStatistics;
};

}

#endif
