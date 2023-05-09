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
#include "Epoch.h"
#include "../Core/LogManager.h"
#include "Channel.h"
#include "WindowFunction.h"
#include "Spectrum.h"


using namespace Core;

// constructor
Epoch::Epoch(ChannelBase* channel, uint32 length)
{
	mChannel = channel;
	mLength = length;
	mPaddingEnabled = true;
	mPosition = 0;

	// no window function by default
	mWindowFunction = NULL;
}


// destructor
Epoch::~Epoch()
{
	delete mWindowFunction;
}


uint32 Epoch::GetNumPaddingSamples() const
{
	if (HasChannel() == false)
		return 0;

	if (mPaddingEnabled == false)
		return 0;

	// calculate number of padding samples
	const uint32 maxAvailableSamples = mChannel->GetNumSamples();
	if (mLength > maxAvailableSamples)
		return mLength - maxAvailableSamples;
	
	return 0;
}


// set the position via offset. Offset zero means, the epoch is positioned right at the end of the channel
void Epoch::SetPositionByOffset(uint32 offset) 
{
	if (HasChannel() == false)
		return;

	mPosition = mChannel->GetMaxSampleIndex() - (uint64)offset;
}


// calculate the number of samples this epoch holds
uint32 Epoch::GetNumSamples() const
{
	if (HasChannel() == false)
		return 0;

	// if padding is disabled, make sure there are enough samples available
	if (mPaddingEnabled == false)
	{
		const uint32 maxAvailableSamples = mChannel->GetNumSamples();

		// epoch does not fit inside channel
		if (mLength > maxAvailableSamples)
			return 0;

	}

	return mLength;
}


// sample accessor for double value (can apply window function)
double Epoch::GetSample(uint32 index) const
{
	if (HasChannel() == false)
		return 0;

	if (mLength == 0)
		return 0.0;

	// calculate index of the sample inside the channel
	const uint32 maxEpochIndex = mLength - 1;
	
	// incomplete epoch and padding disabled : epoch contains only zero values
	if (mPosition < maxEpochIndex && mPaddingEnabled == false)
		return 0.0;

	// invalid index: prevent underflow in next line
	if (maxEpochIndex > mPosition + index)
		return 0.0;

	const uint64 sampleIndex = mPosition + index - maxEpochIndex;

	Channel<double>* channel = mChannel->AsType<double>();

	// do zero padding
	if (channel->IsValidSample(sampleIndex) == false)
	{
		// make sure padding is explicitly enabled
		CORE_ASSERT (mPaddingEnabled == true);

		// return zero value
		return 0.0;
	}

	// get the sample value (copies the value)
	double value = channel->GetSample(sampleIndex);

	// apply window function (if any)
	if (mWindowFunction != NULL)
		value = value * mWindowFunction->Evaluate(index, mLength);

	return value;
}

//
//// sample accessor for all other values
//template<class T>
//const T& Epoch::GetSample(uint32 index) const
//{
//	// calculate index of the sample inside the channel
//	const uint32 maxEpochIndex = mLength - 1;
//	const uint32 sampleIndex = mPosition - maxEpochIndex + index;
//
//	// return the sample value
//	return mChannel->AsType<T>()->GetSample(sampleIndex);
//}

//
// epoch helpers
//

// sum up all elements
double Epoch::Sum() const
{
	double sum = 0;
	for (uint32 i=GetNumPaddingSamples(); i<mLength; ++i)
		sum += GetSample(i);
	
	return sum;
}


// multiply all elements
double Epoch::Product() const
{
	double product = 0;
	for (uint32 i = GetNumPaddingSamples(); i<mLength; ++i)
		product *= GetSample(i);

	return product;
}


//calculate the average
double Epoch::Mean() const
{
	const uint32 numValidSamples = mLength - GetNumPaddingSamples();
	if (numValidSamples == 0)
		return 0;

	return Sum() / numValidSamples;
}


// find smallest element (with applied window function, if any)
double Epoch::Min() const
{
	const uint32 numValidSamples = mLength - GetNumPaddingSamples();
	if (numValidSamples == 0)
		return 0;

	double min = DBL_MAX;
	for (uint32 i=GetNumPaddingSamples(); i<mLength; ++i)
		min = Core::Min(min, GetSample(i));
	
	return min;
}


