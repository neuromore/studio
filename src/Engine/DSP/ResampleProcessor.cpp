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
#include "ResampleProcessor.h"
#include "Channel.h"
#include "../Core/LogManager.h"

using namespace Core;

// constructor
ResampleProcessor::ResampleProcessor() : ChannelProcessor()
{
	Init();
	mResampleFunction = NULL;
	mSettings.mResampleMode = EResampleMode::REALTIME;
	mSettings.mTargetSampleRate = 0;
	mSettings.mStartTime = 0;
}


// destructor
ResampleProcessor::~ResampleProcessor()
{
}


void ResampleProcessor::Init()
{
	// one double channel input/output
	AddInput<double>();
	AddOutput<double>();
}


void ResampleProcessor::ReInit()
{
	// ReInit baseclass
	ChannelProcessor::ReInit();

	mIsInitialized = false;

	ChannelBase* input = GetInput();
	ChannelBase* output = GetOutput();

	// input (dis)connected?
	if (input == NULL)
	{
		// disconnect from clock generator
		mOutputClock.SetReferenceChannel(NULL);
		mOutputClock.Reset();

		// nothing more to do
		return;
	}

	// 1) determine sample mode by looking at the sample rate ratio

	const double inputSampleRate = input->GetSampleRate();
	const double outputSampleRate = mSettings.mTargetSampleRate;
	
	if (outputSampleRate <= 0)
	{
		// output sample rate not valid : just forward samples instead 
		mSettings.mResampleType = NO_RESAMPLING;
		mSettings.mResampleMode = MANUAL;
		mSettings.mResampleAlgo = FORWARD;
		mFactor = 1.0;
	}
	else if (inputSampleRate <= 0)// || input->IsIndependent() == true)  // TODO think how we must handle independent channels in the resampler - the output clock must be driven by the input every single resample method! this changes a lot of things here i think
	{
		// use the "output last" method if the input channel hast no fixed sample rate or is not synchronized
		mSettings.mResampleType = NAIVE;
		mSettings.mResampleMode = MANUAL;
		mSettings.mResampleAlgo = OUTPUT_LAST;
		mFactor = 1.0;
	}
	else
	{
		// ratio: number of output samples per one input sample
		const double ratio = outputSampleRate / inputSampleRate;
		const double epsilon = 10e-6;

		// identical sample rate,  just forward the samples
		if (Math::AbsD(ratio - 1.0) < epsilon)
		{
			mSettings.mResampleType = NO_RESAMPLING;
			mSettings.mResampleMode = MANUAL;
			mSettings.mResampleAlgo = FORWARD;
			mFactor = 1.0;
		}
		else 
		{
			mFactor = ratio;
			 
			// integer or fractional sample rate ratios?
			if (Math::AbsD( Math::FloorD(ratio) - ratio) < epsilon)
			{
				if (ratio > 1.0)
				{
					mSettings.mResampleType = UPSAMPLE_INTEGER;
					mFactor = ratio;
				}
				else 
				{
					mSettings.mResampleType = DOWNSAMPLE_INTEGER;
					mFactor = ratio;
				}
			}
			else
			{
				// fractional sample rate ratio
				if (ratio > 1.0)
					mSettings.mResampleType = UPSAMPLE_FRACTIONAL;
				else 
					mSettings.mResampleType = DOWNSAMPLE_FRACTIONAL;
			}
			
			// select appropriate resample algorithm (if not in manual mode)
			if (mSettings.mResampleMode != MANUAL)
				mSettings.mResampleAlgo = SelectAlgorithm(mSettings.mResampleType, mSettings.mResampleMode);
		}
	}

	
	// 3) configure resampler
	
	// update algo function pointer
	mResampleFunction = GetResampleFunction(mSettings.mResampleAlgo);

	// update output channel sample rate
	output->SetSampleRate(outputSampleRate);

	// resampler output is never independent, its synced to the engine
	output->SetIndependent(false);

	// calculate integer factor
	if (mFactor > 1.0)
		mIntFactor = (uint32)(mFactor);
	else
		mIntFactor = (uint32)(1.0 / mFactor);

	// kernel size (TODO can be increased here)
	if (mIntFactor >= 1)
		mKernelSize = mIntFactor;
	else 
		mKernelSize = 0;

	// setup sampling epoch
	mSamplingKernel.SetChannel(input);
	mSamplingKernel.SetLength(mKernelSize);
	mSamplingKernel.SetWindowFunction(NULL);		// use boxcar window

	// setup clock
	mOutputClock.Reset();

	switch (mSettings.mResampleAlgo)
	{
		// case 0: no clock required
		case FORWARD:
			mOutputClock.SetReferenceChannel(NULL);
			mOutputClock.Stop();
			break;
			
		// case 1: clock is driven by update loop -> remove reference channel
		case OUTPUT_LAST:
			mOutputClock.SetMode(ClockGenerator::INDEPENDENT);
			mOutputClock.SetReferenceChannel(NULL);
			mOutputClock.Start();
			break;

		// case 2: clock is synced to input channel, but can run one sample ahead
		case NEAREST_NEIGHBOR:
		case FIRST_ORDER_HOLD:
			mOutputClock.SetMode(ClockGenerator::SYNCED_AHEAD);
			mOutputClock.SetReferenceChannel(input);
			mOutputClock.Start();
			break;

		// case 3: clock is given by the input channel
		default:
		case LINEAR_INTERPOLATE:
		case BOXCAR:
			mOutputClock.SetMode(ClockGenerator::SYNCED);
			mOutputClock.SetReferenceChannel(input);
			mOutputClock.Start();
			break;
	}

	mOutputClock.SetFrequency(mSettings.mTargetSampleRate);
	mOutputClock.SetStartTime(mSettings.mStartTime);

	mIsInitialized = true;
}


