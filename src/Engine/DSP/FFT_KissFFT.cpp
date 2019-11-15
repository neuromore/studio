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

// include required files
#include "FFT.h"
#include "../Core/LogManager.h"


#ifdef USE_KISSFFT

// constructor
FFT::FFT()
{
	mNumSamples = 0;
	mPlan = 0;

#ifdef _OPENMP
	DO NOT COMPILE WITH OPENMP ENABLED !!!
	THIS WILL COMPLETELY FUCK PERFORMANCE
#endif
}


// destructor
FFT::~FFT()
{
	kiss_fft_cleanup();
}


// initialize the buffers used for the fast fourier calculations
void FFT::Init(uint32 numSamples)
{
	// can we skip the reinitalization of the the buffers?
	if ((uint32)mNumSamples == numSamples)
		return;

	// force even number of input samples (for now.. may be changed in the future)
	CORE_ASSERT(mNumSamples % 2 == 0);

	mNumSamples = numSamples;
	
	// resize the data buffers to fit the upcoming samples
	mRealValues.Resize(mNumSamples);
	mComplexValues.Resize(mNumSamples / 2 + 1);	

	// init kiss config
	mPlan = kiss_fft_alloc(mNumSamples, 0, NULL, NULL);
	//mInversePlan = kiss_fft_alloc(mNumSamples,1,NULL,NULL);

	// init kiss fft buffer
	mComplexInput.Resize(mNumSamples);
	mComplexOutput.Resize(mNumSamples);
	for (uint32 i=0; i<mNumSamples; i++)
	{
		mComplexInput[i].mImag = 0;
		mComplexOutput[i].mImag = 0;
	}
}


void FFT::CalcFFT()
{
	// copy real input data to complex input buffer
	for (uint32 i=0; i<mNumSamples; i++)
		mComplexInput[i].mReal = mRealValues[i];
	
	// run fft
	kiss_fft( mPlan, (kiss_fft_cpx*)mComplexInput.GetPtr(), (kiss_fft_cpx*)mComplexOutput.GetPtr() );

	// copy lower half of the mirrored spectrum
	for (uint32 i=0; i<mNumSamples/2+1; i++)
		mComplexValues[i] = mComplexOutput[i];
}


void FFT::CalcInverseFFT()
{
	// 1) run inverse fft on complex input
	// 2) copy real values from output 
}

#endif
