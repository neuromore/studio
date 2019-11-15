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
#include "MultiChannelReader.h"
#include "Channel.h"
#include "../Core/LogManager.h"


using namespace Core;

// constructor
MultiChannelReader::MultiChannelReader(MultiChannel* inputChannels)
{
	mInputChannels = inputChannels;
}


// destructor
MultiChannelReader::~MultiChannelReader()
{
}


void MultiChannelReader::Reset()
{ 
	LogTrace("Reset");

	// reset all channel reader
	const uint32 numChannels = mChannelReaders.Size();
	for (uint32 i = 0; i < numChannels; ++i)
	{
		mChannelReaders[i].Reset();
	}
	mInputChangeDetected = false;
}


void MultiChannelReader::DetectInputChanges()
{
	LogTrace("DetectInputChanges");

	// reset flag
	mInputChangeDetected = false;
	
	// check if channel set has changed
	if (mInputChannels != NULL)
	{
		const uint32 numChannels = mInputChannels->GetNumChannels();
		
		// number of channels has changed
		if (mChannelReaders.Size() != numChannels)
		{
			mChannelReaders.Resize(numChannels);

			// remember this in own flag (otherwise things like the deletion of the last channel in the set would not be detected)
			mInputChangeDetected = true;

			LogDebug("change in input multichannel size was detected");
		}

		// NOTE: reference changes are detected inside the channel reader in SetChannel()

		// set references to input readers (they will detect the other changes)
		for (uint32 i=0; i<numChannels; ++i)
		{
			LogDebug("updating channel reader input channel references");

			ChannelBase* channel = mInputChannels->GetChannel(i);
			CORE_ASSERT(channel != NULL);
			mChannelReaders[i].SetChannel(channel);
		}
	}
	else
	{
		// input channels were removed
		if (mChannelReaders.Size() != 0)
		{
			LogDebug("all input channels were removed");

			mInputChangeDetected = true;
			mChannelReaders.Clear();
		}
	}

	// detect the changes in the individual channels
	const uint32 numChannels = mChannelReaders.Size();
	for (uint32 i=0; i<numChannels; ++i)
		mChannelReaders[i].DetectInputChanges();
}


// update all readers
void MultiChannelReader::Update()
{
	const uint32 numChannels = mChannelReaders.Size();
	for (uint32 i=0; i<numChannels; ++i)
		mChannelReaders[i].Update();
}


// start all readers at time
void MultiChannelReader::Start(const Time& time)
{
	LogTrace("Start");
	LogDebug("starting channel readers at t=%f", time.InSeconds());

	const uint32 numChannels = mChannelReaders.Size();
	for (uint32 i = 0; i<numChannels; ++i)
		mChannelReaders[i].Start(time);
}



// mark x new samples in all readers as processed
void MultiChannelReader::Advance(uint32 numSamples)
{
	const uint32 numChannels = mChannelReaders.Size();
	for (uint32 i = 0; i<numChannels; ++i)
		mChannelReaders[i].Advance(numSamples);
}


// mark all new samples in all readers as processed
void MultiChannelReader::Flush(bool independent)
{
	LogTrace("Advance");

	// advance all channel readers to the (individual) end
	if (independent == true)
	{
		const uint32 numChannels = mChannelReaders.Size();
		for (uint32 i = 0; i<numChannels; ++i)
			mChannelReaders[i].Flush();
	}
	else
	{
		// Note: assumes all channels have the same sample rate

		// advance them all by the same number samples as far they allow it
		const uint32 minNumNewSamples = GetMinNumNewSamples();

		const uint32 numChannels = mChannelReaders.Size();
		for (uint32 i=0; i<numChannels; ++i)
			mChannelReaders[i].Advance(minNumNewSamples);
	}
}


// change the input to the multichannel reader (you have to call DetectChanges() after this so the reader works)
void MultiChannelReader::SetInput(MultiChannel* inputChannels)
{
	mInputChannels = inputChannels;
}


// get channel by index
ChannelBase* MultiChannelReader::GetChannel(uint32 index)
{
	if (index >= mChannelReaders.Size())
		return NULL;

	return mChannelReaders[index].GetChannel(); 
}

// get channel by index
const ChannelBase* MultiChannelReader::GetChannel(uint32 index) const
{
	if (index >= mChannelReaders.Size())
		return NULL;

	return mChannelReaders[index].GetChannel(); 
}


// get reader by index
ChannelReader* MultiChannelReader::GetReader(uint32 index)
{
	if (index >= mChannelReaders.Size() )
		return NULL;

	return &mChannelReaders[index]; 
}


ChannelReader* MultiChannelReader::FindReader(const ChannelBase* channel)
{
	if (channel == NULL)
		return NULL;

	const uint32 numReaders = mChannelReaders.Size();
	for (uint32 i=0; i<numReaders; ++i)
	{
		ChannelReader* reader = &mChannelReaders[i];
		if (reader->GetChannel() == channel)
			return reader;
	}

	return NULL;
}


