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
#include "DominantFrequencyNode.h"
#include "../Core/Math.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
DominantFrequencyNode::DominantFrequencyNode(Graph* graph) : ProcessorNode(graph, new DominantFrequencyNode::Processor())
{
	mSettings.mBand.SetFromFrequencyBand( GetEngine()->GetSpectrumAnalyzerSettings()->GetFrequencyBand(0) );
}


// destructor
DominantFrequencyNode::~DominantFrequencyNode()
{
}


// initialize the node
void DominantFrequencyNode::Init()
{
	// init base class first
	ProcessorNode::Init();
	
	// SETUP PORTS

	GetInputPort(INPUTPORT_SPECTRUM).Setup("In (Spectrum)", "x", AttributeChannels<Spectrum>::TYPE_ID, PORTID_INPUT_SPECTRUM);
	GetOutputPort(OUTPUTPORT_CHANNEL).Setup("Out", "y", AttributeChannels<double>::TYPE_ID,  PORTID_OUTPUT_CHANNEL);
	
	// ATTRIBUTES

	// frequency bands
	SpectrumAnalyzerSettings* settings = GetEngine()->GetSpectrumAnalyzerSettings();
	const uint32 numBands = settings->GetNumFrequencyBands();
	Core::AttributeSettings* bandsAttr = RegisterAttribute( "Preset", "FrequencyBands", "Brainwave Frequencyband presets.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX );
	bandsAttr->ResizeComboValues( numBands+1 );
	for (uint32 i = 0; i < numBands; ++i)
		bandsAttr->SetComboValue( i, settings->GetFrequencyBand(i)->GetName() );
	bandsAttr->SetComboValue( numBands, "Custom" );
	bandsAttr->SetDefaultValue( Core::AttributeInt32::Create(0) );

	// lower frequency
	Core::AttributeSettings* minFreqAttr = RegisterAttribute( "Lower Frequency", "MinFrequency", "The lower bound of the frequency range.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSLIDER );
	minFreqAttr->SetMinValue( Core::AttributeFloat::Create(0) );
	minFreqAttr->SetMaxValue( Core::AttributeFloat::Create(200) );
	minFreqAttr->SetDefaultValue( Core::AttributeFloat::Create( mSettings.mBand.GetMinFrequency() ) );

	// upper frequency
	Core::AttributeSettings* maxFreqAttr = RegisterAttribute( "Upper Frequency", "MaxFrequency", "The upper bound of the frequency range.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSLIDER );
	maxFreqAttr->SetMinValue( Core::AttributeFloat::Create(0) );
	maxFreqAttr->SetMaxValue( Core::AttributeFloat::Create(200) );
	maxFreqAttr->SetDefaultValue( Core::AttributeFloat::Create( mSettings.mBand.GetMaxFrequency() ) );
}


void DominantFrequencyNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


// update the node
void DominantFrequencyNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update the baseclass
	ProcessorNode::Update(elapsed, delta);
}


// update the data
void DominantFrequencyNode::OnAttributesChanged()
{
	SpectrumAnalyzerSettings* settings = GetEngine()->GetSpectrumAnalyzerSettings();
	const uint32 numBands = settings->GetNumFrequencyBands();
	const uint32 selectedPreset = GetInt32Attribute( ATTRIB_PRESET );

	// detect changes in selected preset
	if (selectedPreset != mSelectedPreset)
	{
		mSelectedPreset = selectedPreset;

		// if a preset is selected, use the preset values and not the attributes
		if (mSelectedPreset != settings->GetNumFrequencyBands())
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
	const int32 minFreq = GetFloatAttribute(ATTRIB_MINFREQ);
	const int32 maxFreq = GetFloatAttribute(ATTRIB_MAXFREQ);

	bool attributesChanged = false;
	
	if (Math::Abs( minFreq - mSettings.mBand.GetMinFrequency()) > 10E-4)
		attributesChanged = true;
	if (Math::Abs( maxFreq - mSettings.mBand.GetMaxFrequency()) > 10E-4)
		attributesChanged = true;
	
	// apply changes to frequency band member
	if (attributesChanged == true)
	{
		// set frequency range
		mSettings.mBand.SetMinFrequency(minFreq);
		mSettings.mBand.SetMaxFrequency(maxFreq);

		// set preset combobox to "custom"
		SetInt32Attribute("FrequencyBands", numBands);	

		// fire events
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_MINFREQ)) );
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_MAXFREQ)) );
	}

	// make sure minfreq < maxFreq
	if (minFreq > maxFreq)
	{
		SetFloatAttribute("MinFrequency", maxFreq);
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_MINFREQ)) );
	}
	// update settings
	SetupProcessors();
}



void DominantFrequencyNode::Processor::ReInit()
{
	// ReInit baseclass
	ChannelProcessor::ReInit();

	mIsInitialized = false;
	ChannelBase* input = GetInput();
	ChannelBase* output = GetOutput();

	// nothing to do until both channels are connected
	if (input == NULL || output == NULL)
		return;

	output->SetSampleRate(input->GetSampleRate());

	mIsInitialized = true;
}


// calculate one average magnitude per incoming spectrum
void DominantFrequencyNode::Processor::Update()
{
	if (mIsInitialized == false)
		return;

	// update base
	ChannelProcessor::Update();

	ChannelReader* input = GetInputReader(0);
	Channel<double>* output = GetOutput()->AsType<double>();

	const uint32 numNewSpectrums = input->GetNumNewSamples();
	for (uint32 i = 0; i<numNewSpectrums; i++)
	{
		const Spectrum&  spectrum = input->PopOldestSample<Spectrum>();
		const double dominantFrequency = spectrum.CalcDominantFrequency(mSettings.mBand.GetMinFrequency(), mSettings.mBand.GetMaxFrequency());
		output->AddSample(dominantFrequency);
	}
}
