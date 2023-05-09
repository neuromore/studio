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
#include "Channel.h"
#include "../Core/Time.h"


using namespace Core;
		
// constructor
template<class T>
Channel<T>::Channel(double sampleRate, uint32 bufferSize) : ChannelBase(bufferSize)
{
	SetSampleRate (sampleRate);

	// initialize sample buffer
	mSamples.AddEmpty();	// start with one chunk

	mBufferSize = 0;
	SetBufferSize(bufferSize);

	mNumSamples	= 0;
	mNumNewSamples = 0;
	mSampleCounter  = 0;
	mTimeSinceLastAddSample = 100; // marks channel as inactive
}
//
//
//// Deprecated!! constructor
//template<class T>
//Channel<T>::Channel(double sampleRate, const String& name, double minValue, double maxValue, String unitString, const Color& color)
//{
//	SetSampleRate(sampleRate);
//	SetName(name);
//	SetMinValue(minValue);
//	SetMaxValue(maxValue);
//	SetUnit(unitString);
//	SetColor(color);
//
//	// initialize sample buffer
//	mSamples.Resize(1);	// start with one chunk
//
//	mBufferSize = 0;
//	mSamples[0].Resize(Max<uint32>(sampleRate * 10.0, 100));
//
//	mNumSamples	= 0;
//	mNumNewSamples = 0;
//	mSampleCounter = 0;
//	mTimeSinceLastAddSample = 100; // marks channel as inactive
//}


// destructor
template<class T>
Channel<T>::~Channel()
{
}


// reconfigure the buffer size
template<class T>
void Channel<T>::SetBufferSize(uint32 numSamples, bool discard)
{
	LogTrace("SetBufferSize");

	// resize only if the channel is configured as a buffer - and only shrink it, if samples do not get lost
	// storage channel: initialize first chunk (size = 1 minute and no less than 100)
	if (numSamples == 0)
	{
		const uint32 chunkSize = CalcChunkSize();	
		mSamples[0].Resize(chunkSize);
	}
	else 
	{
		// buffer channel: use first 'chunk' as the buffers
		if (numSamples > mSamples[0].Size())
		{
			LogDebug("resizing sample array from %i to %i", mSamples[0].Size(), numSamples);
			mSamples[0].Resize(numSamples);
		}
	}

	mBufferSize = numSamples;
	if (discard)
		Clear();
}


// copy and add a sample to the Channel (do not use this if channel is a buffer)
template<class T>
void Channel<T>::AddSample(const T& value)
{
	LogTraceRT("AddSample");

	T* sampleRef = GetNextSampleRef();

	// copy value to memory location
	(*sampleRef) = value;
}


// copy and add a sample to the Channel (do not use this if channel is a buffer)
template<>
void Channel<double>::AddSample(const double& value)
{
	LogTraceRT("AddSample");
	LogDebugRT("adding sample value %f", value);

	double* sampleRef = GetNextSampleRef();
	

	// copy value to memory location
	(*sampleRef) = value;
}


// clear the Channel 
template<class T>
void Channel<T>::Clear(bool deallocate)
{
	LogTrace("Clear");

	// dealloc samples only if channel is not a buffer
	if (IsBuffer() == false)
	{
		LogDebug("clearing sample array (%i chunks)", mSamples.Size());
		mSamples.Clear();
		mSamples.AddEmpty();
		const uint32 chunkSize = CalcChunkSize();;	
		mSamples[0].Resize(chunkSize);
	}

	mNumSamples	= 0;
	mNumNewSamples = 0;
	mSampleCounter = 0;
	mTimeSinceLastAddSample = 100; // marks channel as inactive
}


// TODO optimize this method (get rid of checks and increse the buffer size elsewhere)
// the central code for adding samples: get a reference to a sample (dont forget do write to it! :P )
template<class T>
T* Channel<T>::GetNextSampleRef()	
{ 
	// grow storage channel by adding chunks
	if (IsBuffer() == false)
	{
		const uint32 chunkSize = mSamples[0].Size();
		const uint64 currentMaxNumSamples = chunkSize * mSamples.Size();

		if (currentMaxNumSamples == mSampleCounter)
		{
			// add another chunk
			mSamples.AddEmpty();
			mSamples.GetLast().Resize(chunkSize);
			LogDebug("added chunk %i (size = %i)", mSamples.Size(), chunkSize);
		}
	}

	// NOTE: increase counter to 'create' the sample before we can access it 
	mNumNewSamples++;
	mSampleCounter++;

	// number of available samples stops increasing when buffer is full
	if (IsBuffer() == false || mNumSamples < mBufferSize)
		mNumSamples++;

	// get reference to new sample 
	T* sampleRef = GetSampleRef(mSampleCounter-1);

	// mark channel  as active
	SetAsActive();

	return sampleRef;
}


