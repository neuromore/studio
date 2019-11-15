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

#ifndef __NEUROMORE_EPOCH_H
#define __NEUROMORE_EPOCH_H

// include required headers
#include "../Config.h"
#include "ChannelBase.h"


// forward declaration
//template<class T> class Channel;
class WindowFunction;

// the sample data interval class
class ENGINE_API Epoch
{
	public:
		// constructor & destructor
		Epoch(ChannelBase* channel = NULL, uint32 length = 0);
		virtual ~Epoch();

		// set/get default window function for this sample data interval
		WindowFunction* GetWindowFunction() const									{ return mWindowFunction; }
		void SetWindowFunction(WindowFunction* windowFunction)						{ mWindowFunction = windowFunction; }

		// the channel this epoch is applied to
		void SetChannel(ChannelBase* channel)										{ mChannel = channel; }
		bool HasChannel() const														{ return (mChannel != NULL); }

		// length of the epoch, in samples
		void SetLength(uint32 length)												{ mLength = length; }
		uint32 GetLength() const													{ return mLength; }

		// epoch position inside the channel
		void SetPosition(uint32 index)												{ mPosition = index; }
		void SetPositionByOffset(uint32 offset);
		uint32 GetPosition() const													{ return mPosition; }
		
		// zero padding
		void SetZeroPaddingEnabled(bool enable)										{ mPaddingEnabled = enable; }
		uint32 GetNumPaddingSamples() const;

		// access samples in the (windowed) epoch. Sample number 0 is the oldest sample.
		uint32 GetNumSamples() const;
		double GetSample(uint32 index) const;
		
		// get last sample of epoch (newest)
		double GetLastSample() const												{ return GetSample(mLength - 1); }

		// get first sample of epoch (oldest)
		double  GetFirstSample() const												{ return GetSample(0); }

		// some often used statistics
		double Sum() const;										// sum up all elements (with applied window function, if any)
		double Product() const;									// sum up all elements (with applied window function, if any)
		double Mean() const;									// calculate the average
		double Min() const;										// find smallest element (with applied window function, if any)
		double Max() const;										// find largest element (with applied window function, if any)
		double Range() const;									// peak-to-peak range
		double SS() const;										// calculate Sum of Squares
		double RMS() const;										// calculate the RMS: sqrt( mean( sum-of-squares ) )
		double Variance() const;								// calculate variance
		double StdDev() const;									// calculate standard deviation
		double GeometricMean() const;							// calculates the geometric mean
		double HarmonicMean() const;							// calculates the harmonic mean

		// quantile stuff needs array for sorting
		double Quantile(uint32 q, uint32 n, Core::Array<double>& sortingArray) const;	// calculates the nth q-Quantile
		double Percentile(uint32 n, Core::Array<double>& sortingArray) const;			// calculates the nth Percentile
		double Median(Core::Array<double>& sortingArray) const;							// median = the first (and single) 2-quantile

	private:
		// the channel this epoch is associated with
		ChannelBase*			mChannel;

		// length of the epoch, in samples
		uint32					mLength;

		// position of the epoch (channel sample index of the last epoch sample)
		uint64					mPosition;

		// enable zero padding
		bool					mPaddingEnabled;

		// the windowfunction that is applied to the samples (NULL also means no window function)
		WindowFunction*			mWindowFunction;
};


#endif
