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

// include required files
#include "Session.h"
#include "EngineManager.h"
#include "Core/LogManager.h"


using namespace Core;

// constructor
Session::Session()
{
	mTotalTime					= 0;
	mElapsedTime				= 0.0;
	mPausedTime					= 0.0;
	mPrepareSeconds				= 0.0;
	mPoints						= 0.0;
	mHavePoints					= false;
	mIsPreparing					= false;
	mIsRunning					= false;
	mIsPaused					= false;
}


// destructor
Session::~Session()
{
}


// prepare the session
void Session::Prepare()
{
	Classifier* c = GetEngine()->GetActiveClassifier();

	if (mIsPreparing || mIsRunning || !c)
		return;

	mIsPreparing = true;

	// init delay of the classifier
	mPrepareSeconds = c->GetFloatAttribute(
		Classifier::ATTRIB_INITTIME, Classifier::DEFAULTINITTIME);

	// emit event
	EMIT_EVENT( OnPrepareSession() );
}


// start the session
void Session::Start()
{
	// is already running
	if (mIsRunning == true)
		return;

	// do not start if no classifier was specified
	Classifier* activeClassifier = GetEngine()->GetActiveClassifier();
	if (activeClassifier == NULL)
	{
		mIsRunning = false;
		return;
	}

	// reset some nodes on session start
	activeClassifier->ResetOnSessionStart();

	// capture the start time and remember it
	mStartTime = Time::Now();

	// start session
	mIsPreparing	= false;
	mIsRunning	= true;
	mIsPaused	= false;
	mPoints		= 0.0;
	mHavePoints = false;
	
	// emit event
	EMIT_EVENT( OnStartSession() );
}


// stop the session
void Session::Stop()
{
	if (mIsRunning == false)
		return;

	mIsRunning = false;
	mIsPaused = false;

	// capture the stop time and remember it
	mStopTime = Time::Now();
	
	// emit event
	EMIT_EVENT( OnStopSession() );
}


// reset the session
void Session::Reset()
{
	mElapsedTime = 0.0;
	mPoints		 = 0.0;
	mHavePoints	 = false;
	mPausedTime  = 0.0;
	mIsPreparing	= false;
	mIsRunning	 = false;
	mIsPaused	 = false;
}


// update the session
void Session::Update(const Core::Time& elapsed, const Core::Time& delta)
{
	Classifier* c = GetEngine()->GetActiveClassifier();

	if (!c)
		return;

	// flip preparing state after init delay
	if (mIsPreparing && elapsed.InSeconds() >= mPrepareSeconds)
	{
		mIsPreparing = false;
		EMIT_EVENT( OnPreparedSession() );
	}

	// only update the session in case it is running
	if (mIsRunning == false)
		return;

	if (mIsPaused == true)
		mPausedTime += delta;
	else
		mElapsedTime += delta;


	const uint32 numPointsNodes = c->GetNumPointsNodes();
	if (numPointsNodes > 0)
	{
		// sum up current total points
		mPoints = 0;
		for (uint32 i = 0; i < numPointsNodes; ++i)
			mPoints += c->GetPointsNode(i)->GetPoints();

		mHavePoints	= true;
	}
}


// get the time remaining for the session
Core::Time Session::GetRemainingTime() const
{
	// no session length specified
	if (mTotalTime == 0.0)
		return 0.0;

	if (mElapsedTime >= mTotalTime)
		return 0.0;

	return mTotalTime - mElapsedTime;
}


// get the normalized session time
double Session::GetProgress() const
{
	// no session length specified
	if (mTotalTime == 0.0)
		return 0.0;
	
	// return normalized elapsed time
	double progress = GetRemainingTime().InSeconds() / mTotalTime.InSeconds();
	progress = Core::Clamp<double>(progress, 0.0, 1.0);	
	return progress;
}
