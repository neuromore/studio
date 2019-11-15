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

#ifndef __NEUROMORE_HISTOGRAM_H
#define __NEUROMORE_HISTOGRAM_H

// include required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../Core/String.h"
#include "../Core/Color.h"
#include "../Core/ComplexMath.h"


// the Histogram class
class ENGINE_API Histogram
{
	public:
		// constructor & destructor
		Histogram() : mNumBins(0), mBinWidth(0), mNumValues(0), mMinValue(0), mMaxValue(0), mRange(0) 	{}
		Histogram(uint32 numBins, double minValue, double maxValue)										{ Init(minValue, maxValue, numBins); }
		~Histogram()																					{}

		void Init (uint32 numBins, double minValue, double maxValue);
		
		// writing/reading from channels
		void Read(double* binValues);
		void Write(double* binValues) const;

		// clear histogram (zero all bins)
		void Clear();

		void Log();

		// add/remove samples to/from histogram
		void AddValue(double value);
		void RemoveValue(double value);
		uint32 GetNumValues() const									{ return mNumValues; }

		uint32 GetNumBins() const									{ return mNumBins; }
		uint32 GetBin(uint32 index) const							{ return mBins[index]; }
		void SetBin(uint32 index, uint32 value)						{ mBins[index] = value; }
		double GetBinWidth() const									{ return mBinWidth; }
		
		double GetMinValue() const									{ return mMinValue; }
		double GetMaxValue() const									{ return mMaxValue; }
	
		double GetBinMinValue(uint32 index) const					{ return mMinValue + index * mBinWidth; }
		double GetBinMaxValue(uint32 index) const					{ return mMinValue + (index + 1) * mBinWidth; }
		double GetBinCenterValue(uint32 index) const				{ return mMinValue + index * mBinWidth * 0.5; }
	
		uint32 CalcBinIndex(double value) const;

		uint32 FindHighBin() const;
		uint32 FindLowBin() const;

		uint32 CalcMinCount() const;
		uint32 CalcMaxCount() const;

	private:
		Core::Array<uint32>			mBins;			// histogram bins	// Note: we should use double instead of integer due to our channel design. Remember: int32 can be represented exactly with doubles, that should be enough :)
		uint32						mNumBins;		// number of bins 
		double						mBinWidth;		// width of a bin (in value-space)

		uint32						mNumValues;		// total number of samples currently in the histogram

		double						mMinValue;		// = min value of bin 0
		double						mMaxValue;		// = max value of bin mNumBins-1
		double						mRange;			// = max - min
};


#endif
