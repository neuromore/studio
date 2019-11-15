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

#ifndef __NEUROMORE_CHANNEL_H
#define __NEUROMORE_CHANNEL_H

// include required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../Core/String.h"
#include "../Core/Color.h"
#include "Spectrum.h"
#include "ChannelBase.h"


// the Channel class
template<class T>
class Channel : public ChannelBase
{
	public:

		enum ETypes
		{
			DOUBLE,
			SPECTRUM
		};
		
		// constructors & destructor
		Channel(double sampleRate = 0, uint32 bufferSize = 0);
		//Channel(double sampleRate = 0, const Core::String& name = "", double minValue = 0, double maxValue = 0, Core::String unitString = "", const Core::Color& color = Core::Color(0, 159.0f/255.0f, 227.0f/255.0f));
		virtual ~Channel();
		
		// type information
		static ENGINE_API const uint32 TYPE_ID;
		uint32 GetType() const override									{ return TYPE_ID; }

		// configure channel
		virtual void SetBufferSize(uint32 numSamples) override;

        uint32 CalcChunkSize() const                                    { return Core::Max<uint32>(mSampleRate * 5.0, 100); }

		// use these for adding samples (both increase the sample counter)
		void AddSample(const T& value);
		T* GetNextSampleRef();
	
		// clear channel
		virtual void Clear(bool deallocate = false) override;

		// get sampless and sample time (pointer or const ref, we need both)
		T* GetSampleRef(uint64 index);
		T* GetLastSampleRef();
		const T& GetSample(uint64 index) const;
		const T& GetLastSample() const;

		// direct memory access (no circular adressing!)
		// NOTE this only enables access to the first array chunk;
		const T& operator[](const uint64 index)							{ return mSamples[0][index]; }
		Core::Array<T>& GetRawArray()									{ return mSamples[0]; }
		void ForceUpdateSampleCounters()								{ mSampleCounter = mSamples[0].Size(); mNumSamples = mSamples.Size(); mTimeSinceLastAddSample = 0;}

		// helpers
		void CalculateAverage(T* outAverage, uint64 minSampleIndex = 0, uint64 maxSampleIndex = CORE_UINT64_MAX);
		void CalculateMaximum(T* outMaximum, uint64 minSampleIndex = 0, uint64 maxSampleIndex = CORE_UINT64_MAX);
		void CalculateMinimum(T* outMinimum, uint64 minSampleIndex = 0, uint64 maxSampleIndex = CORE_UINT64_MAX);

		static const T sZeroValue; 
		static const T sMaxValue;
		static const T sMinValue;

		// memory usage
		uint64 CalculateMemoryAllocated(bool countBuffersOnly = false) const override;
		uint64 CalculateMemoryUsed(bool countBuffersOnly = false) const override;

	protected:
		// the sample storage arrays
		Core::Array<Core::Array<T>>  mSamples;	 
};


#endif
