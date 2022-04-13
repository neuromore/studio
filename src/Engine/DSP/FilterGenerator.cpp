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
#include "FilterGenerator.h"


using namespace Core;

// constructor
FilterGenerator::FilterGenerator()
{
}


// destructor
FilterGenerator::~FilterGenerator()
{
}


// initialize low and highpass coefficients (bandstop and bandpass are combinations of a low and highpass)
Filter* FilterGenerator::CreateFilter(Filter::FilterSettings* settings, TransformType transformType)
{
	// Compute coefficients from specification (they are also stored in settings)
	if (settings->mCoefficients.IsInitialized() == false)
		ComputeCoefficients(settings, transformType);

	Filter* filter = new Filter(settings);
	return filter;
}


Filter::FilterCoefficients* FilterGenerator::ComputeCoefficients(Filter::FilterSettings* settings, TransformType transformType)
{
	switch (settings->mFilterMethod)
	{
		// IIR Filter
		case Filter::BUTTERWORTH:
			return ComputeCoefficientsIIR(settings, transformType);

		case Filter::BESSEL:
		case Filter::CHEBYSHEV:
		case Filter::CHEBYSHEVII:
		case Filter::ELLIPTIC:

		// FIR Filter
		case Filter::HANN:
		case Filter::HAMMING:
		case Filter::BLACKMAN:
		case Filter::SINC:
			LogInfo("Filter %s is not implemented yet", Filter::GetFilterMethodName(settings->mFilterMethod));
			//return ComputeCoefficientsFIR(settings, transformType);
			break;

		default: CORE_ASSERT(false);
	}

	return NULL;
}


