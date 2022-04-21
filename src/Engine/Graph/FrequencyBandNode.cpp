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
#include "FrequencyBandNode.h"
#include "../Core/Math.h"
#include "../EngineManager.h"
#include "SPNode.h"


using namespace Core;

// constructor
FrequencyBandNode::FrequencyBandNode(Graph* graph) : ProcessorNode(graph, new FrequencyBandNode::Processor())
{
	mSettings.mBand.SetFromFrequencyBand( GetEngine()->GetSpectrumAnalyzerSettings()->GetFrequencyBand(0) );
	mSettings.mValueType = VALUE_AMPLITUDE;
}


// destructor
FrequencyBandNode::~FrequencyBandNode()
{
}


// initialize the node
void FrequencyBandNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	// SETUP PORTS

	GetInputPort(INPUTPORT_SPECTRUM).Setup("In (Spectrum)", "x", AttributeChannels<Spectrum>::TYPE_ID, PORTID_INPUT_SPECTRUM);
	GetOutputPort(OUTPUTPORT_CHANNEL).Setup("Out", "y", AttributeChannels<double>::TYPE_ID, PORTID_OUTPUT_CHANNEL);
	
	// ATTRIBUTES

	// value types
	Core::AttributeSettings* valueAttr = RegisterAttribute( "Value", "ValueType", "The value to average.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX );
	valueAttr->ResizeComboValues(3);
	valueAttr->SetComboValue(VALUE_AMPLITUDE, "Average Amplitude");
	valueAttr->SetComboValue(VALUE_POWER, "Average Power");
	valueAttr->SetComboValue(VALUE_PHASE, "Average Phase");
	valueAttr->SetDefaultValue(Core::AttributeInt32::Create(0));

	// frequency bands
	SpectrumAnalyzerSettings* settings = GetEngine()->GetSpectrumAnalyzerSettings();
	const uint32 numBands = settings->GetNumFrequencyBands();
	Core::AttributeSettings* bandsAttr = RegisterAttribute( "Band", "FrequencyBands", "Brainwave Frequencyband presets.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX );
	bandsAttr->ResizeComboValues( numBands + 2 );
	for (uint32 i = 0; i < numBands; ++i)
		bandsAttr->SetComboValue(i, settings->GetFrequencyBand(i)->GetName());
	bandsAttr->SetComboValue(numBands, "Custom (Fixed)");
	bandsAttr->SetComboValue(numBands+1, "Custom (Dynamic Input)");
	bandsAttr->SetDefaultValue(Core::AttributeInt32::Create(0));

	// upper frequency
	Core::AttributeSettings* maxFreqAttr = RegisterAttribute( "Upper Frequency", "MaxFrequency", "The upper bound of the frequency range.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSLIDER );
	maxFreqAttr->SetMinValue(Core::AttributeFloat::Create(0));
	maxFreqAttr->SetMaxValue(Core::AttributeFloat::Create(200));
	maxFreqAttr->SetDefaultValue(Core::AttributeFloat::Create(mSettings.mBand.GetMaxFrequency()));

	// lower frequency
	Core::AttributeSettings* minFreqAttr = RegisterAttribute( "Lower Frequency", "MinFrequency", "The lower bound of the frequency range.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSLIDER );
	minFreqAttr->SetMinValue(Core::AttributeFloat::Create(0));
	minFreqAttr->SetMaxValue(Core::AttributeFloat::Create(200));
	minFreqAttr->SetDefaultValue(Core::AttributeFloat::Create(mSettings.mBand.GetMinFrequency()));

	// wizard selectable attribute
	Core::AttributeSettings* attribQuickConfig = RegisterAttribute("Wizard Selectable", "wizardselectable", "Selectable in Experience Wizard", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attribQuickConfig->SetDefaultValue(Core::AttributeBool::Create(false));
}


void FrequencyBandNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	// set defaul frequency band from attribute first
	mSettings.mBand.SetMinFrequency( GetFloatAttribute(ATTRIB_MINFREQ) );
	mSettings.mBand.SetMaxFrequency( GetFloatAttribute(ATTRIB_MAXFREQ) );

	PostReInit(elapsed, delta);
}


void FrequencyBandNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// get the frequency band settings from the inputs (if there are any) and reconfigure processors 
	if (GetNumInputPorts() == 3)
	{
		bool settingsChanged = false;

		// get value from inputs
		if (GetInputPort(INPUTPORT_MAX_FREQUENCY).HasConnection() && GetInputPort(INPUTPORT_MAX_FREQUENCY).GetChannels()->GetNumChannels() > 0)
		{
			Channel<double>* maxFreqChannel = GetInputPort(INPUTPORT_MAX_FREQUENCY).GetChannels()->GetChannel(0)->AsType<double>();
			if (maxFreqChannel->GetNumSamples() > 0)
			{
				const double maxFreq = maxFreqChannel->GetLastSample();
				mSettings.mBand.SetMaxFrequency( maxFreq );
				settingsChanged = true;
				ChannelReader* reader = mInputReader.FindReader(maxFreqChannel);
				CORE_ASSERT(reader != NULL);
				reader->Flush();
			}
		}

		if (GetInputPort(INPUTPORT_MIN_FREQUENCY).HasConnection() && GetInputPort(INPUTPORT_MIN_FREQUENCY).GetChannels()->GetNumChannels() > 0)
		{
			Channel<double>* minFreqChannel = GetInputPort(INPUTPORT_MIN_FREQUENCY).GetChannels()->GetChannel(0)->AsType<double>();
			if (minFreqChannel->GetNumSamples() > 0)
			{
				const double minFreq = minFreqChannel->GetLastSample();
				mSettings.mBand.SetMinFrequency( minFreq );
				settingsChanged = true;
				ChannelReader* reader = mInputReader.FindReader(minFreqChannel);
				CORE_ASSERT(reader != NULL);
				reader->Flush();
			}
		}

		if (settingsChanged == true)
			ProcessorNode::SetupProcessors();
	}

	// update baseclass
	ProcessorNode::Update(elapsed, delta);
}


// update the data
void FrequencyBandNode::OnAttributesChanged()
{
	SpectrumAnalyzerSettings* settings = GetEngine()->GetSpectrumAnalyzerSettings();
	const uint32 averageType = GetInt32Attribute( ATTRIB_VALUETYPE );
	const uint32 numBands = settings->GetNumFrequencyBands();
	const uint32 selectedPreset = GetInt32Attribute( ATTRIB_BAND );

	mSettings.mValueType = (EValueType)averageType;

	// detect changes in selected preset
	if (selectedPreset != mSelectedPreset)
	{
		mSelectedPreset = selectedPreset;
		
		UpdateInputPorts();

		// if a preset is selected, use the preset values and not the attributes
		if (mSelectedPreset < settings->GetNumFrequencyBands() - 1)
		{

			// copy over values from preset frequency band
			mSettings.mBand.SetFromFrequencyBand(settings->GetFrequencyBand(mSelectedPreset));
			SetFloatAttribute( "MinFrequency", mSettings.mBand.GetMinFrequency() );
			SetFloatAttribute( "MaxFrequency", mSettings.mBand.GetMaxFrequency() );

			// fire event
			EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_MINFREQ)) );
			EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_MAXFREQ)) );

			// update settings
			SetupProcessors();

			return;
		}

	}

	// HACK no support for float frequency ranges yet! (we'll later use filters for that)
	// detect changes in min/max value and name
	const double minFreq = GetFloatAttribute( ATTRIB_MINFREQ );
	const double maxFreq = GetFloatAttribute( ATTRIB_MAXFREQ );

	bool attributesChanged = false;
	
	if (Math::Abs( minFreq - mSettings.mBand.GetMinFrequency() ) > 10E-4)
		attributesChanged = true;
	if (Math::Abs( maxFreq - mSettings.mBand.GetMaxFrequency() ) > 10E-4)
		attributesChanged = true;
	
	// apply changes to frequency band member
	if (attributesChanged == true && selectedPreset < numBands)
	{
		// set frequency range
		mSettings.mBand.SetMinFrequency(minFreq);
		mSettings.mBand.SetMaxFrequency(maxFreq);

		// set preset combobox to "custom"
		SetInt32Attribute( "FrequencyBands", numBands );	

		// fire event
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_MINFREQ)) );
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_MAXFREQ)) );
	}
	
	// make sure minfreq < maxFreq
	if (minFreq > maxFreq)
	{
		SetFloatAttribute("MinFrequency", maxFreq);
		attributesChanged = true;
		
		// fire event
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_MINFREQ)) );
	}

	// update settings
	SetupProcessors();
}