// TODO optimize this - a channel never changes its behaviour, its either a buffer or a storage - we surely can get rid of the branching here
template<class T>
const T& Channel<T>::GetSample(uint64 index) const
{
	if (IsBuffer() == true)
	{
		// make sure the buffer actually has this sample available
	#ifdef CORE_DEBUG
		CORE_ASSERT(index <= mSampleCounter - 1);
		CORE_ASSERT(index >= mSampleCounter - mNumSamples);
	#endif

		// calculate index of sample in circular buffer
		const uint32 arrIndex = index % mBufferSize;
		
		LogDebugRT("accessing sample reference %i (array index %i)", index, arrIndex);

		return mSamples[0][arrIndex];
	}
	else
	{
		const uint64 chunkSize = mSamples[0].Size();
		const uint64 currentMaxNumSamples = chunkSize * mSamples.Size();

		// make sure that sample is contained in array
		CORE_ASSERT(index < currentMaxNumSamples);
	
		const uint64 chunkIndex = index / chunkSize;
		const uint64 sampleIndex = index % chunkSize;

		//LogDebugRT("accessing storage sample %i (chunk %i, index %i)", index, chunkIndex, sampleIndex);

		return mSamples[chunkIndex][sampleIndex];
	}
}


// access samples by const ref
template<class T>
T* Channel<T>::GetSampleRef(uint64 index)
{
	return const_cast<T*>(&GetSample(index));
}


// access last sample by const ref
template<class T>
const T& Channel<T>::GetLastSample() const
{ 
	// no samples in channel -> we have a problem!
	//CORE_ASSERT(mSampleCounter > 0);
	if (mSampleCounter == 0)
		return sZeroValue;

	return GetSample(mSampleCounter-1);
}


// acces last sample as pointer
template<class T>
T* Channel<T>::GetLastSampleRef()
{
	// no samples in channel -> we have a problem!
	CORE_ASSERT(mSampleCounter > 0);

	return GetSampleRef(mSampleCounter - 1);
}




// helpers
template<class T>
void Channel<T>::CalculateAverage(T* outAverage, uint64 minSampleIndex, uint64 maxSampleIndex)
{
	// make sure we have at least one sample
	if (GetNumSamples() == 0)
		return;

	// in case no or bad arguments were specified
	if (minSampleIndex < GetMinSampleIndex())
		minSampleIndex = GetMinSampleIndex();
	if (maxSampleIndex > GetMaxSampleIndex())
		maxSampleIndex = GetMaxSampleIndex();

	if (mSampleCounter == 0)
		return;
	
	T average = sZeroValue;
	for (uint32 i = minSampleIndex; i <= maxSampleIndex; ++i)		// less OR equal because its an index
		average += GetSample(i);

	const uint32 numSamples = maxSampleIndex - minSampleIndex + 1;
	*outAverage = average / numSamples;
}


template<class T>
void Channel<T>::CalculateMaximum(T* outMaximum, uint64 minSampleIndex, uint64 maxSampleIndex)
{
	// make sure we have at least one sample
	if (GetNumSamples() == 0)
		return;

	// in case no or bad arguments were specified
	if (minSampleIndex < GetMinSampleIndex())
		minSampleIndex = GetMinSampleIndex();
	if (maxSampleIndex > GetMaxSampleIndex())
		maxSampleIndex = GetMaxSampleIndex();

	T max = sMinValue;
	for (uint32 i = minSampleIndex; i <= maxSampleIndex; ++i)		// less OR equal because its an index
		max = Max<T>(max, GetSample(i));
	
	if (max != sMinValue)
		*outMaximum = max;
}


template<class T>
void Channel<T>::CalculateMinimum(T* outMinimum, uint64 minSampleIndex, uint64 maxSampleIndex)
{
	// make sure we have at least one sample
	if (GetNumSamples() == 0)
		return;

	// in case no or bad arguments were specified
	if (minSampleIndex < GetMinSampleIndex())
		minSampleIndex = GetMinSampleIndex();
	if (maxSampleIndex > GetMaxSampleIndex())
		maxSampleIndex = GetMaxSampleIndex();

	T min = sMaxValue;
	for (uint32 i = minSampleIndex; i <= maxSampleIndex; ++i)		// less OR equal because its an index
		min = Min<T>(min, GetSample(i));

	if (min != sMaxValue)
		*outMinimum = min;
}


