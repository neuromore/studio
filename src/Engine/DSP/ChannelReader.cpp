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
#include "ChannelReader.h"
#include "Channel.h"


using namespace Core;

// constructor
ChannelReader::ChannelReader(ChannelBase* inputChannel)
{
	mChannel = inputChannel;

	mEpochLength = 1;
	mEpochShift = 1;
	mEpochZeroPadding = 0;

	Reset();
}


// destructor
ChannelReader::~ChannelReader()
{
}


void ChannelReader::Reset()
{ 
	LogTrace("Reset");
	
	mHasStarted = false;

	mLastSampleRate = 0;
	mLastBufferSize = 0;
	mLastSampleCounter = 0;
	mStartTime = 0;

	mNumNewSamples = 0;
	mNumSamplesProcessed = 0;
	mNumSamplesReceived = 0;
	
	// reset change detection flags
	mInputConnectionChanged = false;
	for (uint32 i=0; i<NUM_CHANGETYPES; ++i)
		mInputChangeDetected[i] = false;

}

void ChannelReader::ResetCounters()
{ 
	LogTrace("ResetCounters");
	
	// start reading at the end of the channel
	if (mChannel != NULL)
	{
		mLastSampleRate = mChannel->GetSampleRate();
		mLastBufferSize = mChannel->GetBufferSize();
		mLastSampleCounter = mChannel->GetSampleCounter();
		LogDebug("ChannelReader::mLastSampleCounter = %i", mLastSampleCounter);
		mStartTime = mChannel->GetLastSampleTime();
	}
	else  // no channel
	{
		mLastSampleRate = 0;
		mLastBufferSize = 0;
		mLastSampleCounter = 0;
		mStartTime = 0;
	}

	mNumNewSamples = 0;
	mNumSamplesProcessed = 0;
	mNumSamplesReceived = 0;
}


// detect changes and check for new samples
void ChannelReader::DetectInputChanges()
{ 
	// no input channel -> we still have to check if it was recently disconnected
	if (mChannel == NULL)
	{
		if (mInputConnectionChanged == true)
		{
			LogDebug("input channel was removed");

			// reset channel reader
			Reset();
			mInputChangeDetected[REFERENCE] = true;
			mInputChangeDetected[ANY] = true;

			mInputConnectionChanged = false;
		}
	}
	else
	{
		bool changeDetected = false;
		
		// reset change detection flags
		for (uint32 i=0; i<NUM_CHANGETYPES; ++i)
			mInputChangeDetected[i] = false;
		
		// change in reference overrides change in sample rate and sample counter!
		if (mInputConnectionChanged == true)
		{
			LogDebug("input channel reference changed");

			mInputChangeDetected[REFERENCE] = true;
			changeDetected = true;
		} 
		
		// sample rate changed
		if (mLastSampleRate != mChannel->GetSampleRate())
		{
			LogDebug("input channel sample rate changed from %f to %f", mLastSampleRate, mChannel->GetSampleRate());
			mInputChangeDetected[SAMPLERATE] = true;
			changeDetected = true;
		}
		
		// sample counter was set back (channel was reset)
		if (mChannel->GetSampleCounter() < mLastSampleCounter)
		{
			LogDebug("input channel sample counter jumped backwards from %i to %i", (uint32)mLastSampleCounter, (uint32)mChannel->GetSampleCounter());
			mInputChangeDetected[RESET] = true;
			changeDetected = true;
		}
		
		// change detected -> set ANY flag and sync reader 
		if (changeDetected == true)
		{
			LogDebug("restarting channel reader due to detected changes");
			
			mNumNewSamples = 0;
			mLastSampleCounter = mChannel->GetSampleCounter();
			mLastSampleRate = mChannel->GetSampleRate();
			mHasStarted = false;
			mInputChangeDetected[ANY] = true;

			// reset
			mInputConnectionChanged = false;
		}
	}
}


