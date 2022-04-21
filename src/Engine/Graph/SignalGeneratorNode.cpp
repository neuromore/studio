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
#include "SignalGeneratorNode.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
SignalGeneratorNode::SignalGeneratorNode(Graph* graph) : InputNode(graph)
{
	// create our channel and fill it with the default values
	mSensor.SetName("Signal Generator");
	mSensor.GetChannel()->SetSampleRate(128);
	mSensor.GetChannel()->SetBufferSize(10);
	mSensor.SetDriftCorrectionEnabled(false);
	mSensor.GetChannel()->SetMinValue(-1.0);
	mSensor.GetChannel()->SetMaxValue(1.0);

	// color the output channel same as the node
	UseChannelColoring();
	
	// configure and start clockgenerator
	mClock.SetFrequency(128);
}


// destructor
SignalGeneratorNode::~SignalGeneratorNode()
{
}


// initialize the node
void SignalGeneratorNode::Init()
{
	// PORTS

	// setup the output ports
	InitOutputPorts(1);
	GetOutputPort(OUTPUTPORT_CHANNEL).SetupAsChannels<double>(GetSignalTypeName(SIGNALTYPE_SINE), "y", OUTPUTPORT_CHANNEL);
	
	// set output channel
	GetOutputPort(OUTPUTPORT_CHANNEL).GetChannels()->AddChannel(mSensor.GetChannel());

	// ATTRIBUTES

	// baseclass attributes
	InputNode::Init();

	// default values (set in constructor)
	const double defaultSampleRate	= mSensor.GetChannel()->GetSampleRate();
	const double defaultAmplitude	= (mSensor.GetChannel()->GetMaxValue() - mSensor.GetChannel()->GetMinValue()) * 0.5;
	const double defaultFrequency   = 1.0;
	const double defaultDCOffset	= 0;
	const double defaultDutycycle	= 0.5;
	const ESignalType defaultSignalType = SIGNALTYPE_SINE;

	// Signal Type
	Core::AttributeSettings* attr = RegisterAttribute( "Signal Type", "SignalType", "The Type of signal to generate.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX );
	attr->SetDefaultValue( Core::AttributeInt32::Create( defaultSignalType ) );
	attr->ResizeComboValues( NUM_SIGNALTYPES );
	for (uint32 i = 0; i < NUM_SIGNALTYPES; i++)
		attr->SetComboValue( i, GetSignalTypeName((ESignalType)i));
	
	// sample rate
	Core::AttributeSettings* attributeSettings = RegisterAttribute("Sample Rate", "sampleRate", "Sample rate of the generated signal.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeSettings->SetDefaultValue( Core::AttributeFloat::Create(defaultSampleRate) );
	attributeSettings->SetMinValue( Core::AttributeFloat::Create(Math::epsilon) );
	attributeSettings->SetMaxValue( Core::AttributeFloat::Create(FLT_MAX) );

	// frequency
	attributeSettings = RegisterAttribute("Frequency", "frequency", "Frequency of the periodic waveforms.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeSettings->SetDefaultValue( Core::AttributeFloat::Create(defaultFrequency) );
	attributeSettings->SetMinValue( Core::AttributeFloat::Create(Math::epsilon) );
	attributeSettings->SetMaxValue( Core::AttributeFloat::Create(FLT_MAX) );

	// amplitude
	attributeSettings = RegisterAttribute("Amplitude", "amplitude", "Amplitude of the waveform (half of peak-to-peak).", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeSettings->SetDefaultValue( Core::AttributeFloat::Create(defaultAmplitude) );
	attributeSettings->SetMinValue( Core::AttributeFloat::Create(0.0) );
	attributeSettings->SetMaxValue( Core::AttributeFloat::Create(FLT_MAX) );
	
	// dc offset
	attributeSettings = RegisterAttribute("DC-Offset", "dcoffset", "DC-Offset of the signal.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeSettings->SetDefaultValue( Core::AttributeFloat::Create(defaultDCOffset) );
	attributeSettings->SetMinValue( Core::AttributeFloat::Create(-FLT_MAX) );
	attributeSettings->SetMaxValue( Core::AttributeFloat::Create(FLT_MAX) );
	
	// dutycycle
	attributeSettings = RegisterAttribute("Duty cycle", "dutycycle", "Duty cycle of the Rectangle Waveform (ratio of On to Off within one period).", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeSettings->SetDefaultValue( Core::AttributeFloat::Create(defaultDutycycle) );
	attributeSettings->SetMinValue( Core::AttributeFloat::Create(0.0) );
	attributeSettings->SetMaxValue( Core::AttributeFloat::Create(FLT_MAX) );


	// hide/show attributes
	ShowAttributesForSignalType(defaultSignalType);
}


void SignalGeneratorNode::Reset()
{
	InputNode::Reset();

	mClock.Stop();
	mClock.Reset();
}


void SignalGeneratorNode::ReInit(const Time& elapsed, const Time& delta)
{
	InputNode::ReInit(elapsed, delta);

	// set output port name
	GetOutputPort(OUTPUTPORT_CHANNEL).SetName(GetSignalTypeName((ESignalType)GetInt32Attribute(ATTRIB_SIGNALTYPE)));

	PostReInit(elapsed, delta);
}


void SignalGeneratorNode::Start(const Time& elapsed)
{
	// call baseclass start
	InputNode::Start(elapsed);

	// configure output channel
	const double sampleRate = GetFloatAttribute(ATTRIB_SAMPLERATE);
	mSensor.GetChannel()->SetSampleRate(sampleRate);

	// construct a name for the channel
	UpdateSensorName();

	// configure and start clock
	mClock.Reset();
	mClock.SetStartTime(elapsed);
	mClock.SetFrequency(sampleRate);
	mClock.Start();
}


// update the node
void SignalGeneratorNode::Update(const Time& elapsed, const Time& delta)
{
	// shared base update helper
	if (BaseUpdate(elapsed, delta) == false)
		return;

	mClock.Update(elapsed, delta);

	InputNode::Update(elapsed, delta);
}


// update the data
void SignalGeneratorNode::OnAttributesChanged()
{
	// set the amplitude
	const double amplitude = GetFloatAttribute(ATTRIB_AMPLITUDE);
	mSensor.GetChannel()->SetMinValue( -amplitude );
	mSensor.GetChannel()->SetMinValue( +amplitude );

	// because name depends on attributes
	UpdateSensorName();

	// in case the sample rate changed, reset the node and channels
	const double sampleRate = GetFloatAttribute(ATTRIB_SAMPLERATE);
	if (mSensor.GetChannel()->GetSampleRate() != sampleRate)
		ResetAsync();

	// show/hide attributes for the selected signal type
	const uint32 signalType = GetInt32Attribute(ATTRIB_SIGNALTYPE);
	ShowAttributesForSignalType((ESignalType)signalType);
	
	// fire events
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_AMPLITUDE)) );
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_FREQUENCY)) );
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_DCOFFSET)) );
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_DUTYCYCLE)) );
}