void ResampleProcessor::Update()
{
	// this is deprecated!
	//if (mIsInitialized == false)
	//{
	//	// output_last algo must always run, even if there are no inputs
	//	if (mSettings.mResampleAlgo == OUTPUT_LAST && mResampleFunction != NULL)
	//		(this->*mResampleFunction)();
	//	return;
	//}

	// update input readers
	ChannelProcessor::Update();

	// call resample method (if set and inputs are connected)
	if (GetInput() != NULL)
		if (mResampleFunction != NULL)
		   (this->*mResampleFunction)();		// call of function via pointer to nonstatic member
}


void ResampleProcessor::Update(const Time& elapsed, const Time& delta)
{
	// update output clock
	mOutputClock.Update(elapsed, delta);

	// base update as last stept
	ChannelProcessor::Update(elapsed, delta);
}

//
// DSP Properties
//

uint32 ResampleProcessor::GetDelay(uint32 inputPortIndex, uint32 outputPortIndex) const
{
	switch (mSettings.mResampleAlgo)
	{
		// algos with zero delay
		case FORWARD: 
		case OUTPUT_LAST:
		case FIRST_ORDER_HOLD:  
		case NEAREST_NEIGHBOR:  
			if ((ResampleProcessor::EResampleType)mSettings.mResampleMode == UPSAMPLE_FRACTIONAL || (ResampleProcessor::EResampleType)mSettings.mResampleMode == UPSAMPLE_INTEGER)
				return 1; 
			else 
				return 0;

		// algos with a delay
		case LINEAR_INTERPOLATE:  
			return 1;

		case BOXCAR:
			return mIntFactor;

		default:
			return 0;
	}
}


double ResampleProcessor::GetSampleRatio(uint32 inputPortIndex, uint32 outputPortIndex) const
{
	return mFactor;
}


