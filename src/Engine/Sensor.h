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

#ifndef __NEUROMORE_SENSOR_H
#define __NEUROMORE_SENSOR_H

// include required headers
#include "Config.h"
#include "Core/StandardHeaders.h"
#include "Core/String.h"
#include "Core/Color.h"
#include "Core/Mutex.h"
#include "DSP/Channel.h"
#include "DSP/ResampleProcessor.h"


// forward declaration
class Device;

// the sensor class
class ENGINE_API Sensor
{
	public:
		Sensor();
		Sensor(const Sensor& sensor); // copy constructor
		Sensor(const char* name, double sampleRate);
		Sensor(const char* name, double sampleRateOut, double sampleRateIn);
		virtual ~Sensor();

		// main update function
		void Update(const Core::Time& elapsed, const Core::Time& delta);

		// reset sensor
		void Reset();

		// synchronize the sensor so the next sample that is received falls on the given relative time
		void Sync(const Core::Time& time, bool usePadding = true);

		// set sensor start time (required for setting resampler clock start time)
		void SetStartTime(const Core::Time& seconds);

		void SetEnabled(bool enable = true)										{ mIsEnabled = enable;}
		bool IsEnabled() const													{ return mIsEnabled;}

		// input sample queue
		void AddQueuedSample(double value);
		uint32 GetNumQueuedSamples() const										{ return mQueuedSamples.Size(); }

		// the output channel
		Channel<double>* GetOutput()											{ return mResampler.GetOutput()->AsType<double>(); }
		Channel<double>* GetOutput() const										{ return mResampler.GetOutput()->AsType<double>(); }
		// legacy
		Channel<double>* GetChannel()											{ return GetOutput(); }

		// the input channel
		Channel<double>* GetInput()												{ return &mInputChannel; }

		//
		// Config
		//
		
		// naming helpers
		void SetName(const char* name)											{ GetOutput()->SetName(name); GetInput()->SetName(name); }
		const char* GetName() const												{ return GetOutput()->GetName(); }
		const Core::String& GetNameString() const								{ return GetOutput()->GetNameString(); }

		// identification
		uint32 GetID() const													{ return mID; }

		// drift correction
		void SetDriftCorrectionEnabled(bool enable = true)						{ mUseDriftCorrection = enable; }
		bool GetDriftCorrectionEnabled() const									{ return mUseDriftCorrection; }
		int32 CalculateDrift() const;

		//
		// Contact Quality 
		//

		enum EContactQuality
		{
			CONTACTQUALITY_NOT_AVAILABLE = 0,
			CONTACTQUALITY_NO_SIGNAL = 1,
			CONTACTQUALITY_VERY_BAD = 2,
			CONTACTQUALITY_POOR = 3,
			CONTACTQUALITY_FAIR = 4,
			CONTACTQUALITY_GOOD = 5,
		};

		bool HasContactQuality() const											{ return mContactQuality != CONTACTQUALITY_NOT_AVAILABLE; }
		void SetContactQuality(EContactQuality contactQuality)					{ mContactQuality = contactQuality; }
		EContactQuality GetContactQuality() const								{ return mContactQuality; }
		const char* GetContactQualityAsString() const;
		static const char* GetContactQualityDescription(EContactQuality quality);
		Core::Color GetContactQualityColor() const;

		// set output sample rate (doesn't change input rate)
		double GetSampleRate() const											{ return mSampleRate; }
		void SetSampleRate(double sampleRate);

		// measured input sample rate 
		double GetRealSampleRate() const										{ return mRealSampleRate; }

		// handle lost samples (from lost packets or similar) by adding dummy samples
		void HandleLostSamples(uint32 numLostSamples);
		uint32 GetNumLostSamples() const										{ return mNumLostSamples; }

		// the current sensor latency in seconds
		double GetLatency()	const												{ return mLatency; }
		void SetLatency(double latency)											{ mLatency = latency; }
		
		// the sensors current tolerated jitter
		double GetExpectedJitter() const;
		void SetExpectedJitter(double seconds)									{ mExpectedJitter = seconds; }
		
		// burst rate calculation
		uint32 GetLastBurstSize();
		uint32 FindMaxBurstSize() const;
		double CalculateAverageBurstSize() const;

		uint32 GetNumDriftSamplesAdded() const									{ return mNumDriftSamplesAdded; }
		uint32 GetNumDriftSamplesRemoved() const								{ return mNumDriftSamplesRemoved; }


	protected:
		Channel<double>			mInputChannel;		// the raw sensor data (before drift correction and upsampling)

		double					mSampleRate;		// the sensors sample rate (rate of the output channel)
		ResampleProcessor		mResampler;			// the resampler

		uint32					mID;

		double					mLatency;			// sample latency in seconds
		double					mExpectedJitter;	// jitter in seconds that will be tolerated before starting drift correction

		EContactQuality			mContactQuality;	// quality of the sensor signal
		bool					mHasContactQuality;	

		bool					mIsEnabled;			// enable/disable sensor (device definitions are static, but we can skip sensors if we want to, using the device config files)

	private:

		// the input sample queue
		Core::Array<double> mQueuedSamples;			
		Core::Mutex			mQueuedSamplesLock;		// lock for queued samples queue
	
		void FeedQueuedSamples();					// push queued samples into channel
		void ClearQueuedSamples();					// remove all queued samples

		// drift correction 
		void CorrectForDrift();						// performs the drift correction 
		bool		mUseDriftCorrection;

		// sensor input stats
		double		mRealSampleRate;				// the actual sample rate of the input stream that goes into the sensor
		uint32		mNumDriftSamplesAdded;			// samples added to correct drift
		uint32		mNumDriftSamplesRemoved;		// samples removed to correct drift
		uint32		mNumLostSamples;

		// array of burst sizes of the last Update() calls for max burst delay calculation
		Core::Array<uint32> mBursts;
};


#endif
