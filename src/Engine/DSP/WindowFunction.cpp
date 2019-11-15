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
#include "WindowFunction.h"
#include "../Core/LogManager.h"


using namespace Core;

// constructor
WindowFunction::WindowFunction()
{
	SetType(WINDOWFUNCTION_RECTANGULAR);
}


// destructor
WindowFunction::~WindowFunction()
{
}


// get the name of the given window function
const char* WindowFunction::GetName(EWindowFunction windowType)
{
	switch (windowType)
	{
		case WINDOWFUNCTION_RECTANGULAR:	{ return "Rectangular"; }
		case WINDOWFUNCTION_TRIANGULAR:		{ return "Triangular"; }
		case WINDOWFUNCTION_WELCH:			{ return "Welch"; }
		case WINDOWFUNCTION_HANN:			{ return "Hann"; }
		case WINDOWFUNCTION_HAMMING:		{ return "Hamming"; }
		case WINDOWFUNCTION_BLACKMAN:		{ return "Blackman"; }
		case WINDOWFUNCTION_NUTTALL:		{ return "Nuttall"; }
		case WINDOWFUNCTION_BLACKMANNUTTALL:{ return "Blackman-Nuttall"; }
		case WINDOWFUNCTION_BLACKMANHARRIS:	{ return "Blackman-Harris"; }
		case WINDOWFUNCTION_FLATTOP:		{ return "Flat top"; }
		case WINDOWFUNCTION_COSINE:			{ return "Cosine"; }
		case WINDOWFUNCTION_GAUSSIAN:		{ return "Gaussian"; }
		case WINDOWFUNCTION_BARTLETTHANN:	{ return "Bartlett-Hann"; }
		case WINDOWFUNCTION_HANNPOISSON:	{ return "Hann-Poisson"; }
		case WINDOWFUNCTION_LANCZOS:		{ return "Lanczos"; }

		default:							CORE_ASSERT(1==0); // function unknown
	};

	// unkown window name
	return "";
}


// get the currently used window function type
WindowFunction::EWindowFunction WindowFunction::GetType() const
{
	return mType;
}


// set the window function to use
void WindowFunction::SetType(EWindowFunction windowType)
{
	mType = windowType;
	switch (mType)
	{
		case WINDOWFUNCTION_RECTANGULAR:	{ mFunction = CalculateRectangularWindow; return; }
		case WINDOWFUNCTION_TRIANGULAR:		{ mFunction = CalculateTriangularWindow; return; }
		case WINDOWFUNCTION_WELCH:			{ mFunction = CalculateWelchWindow; return; }
		case WINDOWFUNCTION_HANN:			{ mFunction = CalculateHannWindow; return; }
		case WINDOWFUNCTION_HAMMING:		{ mFunction = CalculateHammingWindow; return; }
		case WINDOWFUNCTION_BLACKMAN:		{ mFunction = CalculateBlackmanWindow; return; }
		case WINDOWFUNCTION_NUTTALL:		{ mFunction = CalculateNuttallWindow; return; }
		case WINDOWFUNCTION_BLACKMANNUTTALL:{ mFunction = CalculateBlackmanNuttallWindow; return; }
		case WINDOWFUNCTION_BLACKMANHARRIS:	{ mFunction = CalculateBlackmanHarrisWindow; return; }
		case WINDOWFUNCTION_FLATTOP:		{ mFunction = CalculateFlatTopWindow; return; }
		case WINDOWFUNCTION_COSINE:			{ mFunction = CalculateCosineWindow; return; }
		case WINDOWFUNCTION_GAUSSIAN:		{ mFunction = CalculateGaussianWindow; return; }
		case WINDOWFUNCTION_BARTLETTHANN:	{ mFunction = CalculateBartlettHannWindow; return; }
		case WINDOWFUNCTION_HANNPOISSON:	{ mFunction = CalculateHannPoissonWindow; return; }
		case WINDOWFUNCTION_LANCZOS:		{ mFunction = CalculateLanczosWindow; return; }

		default: CORE_ASSERT(1==0);		// function unknown
	};
}