void FrequencyBandNode::UpdateInputPorts()
{
	const uint32 customDynamicIndex = GetEngine()->GetSpectrumAnalyzerSettings()->GetNumFrequencyBands() + 1;
	const bool showRangeInputs = ((uint32)GetInt32Attribute(ATTRIB_BAND) == customDynamicIndex);

	if (showRangeInputs && GetNumInputPorts() == 1)
	{
		InitInputPorts(3);
		GetInputPort(INPUTPORT_MAX_FREQUENCY).Setup("Max Frequency", "MaxFrequency", AttributeChannels<double>::TYPE_ID, INPUTPORT_MAX_FREQUENCY);
		GetInputPort(INPUTPORT_MIN_FREQUENCY).Setup("Min Frequency", "MinFrequency", AttributeChannels<double>::TYPE_ID, INPUTPORT_MIN_FREQUENCY);
	}
	else if (showRangeInputs == false)
	{
		InitInputPorts(1);
	}

}


void FrequencyBandNode::Processor::ReInit() 
{
	// ReInit baseclass
	ChannelProcessor::ReInit();

	mIsInitialized = false;
	ChannelBase* input = GetInput();
	Channel<double>* output = GetOutput()->AsType<double>();

	// nothing to do until both channels are connected
	if (input == NULL || output == NULL)
		return;

	output->SetSampleRate(input->GetSampleRate());

	mIsInitialized = true;
}


// calculate one average magnitude per incoming spectrum
void FrequencyBandNode::Processor::Update()
{
	if (mIsInitialized == false)
		return;

	// update base
	ChannelProcessor::Update();

	ChannelReader* input = GetInputReader(0);
	Channel<double>* output = GetOutput()->AsType<double>();

	const uint32 numNewSpectrums = input->GetNumNewSamples();

	switch (mSettings.mValueType)
	{
		case VALUE_AMPLITUDE:

			for (uint32 i = 0; i<numNewSpectrums; i++)
			{
				const Spectrum&  spectrum = input->PopOldestSample<Spectrum>();
				double magnitude = mSettings.mBand.CalcMagnitude(&spectrum);
				output->AddSample(magnitude);
			}
			break;

		case VALUE_POWER:

			for (uint32 i = 0; i<numNewSpectrums; i++)
			{
				const Spectrum&  spectrum = input->PopOldestSample<Spectrum>();
				double magnitude = mSettings.mBand.CalcPower(&spectrum);
				output->AddSample(magnitude);
			}
			break;

		case VALUE_PHASE:

			for (uint32 i = 0; i<numNewSpectrums; i++)
			{
				const Spectrum&  spectrum = input->PopOldestSample<Spectrum>();
				double magnitude = mSettings.mBand.CalcPhase(&spectrum);
				output->AddSample(magnitude);
			}
			break;
	}

}
