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

#ifndef __NEUROMORE_SPECTRUMANALYZERSETTINGS_H
#define __NEUROMORE_SPECTRUMANALYZERSETTINGS_H

// include required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../Core/Array.h"
#include "FFTProcessor.h"
#include "Spectrum.h"
#include "WindowFunction.h"
#include "FrequencyBand.h"


// the frequency spectrum analyzer settings class
class ENGINE_API SpectrumAnalyzerSettings
{
	public:
		// constructor & destructor
		SpectrumAnalyzerSettings();
		virtual ~SpectrumAnalyzerSettings();

		// FFT Processor settings
		const FFTProcessor::FFTSettings& GetFFTSettings() const				{ return mFFTSettings; }

		// FFT order and the corresponding number of samples to use for the FFT calculations
		void SetFFTOrder(uint32 fftOrder);
		uint32 GetFFTOrder() const											{ return mFFTSettings.mFFTOrder; }
		uint32 GetNumFFTSamples() const										{ return mFFTSettings.mNumFFTSamples; }
		uint32 GetNumFFTBins() const										{ return mFFTSettings.mNumFFTSamples / 2 + 1; }

		// frequency bands
		uint32 GetNumFrequencyBands() const									{ return mBands.Size(); }
		FrequencyBand* GetFrequencyBand(uint32 index)						{ return &mBands[index]; }

		// window function 
		WindowFunction* GetWindowFunction()									{ return &mFFTSettings.mWindowFunction; }

		// window shifting
		void SetNumWindowShiftSamples(uint32 numSamples)					{ mFFTSettings.mEpochShift = numSamples; }
		uint32 GetNumWindowShiftSamples() const								{ return mFFTSettings.mEpochShift; }
		void SetWindowOverlap(double percentage);

	private:
		FFTProcessor::FFTSettings		mFFTSettings;
		Core::Array<FrequencyBand>		mBands;							// The frequency bands to calculate like the Delta, Theta, Alpha bands. */
};


#endif