// IIR filter (Bessel, Butterworth, Chebyshev)
Filter::FilterCoefficients* FilterGenerator::ComputeCoefficientsIIR(Filter::FilterSettings* settings, TransformType transformType)
{
	LogDebug("Computing coefficients for IIR Filter with following settings:");
	settings->Log();

	// automatically select matched Z-Transform for chebychev
	if (transformType == AUTOMATIC && settings->mFilterMethod == Filter::CHEBYSHEV)
		transformType = MATCHED_Z_TRANSFORM;
	else
		transformType = BILINEAR_TRANSFORM;

	LogDebug ("note: using %s", (transformType==BILINEAR_TRANSFORM?"Bilinear transform":"Matched Z-Transform") );
	
	// Filter Poles and Zeroes in complex S- and Z-Plane (leave room for the maxmimum number of Poles/Zeroes)
	const uint32 maxPN = settings->mFilterOrder * 2;
	ComplexCoefficients sPlane = ComplexCoefficients(maxPN);
	ComplexCoefficients zPlane = ComplexCoefficients(maxPN);

	//
	// Step 1: compute the filter poles on the S-Plane
	//

	ComputePrototypeLowpass(settings, sPlane);
	
	LogDebug("Completed. S-Plane looks like this:");
	sPlane.Log();

	//
	// Step 2 : prewarp cutoff frequencies if bilinear transform is used, and calculate normalized angular frequency
	//
	
	// normalized angular frequency omega 1 and 2
	double w1, w2; 
	const double alpha1 = settings->mLowCutFrequency / settings->mSampleRate;
	const double alpha2 = settings->mHighCutFrequency / settings->mSampleRate;
	
	if (transformType == BILINEAR_TRANSFORM)
	{
		w1 = Math::TanD(Math::piD * alpha1) * 2.0;
		w2 = Math::TanD(Math::piD * alpha2) * 2.0;
	}
	else
	{
		w1 = alpha1 * Math::twoPiD;
		w2 = alpha2 * Math::twoPiD;
	}

	LogDebug("Warped Alpha 1 = %.15f",w1 / 2.0 / Math::piD);
	LogDebug("Warped Alpha 2 = %.15f",w2 / 2.0 / Math::piD);

	//
	// Step 3 : Transform lowpass prototype to the wanted filter type (LP/HP/BP/BS)
	//
	LogDebug("");
	LogDebug("Step 3: Transforming lowpass prototype to required filter");

	TransformPrototypeLowpass(settings, sPlane, zPlane, w1, w2);

	LogDebug("Completed. S-Plane looks like this:");
	sPlane.Log();

	// shrink arrays (we now know the final number of poles and zeroes)
	sPlane.Shrink();


	// 
	// Step 4: transform poles and zeroes from S-Plane to Z-plane using either BLT or matched Z-Transform
	//

	LogDebug("");
	LogDebug("Step 4: Transforming from S-Plane to Z-Plane");

	// init poles on Z-Plane
	zPlane.mNumPoles = sPlane.mNumPoles;
	zPlane.mNumZeroes = sPlane.mNumZeroes;

	if (transformType == BILINEAR_TRANSFORM)
	{
		// using Bilinear Transform
		for (uint32 i = 0; i < sPlane.mNumPoles; ++i)
			zPlane.mPoles[i] = BLT(sPlane.mPoles[i]);
		for (uint32 i = 0; i < sPlane.mNumZeroes; ++i)
			zPlane.mZeroes[i] = BLT(sPlane.mZeroes[i]);

		// set all remaining zeros to -1
		while (zPlane.mNumZeroes < zPlane.mNumPoles)
			zPlane.mZeroes[zPlane.mNumZeroes++] = -1.0;
	} 
	else
	{
		// using matched Z-Transform
		for (uint32 i = 0; i < sPlane.mNumPoles; ++i)
			zPlane.mPoles[i] = ComplexMath::Exp(sPlane.mPoles[i]);
		for (uint32 i = 0; i < sPlane.mNumZeroes; ++i)
			zPlane.mZeroes[i] = ComplexMath::Exp(sPlane.mZeroes[i]);
	}

	LogDebug("Completed. Z-Plane values are:");
	zPlane.Log();
	

	// shrink arrays (we now know the final number of poles and zeroes)
	zPlane.Shrink();

	//
	// Step for Chebychev Type II: invert poles and zeroes of Chebychev Type II
	//

	if (settings->mFilterMethod  == Filter::CHEBYSHEVII)
	{
		for (uint32 i = 0; i < zPlane.mNumPoles; i++)
		{ 
			zPlane.mPoles[i]  = 1.0 / zPlane.mPoles[i];
			zPlane.mZeroes[i]  = 1.0 / zPlane.mZeroes[i];
		}
	}


	//
	// Step 5: convert Z-Plane poles and zeros from to polynomial parameters (the normal form)
	//

	// complex coefficients
	Array<Complex> topCoeffs, botCoeffs;

	ExpandPoly(zPlane.mZeroes, topCoeffs);
	ExpandPoly(zPlane.mPoles, botCoeffs);

	// calculate DC / HF / FC gain
	Complex gainDC = EvaluateResponse(topCoeffs, botCoeffs, 1.0);
	Complex gainHF = EvaluateResponse(topCoeffs, botCoeffs, -1.0);
	
	const double theta = Math::piD * (alpha1 + alpha2);  // 2PI * (a1+a2)/2.0  // center frequency
	Complex gainFC = EvaluateResponse(topCoeffs, botCoeffs, ComplexMath::ExpJ(theta));

	// last step! compute normal form coefficients
	Filter::FilterCoefficients* coeffs  = &settings->mCoefficients;// new Filter::FilterCoefficients(zPlane.mNumZeroes + 1, zPlane.mNumPoles + 1);
	
	coeffs->mNumZeroes = zPlane.mNumZeroes + 1;
	coeffs->mZeroes.Resize (coeffs->mNumZeroes);
	for (uint32 i = 0; i <= zPlane.mNumZeroes; ++i)
		coeffs->mZeroes[i] = topCoeffs[i].mReal / botCoeffs[zPlane.mNumZeroes].mReal;

	coeffs->mNumPoles = zPlane.mNumPoles + 1;
	coeffs->mPoles.Resize (zPlane.mNumPoles+1);
	for (uint32 i = 0; i <= zPlane.mNumPoles; ++i)
		coeffs->mPoles[i] = botCoeffs[i].mReal / botCoeffs[zPlane.mNumPoles].mReal;

	// select correct gain, depending on filter type
	double gain = 1.0;
	switch (settings->mFilterType)
	{
		case Filter::LOWPASS:	gain = gainDC.Norm(); break;
		case Filter::HIGHPASS:	gain = gainHF.Norm(); break;
		case Filter::BANDPASS:	gain = gainFC.Norm(); ; break;
		//case Filter::ALLPASS:   gain = gainFC.Norm(); break;
		case Filter::BANDSTOP: 
		{
			Complex gainSqrt = ComplexMath::Sqrt(gainDC * gainHF);
			gain = gainSqrt.Norm();
		} break;
		default: gain = 1.0;
	}
	settings->mGain = gain;
	LogDebug("Gain = %f", gain);

	LogDebug("done. Filter Coefficients are:");
	coeffs->Log();

	// FIN
	return coeffs;
}