//-----------------------------------------------
// the window functions
//-----------------------------------------------

#define ZERO_OUTBOUNDS(index, numSamples)	if (index < 0.0 || index >= numSamples) { return 0.0; }

// rectangular window
double WindowFunction::CalculateRectangularWindow(double index, double numSamples)
{
	ZERO_OUTBOUNDS( index, numSamples );
	return 1.0;
}


// triangular window
double WindowFunction::CalculateTriangularWindow(double index, double numSamples)
{
	ZERO_OUTBOUNDS( index, numSamples );

	const double numerator		= index - ( (numSamples-1.0) * 0.5 );
	const double denominator	= (numSamples+1.0) * 0.5;

    return 1.0 - Math::Abs( numerator / denominator );
}


// welch window
double WindowFunction::CalculateWelchWindow(double index, double numSamples)
{
	ZERO_OUTBOUNDS( index, numSamples );

	const double numerator		= index - ( (numSamples-1.0) * 0.5 );
	const double denominator	= (numSamples+1.0) * 0.5;
	const double fraction		= numerator / denominator;

	return 1.0 - fraction * fraction;
}


// generalized hamming window
double WindowFunction::CalculateGeneralizedHammingWindow(double index, double numSamples, double alpha, double beta)
{
	ZERO_OUTBOUNDS( index, numSamples );
	return alpha - beta * cos( (2.0 * Math::pi * index) / (numSamples - 1.0) );
}


// Hann (Hanning) window (unlagged version)
double WindowFunction::CalculateHannWindow(double index, double numSamples)
{
	ZERO_OUTBOUNDS( index, numSamples );
	return CalculateGeneralizedHammingWindow( index, numSamples, 0.5, 0.5 );
}


double WindowFunction::CalculateHammingWindow(double index, double numSamples)
{
	ZERO_OUTBOUNDS( index, numSamples );
	return CalculateGeneralizedHammingWindow( index, numSamples, 0.53836, 0.46164 );
}


// Blackman window
double WindowFunction::CalculateBlackmanWindow(double index, double numSamples)
{
	ZERO_OUTBOUNDS( index, numSamples );

	//const double alpha	= 0.16;				// by common convention, as 0.16 approximates most closely to the "exact Blackman"
	//const double alpha0	= (1-alpha) * 0.5;
	//const double alpha1	= 0.5;
	//const double alpha2	= alpha * 0.5;
	const double alpha0		= 0.42659;			// exact values, these place zeros at the third and fourth sidelobes
	const double alpha1		= 0.49656;
	const double alpha2		= 0.076849;

	return alpha0 - alpha1 * cos( (2*Math::pi*index) / (numSamples-1.0) ) + alpha2 * cos( (4*Math::pi*index) / (numSamples-1.0) );
}


// generalized 3. order cosine window
double WindowFunction::CalculateGeneralizedThirdOrderCosineWindow(double index, double numSamples, double alpha0, double alpha1, double alpha2, double alpha3)
{
	ZERO_OUTBOUNDS( index, numSamples );
	return alpha0 - alpha1 * cos( (2.0*Math::pi*index) / (numSamples-1.0) ) + alpha2 * cos( (4.0*Math::pi*index) / (numSamples-1.0) ) - alpha3 * cos( (6.0*Math::pi*index) / (numSamples-1.0) );
}


// Nuttall window (continuous first derivate)
double WindowFunction::CalculateNuttallWindow(double index, double numSamples)
{
	ZERO_OUTBOUNDS( index, numSamples );

	const double alpha0	= 0.355768;
	const double alpha1	= 0.487396;
	const double alpha2	= 0.144232;
	const double alpha3	= 0.012604;

	return CalculateGeneralizedThirdOrderCosineWindow( index, numSamples, alpha0, alpha1, alpha2, alpha3 );
}


