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
#include "ChannelProcessor.h"
#include "Channel.h"
#include "../Core/LogManager.h"


using namespace Core;

// constructor
ChannelProcessor::ChannelProcessor()
{
}


// destructor
ChannelProcessor::~ChannelProcessor()
{
	// destruct all channel inputs that are owned by the processor
	const uint32 numInputs = GetNumInputs();
	for (uint32 i=0; i<numInputs; ++i)
		if (mIsOwnInputReader[i] == true)
			delete mInputs[i];
	mInputs.Clear();
	mIsOwnInputReader.Clear();

	// destruct all output channels
	const uint32 numOutputs = GetNumOutputs();
	for (uint32 i=0; i<numOutputs; ++i)
		delete mOutputs[i];

	mOutputs.Clear();
}


// reinit processor (this is a full state reset, not comparable to Node::ReInit())
void ChannelProcessor::ReInit()
{ 
	LogTraceRT("ReInit");

	// call reinit of all input readers to detect changes
	const uint32 numInputs = GetNumInputs();
	for (uint32 i=0; i<numInputs; ++i)
	{
		if (mIsOwnInputReader[i] == true)
			mInputs[i]->Reset();
	}
}


// main update method for channel processor
void ChannelProcessor::Update()
{
	LogTraceRT("Update");

	// update channel input readers: get the number of new input samples
	const uint32 numInputs = GetNumInputs();
	for (uint32 i=0; i<numInputs; ++i)
		if (mIsOwnInputReader[i] == true)
 			mInputs[i]->Update();

	// before processor adds the samples: reset the new-samples-counter
	const uint32 numOutputs= GetNumOutputs();
	for (uint32 i = 0; i<numOutputs; ++i)
	{
		mOutputs[i]->BeginAddSamples();
	}
}


ChannelBase* ChannelProcessor::GetInput(uint32 index) const
{
	// return input channel, if there is any 
	ChannelReader* input = mInputs[index];

	if (input == NULL)
		return NULL;

	return input->GetChannel();
}


void ChannelProcessor::SetDelegateInputReader(uint32 index, ChannelReader* reader)
{
	CORE_ASSERT(index < mInputs.Size());

	// reader was already set
	if (mInputs[index] == reader)
		return;

	// delete own reader first
	if (mIsOwnInputReader[index] == true)
		delete mInputs[index];

	// replace channel reader and set the flag
	mInputs[index] = reader;
	mIsOwnInputReader[index] = false;
}


void ChannelProcessor::RemoveDelegateInputReader(uint32 index)
{
	LogTrace("RemoveDelegateInputReader");

	CORE_ASSERT(index < mInputs.Size());
	
	// do nothing if this already is our own reader
	if (mIsOwnInputReader[index] == true)
		return;

	// allocate new reader (do not delete the old one as it belongs to some other object)
	mInputs[index] = new ChannelReader();
	mIsOwnInputReader[index] = true;
}