// the function that fills the sensors with samples
void SignalGeneratorNode::GenerateSamples()
{
	
	// uncomment this to enable a simple latency simulation (only 1 out of n updates generates samples)
	//if (Random::RandD(0, 10) > 1.0)
	//	return;

	// number of samples we have to output
	const uint32 numNewSamples = mClock.GetNumNewTicks();						

	// signal configuration
	const uint32 signalType = GetInt32Attribute(ATTRIB_SIGNALTYPE);
	const double amplitude = GetFloatAttribute(ATTRIB_AMPLITUDE);
	const double frequency = GetFloatAttribute(ATTRIB_FREQUENCY);
	const double dcoffset = GetFloatAttribute(ATTRIB_DCOFFSET);
	const double dutycycle = GetFloatAttribute(ATTRIB_DUTYCYCLE);

	//configure channel
	mSensor.GetChannel()->SetMinValue(dcoffset - amplitude);
	mSensor.GetChannel()->SetMaxValue(dcoffset + amplitude);

	double time, value;

	// create the samples
	switch (signalType)
	{
		// sinusoidal waveform
	case SIGNALTYPE_SINE:
	{
		for (uint32 i = 0; i<numNewSamples; ++i)
		{
			time = mClock.GetTickTime(mClock.PopOldestTick()).InSeconds();

			// calculate the sin of the current time value
			value = dcoffset + Math::Sin(time * Math::twoPi * frequency) * amplitude;

			// add the sample
			mSensor.AddQueuedSample(value);
		}
		break;
	}

		// rectangle function, with duty cycle parameter
	case SIGNALTYPE_SQUARE:
	{
		for (uint32 i = 0; i<numNewSamples; ++i)
		{
			time = mClock.GetTickTime(mClock.PopOldestTick()).InSeconds();

			// generate rectangle wave, scale to amplitude and add dc-offset
			value = dcoffset + amplitude * RectangleWaveFunction(time, frequency, dutycycle);

			mSensor.AddQueuedSample(value);
		}
		break;
	}

		// white noise 
	case SIGNALTYPE_NOISE:
	{
		for (uint32 i = 0; i<numNewSamples; ++i)
		{
			// generate random values (amplitude centered around the dcoffset)
			value = Math::RandD(dcoffset - amplitude, dcoffset + amplitude);

			mSensor.AddQueuedSample(value);
		}
		break;
	}

		// ramp function (monotonously increases by $amplitude per 1/$frequency)
	case SIGNALTYPE_RAMP:
	{
		for (uint32 i = 0; i<numNewSamples; ++i)
		{
			time = mClock.GetTickTime(mClock.PopOldestTick()).InSeconds();

			// generate linear ramp
			value = dcoffset + amplitude * time / frequency;

			mSensor.AddQueuedSample(value);
		}
		break;
	}

		// sawtooth function (like ramp, but periodically)
	case SIGNALTYPE_SAWTOOTH:
	{
		for (uint32 i = 0; i<numNewSamples; ++i)
		{
			time = mClock.GetTickTime(mClock.PopOldestTick()).InSeconds();

			// generate rectangle wave, scale to amplitude and add dc-offset
			value = dcoffset + amplitude * SawtoothWaveFunction(time, frequency);

			mSensor.AddQueuedSample(value);
		}
		break;
	}

		// triangular waveform (like sawtooth, but ramping up AND down in one cycle)
	case SIGNALTYPE_TRIANGLE:
	{
		for (uint32 i = 0; i<numNewSamples; ++i)
		{
			time = mClock.GetTickTime(mClock.PopOldestTick()).InSeconds();

			// generate rectangle wave, scale to amplitude and add dc-offset
			value = dcoffset + amplitude * TriangleWaveFunction(time, frequency);

			mSensor.AddQueuedSample(value);
		}
		break;
	}

		// TODO: remove this or replace it with something better, but leave it for the beta
	case SIGNALTYPE_BRAINWAVE:
	{
		const double a = amplitude;
		const double s = 10;

		for (uint32 i = 0; i<numNewSamples; ++i)
		{
			time = mClock.GetTickTime(mClock.PopOldestTick()).InSeconds();
			value = 0;

			// delta sinuses 0..3
			double var = 1.0 + (sin((double)s + 2.0 * Core::Math::pi * time / 23.0) * sin(1.0 - 2.0 * Core::Math::pi * time / 13.0) + sin((double)s - 2.0 * Core::Math::pi * time / 13.0)) / 2.0;
			for (double j = 0.0; j <= 3.5; j += 0.31)
				value += 0.1 * var * sin(fmod(j, 0.11 + (s + 1)) + 2.0 * Core::Math::pi * j * time) * a;

			// alpha sinuses 8..12
			var = 1.0 + (sin((double)s + 0.001284952 * time) * sin(0.034309 * time) + sin(0.1 + 0.01549 * time) * sin((double)s - 2.0 * Core::Math::pi * (0.0133543) * time)) / 2.0;
			for (double j = 8.0; j <= 12.0; j += 0.23)
				value += 0.1 * var * sin(fmod(j, 0.42 + (s + 1)) + 2.0 * Core::Math::pi * j * time) * a;

			// SMR 13..16
			var = 1.0 + (sin((double)s + 0.0284952 * time) * sin(1.0 + 0.034309 * time) + sin(0.01549 * time) * sin((double)s - 2.0 + 2.0 * Core::Math::pi * (0.033654) * time) * sin(2.0 * Core::Math::pi * (0.00765465) * time)) / 2.0;
			for (double j = 13.0; j <= 16.0; j += 0.17)
				value += 0.1 * var * sin(fmod(j, 0.23 + (s + 1)) + 2.0 * Core::Math::pi * j * time) * a;

			// dc noise
			//value += 0.1 * a * (double)rand() / RAND_MAX;

			// ac noise
			value += 0.3 * a * ((double)rand() / RAND_MAX - 0.5) * 2.0;

			mSensor.AddQueuedSample(value);
		}
		break;
	}

	default: break;
	}

	// mark ticks as processed
	mClock.ClearNewTicks();
}


