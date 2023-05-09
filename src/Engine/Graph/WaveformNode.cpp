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
#include "WaveformNode.h"
#include "../Core/EventManager.h"


using namespace Core;

// constructor
WaveformNode::WaveformNode(Graph* graph) : ProcessorNode(graph, new WaveformProcessor())
{
	mSettings.mSampleRate = 128;
	mSettings.mWaveformDefinition.Clear();
	mSettings.mWaveformType = WaveformProcessor::WAVEFORM_STEPS;
	mSettings.mPlayMode = WaveformProcessor::PLAY_ALWAYS;
	mSettings.mContinuousOutput = true;
}


// destructor
WaveformNode::~WaveformNode()
{
}


// initialize the node
void WaveformNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	RequireInputConnection(false);

	// SETUP PORTS

	GetInputPort(INPUTPORT_ACTIVATE).Setup("Activate", "Activate", AttributeChannels<double>::TYPE_ID, INPUTPORT_ACTIVATE);
	GetOutputPort(OUTPUTPORT_SIGNAL).Setup("Out", "y", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_SIGNAL);
	
	// ATTRIBUTES

	// initialize with default values from settings
	
	// Sample Rate
	AttributeSettings* attrSampleRate = RegisterAttribute("Sample Rate", "SampleRate", "The output sample rate.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attrSampleRate->SetDefaultValue(Core::AttributeFloat::Create(mSettings.mSampleRate));
	attrSampleRate->SetMinValue(Core::AttributeFloat::Create(FLT_EPSILON));
	attrSampleRate->SetMaxValue(Core::AttributeFloat::Create(FLT_MAX));

	// Waveform String
	AttributeSettings* attrWaveform = RegisterAttribute("Waveform", "Waveform", "The waveform as a string in the format \"value1:duration1;value2:duration2;value3:duration3; .. : .. ;\".", Core::ATTRIBUTE_INTERFACETYPE_STRING);
	attrWaveform->SetDefaultValue(Core::AttributeString::Create(mSettings.mWaveformDefinition));

	// Waveform Type
	AttributeSettings* attrWaveformType = RegisterAttribute("Waveform Type", "WaveformType", "How the waveform is constructed from the points.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attrWaveformType->AddComboValue("Steps");
	attrWaveformType->AddComboValue("Linear");
	//attrWaveformType->AddComboValue("Bicubic");
	//attrWaveformType->AddComboValue("B-Spline");
	attrWaveformType->SetDefaultValue(Core::AttributeInt32::Create(mSettings.mWaveformType));

	// Play Mode
	AttributeSettings* attrPlayMode = RegisterAttribute("Play Mode", "PlayMode", "How the waveform is played.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attrPlayMode->AddComboValue("Play Always and Forever");
	attrPlayMode->AddComboValue("Play Once if sample received");
	attrPlayMode->AddComboValue("Play Once if Input > 0.0");
	attrPlayMode->AddComboValue("Play While Input > 0.0");
	attrPlayMode->SetDefaultValue(Core::AttributeInt32::Create(mSettings.mPlayMode));

	// continous output flag
	AttributeSettings* attrContinuousOutput = RegisterAttribute("Continuous Output", "ContinuousOutput", "If enabled, the output produces a constant sample rate and outputs 0.0 if no waveform is generated. Disabling results in bursts of samples instead of a stream.", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attrContinuousOutput->SetDefaultValue(Core::AttributeBool::Create(mSettings.mContinuousOutput));

}


void WaveformNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}



void WaveformNode::Start(const Time& elapsed)
{
	// create and reinit processors etc
	ProcessorNode::Start(elapsed);

	// set resampler start time
	const uint32 numProcessors = mProcessors.Size();
	for (uint32 i = 0; i < numProcessors; ++i)
	{
		// note: no type check required for casting
		WaveformProcessor* processor = static_cast<WaveformProcessor*>(mProcessors[i]);
		processor->SetStartTime(elapsed);
	}
}



// update the node
void WaveformNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update baseclass
	ProcessorNode::Update(elapsed, delta);
}


// attributes have changed
void WaveformNode::OnAttributesChanged()
{
	// check if attributes have changed
	const double sampleRate = GetFloatAttribute(ATTRIB_SAMPLERATE);
	const char* waveformString = GetStringAttribute(ATTRIB_WAVEFORM);
	const uint32 waveformType = GetInt32Attribute(ATTRIB_WAVEFORM_TYPE);
	const uint32 playMode = GetInt32Attribute(ATTRIB_PLAYMODE);
	const bool continuousOutput = GetBoolAttribute(ATTRIB_CONTINOUSOUTPUT);

	// return anything has changed
	if (playMode == (uint32)mSettings.mPlayMode						&&
		waveformType == (uint32)mSettings.mWaveformType				&&
		sampleRate == mSettings.mSampleRate							&&
		mSettings.mWaveformDefinition.Compare(waveformString) == 0	&&
		continuousOutput == mSettings.mContinuousOutput)
	{
		return;
	}

	mSettings.mSampleRate = sampleRate;
	mSettings.mWaveformDefinition = waveformString;
	mSettings.mWaveformType = (WaveformProcessor::EWaveformType)waveformType;
	mSettings.mPlayMode = (WaveformProcessor::EPlayMode)playMode;
	mSettings.mContinuousOutput = continuousOutput;

	// reset required if anything has changed
	ResetAsync();
}


///////////////////////////////////

WaveformNode::WaveformProcessor::WaveformProcessor()
{
	Init();
	mSettings.mSampleRate = 128;
	mSettings.mPlayMode = WaveformProcessor::PLAY_ALWAYS;
	mSettings.mStartTime = 0.0;
	mSettings.mWaveformDefinition = "";
	mSettings.mWaveformType = WAVEFORM_STEPS;
	mSettings.mContinuousOutput = true;

	mIsPlaying = false;
	mPlayedOnce = false;
}

WaveformNode::WaveformProcessor::~WaveformProcessor()
{
}


void WaveformNode::WaveformProcessor::Init()
{
	AddInput<double>();
	AddOutput<double>();
}


void WaveformNode::WaveformProcessor::ReInit()
{
	// ReInit baseclass
	ChannelProcessor::ReInit();

	// setup clock
	mOutputClock.Reset();
	mOutputClock.SetMode(ClockGenerator::INDEPENDENT);
	mOutputClock.SetReferenceChannel(NULL);
	mOutputClock.SetFrequency(mSettings.mSampleRate);

	// use constant sample rate output if continous output is on
	if (mSettings.mContinuousOutput == true)
		GetOutput()->SetSampleRate(mSettings.mSampleRate);
	else
		GetOutput()->SetSampleRate(0);

	// precalc waveform
	RecalculateWaveform();

	// have no waveform -> can't init
	if (mValues.Size() < 1 || mWaveformLength <= 0.0)
	{
		mIsInitialized = false;
		return;
	}

	// reset flags before starting
	mIsPlaying = false;
	mPlayedOnce = false;

	// start processor
	mIsInitialized = true;
}


void WaveformNode::WaveformProcessor::Update(const Time& elapsed, const Time& delta)
{

	// base update, also calls ChannelProcessor::Update()
	ChannelProcessor::Update(elapsed, delta);

	if (mIsInitialized == false)
		return;

	Channel<double>* input = GetInput()->AsType<double>();
	ChannelReader* inputReader = GetInputReader();
	Channel<double>* output = GetOutput()->AsType<double>();

	// update output clock
	mOutputClock.Update(elapsed, delta);

	uint32 numNewTicks = mOutputClock.GetNumNewTicks();				// samples to output
	uint32 numNewInputSamples = inputReader->GetNumNewSamples();	// samples received on "activate" port

	//LogDebug("Update: time is %.2f, have %i output ticks and %i input samples", elapsed, numNewTicks, numNewInputSamples);

	// handle state depending on playback setting
	const bool previousPlayingState = mIsPlaying;
	switch (mSettings.mPlayMode)
	{
		// always running
		case PLAY_ALWAYS:
			mIsPlaying = true;
			break;

			// play once if sample received
		case PLAY_ONCE_IF_SAMPLE:
			{
				// start playing
				if (mIsPlaying == false && numNewInputSamples > 0)
				{
					mIsPlaying = true;
					break;
				}

				if (numNewTicks == 0)
					break;

				// stop playing if end of waveform is reached
				const double lastTickTime = mOutputClock.GetTickTime(mOutputClock.GetTick(numNewTicks - 1)).InSeconds() - mSettings.mStartTime.InSeconds();
				//LogDebug("Last Tick Time is %.2f", lastTickTime);
				if (mIsPlaying == true && lastTickTime > mWaveformLength)
					mIsPlaying = false;

			} break;

		// play once: make sure we don't overrun while generating the waveform -> clamp the number of samples to generate
		case PLAY_ONCE_IF_TRUE:
		{
			const bool isActive = input != NULL && input->GetNumSamples() > 0 && input->GetLastSample() > 0.0;

			// start playing if last input sample value is > 0
			
			if (isActive == true && mPlayedOnce == false)
			{
				if (mIsPlaying == false)
				{
					mIsPlaying = true;
					mPlayedOnce = true;
					break;
				}
			}
			else if (isActive == false && mPlayedOnce == true)
			{
				// reset played-once flag so it will play again once input becomes active again
				mPlayedOnce = false;
				break;
			}
			
			if (numNewTicks == 0)
				break;

			// stop playing if end of waveform is reached or active state is false
			const double lastTickTime = mOutputClock.GetTickTime(mOutputClock.GetTick(numNewTicks - 1)).InSeconds() - mSettings.mStartTime.InSeconds();
			//LogDebug("Last Tick Time is %.2f, isActive=%s, duration = %.2f", lastTickTime, (isActive ? "true" : "false"), waveformDuration);
			if (mIsPlaying == true && lastTickTime > mWaveformLength)
				mIsPlaying = false;

		}	break;

		// waveform generator is running if we have an input sample with value  > 0.0
		case PLAY_WHILE_TRUE:
		{
			const bool isActive = input != NULL && input->GetNumSamples() > 0 && input->GetLastSample() > 0.0;

			// start playing if last input sample value is > 0
			if (mIsPlaying == false && isActive == true)
				mIsPlaying = true;

			if (numNewTicks == 0)
				break;

			// stop playing if end of waveform is reached or active state is false
			//const double lastTickTime = mOutputClock.GetTickTime(mOutputClock.GetTick(numNewTicks - 1)) - mSettings.mStartTime;
			//LogDebug("Last Tick Time is %.2f, isActive=%s, duration = %.2f", lastTickTime, (isActive ? "true" : "false"), mWaveformLength);
			if (mIsPlaying == true && isActive == false)
				mIsPlaying = false;

		}	break;
	}

	// start/stop clock
	if (mIsPlaying != previousPlayingState)
	{
		if (mIsPlaying == true)
		{
			//LogDebug("Starting output clock @ %.2f", elapsed);

			SetStartTime(elapsed);

			// setup and restart clock
			mOutputClock.Reset();
			mOutputClock.SetMode(ClockGenerator::INDEPENDENT);
			mOutputClock.SetReferenceChannel(NULL);
			mOutputClock.SetFrequency(mSettings.mSampleRate);
			mOutputClock.SetStartTime(elapsed);
			mOutputClock.Start();
			return;

		}
		else
		{
			//LogDebug("Exiting Playback Mode @ %.2f", elapsed);

			// leave clock running only if continous output is enabled
			if (mSettings.mContinuousOutput == false)
			{
				mOutputClock.Stop();
				return;
			}
		}
	}

	// output samples

	if (mIsPlaying == true)
	{
		// generate samples
		double value = 0.0;
		for (uint32 i = 0; i < numNewTicks; i++)
		{
			// time within waveform
			const double sampleTime = mOutputClock.GetTickTime(mOutputClock.GetTick(i)).InSeconds() - mSettings.mStartTime.InSeconds();
			const double timeWithinWaveform = Math::FModD(sampleTime, mWaveformLength);

			// check if we have reached the end of the waveform
			if ((mSettings.mPlayMode == PLAY_ONCE_IF_SAMPLE || mSettings.mPlayMode == PLAY_ONCE_IF_TRUE) && sampleTime  >= mWaveformLength)
			{
				mIsPlaying = false;

				// leave clock running only if continous output is enabled
				if (mSettings.mContinuousOutput == false)
					mOutputClock.Stop();

				break;
			}

			//
			// now look up waveform value and output sample
			//`

			// 1) steps (zero order hold)
			if (mSettings.mWaveformType == WAVEFORM_STEPS)
			{
				// find the current value with t>=timeWithinWaveform
				const uint32 numEntries = mTimestamps.Size();
				for (uint32 j=0; j<numEntries; ++j)
				{
					if (mTimestamps[j] < timeWithinWaveform)
						continue;
					
					value = mValues[j];
					break;
				}
			}
			// 2) linear interpolated
			else if (mSettings.mWaveformType == WAVEFORM_LINEAR)
			{
				// find the current value with t>=timeWithinWaveform
				const uint32 numEntries = mTimestamps.Size();
				for (uint32 j=0; j<numEntries; ++j)
				{
					if (mTimestamps[j] < timeWithinWaveform)
						continue;

					// interpolate (if not edge case)
					if (j<numEntries-1)
					{
						const double intervalLength = (mTimestamps[j+1] - mTimestamps[j]);
						const double timeWithinInterval = (timeWithinWaveform - mTimestamps[j]);
						const double normedTime = timeWithinInterval / intervalLength;
						value = LinearInterpolate(mValues[j], mValues[j+1], normedTime);
					}
					else
						value = mValues[j];

					break;
				}
			}
			// else ... // TODO more interpolation types

			output->AddSample(value);
		}
	}
	else
	{
		// output zero values instead of waveform
		if (mSettings.mContinuousOutput == true)
		{
			for (uint32 i = 0; i < numNewTicks; i++)
				output->AddSample(0.0);
		}
	}

	inputReader->Flush();
	mOutputClock.ClearNewTicks();
}


void WaveformNode::WaveformProcessor::RecalculateWaveform()
{
	// parse the waveform string into value-duration pairs
	mValues.Clear();
	mDurations.Clear();
	ParseWaveformString(mSettings.mWaveformDefinition, mValues, mDurations);

	CORE_ASSERT(mValues.Size() == mDurations.Size());
	mNumElements = Min(mValues.Size(), mDurations.Size());

	// need at least one element
	if (mNumElements == 0)
		return;
	
	// calculate the timestamp array (sum up the durations)
	mTimestamps.Reserve(mNumElements);
	double totalTime = 0;
	for (uint32 i = 0; i < mNumElements; ++i)
	{
		totalTime += mDurations[i];
		mDurations[i] = totalTime;
	}

	mWaveformLength = totalTime;

	//// dont forget the n+1th point :) we could ignore it, but we want a single-point waveform to work so we just repeat the last value ("hold" it)
	//// (other option would be to exclude the duration of the last point, because it is always redundant)
	//mWaveformValues.Add(mValues[mNumElements - 1]);
	//mWaveformTimestamps.Add(totalTime);
}


void WaveformNode::WaveformProcessor::ParseWaveformString(const String& waveformString, Array<double>& outValues, Array<double>& outDurations)
{
	// split pairs at semicolon
	Array<String> pairs = waveformString.Split(StringCharacter::semiColon);

	// go through pairs and parse numbers pairs into value/duration
	double value, duration;

	const uint32 numPairs = pairs.Size();
	for (uint32 i = 0; i < numPairs; ++i)
	{
		Array<String> valueAndDuration = pairs[i].Split(StringCharacter::colon);
		const uint32 numElements = valueAndDuration.Size();
		
		if (numElements == 0)
			continue;

		 
		// first element is value
		valueAndDuration[0].Trim();
		if (valueAndDuration[0].IsValidFloat() == true)
		{
			value = valueAndDuration[0].ToFloat();
		}
		
		// second element is duration (use 0 if it doesnt exist)
		duration = 0.0;
		if (numElements >= 2)
		{
			valueAndDuration[1].Trim();
			if (valueAndDuration[1].IsValidFloat() == true)
				duration = valueAndDuration[1].ToFloat();
		}

		// add points to arrays
		outValues.Add(value);
		outDurations.Add(duration);
	}
}