void FilterGenerator::ComputePrototypeLowpass(Filter::FilterSettings* settings, ComplexCoefficients& sPlane)
{
	//
	// Butterworth
	//

	// place poles on S-Plane to get a butterworth filter (also the first step for chebyshev)
	if (settings->mFilterMethod == Filter::BUTTERWORTH || 
		settings->mFilterMethod == Filter::CHEBYSHEV   ||
		settings->mFilterMethod == Filter::CHEBYSHEVII )
	{
		LogDebug("Placing %i poles (Butterworth)", settings->mFilterOrder);
		const uint32 maxIndex = 2 * settings->mFilterOrder;
		for (uint32 i = 0; i < maxIndex ; i++)
		{ 
			double theta;

			// place poles on unit circle on the plane
			if ( (settings->mFilterOrder & 1) == true)	// odd
				theta = ( i      * Math::piD) / settings->mFilterOrder;
			else
				theta = ((i+0.5) * Math::piD) / settings->mFilterOrder;

			// pole =  e^(i*theta)
			Complex pole = ComplexMath::ExpJ(theta);	

			// all poles are located in quadrant 2 and 3 (lowpass)
			if (pole.mReal < 0.0)
			{
				sPlane.mPoles[sPlane.mNumPoles++] = pole;

				//LogDebug("%.15f, %.15f", pole.mReal, pole.mImag);
			
				const double poleDist = pole.Norm();
				if (poleDist > 1.0)
					LogWarning("pole is located outside of unit circle (%.15f from center)", poleDist);
			}
		}
	}

	//
	//  Chebyshev
	//

	// Modify Butterworth poles to get a Chebyshev Filter (see p. 136 DeFatta et al.)
	if (settings->mFilterMethod  == Filter::CHEBYSHEV || 
		settings->mFilterMethod  == Filter::CHEBYSHEVII)
	{
		LogDebug("Modifying Poles to get a Chebyshev Type 1 Filter", settings->mFilterOrder);
		CORE_ASSERT(settings->mChebyshevRipple < 0);
		double rip = Math::PowD(10.0, -settings->mChebyshevRipple / 10.0);
		double eps = Math::SqrtD(rip - 1.0);
		double y = Math::ASinhD(1.0 / eps) / (double) settings->mFilterOrder;
		CORE_ASSERT (y > 0.0);
		for (uint32 i = 0; i < sPlane.mNumPoles; i++)
		{ 
			sPlane.mPoles[i].mReal *= Math::SinhD(y);
			sPlane.mPoles[i].mImag *= Math::CoshD(y);
		}
		
	}

	//
	// Bessel
	//

	if (settings->mFilterMethod == Filter::BESSEL) 
	{
		// TODO implement after Butterworth and Chebyshev are working
		CORE_ASSERT(false);
	}

	//
	// Elliptic
	//
	
	if (settings->mFilterMethod == Filter::ELLIPTIC) 
	{
		CORE_ASSERT(false);
	}


}


