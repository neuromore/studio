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

#ifndef __NEUROMORE_FFTPROCESSOR_H
#define __NEUROMORE_FFTPROCESSOR_H

// include required headers
#include "../Config.h"
#include "ChannelProcessor.h"
#include "WindowFunction.h"
#include "FFT.h"
#include "Channel.h"


// FFT processor
class ENGINE_API FFTProcessor : public ChannelProcessor
{
	public:
		enum { TYPE_ID = 0x0016 };

		class FFTSettings : public ChannelProcessor::Settings
		{
			public:
				enum { TYPE_ID = 0x0016 };

				FFTSettings()									{ mFFTOrder = 8; mUseZeroPadding = false; mEpochMode = ON; mEpochShift = 1; }
				virtual ~FFTSettings()							{}
			
				uint32 GetType() const override					{ return FFTProcessor::TYPE_ID; }

				uint32			mFFTOrder;
				uint32			mNumFFTSamples;
				WindowFunction	mWindowFunction;
				bool			mUseZeroPadding;		// if false, incomplete epochs are zeroed out completely; otherwise it will be padded

				enum EEpochMode { ON, OFF, CUSTOM };
				EEpochMode		mEpochMode;
				uint32			mEpochShift;
		};

		// constructors & destructor
		FFTProcessor();
		virtual ~FFTProcessor();

		uint32 GetType() const override											{ return TYPE_ID; }
		ChannelProcessor* Clone() override										{ FFTProcessor* clone = new FFTProcessor(); return clone; }

		void Init() override;
		void ReInit() override;
		void Update() override;

		// settings
		void Setup(const ChannelProcessor::Settings& settings) override			{ mSettings = static_cast<const FFTSettings&>(settings); }
		const Settings& GetSettings() const	override							{ return mSettings; }
		
		void SetFFTOrder(uint32 order)											{ mSettings.mFFTOrder = order; }
		void SetEpochShift(uint32 shift)										{ mSettings.mEpochShift = shift; }
		void SetUseZeroPadding(bool enable)										{ mSettings.mUseZeroPadding = enable; }

		const WindowFunction& GetWindowFunction()								{ return mSettings.mWindowFunction; }
	
		// DSP related properties
		uint32 GetDelay(uint32 inputPortIndex, uint32 outputPortIndex) const override;
		double GetLatency(uint32 inputPortIndex, uint32 outputPortIndex) const override			{ return (mSettings.mNumFFTSamples / 2.0) / GetOutput()->GetSampleRate(); /* very coarse assumption (half epoch)*/ }
		double GetSampleRatio(uint32 inputPortIndex, uint32 outputPortIndex) const override		{ return mSettings.mEpochShift - 1; }
		uint32 GetNumEpochSamples(uint32 inputPortIndex) const override							{ return mSettings.mNumFFTSamples + mSettings.mEpochShift; }

	private:
		// processor settins
		FFTSettings			mSettings;	

		// FFT
		FFT					mFFT;
};


#endif
