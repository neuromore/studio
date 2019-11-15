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

#ifndef __NEUROMORE_STATISTICSPROCESSOR_H
#define __NEUROMORE_STATISTICSPROCESSOR_H

// include required headers
#include "../Config.h"
#include "ChannelProcessor.h"


// calculates statistics like min/max/mean/median/std-devi of a channel
class ENGINE_API StatisticsProcessor : public ChannelProcessor
{
	public:
		enum { TYPE_ID = 0x0020 };

		enum EStatisticMethod
		{
			Minimum,
			Maximum,
			Range,				// max-min
			Mean,				// average
			Median,				// sort and take center element
			Variance,			// sum of squared difference of value to mean, divided by samples
			StandardDeviation,	// standard deviation, sqrt of variance
			RMS,				// quadratic mean
			Percentile,			// Percentile
			Sum,				// sum of elements
			Product,			// product of elements
			HarmonicMean,		// harmonic mean n / (1/x1 + 1/x2 + ..)
			GeometricMean,		// geometric mean (nth root of x1*x2*x3...)
			//IQR,				// inter quantile range
			NUM_STATISTICTYPES
		};

		static const char* GetStatisticMethodName(EStatisticMethod method);
		static const char* GetStatisticMethodNameShort(EStatisticMethod method);

		class StatisticsSettings : public ChannelProcessor::Settings
		{
			public:
				enum { TYPE_ID = 0x0020 };

				StatisticsSettings()							{ mSetByTime = true;  mIntervalDuration = 1; mNumSamples = 0; mMethod = Mean; mPercentile = 95; mEpochMode = ON, mEpochShift = 1; mZeroPadding = false; }
				virtual ~StatisticsSettings()					{}
			
				uint32 GetType() const override					{ return StatisticsProcessor::TYPE_ID; }

				bool				mSetByTime;				// if true numsamples will be calculated from interval duration and channel sample rate
				double				mIntervalDuration;			// length of the statistics interval, expressed in seconds
				uint32				mNumSamples;				// length of the statistics interval, expressed in samples
				EStatisticMethod	mMethod;					// the statistic that is output

				// additional parameters
				double				mPercentile;				// which percentile to output
				
				enum EEpochMode { ON, OFF, CUSTOM };
				EEpochMode		mEpochMode;
				uint32			mEpochShift;
				bool			mZeroPadding;
		};

		// constructors & destructor
		StatisticsProcessor();
		virtual ~StatisticsProcessor();

		uint32 GetType() const override							{ return TYPE_ID; }
		ChannelProcessor* Clone() override						{ StatisticsProcessor* clone = new StatisticsProcessor(); clone->Setup(mSettings); return clone; }

		// processor update and init functions
		void Init() override;
		void ReInit() override;
		void Update() override;

		// settings
		void Setup(const ChannelProcessor::Settings& settings) override;
		virtual const Settings& GetSettings() const override	{ return mSettings; }

		// properties
		uint32 GetDelay(uint32 inputPortIndex, uint32 outputPortIndex) const override;
		double GetLatency(uint32 inputPortIndex, uint32 outputPortIndex) const override		{ return mSettings.mIntervalDuration / 2.0 /* assumption: half of epoch*/; }
		double GetSampleRatio(uint32 inputPortIndex, uint32 outputPortIndex) const override;
		uint32 GetNumStartupSamples (uint32 inputPortIndex) const override;
		uint32 GetNumEpochSamples(uint32 inputPortIndex) const override						{ return mSettings.mNumSamples; }
	
	private:
		StatisticsSettings		mSettings;

		// temporary array for things like sorting
		Core::Array<double>		mTempArray;
};


#endif
