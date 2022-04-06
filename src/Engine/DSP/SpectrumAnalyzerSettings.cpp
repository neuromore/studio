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
#include "SpectrumAnalyzerSettings.h"
#include "../Core/LogManager.h"


using namespace Core;

// constructor
SpectrumAnalyzerSettings::SpectrumAnalyzerSettings()
{
	// 128 bins
	SetFFTOrder(7);

	// shift window by one sample at a time
	SetNumWindowShiftSamples(8);

	// add the frequency bands
	mBands.Add( FrequencyBand( 0.5,	  3.9,	RGBA(219, 211,  42),	"Delta", "Indicating deep sleep, restfulness, and conversely excitement or agitation when delta waves are suppressed.") );
	mBands.Add( FrequencyBand( 4.0,   7.9,	RGBA(245,  80,  71),	"Theta", "Indicating deep meditative states, daydreaming and automatic tasks.") );
	mBands.Add( FrequencyBand( 8.0,	 12.0,	RGBA(237,   0, 119),	"Alpha", "Indicating relaxed alertness, restful and meditative states.") );
	mBands.Add( FrequencyBand( 13.0, 16.0,	RGBA(153,  26, 23),		"SMR",	 "Sensorimotor cortex") );
	mBands.Add( FrequencyBand(16.0,	 30.0,	RGBA(116,  23, 190),	"Beta",  "Indicating wakefulness, alertness, mental engagement and conscious processing of information.") );
	mBands.Add( FrequencyBand(32.0, 100.0,	RGBA( 23,  26, 153),	"Gamma", "Somatosensory cortex") );
}


// destructor
SpectrumAnalyzerSettings::~SpectrumAnalyzerSettings()
{
}


// set the FFT order to use
void SpectrumAnalyzerSettings::SetFFTOrder(uint32 fftOrder)
{
	// check if the order actually changed, if not skip directly
	if (mFFTSettings.mFFTOrder == fftOrder)
		return;

	mFFTSettings.mFFTOrder		= fftOrder;
	mFFTSettings.mNumFFTSamples  = Math::Pow(2, fftOrder);
}
