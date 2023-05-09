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

// include required headers
#include "MultiChannel.h"


// get the type of the channel set (common to all channels)
uint32 MultiChannel::GetType() const
{
	const uint32 numChannels = mChannels.Size();

	CORE_ASSERT(numChannels > 0);
	if (numChannels == 0)
		return 0;

	return mChannels[0]->GetType();
}

// add a single channel
void MultiChannel::AddChannel(ChannelBase* channel)
{
	CORE_ASSERT(channel != NULL);
	
	// NOTE: removed this because SPNode::mInputChannels must take different kinds of channels, for example in the ViewerNode
	// assert that the channel is compatible before adding it to the set
	//CORE_ASSERT(IsCompatible(channel) == true);

	mChannels.Add(channel);
}


// add another channel set
void MultiChannel::AddMultiChannel(MultiChannel* channels)
{
	CORE_ASSERT(channels != NULL);

	const uint32 numChannels = channels->GetNumChannels();
	for (uint32 i=0; i<numChannels; ++i)
		AddChannel(channels->GetChannel(i));
}


// set channel references using another channel set
void MultiChannel::SetMultiChannel(MultiChannel* channels)
{
	CORE_ASSERT(channels != NULL);

	mChannels.Clear();
	AddMultiChannel(channels);
}


// reset all channels in the set
void MultiChannel::Reset()
{
	const uint32 numChannels = mChannels.Size();
	for (uint32 i=0; i<numChannels; ++i)
		mChannels[i]->Reset();
}


// get the sample rate of the channel set (same for all channels)
double MultiChannel::GetSampleRate() const
{
	const uint32 numChannels = mChannels.Size();

	if (numChannels == 0)
		return 0;

	return mChannels[0]->GetSampleRate();
}


// calculate min range accross all channels
double MultiChannel::GetMinValue() const
{ 
	const uint32 numChannels = mChannels.Size();
	if (numChannels == 0)
		return 0;

	double minValue = DBL_MAX;
	for (uint32 i=0; i<numChannels; ++i)
		minValue = Core::Min( minValue, mChannels[i]->GetMinValue());

	return minValue; 
}


// calculate max range accross all channels
double MultiChannel::GetMaxValue() const
{ 
	const uint32 numChannels = mChannels.Size();
	if (numChannels == 0)
		return 1.0;

	double maxValue = -DBL_MAX;
	for (uint32 i=0; i<numChannels; ++i)
		maxValue = Core::Max( maxValue, mChannels[i]->GetMaxValue());

	return maxValue; 
}



// returns true if at least one channel in the set is active
bool MultiChannel::IsActive() const
{
	bool isActive = false;

	const uint32 numChannels = mChannels.Size();
	for (uint32 i=0; i<numChannels; ++i)
		isActive |= mChannels[i]->IsActive();

	return isActive;
}


// returns true if at least one channel in the set is highlighted
bool MultiChannel::IsHighlighted() const
{
	bool isHighlighted = false;

	const uint32 numChannels = mChannels.Size();
	for (uint32 i=0; i<numChannels; ++i)
		isHighlighted |= mChannels[i]->IsHighlighted();

	return isHighlighted;
}


// set buffer size of all channels
void MultiChannel::SetBufferSize(uint32 numSamples, bool discard)
{
	const uint32 numChannels = mChannels.Size();
	for (uint32 i=0; i<numChannels; ++i)
		mChannels[i]->SetBufferSize(numSamples, discard);
}


uint32 MultiChannel::GetMinBufferSize() const
{
	const uint32 numChannels = mChannels.Size();

	if (numChannels == 0)
		return 0;
	
	uint32 minBufferSize = CORE_INT32_MAX;
	for (uint32 i=0; i<numChannels; ++i)
	{
		minBufferSize = Core::Min(minBufferSize, mChannels[i]->GetBufferSize());
	}

	return minBufferSize;
}


bool MultiChannel::IsBuffer() const
{
	return GetMinBufferSize() != 0;
}

// if a channel is compatible with the set
bool MultiChannel::IsCompatible(ChannelBase* otherChannel)
{
	if (otherChannel == NULL)
		return false;

	const uint32 numChannels = mChannels.Size();
	if (numChannels == 0)
		return true;

	const uint32 typeID = otherChannel->GetType();

	// compare all other channels' properties against the given channel
	for (uint32 i=1; i<numChannels; ++i)
	{
		ChannelBase* channel = mChannels[i];

		if (channel->GetType() != typeID)
			return false;
	}
	
	return true;
}


// compare channels against each other (type and sample rate must match)
bool MultiChannel::Validate()
{
	const uint32 numChannels = mChannels.Size();
	if (numChannels == 0)
		return false;

	ChannelBase* channel = mChannels[0];
	const uint32 type = channel->GetType();
	const uint32 sampleRate = channel->GetSampleRate();

	for (uint32 i=1; i<numChannels; ++i)
	{
		channel = mChannels[i];
		if (channel->GetType() != type	|| channel->GetSampleRate() != sampleRate)
			return false;
	}
	
	return true;
}


uint32 MultiChannel::CalculateMemoryAllocated(bool countBuffersOnly)
{
	uint32 numBytes = 0;
	const uint32 numChannels = mChannels.Size();
	for (uint32 i=0; i<numChannels; ++i)
		numBytes += mChannels[i]->CalculateMemoryAllocated(countBuffersOnly);

	return numBytes;
}


uint32 MultiChannel::CalculateMemoryUsed(bool countBuffersOnly)
{
	uint32 numBytes = 0;
	const uint32 numChannels = mChannels.Size();
	for (uint32 i=0; i<numChannels; ++i)
		numBytes += mChannels[i]->CalculateMemoryUsed(countBuffersOnly);

	return numBytes;
}
