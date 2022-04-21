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
#include "Filter.h"


using namespace Core;

// constructor
Filter::Filter(FilterSettings* settings)
{
	mSettings = settings;

	const uint32 numInputSamples = mSettings->mCoefficients.mNumPoles;
	const uint32 numOutputSamples = mSettings->mCoefficients.mNumZeroes;

	mXBuffer.Resize(numInputSamples);
	mYBuffer.Resize(numOutputSamples);

	MemSet(mXBuffer.GetPtr(), 0, numInputSamples*sizeof(double));
	MemSet(mYBuffer.GetPtr(), 0, numOutputSamples*sizeof(double));
}


// destructor
Filter::~Filter()
{
}


// apply filter
double Filter::Evaluate(double input)
{
	// shift input and output buffer
	Shift (mXBuffer);
	Shift (mYBuffer);

	return Evaluate(input, &mSettings->mCoefficients, mSettings->mGain, mXBuffer.GetPtr(), mYBuffer.GetPtr());
}


// calculates: y(n) = zero(n)*x(n) + zero(n-1)*x[n-1] + ... + pole(n-1) * y(n-1) + pole(n-2) * y(n-2) + ...
double Filter::Evaluate(double input, FilterCoefficients* coeffs, double gain, double* X, double* Y)
{
	// add new input value
	X[0] = input / gain;

	// sum of feed-forward terms
	double sumX = 0;
	for (uint32 i = 0; i < coeffs->mNumZeroes; ++i)
		sumX += coeffs->mZeroes[i] * X[coeffs->mNumZeroes-1-i];

	// sum of feed-backward terms
	double sumY = 0;
	for (uint32 i = 0; i < coeffs->mNumPoles-1; ++i)		// highest coefficient does not appear on right side of reccurance equation
		sumY += coeffs->mPoles[i] * Y[coeffs->mNumPoles-1-i];

	// result
	const double result = sumX - sumY;

	// remember new output value
	Y[0] = result;

	return result;
}


// Transfer Function H(z) = Y(z) / X(z)
Complex Filter::EvaluateTransferFunction(Complex z)
{
	return mSettings->mCoefficients.EvaluateTransferFunction(z);
}


const char* Filter::GetFilterMethodName(EFilterMethod method)
{
	switch (method)
	{
		case BESSEL: return "Bessel";
		case BUTTERWORTH: return "Butterworth";
		case CHEBYSHEV: return "Chebyshev I";
		case CHEBYSHEVII: return "Chebyshev II";
		case ELLIPTIC: return "Elliptic";
		case HANN: return "Hann";
		case HAMMING: return "Hamming";
		case BLACKMAN: return "Blackman";
		case SINC: return "Sinc";
		default: return "Unknown";
	}
}


const char* Filter::GetFilterTypeName(EFilterType type)
{
	switch (type)
	{
		case LOWPASS: return "Lowpass";
		case HIGHPASS: return "Highpass";
		case BANDPASS: return "Bandpass";
		case BANDSTOP: return "Bandstop";
		default: return "Unknown";
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// constructor
Filter::FilterCoefficients::FilterCoefficients(uint32 numZeroes, uint32 numPoles)
{
    mNumPoles = numPoles;
    mNumZeroes = numZeroes;
    
	// allocate coefficient arrays
	mZeroes.Resize(mNumZeroes);
	mPoles.Resize(mNumPoles);
}


// evaluate the response function described by the coefficients
Complex Filter::FilterCoefficients::EvaluateResponse(Complex z) const
{
	// numerator / denominator sums
	Complex top = 0, bot = 0;

	// fast evaluation
	for (int32 i=mNumZeroes; i >= 0; i--)
		top = top +(top * z) + mZeroes[i];

	for (int32 i=mNumPoles; i>=0; i--)
		bot = bot + (bot * z) + mPoles[i];

	return top / bot;
}


// evaluate the transfer function described by the coefficients
Complex Filter::FilterCoefficients::EvaluateTransferFunction(Complex z) const
{
	// poles
	Complex sumX = 0;
	if (mNumPoles > 0)		// IIR Filter
		for (uint32 p = 0; p < mNumPoles; ++p)
			sumX = sumX + mPoles[p] *  (1.0 / ComplexMath::Pow(z,p));
	else					// FIR Filter
		sumX = 1.0;
					
	// zeroes
	Complex sumY = 0;
	for (uint32 n = 0; n < mNumZeroes; ++n)
		sumY = mZeroes[n] * (1.0 / ComplexMath::Pow(z,n));

	// result
	return sumY / sumX;
}


void Filter::FilterCoefficients::Clear() 
{
	mPoles.Clear();
	mZeroes.Clear();
	mNumPoles = 0;
	mNumZeroes = 0;
}


bool Filter::FilterCoefficients::IsInitialized() const
{
	return !(mNumPoles == 0 && mNumZeroes == 0);
}


void Filter::FilterCoefficients::Log() const 
{
	LogDebug("FilterCoefficients: %i poles and %i zeroes", mNumPoles, mNumZeroes);
	String tmpstr, logstr = "";

	for (uint32 i = 0; i < mNumPoles; ++i)
	{
		tmpstr.Format("%.12f, ", mPoles[i]);
		logstr = logstr + tmpstr;
	}
	LogDebug("Poles: %s", logstr.AsChar());

	logstr = "";
	for (uint32 i = 0; i < mNumZeroes; ++i)
	{
		tmpstr.Format("%.12f, ", mZeroes[i]);
		logstr = logstr + tmpstr;
	}
	LogDebug("Zeroes: %s", logstr.AsChar());			
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// constructor
Filter::FilterSettings::FilterSettings()
{
	mSampleRate			= 0; 
	mFilterType			= Filter::LOWPASS;
	mFilterMethod		= Filter::BUTTERWORTH;
	mFilterOrder		= 3;
	mCascadeLength		= 1;
	mLowCutFrequency	= 7;
	mHighCutFrequency	= 12;
	mGain				= -1;
	mChebyshevRipple	= -10;
}


// destructor
Filter::FilterSettings::~FilterSettings()
{
}


// log
void Filter::FilterSettings::Log() const 
{
	String typeName;

	switch (mFilterType)
	{
		case Filter::LOWPASS:  typeName = "LP"; break;
		case Filter::HIGHPASS: typeName = "HP"; break;
		case Filter::BANDPASS: typeName = "BP"; break;
		case Filter::BANDSTOP: typeName = "BS"; break;
        default:                                break;
	}

	LogDebug( "FilterSettings: Type %i (%s) of order %i with low=%.2f and high=%.2f using a sample rate of %.2f", (uint32)mFilterType, typeName.AsChar(), mFilterOrder, mLowCutFrequency, mHighCutFrequency, mSampleRate );
}