void ChannelReader::Update()
{
	LogTraceRT("Update");

	if (mChannel == NULL)
		return;

	// until startet: check if input channel has reached the start time yet
	if (mHasStarted == false)
	{
		if (mChannel->GetSampleRate() > 0)
		{
			// channel has constant samplerate: align start time precisely
			Time newestSampleTime = mChannel->GetLastSampleTime();

			// input channel has not reached the start time -> do not start
			if (newestSampleTime < mStartTime)
				return;

			// now we start: calculate how many samples have advanced since start time
			const Time interval = newestSampleTime - mStartTime;
			uint64 numSamples = interval.InSeconds() * mChannel->GetSampleRate();

			// clamp to maximum available samples
			numSamples = Min(numSamples, mChannel->GetSampleCounter());

			// init counters
			mNumNewSamples = numSamples;
			mNumSamplesReceived = numSamples;
			mLastSampleCounter = mChannel->GetSampleCounter() - numSamples;

			LogDebug("begin reading at index %i (first burst is %i)", (uint32)mChannel->GetSampleCounter(), mNumNewSamples);

			mHasStarted = true;
			return;
		}
		else
		{
			// start immediately if channel has non-constant samplerate (samples don't have individual timestamps)
			mNumNewSamples = 0;
			mNumSamplesReceived = 0;
			mLastSampleCounter = mChannel->GetSampleCounter();
			mHasStarted = true;
			return;
		}

	}

	// normal operation : just forward number of new samples
	const uint32 numNewSamples = mChannel->GetNumNewSamples();
	mNumNewSamples += numNewSamples;
	mNumSamplesReceived += mNumNewSamples;

	//if (mHasStarted == false)
	//{
	//	// check if the channel has reached the requested start time yet 
	//	// if it has not, we drop as many samples as we need so the reader will start at the correct index
	//	if (mChannel->GetLastSampleTime() < mStartTime)
	//	{
	//		SkipNewSamples(mNumNewSamples);
	//	}
	//	else
	//	{
	//		// calculate how many samples we must skip
	//		const Time intervalDuration = mStartTime - mChannel->GetLastSampleTime();
	//		const uint32 intervalLength = intervalDuration.InSeconds() * mChannel->GetSampleRate();
	//		Advance(intervalLength);
	//		mHasStarted = true;
	//	}
	//}

	//const uint32 numSamplesAvailable = mChannel->GetNumSamples();
	//bool overflow = false;
	//while (mNumNewSamples > numSamplesAvailable)
	//{
	//	// NOTE this should not happen if everything is handled right by the node that uses it
	//	overflow = true;
	//	
	//	// remove oldest sample
	//	mNumNewSamples--;
	//	mLastSampleCounter++;
	//	mNumSamplesProcessed++;
	//}

	//if (overflow == true)
	//	LogDebug("channel buffer limit reached! counter indiciates %i samples, channel has %i", mNumNewSamples, numSamplesAvailable);

}


void ChannelReader::Start(const Core::Time& time)
{
	LogTrace("Start");

	ResetCounters();
	mStartTime = time;
}



void ChannelReader::SetChannel(ChannelBase* inputChannel)
{
	LogTraceRT("SetChannel");

	// reference has not changed
	if (mChannel == inputChannel)
		return;

	// set channel
	mChannel = inputChannel;
	
	LogDebug("set mChannel to %x", inputChannel);
	
	// reset reader
	Reset();

	// set channel-reference-changed flag if this is not the first connection that is made
	mInputConnectionChanged = true;
}


//
// Access Samples individually
//

// return one of the new Samples (const reference to the sample in the channel) where index=0 is the oldest one
template<class T>
const T& ChannelReader::GetSample(uint32 index)
{
	const uint32 maxNumSamples	= mChannel->GetSampleCounter();
	const uint32 sampleIndex	= maxNumSamples - mNumNewSamples + index;
	CORE_ASSERT( sampleIndex < maxNumSamples );

	return mChannel->AsType<T>()->GetSample(sampleIndex);
}


// get the oldest of the new samples without popping it
template<class T>
const T& ChannelReader::GetOldestSample()
{
	CORE_ASSERT( mNumNewSamples > 0 );

	const T& sample = GetSample<T>(0);
	return sample;
}


// pop operation on the oldest of the new samples on the input channel
template<class T>
const T& ChannelReader::PopOldestSample()
{
	CORE_ASSERT(mNumNewSamples > 0);
	const T& sample = GetSample<T>(0);

	Advance(1);

	return sample;
}

// get the newest of the new samples without popping it
template<class T>
const T& ChannelReader::GetNewestSample()
{
	const T& sample = GetSample<T>(mNumNewSamples-1);
	return sample;
}


// marks the oldest numSamples samples as processed
void ChannelReader::Advance(uint32 numSamples)
{
	CORE_ASSERT(numSamples <= mNumNewSamples);
	const uint64 numSamplesClamped = Min<uint64>(mNumNewSamples, numSamples);

	mNumNewSamples -= numSamplesClamped;
	mLastSampleCounter += numSamplesClamped;
	mNumSamplesProcessed += numSamplesClamped;
}