void FilterGenerator::TransformPrototypeLowpass(Filter::FilterSettings* settings, ComplexCoefficients& sPlane, ComplexCoefficients& zPlane, double w1, double w2)
{
	switch (settings->mFilterType)
	{
		// Lowpass Filter
		case Filter::LOWPASS:
		{

			for (uint32 i = 0; i < sPlane.mNumPoles; ++i)
				sPlane.mPoles[i] *= w1;

			// no zeroes
			sPlane.mNumZeroes = 0;

		}	break;
	
		// Highpass Filter
		case Filter::HIGHPASS:
		{
			// invert poles
			for (uint32 i = 0; i < sPlane.mNumPoles; ++i)
				sPlane.mPoles[i] = w1 / sPlane.mPoles[i];

			// place as many zeroes as we have poles poles at (0,0)
			sPlane.mNumZeroes = sPlane.mNumPoles;
			for (uint32 i = 0; i < sPlane.mNumPoles; ++i)
				sPlane.mZeroes[i] = 0.0;

		}	break;

		// Bandpass Filter
		case Filter::BANDPASS:
		{
			// same number of zeroes as we have poles in the lowpass prototype
			const uint32 N = sPlane.mNumPoles;
			sPlane.mNumZeroes = N;

			// double the number of poles
			sPlane.mNumPoles = 2 * N;

			// to form a bandpass, the N conjugate complex poles are transformed to 2N conj. cmplx pole pairs
			const double w0 = Math::SqrtD(w1*w2);
			const double bw = w2-w1;
			for (uint32 i=0; i < N; ++i)
			{ 
				Complex hba = 0.5 * (sPlane.mPoles[i] * bw);

				// TODO VERIFY!
				Complex temp = ComplexMath::Sqrt(1.0 - ComplexMath::Pow(Complex(w0) / hba, 2));
				
				// conj. cmplx pole pairs
				sPlane.mPoles[i]   = hba * (1.0 + temp);
				sPlane.mPoles[N+i] = hba * (1.0 - temp);
			}
			
			// place N zeroes at (0,0)
			for (uint32 i = 0; i < N; ++i)
				sPlane.mZeroes[i] = 0.0;
			
		} break;

		// Bandstop Filter
		case Filter::BANDSTOP:
		{
			// 2N poles and 2N zeroes
			const uint32 N = sPlane.mNumPoles;
			sPlane.mNumZeroes = 2 * N;
			sPlane.mNumPoles = 2 * N;

			// transform poles
			const double w0 = Math::SqrtD(w1*w2);
			const double bw = w2-w1;
			for (uint32 i=0; i < N; ++i)
			{ 
				Complex hba = 0.5 * (bw / sPlane.mPoles[i]);
				
				// TODO VERIFY!
				Complex temp = ComplexMath::Sqrt(1.0 - ComplexMath::Pow(Complex(w0) / hba, 2));
				sPlane.mPoles[i]   = hba * (1.0 + temp);
				sPlane.mPoles[N+i] = hba * (1.0 - temp);
			}
			
			// 2N zeroes, located at (0, +w0) and (0,-w0)
			for (uint32 i = 0; i < N; ++i)
			{
				sPlane.mZeroes[i]   = Complex(0.0,  w0);
				sPlane.mZeroes[N+i] = Complex(0.0, -w0);
			}

		} break;
            
        default:
        {
        }
	}
}


// Window-based FIR filter design (Cosine windows, Sinc, Kaiser and others)
Filter::FilterCoefficients* FilterGenerator::ComputeCoefficientsFIR(Filter::FilterSettings* settings, TransformType transformType)
{

	// Step 1: calculate window samples
	// Step 2: choose appropriate ideal filte coefficients (HP/LP/BP/BS) and sample it
	// Step 3: apply window to the samples of step 2
	// Step 4: 
	// Step 4: apply inverse DFT to get filter coefficients


	// Step 2: calculate the frequency response of the ideal filters
	
	const uint32 N = settings->mFilterOrder;		// num coeffs
	const double M = N / 2.0;						// non-integer if N is odd
	double w1 = settings->mLowCutFrequency;			// omega 1
	double w2 = settings->mHighCutFrequency;		// omega 2

	Array<double> idealFilter(N);

	switch (settings->mFilterType)
	{
		case Filter::LOWPASS: 
			for (uint32 n = 0; n < N; ++n)
				if (n != M)
					idealFilter[n] = Math::SinD(w1*(n-M)) / (Math::pi*(n-M));
				else
					idealFilter[n] = w1 / Math::pi;
			break;
		
		case Filter::HIGHPASS: 
			for (uint32 n = 0; n < N; ++n)
				if (n != M)
					idealFilter[n] = 1.0 - w2 / Math::piD;
				else
					idealFilter[n] = -Math::SinD(w2*(n-M)) / (Math::piD*(n-M));
			break;
		
		case Filter::BANDPASS: 
			for (uint32 n = 0; n < N; ++n)
				if (n != M)
					idealFilter[n] = (Math::SinD(w2*(n-M)) - Math::SinD(w1*(n-M))) / (Math::piD*(n-M));
				else
					idealFilter[n] = (w2-w1) / Math::piD;
			break;
		
		case Filter::BANDSTOP: 
			for (uint32 n = 0; n < N; ++n)
				if (n != M)
					idealFilter[n] = (Math::SinD(w1*(n-M)) - Math::SinD(w2*(n-M))) / (Math::piD*(n-M));
				else
					idealFilter[n] = 1.0 - (w2-w1) / Math::piD;
			break;
		
		default: CORE_ASSERT(false);
	}


	Filter::FilterCoefficients* result = new Filter::FilterCoefficients();
	return result;
}


