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

// include required headers
#include "OutputNode.h"


using namespace Core;

// constructor
OutputNode::OutputNode(Graph* parentGraph) : SPNode(parentGraph)
{
}


// destructor
OutputNode::~OutputNode()
{
}


// initialize the node
void OutputNode::Init()
{
	RequireInputConnection();

	// create output signal quality
	AttributeSettings* functionParam = RegisterAttribute("Resolution", "signalRes", "Select the signal resolution. This affects the size of the stored or uploaded data and the feedback latency.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	functionParam->ResizeComboValues( (uint32)RESOLUTION_NUM );
	for (uint32 i=0; i<RESOLUTION_NUM; ++i)
		functionParam->SetComboValue( i, GetResolutionString((SignalResolution)i) );
	functionParam->SetDefaultValue( AttributeInt32::Create(RESOLUTION_HIGH) );

	// upload checkbox
	AttributeSettings* attributeUpload = RegisterAttribute("Upload", "upload", "Upload the data stream to neuromore Cloud after a successful session.", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeUpload->SetDefaultValue(AttributeBool::Create(false));
}


// reinitialize the node
void OutputNode::ReInit(const Time& elapsed, const Time& delta)
{
	// reinit base
	SPNode::ReInit(elapsed, delta);
}


void OutputNode::Start(const Time& elapsed)
{
	// get selected signal resolution
	SignalResolution resolution = GetSignalResolution();
	
	// start input readers
	const Time startTime = mInputReader.FindMinLastSampleTime();
	mInputReader.Start(startTime);

	int readerIndex = 0;
	const uint32 numChannels = mChannels.Size();
	for (uint32 i = 0; i < numChannels; ++i)
	{
		// for now, we only use the first channel
		MultiChannel* channels = GetInputPort(i).GetChannels();
		ChannelBase* inputChannel = GetRawInputChannel(i);

		if (channels == NULL || inputChannel == NULL)
		{
			mResamplers[i].RemoveDelegateInputReader(0);
		}
		else
		{
			// connect input (using delegate channel reader)
			mResamplers[i].SetDelegateInputReader(0, mInputReader.GetReader(readerIndex));
			readerIndex++;

			// configure resampler 
			const double sampleRate = GetSampleRate(resolution, inputChannel);
			mResamplers[i].SetOutputSampleRate(sampleRate);
			mResamplers[i].SetResampleMode(ResampleProcessor::REALTIME);
		
			// set start times
			mResamplers[i].SetStartTime(startTime);
			mChannels[i]->SetStartTime(startTime);

			// finally reinit resampler
			mResamplers[i].ReInit();
		}
	}
}


// rewind
void OutputNode::Reset()
{
	//LogDebug("OutputNode::Reset()");

	// call baseclass reset
	SPNode::Reset();

	// iterate over all resamplers and channels, and reset them
	const uint32 numChannels = mChannels.Size();
	for (uint32 i=0; i<numChannels; ++i)
	{
		// reinit resampler
		mResamplers[i].RemoveDelegateInputReader(0);
		mResamplers[i].ReInit();

		// reset output channel and burst sizes
		mChannels[i]->Reset();
		mLastBurstSizes[i] = 0;
	}
}


void OutputNode::Update(const Time& elapsed, const Time& delta)
{
	// call baseclass update
	SPNode::Update(elapsed, delta);
}


void OutputNode::UpdateResamplers(const Time& elapsed, const Time& delta)
{
	// get number of channels / resamplers
	const uint32 numChannels = mChannels.Size();
	CORE_ASSERT(numChannels == GetNumInputPorts());

	// update all resamplers and measure the burst size
	for (uint32 i = 0; i<numChannels; ++i)
	{
		// update resampler and count number of added samples (burst size)
		const uint32 numSamplesBefore = mChannels[i]->GetSampleCounter();
		mResamplers[i].Update(elapsed, delta);
		const uint32 numSamplesAfter = mChannels[i]->GetSampleCounter();

		// set burst size
		if (numSamplesAfter >= numSamplesBefore)
			mLastBurstSizes[i] = numSamplesAfter - numSamplesBefore;

		// update output channel time
		mChannels[i]->SetElapsedTime(elapsed);
		mChannels[i]->UpdateLatency();
	}

	// clear node input reader
	mInputReader.Flush(true);
}


void OutputNode::OnAttributesChanged()
{
	// check if resolution was changed and force node reinit
	SignalResolution resolution = GetSignalResolution();

	if (resolution != mCurrentResolution)
	{
		mCurrentResolution = resolution;
		ResetAsync();
	}
}


// synchronize the the input channels (sensors) so the next sample that is added falls on this time
void OutputNode::Sync(double syncTime)
{
	// pad output channels so everything is aligned after the sync
	const uint32 numChannels = mChannels.Size();
	for (uint32 i=0; i<numChannels; ++i)
	{
		Channel<double>* channel = mChannels[i];

		// FIXME FindStartupDelay() is probably not enough, we need the startup delay of every input separately (works right now, because all inputs are resampled the same)
		//const uint32 numPaddingSamples = FindMaximumDelayForInput(i) * channel->GetSampleRate();
		//for (uint32 s=0; s<numPaddingSamples; ++s)
		//	channel->AddSample(0);// FIXME implement a padding method in channel?

		// output channel starts at t=0
		channel->SetStartTime(0.0);
	}
}


 // add resampler and output channel
void OutputNode::AddOutput()
{
	// create resampler and configure its output channel as storage
	mResamplers.AddEmpty();
	ResampleProcessor& resampler = mResamplers.GetLast();
	resampler.SetResampleMode(ResampleProcessor::EResampleMode::REALTIME);
	//resampler.SetResampleMode(ResampleProcessor::MANUAL);
	//resampler.SetResampleAlgo(ResampleProcessor::OUTPUT_LAST);

	mLastBurstSizes.Add(0);

	Channel<double>* channel = resampler.GetOutput()->AsType<double>();
	channel->SetBufferSize(0);
	mChannels.Add(channel);
}


uint32 OutputNode::GetNumCurrentValues() const
{
	return mInputReader.GetNumChannels();
}

// get current value of specified input channel
double OutputNode::GetCurrentValue(uint32 channelIndex) const
{
	CORE_ASSERT(channelIndex <= mInputReader.GetNumChannels());

	if (mIsInitialized == false)
		return 0.0;

	if (channelIndex >= mInputReader.GetNumChannels())
		return 0.0;

	if (mInputReader.GetChannel(channelIndex)->GetNumSamples() == 0)
		return 0.0;

	return mInputReader.GetChannel(channelIndex)->AsType<double>()->GetLastSample();
}


// check if feedback node has a current value
bool OutputNode::IsEmpty(uint32 channelIndex) const
{
	CORE_ASSERT(channelIndex <= mInputReader.GetNumChannels());

	if (channelIndex >= mInputReader.GetNumChannels())
		return true;

	if (mInputReader.GetChannel(channelIndex)->GetNumSamples() == 0)
		return true;

	return false;
}



// if not overridden, this returns the first channel in the set connected at this port
Channel<double>* OutputNode::GetRawInputChannel(uint32 portIndex)
{
	if (GetInputPort(portIndex).HasConnection() == false)
		return NULL;

	MultiChannel* channels = GetInputPort(portIndex).GetChannels();
	
	if (channels->GetNumChannels() == 0)
		return NULL;

	ChannelBase* channel = channels->GetChannel(0);
	if (channel->GetType() != Channel<double>::TYPE_ID)
		return NULL;

	return channels->GetChannel(0)->AsType<double>();
}


const char* OutputNode::GetResolutionString(SignalResolution resolution)
{
	switch (resolution)
	{
		case OutputNode::RESOLUTION_RAW:	return "Original";
		case OutputNode::RESOLUTION_HIGH:	return "High";
		case OutputNode::RESOLUTION_MID:	return "Mid";
		case OutputNode::RESOLUTION_LOW:	return "Low";
        default:                            return "Unkonwn";
	}
}


double OutputNode::GetSampleRate(SignalResolution resolution, ChannelBase* reference)
{
	switch (resolution)
	{
		case OutputNode::RESOLUTION_RAW:	return reference->GetSampleRate();
		case OutputNode::RESOLUTION_HIGH:	return 30.0;
		case OutputNode::RESOLUTION_MID:	return 10.0;
		case OutputNode::RESOLUTION_LOW:	return 4.0;
        default:                            return 0.0;
	}
}


uint32 OutputNode::CalculateOutputMemoryUsed()
{
	uint32 result = 0;

	// get the number of channels, iterate through and accumulate the used memory
	const uint32 numChannels = mChannels.Size();
	for (uint32 i=0; i<numChannels; ++i)
		result += mChannels[i]->CalculateMemoryUsed();

	return result;
}


bool OutputNode::IsValidInput(uint32 portIndex)
{
	// channel input
	MultiChannel* input = GetInputPort(portIndex).GetChannels();

	if (input == NULL || input->GetNumChannels() == 0)
		return false;

	if (input->GetChannel(0)->GetNumSamples() == 0)
		return false;

	if (mChannels[portIndex]->IsEmpty() == true)
		return false;

	return true;
}


// add resampler delay to input delay
double OutputNode::FindMaximumDelayForInput(uint32 inputPortIndex)
{
	const double inputDelay = SPNode::FindMaximumDelayForInput(inputPortIndex);

	// FIXME do not use first resampler (asumption: all resmaplers are configured identically)
	const double resamplerDelay = GetDelay(inputPortIndex, 0);

	return inputDelay + resamplerDelay;	
}



// add resampler latency to input latency
double OutputNode::FindMaximumLatencyForInput(uint32 inputPortIndex)
{
	const double inputLatency = SPNode::FindMaximumLatencyForInput(inputPortIndex);

	// FIXME do not use first resampler (asumption: all resmaplers are configured identically)
	const double resamplerLatency = GetLatency(inputPortIndex, 0);

	return inputLatency + resamplerLatency;	
}


// forward resampler delay
double OutputNode::GetDelay(uint32 inputPortIndex, uint32 outputPortIndex) const 
{
	CORE_ASSERT(inputPortIndex < mResamplers.Size());

	if (mResamplers[inputPortIndex].IsInitialized() == false)
		return 0;

	return mResamplers[inputPortIndex].GetDelay(0,0) / mResamplers[inputPortIndex].GetSettings().mTargetSampleRate;
}


// forward resampler latency
double OutputNode::GetLatency(uint32 inputPortIndex, uint32 outputPortIndex) const 
{
	CORE_ASSERT(inputPortIndex < mResamplers.Size());
	return mResamplers[inputPortIndex].GetLatency(0,0);
}


Core::String& OutputNode::GetDebugString(Core::String& inout)
{
	SPNode::GetDebugString(inout);

	const uint32 numOutputs = mResamplers.Size();
	for (uint32 i = 0; i < numOutputs; ++i)
	{
		ChannelBase* channel = GetOutputChannel(i);
		mTempString.Format("Output %i \"%s\"\n", i, channel->GetName());
		inout += mTempString;
		mTempString.Format("  Resampler @ %.2fHz\n", channel->GetSampleRate());
		inout += mTempString;
		mTempString.Format("  Start: %.5fs\n", channel->GetStartTime().InSeconds());
		inout += mTempString;
		mTempString.Format("  Last: %.5fs\n", channel->GetLastSampleTime().InSeconds());
		inout += mTempString;
		mTempString.Format("  Added: %i\n", channel->GetNumNewSamples());
		inout += mTempString;
		mTempString.Format("  Counter: %i\n", channel->GetSampleCounter());
		inout += mTempString;
	}

	return inout;
}


