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

#ifndef __NEUROMORE_FILTER_H
#define __NEUROMORE_FILTER_H

// include required headers
#include "../Config.h"
#include "../Core/Array.h"
#include "../Core/LogManager.h"
#include "../Core/StandardHeaders.h"
#include "../Core/String.h"
#include "../Core/ComplexMath.h"


// the FIR/IIR Filter class
class ENGINE_API Filter 
{
	public:
		enum EFilterType
		{
			LOWPASS,
			HIGHPASS,
			BANDPASS,
			BANDSTOP,
			NUM_FILTERTYPES
		};

		enum EFilterMethod
		{
			// IIR
			BESSEL,
			BUTTERWORTH,
			CHEBYSHEV, 
			CHEBYSHEVII, 
			ELLIPTIC,

			// FIR
			HANN,
			HAMMING,
			BLACKMAN,
			SINC,
			NUM_FILTERMETHODS
		};

		static const char* GetFilterMethodName(EFilterMethod method);
		static const char* GetFilterTypeName(EFilterType type);

		// filter coefficients (normal form)
		class FilterCoefficients
		{
			public:
				// constructor & destructor
				FilterCoefficients(uint32 numZeroes=0, uint32 numPoles=0);

				void Clear();
				bool IsInitialized() const;
				void Log() const;

				// arrays of poles and zeroes
				Core::Array<double>	mPoles;
				Core::Array<double>	mZeroes;
				uint32				mNumPoles;
				uint32				mNumZeroes;

				// evaluate the response function described by the coefficients
				Core::Complex EvaluateResponse (Core::Complex z) const;

				// evaluate the transfer function described by the coefficients
				Core::Complex EvaluateTransferFunction (Core::Complex z) const;

		};
		

		// the FIR/IIR FilterSettings class
		class FilterSettings 
		{
			public:
				// constructors & destructor
				FilterSettings();
				virtual ~FilterSettings();

				void Log() const;

				// filter configuration
				double				mSampleRate;
				EFilterType			mFilterType;
				EFilterMethod		mFilterMethod;
				uint32				mFilterOrder;
				uint32				mCascadeLength;
				double				mLowCutFrequency;
				double				mHighCutFrequency;

				FilterCoefficients	mCoefficients;
				double				mGain;

				// method-specific parameters
				double				mChebyshevRipple;

				// TODO kaiser window parameters and others

		};

		// represents a transfer function that can be evaluated
		/*class TransferFunction
		{
			CORE_MEMORYOBJECTCATEGORY( FilterCoefficients, Core::CORE_DEFAULT_ALIGNMENT, MEMCATEGORY_CORE_FILTER)
		
			public:
				TransferFunction (FilterCoefficients* coeffs) : mCoefficients(coeffs) {}
				~TransferFunction() {}

			private:
				FilterCoefficients* mCoefficients;
		};*/

	public:
		// constructors & destructor
		Filter(FilterSettings* settings);
		virtual ~Filter();

		// apply the filter (one sample goes in, one sample comes out)
		double Evaluate(double input);

		// delay of the filter in number of samples
		uint32 GetGroupDelay();

		Core::Complex EvaluateTransferFunction(Core::Complex frequency);

	private:
		// configuration of this filter
		FilterSettings*			mSettings;

		// in/out delay buffers 
		Core::Array<double>		mXBuffer;	// input delay buffer
		Core::Array<double>		mYBuffer;	// output delay buffer

		// internal filter evaluate function
		double Evaluate(double input, FilterCoefficients* coeffs, double gain, double* X, double* Y);

		// buffer helper: shift buffer one step in time (upwards)
		inline void Shift (Core::Array<double>& buffer)
		{
			const uint32 numVals = buffer.Size();
			CORE_ASSERT(numVals > 0);
			for (uint32 i=numVals-1; i>0; i--)
				buffer[i] = buffer[i-1];
		}

		Core::Complex EvaluateTransferFunction(Core::Complex z, FilterCoefficients* coeffs);
};


#endif
