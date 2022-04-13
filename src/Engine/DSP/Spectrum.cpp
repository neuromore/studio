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
#include "Spectrum.h"
#include "../Core/LogManager.h"


using namespace Core;

// constructor 1: known number of bands
Spectrum::Spectrum(double minFrequency, double maxFrequency, uint32 numBins)
{
	CORE_ASSERT(minFrequency == 0);
	mMaxFrequency = maxFrequency;
	mTime = 0.0;
	mBins.Resize(numBins);
}


// constructor 2: data array is already on hand
Spectrum::Spectrum(double maxFrequency, const Core::Array<Complex>& bins)
{ 
	mMaxFrequency = maxFrequency; 
	mTime = 0.0; 
	Init(bins); 
}


// get the frequency intensity in decibels
double Spectrum::GetFrequencyDecibels(double value)
{
	// convert the magnitude to dB value (power quantity)
	// http://en.wikipedia.org/wiki/Decibel
	return 10.0 * log10( value * value );
}


// get the center frequency for a specific bin
double Spectrum::CalcFrequency(uint32 index) const
{
	const int numBins = GetNumBins();
	
	if (numBins == 0)
		 return 0;
	if (numBins == 1)
		 return mMaxFrequency;

	const double normalizedIndex = index / (double)(numBins-1);
	return normalizedIndex * mMaxFrequency;
}


// get the bin with a center frequency closest to the given parameter
uint32 Spectrum::CalcBinIndex(double frequency) const
{
	if (frequency <= 0)
		return 0;
	else if (frequency >= mMaxFrequency)
		return mBins.Size()-1;

	if (mMaxFrequency == 0)
		return 0;

	// FIXME we should not always assume that we have a DC bin (minFrequency = 0)
	if (GetNumBins() < 1)
		return CORE_INVALIDINDEX32;

	// index 0		 equals dc bin
	// index numBins equals mMaxFrequency   <- ??????
	const uint32 index = frequency / mMaxFrequency * (GetNumBins()-1);		// NOTE we subtract the DC bin here
	return index;
}


// finds the dominant frequency within a certain frequency range
double Spectrum::CalcDominantFrequency(double minFrequency, double maxFrequency) const
{
	uint32 dominantIndex = CORE_INVALIDINDEX32;
	double dominantMagnitude = 0.0;

	// naive version: find the bin with maximum value (but exclude 0Hz Bin which holds the DC value of the signal)
	const uint32 numBins = GetNumBins();
	for (uint32 i=1; i<numBins; ++i)
	{
		const double binFrequency = CalcFrequency(i);

		// skip the bin if the frequency is not within range
		if (binFrequency > maxFrequency || binFrequency < minFrequency)
			continue;

		const double magnitude = GetBin(i);

		// find max frequency
		if (magnitude > dominantMagnitude)
		{
			dominantIndex = i;
			dominantMagnitude = magnitude;
		}

	}

	// no max bin found (why ever) -> return 0.0;
	if (dominantIndex == CORE_INVALIDINDEX32)
		return 0.0;

	const double dominantFrequency = CalcFrequency(dominantIndex);
	return dominantFrequency;
}


double Spectrum::CalcMaxBin(uint32 startBinIndex, uint32 endBinIndex) const
{
	double result = -DBL_MAX;

	// automatically calculate maximum over the whole range if arguments are zero
	if (startBinIndex == 0 && endBinIndex == 0)
		endBinIndex = mBins.Size() - 1;

	// iterate from the start bin index to the end bin index
	for (uint32 i=startBinIndex; i<=endBinIndex; ++i)
		result = Max<double>( result, GetBin(i) );

	// no bins in range
	if (result == -DBL_MAX)
		return 0;
	else
		return result;
}


double Spectrum::CalcMinBin(uint32 startBinIndex, uint32 endBinIndex) const
{
	double result = DBL_MAX;

	// automatically calculate maximum over the whole range if arguments are zero
	if (startBinIndex == 0 && endBinIndex == 0)
		endBinIndex = mBins.Size() - 1;

	// iterate from the start bin index to the end bin index
	for (uint32 i = startBinIndex; i <= endBinIndex; ++i)
		result = Min<double>(result, GetBin(i));

	// no bins in range
	if (result == DBL_MAX)
		return 0;
	else
		return result;
}


void Spectrum::Reset()
{
	const uint32 numSamples = mBins.Size();
	Core::MemSet( mBins.GetPtr(), 0, numSamples*sizeof(Core::Complex) );
}


void Spectrum::Init(const Core::Array<Complex>& bins)
{
	mBins = bins;
}


uint32 Spectrum::CalculateMemoryUsage() const
{
	const uint32 numSamples = mBins.Size();
	const uint32 numBytes = numSamples * sizeof(Complex);
	
	return numBytes;
}

double Spectrum::GetBin(uint32 index) const
{
	// TODO get rid of this, it is uncessesary and sucks performance
	// NOTE: this is required because spectrums channels are not strictly enforcing that all samples have the same size and is handled badly inside the nodes. This will be fixed with the v2 channels.
	const uint32 numBins = mBins.Size();
	CORE_ASSERT(index < numBins);
	if (index < numBins)
		return mBins[index].Norm();
	else
		return 0;		// gracefull degradation in release: return zero if bin does not exist
}


Complex Spectrum::GetComplexBin(uint32 index) const
{
	const uint32 numBins = mBins.Size();
	CORE_ASSERT(index < numBins);
	if (index < numBins)
		return mBins[index];
	else
		return Complex(0,0);		// gracefull degradation in release: return zero if bin does not exist
}