// Blackman-Nuttall window
double WindowFunction::CalculateBlackmanNuttallWindow(double index, double numSamples)
{
	ZERO_OUTBOUNDS( index, numSamples );

	const double alpha0	= 0.3635819;
	const double alpha1	= 0.4891775;
	const double alpha2	= 0.1365995;
	const double alpha3	= 0.0106411;

	return CalculateGeneralizedThirdOrderCosineWindow( index, numSamples, alpha0, alpha1, alpha2, alpha3 );
}


// Blackman-Harris window
double WindowFunction::CalculateBlackmanHarrisWindow(double index, double numSamples)
{
	ZERO_OUTBOUNDS( index, numSamples );

	const double alpha0	= 0.35875;
	const double alpha1	= 0.48829;
	const double alpha2	= 0.14128;
	const double alpha3	= 0.01168;

	return CalculateGeneralizedThirdOrderCosineWindow( index, numSamples, alpha0, alpha1, alpha2, alpha3 );
}


// Flat top window
double WindowFunction::CalculateFlatTopWindow(double index, double numSamples)
{
	ZERO_OUTBOUNDS( index, numSamples );

	const double alpha0	= 1.0;
	const double alpha1	= 1.93;
	const double alpha2	= 1.29;
	const double alpha3	= 0.388;
	const double alpha4	= 0.028;

	return alpha0 - alpha1 * cos( (2.0*Math::pi*index) / (numSamples-1.0) ) + alpha2 * cos( (4.0*Math::pi*index) / (numSamples-1.0) ) - alpha3 * cos( (6.0*Math::pi*index) / (numSamples-1.0) ) + alpha4 * cos( (8.0*Math::pi*index) / (numSamples-1.0) );
}


// Cosine window
double WindowFunction::CalculateCosineWindow(double index, double numSamples)
{
	ZERO_OUTBOUNDS( index, numSamples );
	return sin( (Math::pi*index) / (numSamples-1.0) );
}


// Gaussian window
double WindowFunction::CalculateGaussianWindow(double index, double numSamples)
{
	ZERO_OUTBOUNDS( index, numSamples );

	const double sigma			= 0.4; // the gaussian window is an adjustable window, sigma is the value to adjust
	const double numerator		= index - ( (numSamples-1.0) * 0.5 );
	const double denominator	= sigma * (numSamples-1.0) * 0.5;
	const double fraction		= numerator / denominator;

	return exp( -0.5 * (fraction*fraction) );
}


// Bartlett-Hann window
double WindowFunction::CalculateBartlettHannWindow(double index, double numSamples)
{
	ZERO_OUTBOUNDS( index, numSamples );

	const double alpha0	= 0.63;
	const double alpha1	= 0.48;
	const double alpha2	= 0.38;

    return alpha0 - alpha1 * Math::Abs( (index/(numSamples-1.0)) - 0.5 ) - alpha2 * cos( (2.0*Math::pi*index) / (numSamples-1.0) );
}


// Hann-Poisson window
double WindowFunction::CalculateHannPoissonWindow(double index, double numSamples)
{
	ZERO_OUTBOUNDS( index, numSamples );

	const double alpha	= 2.0;
	const double first	= 1.0 - cos( (2.0*Math::pi*index) / (numSamples-1.0) );
    const double second	= (-alpha * Math::Abs(numSamples-1.0-2.0*index)) / (numSamples-1.0);

	return 0.5 * first * exp(second);
}


// sinc function used for the Lanczos window
double WindowFunction::sinc(double x)
{
    if (x==0)
        return 1.0;

	return sin(Math::pi*x) / (Math::pi*x);
}


// Lanczos window
double WindowFunction::CalculateLanczosWindow(double index, double numSamples)
{
	ZERO_OUTBOUNDS( index, numSamples );

	const double fraction = (2.0*index) / (numSamples-1.0);
	return sinc( fraction - 1.0 );
}