template<>
ENGINE_API void Channel<Spectrum>::CalculateAverage(Spectrum* outAverage, uint64 minSampleIndex, uint64 maxSampleIndex)
{
	// make sure we have at least one sample
	if (GetNumSamples() == 0)
		return;

	// in case no or bad arguments were specified
	if (minSampleIndex < GetMinSampleIndex())
		minSampleIndex = GetMinSampleIndex();
	if (maxSampleIndex > GetMaxSampleIndex())
		maxSampleIndex = GetMaxSampleIndex();

	// get config
	Spectrum* config = &mSamples[0][0];
	const uint32 numBins = config->GetNumBins();

	// resize output spectrum and set frequency range
	outAverage->SetNumBins(numBins);
	outAverage->SetMaxFrequency(config->GetMaxFrequency());

	// calculate average of each bin individually
	Complex binSum = 0;
	for (uint32 b=0; b<numBins; ++b)
	{
		binSum = 0;
		for (uint32 i = minSampleIndex; i <= maxSampleIndex; ++i)
			binSum += GetSample(i).GetBin(b);
		
		if (mNumSamples == 0)
			outAverage->SetBin(b, 0);
		else
			outAverage->SetBin(b, binSum / mNumSamples);
	}
}


template<>
void Channel<Spectrum>::CalculateMaximum(Spectrum* outMaximum, uint64 minSampleIndex, uint64 maxSampleIndex)
{
	// NOT IMPLEMENTED
	CORE_ASSERT(false);
}


template<>
void Channel<Spectrum>::CalculateMinimum(Spectrum* outMinimum, uint64 minSampleIndex, uint64 maxSampleIndex)
{
	// NOT IMPLEMENTED
	CORE_ASSERT(false);
}


template<>
uint64 Channel<double>::CalculateMemoryAllocated(bool countBuffersOnly) const
{
	if (countBuffersOnly == true && IsBuffer() == false)
		return 0;
	
	uint64 numBytes = 0;
	for (uint32 i=0; i<mSamples.Size(); ++i)
		numBytes += mSamples[i].Size() * sizeof(double);

	return numBytes;
}


template<>
uint64 Channel<double>::CalculateMemoryUsed(bool countBuffersOnly) const
{
	if (countBuffersOnly == true && IsBuffer() == false)
		return 0;

	const uint64 numSamples = GetNumSamples();
	const uint64 numBytes = numSamples * sizeof(double);

	return numBytes;
}


template<>
uint64 Channel<Spectrum>::CalculateMemoryUsed(bool countBuffersOnly) const
{
    if (countBuffersOnly == true && IsBuffer() == false)
        return 0;
    
    uint64 numBytes = 0;
    
    const uint64 numSamples = GetNumSamples();
    const uint64 lastSampleIndex = GetMaxSampleIndex();
    for (uint32 i=0; i<numSamples; ++i)
    {
        // access the correct samples (just to be correct.. even though all samples should be of the same size)
        const uint64 sampleIndex = lastSampleIndex - i;
        numBytes += GetSample(sampleIndex).CalculateMemoryUsage();
    }
    
    return numBytes;
}


template<>
uint64 Channel<Spectrum>::CalculateMemoryAllocated(bool countBuffersOnly) const
{

	return CalculateMemoryUsed(countBuffersOnly);

	// NOTE took this out for chunk channel quickfix; doesn't make sense anyways right now
}

//=========================================================================

// specialize the type information
template<> uint32 const Channel<double>::TYPE_ID				= Channel::DOUBLE;
template<> uint32 const Channel<Spectrum>::TYPE_ID				= Channel::SPECTRUM;	

// specialize the type information
template<> double const Channel<double>::sZeroValue				= 0;
template<> double const Channel<double>::sMaxValue				= DBL_MAX;
template<> double const Channel<double>::sMinValue				= -sMaxValue;
template<> Spectrum const Channel<Spectrum>::sZeroValue			= Spectrum();		// NOT IMPLEMENTED
template<> Spectrum const Channel<Spectrum>::sMaxValue			= Spectrum();		// NOT IMPLEMENTED
template<> Spectrum const Channel<Spectrum>::sMinValue			= Spectrum();		// NOT IMPLEMENTED
//template<> int32 const Channel<int32>::sMaxValue				= 0xFFFFFFFF;
//template<> int32 const Channel<int32>::sMinValue				= 0;
//template<> Core::Complex const Channel<Core::Complex>::sMaxValue = Complex(DBL_MAX, DBL_MAX);
//template<> Core::Complex const Channel<Core::Complex>::sMinValue = sMaxValue;

// explicit template instantiation
template class ENGINE_API Channel<double>;
template class ENGINE_API Channel<Spectrum>;


