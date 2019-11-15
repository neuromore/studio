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

#ifndef __NEUROMORE_SPECTRUM_H
#define __NEUROMORE_SPECTRUM_H

// include required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../Core/String.h"
#include "../Core/Color.h"
#include "../Core/ComplexMath.h"


// the frequency spectrum class
class ENGINE_API Spectrum
{
	public:
		// constructor & destructor
		Spectrum(double minFrequency = 0, double maxFrequency = 0, uint32 numBands = 0);
		Spectrum(double maxFrequency, const Core::Array<Core::Complex>& bins);
		virtual ~Spectrum()															{}
		
		void Reset();
		void Init(const Core::Array<Core::Complex>& bins);

		double GetTime() const														{ return mTime; }
		void SetTime(double time)													{ mTime = time; }

		void SetNumBins(uint32 numBins)												{ mBins.Resize(numBins); }
		uint32 GetNumBins() const													{ return mBins.Size(); }
		
		void SetMaxFrequency(double frequency)										{ mMaxFrequency = frequency; }
		double GetMaxFrequency() const												{ return mMaxFrequency; }
		
		// calc frequency of a bin and vice verca
		double CalcFrequency(uint32 binIndex) const;
		uint32 CalcBinIndex(double frequency) const;

		// converts the magnitude to dB value
		static double GetFrequencyDecibels(double value);

		double CalcDominantFrequency(double minFrequency, double maxFrequency) const;
		double CalcMaxBin(uint32 startBinIndex = 0, uint32 endBinIndex = 0) const;
		double CalcMinBin(uint32 startBinIndex = 0, uint32 endBinIndex = 0) const;

		double GetBin(uint32 index) const;
		Core::Complex GetComplexBin(uint32 index) const;

		void SetBin(uint32 index, Core::Complex complex)						{ mBins[index] = complex; }
		bool IsEmpty() const													{ return mBins.IsEmpty(); }

		uint32 CalculateMemoryUsage() const;

	private:
		Core::Array<Core::Complex>	mBins;
		double						mTime;
		double						mMaxFrequency;
};


#endif
