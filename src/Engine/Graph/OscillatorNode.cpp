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
#include "OscillatorNode.h"
#include "../Core/EventManager.h"


using namespace Core;

// constructor
OscillatorNode::OscillatorNode(Graph* graph) : ProcessorNode(graph, new OscillatorProcessor())
{
	mSettings.mSampleRate = 128;
	mSettings.mWaveformType = OscillatorProcessor::WAVEFORM_SINE;
	mSettings.mDefaultAmplitude = 1.0;
	mSettings.mDefaultFrequency = 440.0;
	mSettings.mDefaultPhase = 0.0;
}


// destructor
OscillatorNode::~OscillatorNode()
{
}


// initialize the node
void OscillatorNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	RequireInputConnection(false);

	// SETUP PORTS

	GetInputPort(INPUTPORT_AMPLITUDE).Setup("Amplitude", "amplitude", AttributeChannels<double>::TYPE_ID, INPUTPORT_AMPLITUDE);
	GetInputPort(INPUTPORT_FREQUENCY).Setup("Frequency", "frequency", AttributeChannels<double>::TYPE_ID, INPUTPORT_FREQUENCY);
	GetInputPort(INPUTPORT_PHASE).Setup("Phase", "phase", AttributeChannels<double>::TYPE_ID, INPUTPORT_PHASE);
	GetOutputPort(OUTPUTPORT_SIGNAL).Setup("Out", "y", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_SIGNAL);
	
	// ATTRIBUTES

	// initialize with default values from settings
	
	// Sample Rate
	AttributeSettings* attrSampleRate = RegisterAttribute("Sample Rate", "SampleRate", "The target sample rate.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attrSampleRate->SetDefaultValue(Core::AttributeFloat::Create(mSettings.mSampleRate));
	attrSampleRate->SetMinValue(Core::AttributeFloat::Create(FLT_EPSILON));
	attrSampleRate->SetMaxValue(Core::AttributeFloat::Create(FLT_MAX));

	// Resample Mode (disabled for now)
	AttributeSettings* attrWaveform = RegisterAttribute("Waveform", "Waveform", "Type of waveform.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attrWaveform->AddComboValue("Sine");
	attrWaveform->AddComboValue("Square");
	attrWaveform->AddComboValue("Triangle");
	attrWaveform->AddComboValue("Sawtooth");
	attrWaveform->SetDefaultValue(Core::AttributeInt32::Create(mSettings.mWaveformType));

	// Default Parameters
	AttributeSettings* attrAmplitude = RegisterAttribute("Amplitude", "Amplitude", "This value is used if nothing is connected to the Amplitude port.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attrAmplitude->SetDefaultValue(Core::AttributeFloat::Create(mSettings.mDefaultAmplitude));
	attrAmplitude->SetMinValue(Core::AttributeFloat::Create(-FLT_MAX));
	attrAmplitude->SetMaxValue(Core::AttributeFloat::Create(FLT_MAX));
	
	AttributeSettings* attrFrequency = RegisterAttribute("Frequency", "Frequency", "This value is used if nothing is connected to the Frequency port.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attrFrequency->SetDefaultValue(Core::AttributeFloat::Create(mSettings.mDefaultFrequency));
	attrFrequency->SetMinValue(Core::AttributeFloat::Create(FLT_EPSILON));
	attrFrequency->SetMaxValue(Core::AttributeFloat::Create(FLT_MAX));

	AttributeSettings* attrPhase = RegisterAttribute("Phase", "Phase", "This value is used if nothing is connected to the Phase port.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attrPhase->SetDefaultValue(Core::AttributeFloat::Create(mSettings.mDefaultPhase));
	attrPhase->SetMinValue(Core::AttributeFloat::Create(-180));
	attrPhase->SetMaxValue(Core::AttributeFloat::Create(180));

}


void OscillatorNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


void OscillatorNode::Start(const Time& elapsed)
{
	// create and reinit processors etc
	ProcessorNode::Start(elapsed);

	// set resampler start time
	const uint32 numProcessors = mProcessors.Size();
	for (uint32 i = 0; i < numProcessors; ++i)
	{
		// note: no type check required for casting
		OscillatorProcessor* processor = static_cast<OscillatorProcessor*>(mProcessors[i]);
		processor->SetStartTime(elapsed);

		// reinit (again!)
		processor->ReInit();
	}
}



// update the node
void OscillatorNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update baseclass
	ProcessorNode::Update(elapsed, delta);
}


// attributes have changed
void OscillatorNode::OnAttributesChanged()
{
	// check if attributes have changed
	const uint32 waveformType = GetInt32Attribute(ATTRIB_WAVEFORM);
	const double sampleRate = GetFloatAttribute(ATTRIB_SAMPLERATE);
	
	mSettings.mWaveformType = (OscillatorProcessor::EWaveformType)waveformType;
	mSettings.mDefaultAmplitude = GetFloatAttribute(ATTRIB_DEFAULT_AMPLITUDE);
	mSettings.mDefaultFrequency = GetFloatAttribute(ATTRIB_DEFAULT_FREQUENCY);
	mSettings.mDefaultPhase = GetFloatAttribute(ATTRIB_DEFAULT_PHASE);

	// reset only if sample rate has changed
	if (mSettings.mSampleRate != sampleRate)
	{
		mSettings.mSampleRate = sampleRate;
		ResetAsync();
	}
	else
	{
		SetupProcessors();
	}
}


///////////////////////////////////

OscillatorNode::OscillatorProcessor::OscillatorProcessor()
{
	Init();
	mSettings.mSampleRate = 128;
	mSettings.mWaveformType = OscillatorProcessor::WAVEFORM_SINE;
	mSettings.mDefaultAmplitude = 1.0;
	mSettings.mDefaultFrequency = 440.0;
	mSettings.mDefaultPhase = 0.0;
	mPhaseAccumulator = 0;
}

OscillatorNode::OscillatorProcessor::~OscillatorProcessor()
{
}


void OscillatorNode::OscillatorProcessor::Init()
{
	AddInput<double>();
	AddInput<double>();
	AddInput<double>();
	AddOutput<double>();
}


void OscillatorNode::OscillatorProcessor::ReInit()
{
	// ReInit baseclass
	ChannelProcessor::ReInit();

	GetOutput()->SetSampleRate(mSettings.mSampleRate);

	// setup clock
	mOutputClock.Reset();
	mOutputClock.SetMode(ClockGenerator::INDEPENDENT);
	mOutputClock.SetReferenceChannel(NULL);
	mOutputClock.SetFrequency(mSettings.mSampleRate);
	mOutputClock.SetStartTime(mSettings.mStartTime);
	mOutputClock.Start();

	mPhaseAccumulator = 0;

	mIsInitialized = true;
}


void OscillatorNode::OscillatorProcessor::Update()
{
	// update input readers
	ChannelProcessor::Update();

	ChannelBase* amplitudeInput = GetInput(INPUTPORT_AMPLITUDE);
	ChannelBase* frequencyInput = GetInput(INPUTPORT_FREQUENCY);
	ChannelBase* phaseInput = GetInput(INPUTPORT_PHASE);
	Channel<double>* output = GetOutput()->AsType<double>();
	
	// waveform function
	WaveformFunction mWaveformFunction = GetWaveformFunction(mSettings.mWaveformType);

	// default parameters
	double amplitude = mSettings.mDefaultAmplitude;
	double frequency = mSettings.mDefaultFrequency;
	double phase = mSettings.mDefaultPhase;

	// get current parameters from inputs
	// TODO add a synchronous output mode, this implementation only works during realtime execution
	if (amplitudeInput != NULL)
	{
		Channel<double>* channel = amplitudeInput->AsType<double>();
		if (channel->GetNumSamples() > 0)
			amplitude = channel->GetLastSample();
	}

	if (frequencyInput != NULL)
	{
		Channel<double>* channel = frequencyInput->AsType<double>();
		if (channel->GetNumSamples() > 0)
			frequency = channel->GetLastSample();
	}

	if (phaseInput != NULL)
	{
		Channel<double>* channel = phaseInput->AsType<double>();
		if (channel->GetNumSamples() > 0)
			phase = channel->GetLastSample();
	}

	// new samples we have to output
	const uint32 numNewSamples = mOutputClock.GetNumNewTicks();

	// generate samples
	for (uint32 i = 0; i < numNewSamples; i++)
	{
		//const Time sampleTime = mOutputClock.GetTickTime(mOutputClock.GetTick(i));

		const double value = amplitude / 2.0 * (this->*mWaveformFunction)(mPhaseAccumulator + phase);

		// increase the phase
		mPhaseAccumulator += Math::twoPiD * frequency / (double)mSettings.mSampleRate;

		output->AddSample(value);
	}

	// fmod the phase accumulator to keep rounding errors in check
	mPhaseAccumulator = Math::FModD(mPhaseAccumulator, Math::twoPiD);

	mOutputClock.ClearNewTicks();
}


void OscillatorNode::OscillatorProcessor::Update(const Time& elapsed, const Time& delta)
{
	// update output clock
	mOutputClock.Update(elapsed, delta);

	// base update as last stept
	ChannelProcessor::Update(elapsed, delta);
}


// get the function pointer of an algorithm
OscillatorNode::OscillatorProcessor::WaveformFunction OscillatorNode::OscillatorProcessor::GetWaveformFunction(EWaveformType waveform)
{
	// get the corresponding function pointer
	WaveformFunction selectedFunction = NULL;
	switch (waveform)
	{
		case WAVEFORM_SINE:			selectedFunction = &OscillatorProcessor::SineWaveFunction;			break;
		case WAVEFORM_SQUARE:		selectedFunction = &OscillatorProcessor::SquareWaveFunction;		break;
		case WAVEFORM_TRIANGLE:		selectedFunction = &OscillatorProcessor::TriangleWaveFunction;		break;
		case WAVEFORM_SAWTOOTH:		selectedFunction = &OscillatorProcessor::SawtoothWaveFunction;		break;
		default:																						break;
	}

	return selectedFunction;
}

double OscillatorNode::OscillatorProcessor::SineWaveFunction(double phi)
{
	return Math::SinD(phi);
}


// rectangle function with duty cycle (value normalized to -1 .. 1)
double OscillatorNode::OscillatorProcessor::SquareWaveFunction(double phi)
{
	if (Math::FModD(phi, Math::twoPiD) > Math::piD)
		return 1.0;
	else
		return -1.0;
}


// sawtooth function
double OscillatorNode::OscillatorProcessor::SawtoothWaveFunction(double phi)
{
	return (Math::FModD(phi, Math::twoPiD) - Math::piD) / Math::piD;
}


// triangle function
double OscillatorNode::OscillatorProcessor::TriangleWaveFunction(double phi)
{
	return Math::AbsD(2.0 * SawtoothWaveFunction(phi)) - 1.0;
}