//
//// sync readers so they are all aligned in time
//void MultiChannelReader::SyncReaders()
//{
//	// get time of earliest and latest sample
//	const Time minTime = FindMinLastSampleTime();
//	const Time maxTime = FindMaxLastSampleTime();
//
//	// align readers to earliest sample by setting the correct number of new samples (from minTime to the last sample in the channel)
//	const uint32 numChannels = mChannelReaders.Size();
//	for (uint32 i = 0; i < numChannels; ++i)
//	{
//		ChannelReader& reader = mChannelReaders[i];
//		ChannelBase* channel = reader.GetChannel();
//		const Time lastSampleTime = channel->GetLastSampleTime();
//		
//		// calculate interval and number of samples the reader should have advanced
//		CORE_ASSERT(maxTime >= lastSampleTime);
//		const Time interval = maxTime - lastSampleTime;
//		const uint32 numSamples = (uint32)(interval.InSeconds() * channel->GetSampleRate() + 0.5); // round to closest integer
//		
//		// calc start index and setup channel reader
//		const uint64 startIndex = channel->GetMaxSampleIndex() - numSamples;
//		reader.Reset();
//		reader.SetStartIndex(startIndex);
//		//reader.Update();
//	}
//}


// get max times of last sample across all channels
Time MultiChannelReader::FindMinLastSampleTime()
{
	Time minTime = 0.0;
	const uint32 numChannels = mChannelReaders.Size();
	for (uint32 i = 0; i < numChannels; ++i)
	{
		ChannelBase* channel = mChannelReaders[i].GetChannel();
		CORE_ASSERT(channel != NULL);
		if (i == 0)
				minTime = channel->GetLastSampleTime();
		else
			minTime = Min<Time>(minTime, channel->GetLastSampleTime());
	}
	return minTime;
}


// get max time of last sample across all channels
Time MultiChannelReader::FindMaxLastSampleTime()
{
	Time maxTime = 0.0;
	const uint32 numChannels = mChannelReaders.Size();
	for (uint32 i = 0; i < numChannels; ++i)
	{
		ChannelBase* channel = mChannelReaders[i].GetChannel();
		CORE_ASSERT(channel != NULL);
		if (i == 0)
			maxTime = channel->GetLastSampleTime();
		else
			maxTime = Max<Time>(maxTime, channel->GetLastSampleTime());
	}
	return maxTime;
}


// calculate the timing difference of the channel readers (this should always be < 1.0/samplerate)
Time MultiChannelReader::CalcReaderSyncOffset()
{
	LogTraceRT("CalcReaderSyncOffset");
	Time maxTime = 0.0;
	Time minTime = 0.0;
	const uint32 numChannels = mChannelReaders.Size();
	for (uint32 i = 0; i < numChannels; ++i)
	{
		// time sync point is the time of the next sample that is processed by the readers (time of oldest sample in the reader)
		const Time nextReadSampleTime = mChannelReaders[i].GetOldestSampleTime();

		if (i == 0)
		{
			maxTime = nextReadSampleTime;
			minTime = nextReadSampleTime;
		}
		else
		{
			maxTime = Max<Time>(maxTime, nextReadSampleTime);
			minTime = Min<Time>(minTime, nextReadSampleTime);
		}
	}
	
	return maxTime - minTime; 
}


// check if sample rates of all channels are identical
bool MultiChannelReader::HasUniformSampleRate()
{
	double targetSampleRate = 0;
	const uint32 numChannels = mChannelReaders.Size();
	for (uint32 i = 0; i < numChannels; ++i)
	{
		ChannelBase* channel = mChannelReaders[i].GetChannel();
		if (i == 0)
			targetSampleRate = channel->GetSampleRate();
		else
			if (channel->GetSampleRate() != targetSampleRate)
				return false;
	}

	return true;
}


// only available for uniform samplerates
double MultiChannelReader::GetSampleRate()
{
	const uint32 numChannels = mChannelReaders.Size();

	if (numChannels == 0)
		return 0.0;
	
	return mChannelReaders[0].GetChannel()->GetSampleRate();
}

 


// get the minimum number of samples that can be read from all readers
uint32 MultiChannelReader::GetMinNumNewSamples() const
{
	const uint32 numChannels = mChannelReaders.Size();
	if (numChannels == 0)
		return 0;
	
	uint32 minNumSamples = CORE_INT32_MAX;
	for (uint32 i=0; i<numChannels; ++i)
	{
		minNumSamples = Min<uint32>(minNumSamples, mChannelReaders[i].GetNumNewSamples());
	}

	return minNumSamples;
}


// return true if at least one of the channels has a change
bool MultiChannelReader::HasInputChanged(ChannelReader::EChangeType type) const
{ 
	const uint32 numChannels = mChannelReaders.Size();
	
	// also check if input references has changed
	if (type == ChannelReader::REFERENCE || type == ChannelReader::ANY)
		if (mInputChangeDetected == true)
			return true;

	for (uint32 i=0; i<numChannels; ++i)
		for (uint32 t=0; t<ChannelReader::NUM_CHANGETYPES; ++t)
			if (mChannelReaders[i].HasInputChanged(type) == true) 
				return true;

	return false;
}