// expand complex poles or zeroes to polynomial coefficients
inline void FilterGenerator::ExpandPoly(Array<Complex>& pz, Array<Complex>& coeffs) 
{
	const uint32 numPZ = pz.Size();
			
	// n+1 coefficients
	coeffs.Resize(numPZ+1);
			
	// init coefs to 0 (except the first element, which is 1)
	coeffs[0] = Complex(1.0);
	for (uint32 i = 1; i < numPZ+1 ; ++i)
		coeffs[i] = 0.0;

	// multiply (z-pz) into the coefficients
	for (uint32 i = 0; i < numPZ; ++i)
	{
		Complex nw = -1.0 * pz[i];
		for (uint32 c = numPZ; c >= 1; c--) 
			coeffs[c] = (nw * coeffs[c]) + coeffs[c-1];
				
		coeffs[0] = nw * coeffs[0];
	}

	// sanity check: computed coeffs of z^k must be real
	for (uint32 i = 0; i < numPZ+1; ++i)
		if (Math::AbsD(coeffs[i].mImag) > 10E-10)
			LogInfo("FilterGenerator: Error while calculating normal form coefficients (poles/zeroes are not conjugate complex (%.20f)!)", coeffs[i].mImag);
}


Complex FilterGenerator::EvaluatePoly(Array<Complex>& coeffs, uint32 maxOrder, Complex z)
{
	Complex result = 0;
	for (int32 i=maxOrder-1; i >= 0; i--)
		result = (result * z) + coeffs[i];
			
	return result;
}


void FilterGenerator::ComplexCoefficients::Log() const 
{
	LogDebug("ComplexCoefficients: %i poles and %i zeroes", mNumPoles, mNumZeroes);
	String tmpstr, logstr = "";

	for (uint32 i = 0; i < mNumPoles; ++i)
	{
		tmpstr.Format("(%.10f, %.10f), ", mPoles[i].mReal, mPoles[i].mImag);
		logstr = logstr + tmpstr;
	}
	LogDebug("Poles: %s", logstr.AsChar());

	logstr = "";
	for (uint32 i = 0; i < mNumZeroes; ++i)
	{
		tmpstr.Format("(%.10f, %.10f), ", mZeroes[i].mReal, mZeroes[i].mImag);
		logstr = logstr + tmpstr;
	}
	LogDebug("Zeroes: %s", logstr.AsChar());
}


/*
// FIXME not sure yet how the cosine FIR filters should be implemented
	return NULL;

	// cosine parameters for the different cosine windows
	double alpha = 0;
	double beta = 0;
	double gamma = 0;

	switch (settings->mFilterMethod)
	{
		case Filter::HANN:
			alpha = 0.5;
			beta = -0.5;
			break;

		case Filter::HAMMING:
			alpha = 0.54;
			beta = -0.46;
			break;

		case Filter::BLACKMAN:
			alpha = 0.42;
			beta = -0.5;
			gamma = 0.08;
			break;
	}

	// create coefficients (FIR, no Zeros)
	const uint32 numPoles = settings->mFilterOrder + 1;
	Filter::FilterCoefficients* coeffs = new Filter::FilterCoefficients(numPoles);

	// Compute poles
	for (uint32 p=0; p<numPoles; ++p)
	{
		// cosine window
		const uint32 pole = alpha + 
							beta  * Math::CosD(      Math::twoPi * (double)p / (double)(numPoles - 1)) +
							gamma * Math::CosD(2.0 * Math::twoPi * (double)p / (double)(numPoles - 1));

		coeffs->mPoles[p] = pole;
	}
	*/