// find largest element (with applied window function, if any)
double Epoch::Max() const
{
	const uint32 numValidSamples = mLength - GetNumPaddingSamples();
	if (numValidSamples == 0)
		return 0;

	double max = -DBL_MAX;
	for (uint32 i=GetNumPaddingSamples(); i<mLength; ++i)
		max = Core::Max(max, GetSample(i));
	
	return max;
}


// peak-to-peak range (absolute)
double Epoch::Range() const
{
	const double min = Min();
	const double max = Max();

	// return absolute range
	if (min < max)
		return max - min;
	else
		return min - max;
}


// calculate the sum of squares
double Epoch::SS() const
{
	double ss = 0;	// sum of squares
	for (uint32 i = GetNumPaddingSamples(); i < mLength; ++i)
		ss += GetSample(i) * GetSample(i);

	return ss;
}


// calculate the RMS: sqrt( mean( sum-of-squares ) )
double Epoch::RMS() const
{
	const uint32 numValidSamples = mLength - GetNumPaddingSamples();
	if (numValidSamples == 0)
		return 0;

	double ss = SS();
	const double rms = Math::SqrtD(ss / numValidSamples);

	return rms;
}


// calculate the variance
double Epoch::Variance() const
{
	const uint32 numValidSamples = mLength - GetNumPaddingSamples();
	if (numValidSamples == 0)
		return 0;

	double ssd = 0;	// sum of squared differences to mean

	// calculate mean
	double valueSum = 0;
	for (uint32 i = GetNumPaddingSamples(); i < mLength; ++i)
		valueSum += GetSample(i);
	const double mean = valueSum / (double)numValidSamples;

	// sum up the squared differences
	for (uint32 i = GetNumPaddingSamples(); i < mLength; ++i)
	{
		const double diff = mean - GetSample(i);
		ssd += diff*diff;
	}
	
	const double variance = ssd / numValidSamples;

	return variance;
}


// calculate the standard deviation
double Epoch::StdDev() const
{
	// standard deviation is the squareroot of variance
	const double variance = Variance();
	const double standardDeviation = Math::SqrtD(variance);

	return standardDeviation;
}


// n / (1/x1 + 1/x2 + ..)
double Epoch::HarmonicMean() const
{
	const uint32 numValidSamples = mLength - GetNumPaddingSamples();
	if (numValidSamples == 0)
		return 0;

	double sum = 0;
	for (uint32 i = GetNumPaddingSamples(); i<mLength; ++i)
	{
		const double value =  GetSample(i);

		// harmonic mean is 0 if one value is 0
		if (value == 0)
			return 0.0;

		sum += (1.0 / value);
	}

	// not sure if this can actually happen
	if (sum == 0)
		return  0.0;

	return numValidSamples / sum;
}

// n-th root of x1*x2*x3*...
double Epoch::GeometricMean() const
{
	const uint32 numValidSamples = mLength - GetNumPaddingSamples();
	if (numValidSamples == 0)
		return 0;

	double product = Product();

	// product must be positive
	if (product <= 0.0)
		return 0.0;

	// calculate the nth root to get geometric mean
	return Math::PowD(product, 1.0 / numValidSamples);
}


double Epoch::Quantile(uint32 q, uint32 n, Array<double>& tempArray) const
{
	// sanity check
	if (n > q)
		return 0.0;

	const uint32 numValidSamples = mLength - GetNumPaddingSamples();
	if (numValidSamples == 0)
		return 0;

	// handle special cases
	if (numValidSamples == 0)
		return 0.0;
	else if (numValidSamples == 1)
		return GetSample(mLength-1);

	// resize and copy values
	tempArray.Resize(numValidSamples);
	for (uint32 i = GetNumPaddingSamples(); i < mLength; ++i)
		tempArray[i] = GetSample(i);

	// sort values
	tempArray.Sort();

	// calculate index
	const double normedIndex = (double)n / (double)q;
	const uint32 index = (numValidSamples - 1) * normedIndex;

	CORE_ASSERT( index <= numValidSamples-1 );
	// return value
	return tempArray[index];
}


double Epoch::Percentile(uint32 n, Array<double>& tempArray) const
{
	return Quantile(100, n, tempArray);
}


double Epoch::Median(Array<double>& tempArray) const
{
	return Quantile(2, 1, tempArray);
}
