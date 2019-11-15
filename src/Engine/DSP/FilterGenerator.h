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

#ifndef __NEUROMORE_FILTERGENERATOR_H
#define __NEUROMORE_FILTERGENERATOR_H

// include required headers
#include "../Config.h"
#include "../Core/LogManager.h"
#include "../Core/StandardHeaders.h"
#include "../Core/String.h"
#include "Filter.h"


// the FilterGenerator class manages and Computes Filter coefficients
class ENGINE_API FilterGenerator 
{
	public:
		// two types of transforms (Automatich uses Matched-Z for Chebyshev, Bilinear for the rest)
		enum TransformType
		{
			AUTOMATIC,
			BILINEAR_TRANSFORM,
			MATCHED_Z_TRANSFORM
		};

		// constructors & destructor
		FilterGenerator();
		virtual ~FilterGenerator();

		// create a filter from specification
		Filter* CreateFilter(Filter::FilterSettings* settings, TransformType transformType=AUTOMATIC);

		Filter::FilterCoefficients* ComputeCoefficients(Filter::FilterSettings* settings, TransformType transformType);

	private:
		// filter design internals
		
		// complex poles and zeroes
		class ComplexCoefficients
		{
		  public:
			ComplexCoefficients(uint32 maxSize)			{ mMaxSize = maxSize; mNumPoles = 0; mNumZeroes = 0; Resize(); }
			inline void Resize()						{ mPoles.Resize(mMaxSize); mZeroes.Resize(mMaxSize); }
			inline void Shrink()						{ mPoles.Resize(mNumPoles); mZeroes.Resize(mNumZeroes); }
			Core::Array<Core::Complex> mPoles;
			Core::Array<Core::Complex> mZeroes;
			uint32 mMaxSize;
			uint32 mNumPoles;
			uint32 mNumZeroes;

			void Log() const;
		};

		// Compute IIR filter (Bessel, Butterwort, Chebyshev)
		Filter::FilterCoefficients* ComputeCoefficientsIIR (Filter::FilterSettings* settings, TransformType transformType);
		void ComputePrototypeLowpass(Filter::FilterSettings* settings, ComplexCoefficients& sPlane);
		void TransformPrototypeLowpass(Filter::FilterSettings* settings, ComplexCoefficients& sPlane, ComplexCoefficients& zPlane, double w1, double w2);

		// Compute FIR filter (Cosine (Hann/Hamming/Blackman) or Windowed sinc)
		Filter::FilterCoefficients* ComputeCoefficientsFIR (Filter::FilterSettings* settings, TransformType transformType);

		// helper functions

		// bilinear transform
		inline Core::Complex BLT (Core::Complex s) const			{ return (2.0 + s) / ( 2.0 - s); }
		
		// expand complex poles or zeroes to polynomial coefficients
		void ExpandPoly (Core::Array<Core::Complex>& pz, Core::Array<Core::Complex>& coeffs) ;
		
		Core::Complex EvaluatePoly (Core::Array<Core::Complex>& coeffs, uint32 maxOrder, Core::Complex z);

		// evaluate the response function described by the complex coefficients
		inline Core::Complex EvaluateResponse (Core::Array<Core::Complex>& top, Core::Array<Core::Complex>& bot, Core::Complex z)		{ return EvaluatePoly(top, top.Size(), z) / EvaluatePoly(bot, bot.Size(), z); }
};


#endif