uint32 ResampleProcessor::GetNumStartupSamples(uint32 inputPortIndex) const
{
	switch (mSettings.mResampleAlgo)
	{
		// one sample needed
		case FORWARD: 
		case OUTPUT_LAST:
		case NEAREST_NEIGHBOR:
		case FIRST_ORDER_HOLD:  
			return 1; 

		// linear interpolation requires at least two samples (the one before the current)
		case LINEAR_INTERPOLATE:  
			return 2;

		case BOXCAR:			
			return mKernelSize*2;

		default:
			return 1;
	}
}


uint32 ResampleProcessor::GetNumEpochSamples(uint32 inputPortIndex) const
{
	switch (mSettings.mResampleAlgo)
	{
		// no epoch needed
		case FORWARD: 
		case OUTPUT_LAST:
		case NEAREST_NEIGHBOR:
		case FIRST_ORDER_HOLD:  
			return 1; 

		// linear interpolation requires at least two samples (the one before the current)
		case LINEAR_INTERPOLATE:  
			return 2;

		case BOXCAR:			
			return mKernelSize*2;

		default:
			return 1;
	}
}


// automatically select the appropriate resample algo
ResampleProcessor::EResampleAlgo ResampleProcessor::SelectAlgorithm(EResampleType type, EResampleMode mode)
{
	EResampleAlgo selectedAlgo = OUTPUT_LAST; // default mode

	// forward regardless of mode setting
	if (type == NO_RESAMPLING)
	{
		selectedAlgo = FORWARD;
	}

	// naive mode does not depend on sample rate
	else if (type == NAIVE)
	{
		selectedAlgo = OUTPUT_LAST;
	}

	// realtime resampling
	else if (mode == REALTIME)
	{
		switch (type)
		{
			case UPSAMPLE_INTEGER:
			case UPSAMPLE_FRACTIONAL:
				selectedAlgo = NEAREST_NEIGHBOR;
				break;

			case DOWNSAMPLE_INTEGER:
			case DOWNSAMPLE_FRACTIONAL:
				selectedAlgo = NEAREST_NEIGHBOR;
				break;
                
            default: break;
		}
	}

	// good quality resampling
	else if (mode == GOOD_QUALITY)
	{
		switch (type)
		{
			case UPSAMPLE_INTEGER:
			case UPSAMPLE_FRACTIONAL:
				selectedAlgo = LINEAR_INTERPOLATE;
				break;

			case DOWNSAMPLE_INTEGER:
			case DOWNSAMPLE_FRACTIONAL:
				selectedAlgo = BOXCAR;
				break;
                
            default: break;
		}
	}

	mSettings.mResampleAlgo = selectedAlgo;

	// return selected mode enum
	return selectedAlgo;
}


// get the function pointer of an algorithm
ResampleProcessor::ResampleFunction ResampleProcessor::GetResampleFunction(EResampleAlgo algo)
{
	// get the corresponding function pointer
	ResampleFunction selectedFunction = NULL;
	switch (algo)
	{
		case FORWARD:				selectedFunction = &ResampleProcessor::DoForward;			break;
		case OUTPUT_LAST:			selectedFunction = &ResampleProcessor::DoOutputLast;		break;
		case NEAREST_NEIGHBOR:		selectedFunction = &ResampleProcessor::DoNearestNeighbor;	break;
		case LINEAR_INTERPOLATE:	selectedFunction = &ResampleProcessor::DoLinearInterpolate;	break;
		case BOXCAR:				selectedFunction = &ResampleProcessor::DoBoxcar;			break;
        default:                                                                                break;
	}

	return selectedFunction;
}


//
// Resample Algos
//

// simply forward all samples
void ResampleProcessor::DoForward()
{
	ChannelReader* inputReader = GetInputReader();
	Channel<double>* output = GetOutput()->AsType<double>();
	
	// push all new samples into the output
	const uint32 numNewSamples = inputReader->GetNumNewSamples();
	for (uint32 i = 0; i < numNewSamples; i++)
	{
		double sample = inputReader->PopOldestSample<double>();
		output->AddSample(sample);
	}
}