// construct the name of the sensor/channel
void SignalGeneratorNode::UpdateSensorName()
{
	//const char* signalName = GetSignalTypeName((ESignalType)GetInt32Attribute(ATTRIB_SIGNALTYPE));
	//const double frequency = GetFloatAttribute(ATTRIB_FREQUENCY);
	//mTempString.Format("%s %.2fHz", signalName, frequency); // example: "Sine 5Hz"
	//mSensor.SetName(mTempString.AsChar());

	mSensor.SetName( GetName() );
}


const char* SignalGeneratorNode::GetSignalTypeName (ESignalType type)
{
	switch (type)
	{
		case SIGNALTYPE_SINE:		return "Sine"; break;
		case SIGNALTYPE_SQUARE:		return "Square"; break;
		case SIGNALTYPE_NOISE:		return "Noise"; break;
		case SIGNALTYPE_RAMP:		return "Ramp"; break;
		case SIGNALTYPE_SAWTOOTH:	return "Sawtooth"; break;
		case SIGNALTYPE_TRIANGLE:	return "Triangle"; break;
		case SIGNALTYPE_BRAINWAVE:	return "Brainwaves"; break;
		default:					return "Unknown";
	}

}

// show only the attributes required for this signal type
void SignalGeneratorNode::ShowAttributesForSignalType(ESignalType type)
{
	switch (type)
	{
		// amplitude, frequency, dcoffset
		case SIGNALTYPE_SINE:
		case SIGNALTYPE_SAWTOOTH:
		case SIGNALTYPE_TRIANGLE:
		case SIGNALTYPE_RAMP:
			GetAttributeSettings(ATTRIB_AMPLITUDE)->SetVisible(true);
			GetAttributeSettings(ATTRIB_FREQUENCY)->SetVisible(true);
			GetAttributeSettings(ATTRIB_DCOFFSET)->SetVisible(true);
			GetAttributeSettings(ATTRIB_DUTYCYCLE)->SetVisible(false);
			break;

		// squarewave has also dutycycle
		case SIGNALTYPE_SQUARE:
			GetAttributeSettings(ATTRIB_AMPLITUDE)->SetVisible(true);
			GetAttributeSettings(ATTRIB_FREQUENCY)->SetVisible(true);
			GetAttributeSettings(ATTRIB_DCOFFSET)->SetVisible(true);
			GetAttributeSettings(ATTRIB_DUTYCYCLE)->SetVisible(true);

			break;

		// noise and brainwave have no frequency
		case SIGNALTYPE_NOISE:
		case SIGNALTYPE_BRAINWAVE:
			GetAttributeSettings(ATTRIB_AMPLITUDE)->SetVisible(true);
			GetAttributeSettings(ATTRIB_DCOFFSET)->SetVisible(true);
			GetAttributeSettings(ATTRIB_FREQUENCY)->SetVisible(false);
			GetAttributeSettings(ATTRIB_DUTYCYCLE)->SetVisible(false);
			break;

		default:
			break;
	}

}
