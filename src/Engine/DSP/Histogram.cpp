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
#include "Histogram.h"
#include "../Core/LogManager.h"


using namespace Core;

// initialize histogram
void Histogram::Init (uint32 numBins, double minValue, double maxValue)
{
	if (minValue > maxValue)
		minValue = maxValue;

	mMinValue	= minValue;
	mMaxValue	= maxValue;
	mRange		= maxValue - minValue;

	mNumBins	= numBins;
	mBinWidth	= mRange / numBins;
	
	mBins.Resize(numBins);
	Clear();
}


void Histogram::Clear()
{
	Core::MemSet( mBins.GetPtr(), 0, mBins.Size()*sizeof(uint32) );

	mNumValues = 0;
}


void Histogram::Read(double* values)
{
	// TODO convert int32->double
	//Core::MemCopy(mBins.GetPtr(), values, mBins.Size());
}


void Histogram::Write(double* values) const
{
	// TODO convert double->int32
	//Core::MemCopy(values, mBins.GetReadPtr(), mBins.Size());
}


void Histogram::AddValue(double value)
{
	if (mBins.Size() == 0)
	{
		LogError( "Histogram::AddValue(): Cannot add value %.2f. Histogram doesn't have any bins.", value );
		return;
	}

	const uint32 binIndex = CalcBinIndex(value);
	mBins[binIndex]++;

	mNumValues++;
}


void Histogram::RemoveValue(double value)
{
	const uint32 binIndex = CalcBinIndex(value);
	
	CORE_ASSERT(mBins[binIndex] != 0);

	// never allow underflow (happens in case class is misshandled)
	if (mBins[binIndex] > 0)
		mBins[binIndex]--;
	
	if (mNumValues > 0)
		mNumValues--;
}


// calculate which bin the value falls into (the save way)
uint32 Histogram::CalcBinIndex(double value) const
{
	// edge-case histogram (empty range)
	if (mRange == 0)
		return 0;

	// clamp to valid range (don't return core invalid index, the caller has to know what he asks for)
	if (value <= mMinValue)
		return 0;
	else if (value >= mMaxValue)
		return mNumBins-1;

	return (value-mMinValue) / mRange * mNumBins;
}


// find the top bin of the histogram (first non-empty bin looking from top-down)
uint32 Histogram::FindHighBin() const
{
	for (int32 i=mNumBins-1; i>=0; --i)
	{
		if (mBins[i] != 0)
			return i;
	}

	// all bins are empty: just return the top bin
	return mNumBins-1;
}

// find the bottom bin of the histogram (first non-empty bin looking from bottom-up)
uint32 Histogram::FindLowBin() const
{
	for (uint32 i=0; i<mNumBins; ++i)
	{
		if (mBins[i] != 0)
			return i;
	}

	// all bins are empty: just return the lowest bin
	return 0;
}


// find the bin with the lowest count and return it
uint32 Histogram::CalcMinCount() const
{
	uint32 result = CORE_INVALIDINDEX32;

	for (uint32 i=0; i<mNumBins; ++i)
		result = Min<uint32>( result, mBins[i] );

	if (result == CORE_INVALIDINDEX32)
		return 0;

	return result;
}


// find the bin with the highest count and return it
uint32 Histogram::CalcMaxCount() const
{
	uint32 result = 0;

	for (uint32 i=0; i<mNumBins; ++i)
		result = Max<uint32>( result, mBins[i] );

	return result;
}


void Histogram::Log()
{
	String tempString;
	LogInfo( "Histogram: [%.2f, %.2f]", mMinValue, mMaxValue );

	const uint32 maxCount = CalcMaxCount();
	const uint32 maxNumChars = 10;

	for (uint32 i=0; i<mNumBins; ++i)
	{
		// normalized count
		double normalizedCount = 0.0;
		if (maxCount > 0)
			normalizedCount = (double)mBins[i] / maxCount;

		// build the count string
		const uint32 numChars = normalizedCount * maxNumChars;
		tempString.Clear();
		for (uint32 j=0; j<numChars; ++j)
			tempString += "=";

		LogInfo("Bin #%i [%.2f, %.2f]:\t%s %i", i, GetBinMinValue(i), GetBinMaxValue(i), tempString.AsChar(), mBins[i]);
	}
}
