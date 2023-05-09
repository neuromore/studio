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
#include "FpsCounter.h"
#include "Math.h"
#include "LogManager.h"


namespace Core
{

// constructor
PerformanceStatistics::PerformanceStatistics()
{
	Reset();
}


// constructor
PerformanceStatistics::PerformanceStatistics(double fps, double theoreticalFps, double averagedAlgorithmTime, double bestCaseTiming, double wordCaseTiming)
{
	mFps					= fps;
	mTheoreticalFps			= theoreticalFps;
	mAveragedAlgorithmTime	= averagedAlgorithmTime;
	mBestCaseTiming			= bestCaseTiming;
	mWorstCaseTiming		= wordCaseTiming;
}


// reset performance statistics
void PerformanceStatistics::Reset()
{
	mFps					= 0.0;
	mTheoreticalFps			= 0.0;
	mAveragedAlgorithmTime	= 0.0;
	mBestCaseTiming			= 0.0;
	mWorstCaseTiming		= 0.0;
}


// constructor
FpsCounter::FpsCounter(double updateEvery)
{
	if (updateEvery < Math::epsilon)
		updateEvery = Math::epsilon;

	mUpdateEvery	= updateEvery;
	mInitialized	= false;
	mIsPaused		= true;
}


// destructor
FpsCounter::~FpsCounter()
{
}


void FpsCounter::Reset()
{
	mTimer.GetTimeDelta();
	mAccumulatedAlgorithmTime	= 0.0;
	mTimeElapsed				= 0.0;
	mCurrentBestCase			= FLT_MAX;
	mCurrentWorstCase			= 0.0;

	mStatistics.Reset();
}


void FpsCounter::BeginTiming()
{
	mIsPaused = false;
	mAlgorithmTimer.GetTimeDelta();
}


void FpsCounter::Pause()
{
	// in case we're already paused, return directly
	if (mIsPaused == true)
		return;

	// in case we want to pause while we have been timing before
	const double algorithmTimeDelta = mAlgorithmTimer.GetTimeDelta().InSeconds();

	// accumulate times
	mTimeElapsed				+= mTimer.GetTimeDelta().InSeconds();
	mAccumulatedAlgorithmTime	+= algorithmTimeDelta;

	mIsPaused = true;
}


void FpsCounter::UnPause()
{
	// in case we're already timing, return directly
	if (mIsPaused == false)
		return;

	// in case we've been in pause state and want to start timing again, reset the algorithm time for the paused period
	mAlgorithmTimer.GetTimeDelta();

	mIsPaused = false;
}


void FpsCounter::StopTiming()
{
	Update();
}


void FpsCounter::Update()
{
	// return directly if we're in pause state
	if (mIsPaused == true)
		return;

	// NOTE: do this at the very beginning to get the most accurate timing values
	const double algorithmTimeDelta = mAlgorithmTimer.GetTimeDelta().InSeconds();

	// initialize at first time call
	if (mInitialized == false)
	{
		Reset();
		mInitialized = true;
	}

	// accumulate times
	mTimeElapsed				+= mTimer.GetTimeDelta().InSeconds();
	mAccumulatedAlgorithmTime	+= algorithmTimeDelta;

	// adjust best and worst cases
	mCurrentBestCase	= Min<double>( mCurrentBestCase, algorithmTimeDelta );
	mCurrentWorstCase	= Max<double>( mCurrentWorstCase, algorithmTimeDelta );

	mNumAlgorithmCalls++;

	// check if it's time for an fps statistics update
	if (mTimeElapsed > mUpdateEvery && mNumAlgorithmCalls > 0)
	{
		double averagedTimeDelta			= mTimeElapsed / mNumAlgorithmCalls;
		mStatistics.mAveragedAlgorithmTime	= mAccumulatedAlgorithmTime / mNumAlgorithmCalls;
		
		// calculate real fps
		mStatistics.mFps = 0.0;
		if (averagedTimeDelta > Math::epsilon)
			mStatistics.mFps = 1.0 / averagedTimeDelta;

		// calculate theoretical fps (in case we'd only have the timed algorithm in the real-time loop without any other calculations)
		mStatistics.mTheoreticalFps = 0.0;
		if (averagedTimeDelta > Math::epsilon)
			mStatistics.mTheoreticalFps = 1.0 / mStatistics.mAveragedAlgorithmTime;

		mStatistics.mBestCaseTiming = mCurrentBestCase;
		mStatistics.mWorstCaseTiming= mCurrentWorstCase;

		mText.Format( "%.0f (%.0f) FPS - %.1fms %.1fms %.1fms", mStatistics.mFps, mStatistics.mTheoreticalFps, mStatistics.mBestCaseTiming * 1000.0, mStatistics.mAveragedAlgorithmTime * 1000.0, mStatistics.mWorstCaseTiming * 1000.0 );

		// reset
		mTimeElapsed				= 0.0;
		mAccumulatedAlgorithmTime	= 0.0;
		mCurrentBestCase			= FLT_MAX;
		mCurrentWorstCase			= 0.0;
		mNumAlgorithmCalls			= 0;
	}
}

}