// output the last value that is currently in the channel
void ResampleProcessor::DoOutputLast()
{
	// new samples we have to output
	const uint32 numNewSamples = mOutputClock.GetNumNewTicks();
	mOutputClock.ClearNewTicks();

	ChannelBase* input = GetInput();
	Channel<double>* output = GetOutput()->AsType<double>();
	
	// get the last value
	double lastValue = 0;
	if (input != NULL && input->IsEmpty() == false)
		lastValue = input->AsType<double>()->GetLastSample();

	// output the required number of samples
	for (uint32 i = 0; i < numNewSamples; i++)
		output->AddSample(lastValue);

	// flush input reader (not used here)
	GetInputReader()->Flush();
}


// output the sample closest in time (correct version, but with delay)
void ResampleProcessor::DoNearestNeighbor()
{
	// new samples we can output
	const uint32 numNewSamples = mOutputClock.GetNumNewTicks();
	
	Channel<double>* input = GetInput()->AsType<double>();
	Channel<double>* output = GetOutput()->AsType<double>();
	
	// output the samples
	for (uint32 i = 0; i < numNewSamples; i++)
	{
		const uint64 tick = mOutputClock.GetTick(0);
		double value = 0;

		// get sample from input that is closest in time to the one we want to output (rounding towards the past)
		const uint64 sampleIndex = input->FindIndexByTime(mOutputClock.GetTickTime(tick));
	  // LogDebug("NN: add %i/%i:  tick %i -> smpl %i", i, numNewSamples,tick, sampleIndex);
		
		// FIXME indices should never be out of bounds, except the very first one due to channel definition (index -1 becomes INVALIDINDEX32)
		if (sampleIndex != CORE_INVALIDINDEX64 && sampleIndex > input->GetMaxSampleIndex())
		{
			// FIXME I hope this case goes away as soon as we eliminate the rounding errors inside the clockgenerator (accumulated float must be changed into integer based timer)
			// UPDATE it still happens from time to time, if the input produces too many samples
		//	LogDebug("Resampler: Inputsample %i is out of range (min: %i  max: %i)", (int)sampleIndex, (int)input->GetMinSampleIndex(), (int)input->GetMaxSampleIndex());
			break;
		}
		else if (input->IsValidSample(sampleIndex) == true)
		{
			value = input->GetSample(sampleIndex);
		} 
		
		output->AddSample(value);
		
		mOutputClock.PopOldestTick();
	}
	
	// flush input reader (not used here)
	GetInputReader()->Flush();
}


// linear interpolate between samples to get inter-sample values
void ResampleProcessor::DoLinearInterpolate()
{

}


// fast boxcar filtered downsampling
void ResampleProcessor::DoBoxcar()
{
	// new samples we can output
	const uint32 numNewSamples = mOutputClock.GetNumNewTicks();
	
	Channel<double>* input = GetInput()->AsType<double>();
	Channel<double>* output = GetOutput()->AsType<double>();
	
	// output the samples
	for (uint32 i = 0; i < numNewSamples; i++)
	{
		uint64 tick = mOutputClock.PopOldestTick();

		// get sample from input that is closest in time to the one we want to output (rounding towards the past)
		const uint64 sampleIndex = input->FindIndexByTime(mOutputClock.GetTickTime(tick));
	  // LogDebug("NN: add %i/%i:  tick %i -> smpl %i", i, numNewSamples,tick, sampleIndex);
		
		// if sample is outside the buffer -> skip it 
		if (input->IsValidSample(sampleIndex) == false)
		{
			//LogDebug("Resampler: Inputsample %i is out of range (min: %i  max: %i)", sampleIndex, input->GetSampleCounter()-1-input->GetNumSamples(), input->GetSampleCounter()-1);
			continue;
		}

		// set sampling kernel position
		mSamplingKernel.SetPosition(sampleIndex);

		// calculate epoch sum and output result
		const double sampleValue=  mSamplingKernel.Sum() / mSamplingKernel.GetLength();
		output->AddSample(sampleValue);
	}
	
	// flush input reader (not used here)
	GetInputReader()->Flush();
}
