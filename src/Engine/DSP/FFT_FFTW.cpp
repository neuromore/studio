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
#include "FFT.h"
#include "../Core/LogManager.h"


#ifdef USE_FFTW

using namespace Core;

// constructor
FFT::FFT()
{
	mNumSamples = 0;
	mPlan = NULL;
	mInversePlan = NULL;

	//fftw_init_threads();
	//fftw_plan_with_nthreads(8);
}


// destructor
FFT::~FFT()
{
	fftw_destroy_plan(mPlan);
	fftw_destroy_plan(mInversePlan);
	//fftw_cleanup_threads();
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

	// zero input and output memory
	//Core::MemSet( mInput.GetPtr(), 0, numSamples*sizeof(double) );
	//Core::MemSet( mOutput.GetPtr(), 0, numSamples*sizeof(double) );

	// initialize our fftw plan for forward or backward fft
	fftw_destroy_plan(mPlan);
	mPlan = fftw_plan_dft_r2c_1d(mNumSamples, mRealValues.GetPtr(), (fftw_complex*)mComplexValues.GetPtr(), 0);
	fftw_destroy_plan(mInversePlan);
	mInversePlan = fftw_plan_dft_c2r_1d(mNumSamples, (fftw_complex*)mComplexValues.GetPtr(), mRealValues.GetPtr(), 0);

	//mPlan = fftw_plan_r2r_1d( mNumSamples, mInput.GetPtr(), mOutput.GetPtr(), FFTW_REDFT00, FFTW_PRESERVE_INPUT );
}


void FFT::CalcFFT()
{
	/*fftw_complex *in, *out;

	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*mNumSamples);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*mNumSamples);

	// copy input
	for (uint32 i=0; i<mNumSamples; ++i)
	{
		in[i][0] = mInput[i];
		in[i][1] = 1.0;
	}

	// initialize our fftw plan, which will store the type of FFT that we want to perform
	// parameters: dimension of the input and output arrays, input array, output array, forward or backward, optimization flag
	unsigned int flags = FFTW_ESTIMATE; // can also be FFTW_MEASURE
	mPlan = fftw_plan_dft_1d( mNumSamples, in, out, FFTW_FORWARD, flags );

	fftw_execute(mPlan);

	for (uint32 i=0; i<mNumSamples; ++i)
		mOutput[i] = out[i][0];

	// get rid of bla
	fftw_destroy_plan(my_plan);
	fftw_free(in);
	fftw_free(out);*/

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// directly using reals

	if (mPlan != NULL)
		fftw_execute(mPlan);
}


void FFT::CalcInverseFFT()
{
	if (mInversePlan != NULL)
		fftw_execute(mInversePlan);
}

#endif
