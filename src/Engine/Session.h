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

#ifndef __NEUROMORE_SESSION_H
#define __NEUROMORE_SESSION_H

// include required headers
#include "Config.h"
#include "Core/StandardHeaders.h"
#include "Core/EventSource.h"
#include "Core/String.h"
#include "Core/Time.h"


// the session class
class ENGINE_API Session : public Core::EventSource
{
	public:
		// constructor & destructor
		Session();
		virtual ~Session();

		void Prepare();
		void Start();
		void Stop();
		void Pause()													{ mIsPaused = true; }
		void Continue()													{ mIsPaused = false; }
		void Reset();
		
		void Update(const Core::Time& elapsed, const Core::Time& delta);

		void SetTotalTime(Core::Time time)								{ mTotalTime = time; }
		Core::Time GetTotalTime() const									{ return mTotalTime; }

		// elapsed time + paused time = total session time
		Core::Time GetElapsedTime() const								{ return mElapsedTime; }
		Core::Time GetPausedTime() const								{ return mPausedTime; }

		Core::Time GetRemainingTime() const;
		double GetProgress() const;

		bool IsPreparing() const										{ return mIsPreparing; }
		bool IsRunning() const											{ return mIsRunning; }
		bool IsPaused() const											{ return mIsPaused; }

		bool HasPoints() const											{ return mHavePoints; }
		double GetPoints() const										{ return mPoints; }
		
		const Core::Time& GetStartTime() const							{ return mStartTime; }
		const Core::Time& GetStopTime()	const							{ return mStopTime; }

	private:
		Core::Time								mStartTime;
		Core::Time								mStopTime;

		Core::Time								mTotalTime;
		Core::Time								mElapsedTime;
		Core::Time								mPausedTime;

		double									mPrepareSeconds;
		double									mPoints;
		bool									mHavePoints;

		bool									mIsPreparing;
		bool									mIsRunning;
		bool									mIsPaused;
};


#endif
