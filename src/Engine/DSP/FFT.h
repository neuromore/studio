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

#ifndef __NEUROMORE_FASTFOURIERTRANSFORM_H
#define __NEUROMORE_FASTFOURIERTRANSFORM_H

// include required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../Core/ComplexMath.h"
#include "../Core/Array.h"


// choose the FFT library to use
//#define USE_FFTW
#define USE_KISSFFT


#ifdef USE_FFTW
#include "../../SDKs/FFTW/api/fftw3.h"
#endif

#ifdef USE_KISSFFT
#include <kissfft/kiss_fft.h>
#endif


// the fast fourier transform class
class ENGINE_API FFT
{
	public:
		// constructor & destructor
		FFT();
		virtual ~FFT();

		// initialize the fast fourier class: resize real and complex data buffers and forward as well as the inverse transform plan
		void Init(uint32 numSamples);

		// forward FFT : real input, complex output
		double* GetInput()								{ return mRealValues.GetPtr(); }
		Core::Complex* GetOutput()						{ return mComplexValues.GetPtr(); }
		void CalcFFT();

		// inverse FFT : complex input, real output
		Core::Complex* GetInverseInput()				{ return mComplexValues.GetPtr(); }
		double* GetInverseOutput()						{ return mRealValues.GetPtr(); }
		void CalcInverseFFT();

	private:
		uint32						mNumSamples;
		Core::Array<double>			mRealValues;		// input to FFT and output of Inverse FFT
		Core::Array<Core::Complex>	mComplexValues;     // output of FFT and input to Inverse FFT

#ifdef USE_FFTW
		fftw_plan					mPlan;
		fftw_plan					mInversePlan;
#endif

		
#ifdef USE_KISSFFT
		kiss_fft_cfg				mPlan;
		//kiss_fft_cfg				mInversePlan;
		Core::Array<Core::Complex>	mComplexInput;		// complex input or output buffer for FFT/IFFT
		Core::Array<Core::Complex>	mComplexOutput;		// complex input or output buffer for FFT/IFFT
#endif
};


#endif
