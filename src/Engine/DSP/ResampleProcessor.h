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

#ifndef __NEUROMORE_RESAMPLEPROCESSOR_H
#define __NEUROMORE_RESAMPLEPROCESSOR_H

// include required headers
#include "../Config.h"
#include "ChannelProcessor.h"
#include "ClockGenerator.h"


// takes spectrums from an input SpectrumChannel, processes them and produces spectrums at the output (via baseclass)
class ENGINE_API ResampleProcessor : public ChannelProcessor
{
	public:
		enum EResampleType
		{
			NO_RESAMPLING		,	// no resampling - just forward the samples
			NAIVE				,	// just output the last input value regularly
			UPSAMPLE_INTEGER	,	// integer ratio sample rate > 1
			UPSAMPLE_FRACTIONAL ,	// rational ratio sample rate > 1
			DOWNSAMPLE_INTEGER	,	// integer ratio sample rate < 1
			DOWNSAMPLE_FRACTIONAL,	// rational ratio sample rate < 1
		};

		enum EResampleAlgo
		{
			FORWARD				,	// no resampling - just forward the samples					both/none/fast/synced				-> used if resample ratio is 1
			OUTPUT_LAST			,	// most naive method: output the last received value		both/both/fast/independent  		-> used if input sample rate is zero
			NEAREST_NEIGHBOR	,	// naive: use the sample that is closest in time			both/both/fast/synced_ahead 		-> used for realtime up/downsampling
			FIRST_ORDER_HOLD	,	// like linear interpolate, but predicts the future			up/both/fast/synced_ahead			-> not used
			LINEAR_INTERPOLATE	,	// interpolate linear in realtime between samples			up/both/good/synced					-> used for medium	upsampling
			BOXCAR 				,   // simple average with integer-sized boxcar kernel			down/both/good/synced				-> used for medium	downsampling
			FIR					,   // Up+Fir+Down Resampling method using int ratios			both/both/best/synced				-> used for good	up/downsampling
		};

		enum EResampleMode
		{
			REALTIME			,	// the fastest (zero-delay) algorithm is chosen
			GOOD_QUALITY		,	// the algorithm with good quality (but not perfect) is chosen; induces a certain delay
			BEST_QUALITY		,	// not used right now
			MANUAL				,	// the selected algorithm is used
		};
		
		enum { TYPE_ID = 0x0028 };

		// constructors & destructor
		ResampleProcessor();
		virtual ~ResampleProcessor();

		uint32 GetType() const override											{ return TYPE_ID; }
		ChannelProcessor* Clone() override										{ ResampleProcessor* clone = new ResampleProcessor(); clone->Setup(mSettings); return clone; }

		void Init() override;
		void ReInit() override;
		void Update() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		// settings
		class Settings : public ChannelProcessor::Settings
		{
			public:
				enum { TYPE_ID = 0x0028 };

				Settings()									{}
				virtual ~Settings()							{}
			
				uint32 GetType() const override					{ return TYPE_ID; }
				
				// specified output sample rate
				double				mTargetSampleRate;

				// current resample type and algorithm
				EResampleType		mResampleType;
				EResampleAlgo	 	mResampleAlgo;
				
				// the mode (in case the resampler should select the appropriate algorithm)
				EResampleMode		mResampleMode;
				Core::Time			mStartTime;
		};

		void Setup(const ChannelProcessor::Settings& settings) override			{ mSettings = static_cast<const Settings&>(settings); }
		const Settings& GetSettings() const	override							{ return mSettings; }

		// configure processor
		void SetOutputSampleRate(double sampleRate)								{ mSettings.mTargetSampleRate = sampleRate; }
		void SetResampleMode(EResampleMode mode)								{ mSettings.mResampleMode = mode; }
		void SetResampleAlgo(EResampleAlgo algo)								{ mSettings.mResampleAlgo = algo; }
		void SetStartTime(Core::Time startTime)									{ mSettings.mStartTime = startTime; }

		// DSP related properties
		uint32 GetDelay(uint32 inputPortIndex, uint32 outputPortIndex) const override;
		double GetSampleRatio(uint32 inputPortIndex, uint32 outputPortIndex) const override;
		uint32 GetNumStartupSamples(uint32 inputPortIndex) const override;
		uint32 GetNumEpochSamples(uint32 inputPortIndex) const override;

	private:
		EResampleAlgo SelectAlgorithm(EResampleType type, EResampleMode mode);
		
		// resample algo function pointer
		typedef void (CORE_CDECL ResampleProcessor::*ResampleFunction)();

		ResampleFunction mResampleFunction;
		ResampleFunction GetResampleFunction(EResampleAlgo algo);

		// resample algos
		void CORE_CDECL DoForward();
		void CORE_CDECL DoOutputLast();
		void CORE_CDECL DoNearestNeighbor();
		void CORE_CDECL DoLinearInterpolate();
		void CORE_CDECL DoBoxcar();

		Settings			mSettings;

		// resample parameters
		double				mFactor;			// output/input sample rate ratio
		uint32				mIntFactor;			// ratio in terms of samples (number of output per input sample for upsampling, and the reverse for downsampling)
		uint32				mKernelSize;		// number of samples in the filter kernel (2*mNumSamples + 1)
		Epoch				mSamplingKernel;

		ClockGenerator		mOutputClock;
};


#endif