// return index of one of the new samples (const reference to the sample in the channel) where index=0 is the oldest one
uint64 ChannelReader::GetSampleIndex(uint32 index)
{
	const uint64 maxNumSamples = mChannel->GetSampleCounter();
	
	// invalid sample requested
	if (mNumNewSamples > maxNumSamples + index)
		return CORE_INVALIDINDEX64;

	const uint64 sampleIndex = maxNumSamples - mNumNewSamples + index;
	CORE_ASSERT(sampleIndex < maxNumSamples);

	return sampleIndex;
}


// get the sample index of the oldest of the new samples
uint64 ChannelReader::GetOldestSampleIndex()
{
	if (mNumNewSamples > 0)
		return GetSampleIndex(0);
	else
	{
		// if all sampels are processed, the oldest sample is given by the sample counter
		const uint64 sampleCounter = mChannel->GetSampleCounter();
		
		// there may be no oldest sample
		if (sampleCounter == 0)
			return CORE_INVALIDINDEX64;
		else
			return sampleCounter - 1;
	}
}


// get the sample index of the oldest of the new samples
Time ChannelReader::GetOldestSampleTime()
{
	const uint64 index = GetOldestSampleIndex();

	// there is no oldest sample -> just return 0.0
	if (index == CORE_INVALIDINDEX64)
		return 0.0;

	return mChannel->GetSampleTime(index);
}


//
// Access whole epochs at once
//


// calculate the number of epochs that fit into the range of new samples
uint32 ChannelReader::GetNumEpochs() const
{
	uint32 numEpochs;

	// epoch length of zero disables epoching
	if (mEpochLength == 0)
		return 0;

	// calculate shift. A zero parameter means we use non-overlapping epochs, so shift := length
	uint32 shift = mEpochShift;
	if (shift == 0)
		shift = mEpochLength;

	// calculate number of epochs using the shift parameter
	numEpochs = mNumNewSamples / shift;
	
	return numEpochs;
}


// return the specified epoch without popping it 
Epoch ChannelReader::GetEpoch(uint32 index) const
{
	// calc epoch position
	const uint64 position = CalcEpochPosition(index);
	
	// configure epoch
	Epoch epoch(mChannel, mEpochLength);
	epoch.SetPosition(position);
	epoch.SetZeroPaddingEnabled(mEpochZeroPadding);

	return epoch;
}


// return the oldest epoch and remove it from the list
Epoch ChannelReader::PopOldestEpoch()
{
	// assert that there is an epoch to pop
	CORE_ASSERT (GetNumEpochs() > 0);

	// calc position of oldest epoch (index 0)
	const uint64 position = CalcEpochPosition(0);
	
	// configure epoch
	Epoch epoch(mChannel, mEpochLength);
	epoch.SetPosition(position);
	epoch.SetZeroPaddingEnabled(mEpochZeroPadding);

	// zero shift means we use non-overlapping epochs

	uint32 shift = 0;
	if (mEpochShift == 0)
		shift = mEpochLength;
	else
		shift = mEpochShift;

	mNumNewSamples -= shift;
	mLastSampleCounter += shift;
	mNumSamplesProcessed += shift;

	return epoch;
}


void  ChannelReader::ClearNewEpochs()
{
	const uint32 numEpochs = GetNumEpochs();
	uint32 numSamples;

	// pop all epochs (same code as in PopOldestepoch, repeated numEpoch times)
	if (mEpochShift == 0)
		numSamples = mEpochLength*numEpochs;
	else
		numSamples = mEpochShift*numEpochs;

	mNumNewSamples -= numSamples;
	mLastSampleCounter += numSamples;
	mNumSamplesProcessed += numSamples;
}


// epoch index helper
uint64 ChannelReader::CalcEpochPosition(uint32 index) const 
{
	CORE_ASSERT (index < GetNumEpochs());
	
	const uint64 maxNumSamples = mChannel->GetSampleCounter();
	const uint64 oldestSampleIndex = maxNumSamples - mNumNewSamples;

	uint64 position;

	// zero shift means we use non-overlapping epochs
	if (mEpochShift == 0)
		position = oldestSampleIndex + index * mEpochLength;
	else	
		position = oldestSampleIndex + index * mEpochShift;

	return position;
}


// explicit instantiation
template const double& ChannelReader::GetSample<double>(uint32 index);
template const Spectrum& ChannelReader::GetSample<Spectrum>(uint32 index);

template const double& ChannelReader::PopOldestSample<double>();
template const Spectrum& ChannelReader::PopOldestSample<Spectrum>();

template const double& ChannelReader::GetOldestSample<double>();
template const Spectrum& ChannelReader::GetOldestSample<Spectrum>();

template const double& ChannelReader::GetNewestSample<double>();
template const Spectrum& ChannelReader::GetNewestSample<Spectrum>();
