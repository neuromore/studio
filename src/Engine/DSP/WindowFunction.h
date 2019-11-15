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

#ifndef __NEUROMORE_WINDOWFUNCTION_H
#define __NEUROMORE_WINDOWFUNCTION_H

// include required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../Core/String.h"
#include "../Core/Color.h"


// the window function class
class ENGINE_API WindowFunction
{
	public:
		// available window functions
		enum EWindowFunction
		{
			WINDOWFUNCTION_RECTANGULAR			= 0,
			WINDOWFUNCTION_TRIANGULAR			= 1,
			WINDOWFUNCTION_WELCH				= 2,
			WINDOWFUNCTION_HANN					= 3,
			WINDOWFUNCTION_HAMMING				= 4,
			WINDOWFUNCTION_BLACKMAN				= 5,
			WINDOWFUNCTION_NUTTALL				= 6,
			WINDOWFUNCTION_BLACKMANNUTTALL		= 7,
			WINDOWFUNCTION_BLACKMANHARRIS		= 8,
			WINDOWFUNCTION_FLATTOP				= 9,
			WINDOWFUNCTION_COSINE				= 10,
			WINDOWFUNCTION_GAUSSIAN				= 11,
			WINDOWFUNCTION_BARTLETTHANN			= 12,
			WINDOWFUNCTION_HANNPOISSON			= 13,
			WINDOWFUNCTION_LANCZOS				= 14,
			WINDOWFUNCTION_NUMFUNCTIONS
		};

		// constructor & destructor
		WindowFunction();
		virtual ~WindowFunction();

		// helpers
		static const char* GetName(EWindowFunction windowType);
		void SetType(EWindowFunction windowType);
		EWindowFunction GetType() const;

		// main function to evaluate the window
		inline double Evaluate(double index, double numSamples)											{ return mFunction(index, numSamples); }

	private:
		// function pointer definition
		typedef double (CORE_CDECL *WindowFunctionPointer)(double index, double numSamples);

		EWindowFunction			mType;
		WindowFunctionPointer	mFunction;

		// B-spline windows
		static double CORE_CDECL CalculateRectangularWindow(double index, double numSamples);
		static double CORE_CDECL CalculateTriangularWindow(double index, double numSamples);

		// other polynomial windows
		static double CORE_CDECL CalculateWelchWindow(double index, double numSamples);

		// generalized hamming windows
		static double CalculateGeneralizedHammingWindow(double index, double numSamples, double alpha, double beta);
		static double CORE_CDECL CalculateHannWindow(double index, double numSamples);
		static double CORE_CDECL CalculateHammingWindow(double index, double numSamples);

		// higher-order generalized cosine windows
		static double CORE_CDECL CalculateBlackmanWindow(double index, double numSamples);
		static double CalculateGeneralizedThirdOrderCosineWindow(double index, double numSamples, double alpha0, double alpha1, double alpha2, double alpha3);
		static double CORE_CDECL CalculateNuttallWindow(double index, double numSamples);
		static double CORE_CDECL CalculateBlackmanNuttallWindow(double index, double numSamples);
		static double CORE_CDECL CalculateBlackmanHarrisWindow(double index, double numSamples);
		static double CORE_CDECL CalculateFlatTopWindow(double index, double numSamples);

		// power-of-cosine windows
		static double CORE_CDECL CalculateCosineWindow(double index, double numSamples);

		// adjustable windows
		static double CORE_CDECL CalculateGaussianWindow(double index, double numSamples);

		// hybrid windows
		static double CORE_CDECL CalculateBartlettHannWindow(double index, double numSamples);
		static double CORE_CDECL CalculateHannPoissonWindow(double index, double numSamples);

		// other windows
		static double sinc(double x);
		static double CORE_CDECL CalculateLanczosWindow(double index, double numSamples);
};


#endif
