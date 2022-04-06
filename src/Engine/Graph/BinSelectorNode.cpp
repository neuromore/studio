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
#include "BinSelectorNode.h"
#include "../Core/Math.h"
#include "../EngineManager.h"
#include "SPNode.h"


using namespace Core;

// constructor
BinSelectorNode::BinSelectorNode(Graph* graph) : SPNode(graph)
{
	mUseMultiChannel = true;
	mFrequencyBand.SetMinFrequency(0);
	mFrequencyBand.SetMaxFrequency(128);
}


// destructor
BinSelectorNode::~BinSelectorNode()
{
	DeleteOutputChannels();
}


// initialize the node
void BinSelectorNode::Init()
{
	// init base class first
	SPNode::Init();

	RequireInputConnection();

	// PORTS
	InitInputPorts(1);
	GetInputPort(INPUTPORT_SPECTRUM).Setup("Spectrum", "x", AttributeChannels<Spectrum>::TYPE_ID, PORTID_INPUT_SPECTRUM);
	
	const uint32 numPortsDefault = 0;
	InitOutputPorts(numPortsDefault);
	
	// ATTRIBUTES

	// frequency bands
	Core::AttributeSettings* binPortsAttr = RegisterAttribute( "Use Multichannel", "useMultiChannel", "Bundle the selected spectrum bins in a multichannel instead of using one port per bin.", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX );
	binPortsAttr->SetDefaultValue(Core::AttributeBool::Create(mUseMultiChannel));

	// lower frequency
	Core::AttributeSettings* minFreqAttr = RegisterAttribute( "Lower Frequency", "MinFrequency", "The lower bound of the frequency range.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSLIDER );
	minFreqAttr->SetMinValue(Core::AttributeFloat::Create(0));
	minFreqAttr->SetMaxValue(Core::AttributeFloat::Create(200));
	minFreqAttr->SetDefaultValue(Core::AttributeFloat::Create(mFrequencyBand.GetMinFrequency()));

	// upper frequency
	Core::AttributeSettings* maxFreqAttr = RegisterAttribute("Upper Frequency", "MaxFrequency", "The upper bound of the frequency range.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSLIDER);
	maxFreqAttr->SetMinValue(Core::AttributeFloat::Create(0));
	maxFreqAttr->SetMaxValue(Core::AttributeFloat::Create(200));
	maxFreqAttr->SetDefaultValue(Core::AttributeFloat::Create(mFrequencyBand.GetMaxFrequency()));

	// hidden port number attribute
	Core::AttributeSettings* attribNumPorts = RegisterAttribute("", "numOutputPorts", "", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	attribNumPorts->SetDefaultValue(Core::AttributeInt32::Create(numPortsDefault));
	attribNumPorts->SetMinValue(Core::AttributeInt32::Create(0));
	attribNumPorts->SetMaxValue(Core::AttributeInt32::Create(INT_MAX));
	attribNumPorts->SetVisible(false);
	
	// lock ports attribute
	Core::AttributeSettings* attribLockPorts = RegisterAttribute("Lock Ports", "lockPorts", "", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attribLockPorts->SetDefaultValue(Core::AttributeBool::Create(false));
}


void BinSelectorNode::Reset()
{
	SPNode::Reset();

	DeleteOutputChannels();
}


void BinSelectorNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	SPNode::ReInit(elapsed, delta);

	if (FindNumInputChannels() > 0)
	{
		// copy input spectrum for further reference (we only care about layout, not content)
		mConfigSpectrum = GetInputPort(INPUTPORT_SPECTRUM).GetChannels()->GetChannel(0)->AsType<Spectrum>()->GetLastSample();
		mIsInitialized = true;
	}
	else
	{
		// can't initialize until we received a sample
		mIsInitialized = false;
	}

	PostReInit(elapsed, delta);
}


void BinSelectorNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update baseclass
	SPNode::Update(elapsed, delta);

	// nothing to do if node is not initialized
	if (mIsInitialized == false)
		return;	

	// process all newly arrived input spectra
	const uint32 minBinIndex = mConfigSpectrum.CalcBinIndex(mFrequencyBand.GetMinFrequency());
	const uint32 maxBinIndex = mConfigSpectrum.CalcBinIndex(mFrequencyBand.GetMaxFrequency());
	const int32 numBins = (maxBinIndex - minBinIndex + 1);
	const uint32 numInputChannels = FindNumInputChannels();

	for (uint32 i = 0; i < (uint32)numInputChannels; ++i)
	{
		ChannelReader* reader = mInputReader.GetReader(i);

		const uint32 numSamples = reader->GetNumNewSamples();
		for (uint32 s = 0; s < numSamples; ++s)
		{
			// copy bin values to the output channels
			const Spectrum& spectrum = reader->PopOldestSample<Spectrum>();
			for (uint32 b = 0; b < (uint32)numBins; ++b)
			{
				const double binValue = spectrum.GetBin(minBinIndex + b);
				mOutputChannels[i*numBins + b].AddSample(binValue);
			}
		}
	}
}


// update the data
void BinSelectorNode::OnAttributesChanged()
{
	// INIT dynamic output ports
	// create and name output ports if there are none
	const uint32 numPorts = GetInt32Attribute(ATTRIB_NUMOUTPUTPORTS);
	if (GetNumOutputPorts() == 0)
	{
		InitOutputPorts(numPorts);
		for (uint32 i = 0; i < numPorts; ++i)
		{
			mTempString.Format("y%i", i);
			GetOutputPort(i).SetupAsChannels<double>("?", mTempString.AsChar(), i);
		}
	}

	// HACK no support for float frequency ranges yet! (we'll later use filters for that)
	// detect changes in min/max value and name
	const bool useMultiChannel	= GetBoolAttribute( ATTRIB_USEMULTICHANNEL );
	const uint32 minFreq		= GetFloatAttribute( ATTRIB_MINFREQ );
	const uint32 maxFreq		= GetFloatAttribute( ATTRIB_MAXFREQ );

	// nothing changed -> nothing to do
	if (useMultiChannel == mUseMultiChannel &&
		Math::Abs(minFreq - (uint32)mFrequencyBand.GetMinFrequency()) < 10E-4 &&
		Math::Abs(maxFreq - (uint32)mFrequencyBand.GetMaxFrequency()) < 10E-4)
	{
		return;
	}
	
	// update settings
	mUseMultiChannel = useMultiChannel;

	if (mFrequencyBand.GetMinFrequency() != minFreq)
	{
		mFrequencyBand.SetMinFrequency(minFreq);
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_MINFREQ)) );
	}

	if (mFrequencyBand.GetMaxFrequency() != maxFreq)
	{
		mFrequencyBand.SetMaxFrequency(maxFreq);
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_MAXFREQ)) );
	}
	
	// make sure that minfreq < maxFreq
	if (minFreq > maxFreq)
	{
		SetFloatAttribute("MinFrequency", maxFreq);
		mFrequencyBand.SetMinFrequency(maxFreq);
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_MINFREQ)) );
	}

	// reinit
	ResetAsync();
}


// create the appropriate number output bins, name them and forward the input channels
void BinSelectorNode::Start(const Time& elapsed)
{
	// create output ports and channels for the current bin config
	
	// do not reinit outputs if ports are locked
	const bool isLocked = GetBoolAttribute(ATTRIB_LOCK_PORTS);
	if (isLocked == false)
		ReInitOutputPorts(mConfigSpectrum, mFrequencyBand);
	ReInitOutputChannels(mConfigSpectrum, mFrequencyBand);

	// remember number of ports
	SetInt32Attribute("numOutputPorts", GetNumOutputPorts());

	// Note: we have to call SPNode::Start after we created the channels
	SPNode::Start(elapsed);
}


// create and name output ports
void BinSelectorNode::ReInitOutputPorts(const Spectrum& spectrum, const FrequencyBand& band)
{
	uint32 minBinIndex = mConfigSpectrum.CalcBinIndex(mFrequencyBand.GetMinFrequency());
	uint32 maxBinIndex = mConfigSpectrum.CalcBinIndex(mFrequencyBand.GetMaxFrequency());
	int32 numBins = (maxBinIndex - minBinIndex + 1);
	const uint32 numChannels = FindNumInputChannels();

	if (numBins > 0)
	{
		// a) create one multichannel for each single input channel 
		if (mUseMultiChannel == true)
		{

			InitOutputPorts(numChannels);

			for (uint32 i = 0; i < numChannels; ++i)
			{
				mTempString.Format("y%i", i);
				GetOutputPort(i).SetupAsChannels<double>("", mTempString.AsChar(), 0);
				mTempString.Format("Bins Ch%i", i);
				GetOutputPort(i).SetName(mTempString.AsChar());
			}

			// overwrite with cleaner name
			if (numChannels == 1)
				GetOutputPort(0).SetName("Bins");
		}
		else
		{
			InitOutputPorts(numBins);
			
			// setup port names: bin frequency
			for (uint32 i = 0; i < (uint32)numBins; ++i)	// includes bin at upper index
			{
				// create and setup port
				mTempString.Format("y%i", i + 1);	// internal name
				GetOutputPort(i).SetupAsChannels<double>("", mTempString.AsChar(), i);
				
				// updat port name with bin frequency
				const double frequency = spectrum.CalcFrequency(minBinIndex + i);
				mTempString.Format("%.1fHz", frequency);
				GetOutputPort(i).SetName(mTempString.AsChar());
			}
		}
	} 
	else 
	{
		// During node loading, we ignore that there is no input connection. We _have_ to create the port configuration that the node was in while saving, so the other connections can be loaded
		const uint32 numOutPorts = GetNumOutputPorts();
		if (numBins > 0 && numOutPorts == 0)
			InitOutputPorts(numBins);

		// set port names to '?'
		for (uint32 i = 0; i < numOutPorts; ++i)	
			GetOutputPort(i).SetName("?");
	}
}


// call this only _after_ ReInitOutputPorts()
void BinSelectorNode::ReInitOutputChannels(const Spectrum& spectrum, const FrequencyBand& band)
{
	const uint32 minBinIndex = GetMinBinIndex(spectrum, band);
	const uint32 numBins = GetNumBins(spectrum, band);
	const uint32 numInputChannels = FindNumInputChannels();

	// 1) create output channels
	const uint32 numOutputChannels = numInputChannels * numBins;
	mOutputChannels.Resize(numOutputChannels);

	// 2) config output channels
	for (uint32 i = 0; i < numInputChannels; ++i)
	{
		for (uint32 b = 0; b < numBins; ++b)
		{
			// configure output channel to same parameters as input channel
			Channel<Spectrum>* inputChannel = GetInputPort(INPUTPORT_SPECTRUM).GetChannels()->GetChannel(i)->AsType<Spectrum>();
			Channel<double>* outputChannel = &mOutputChannels[i * numBins + b];
			outputChannel->SetBufferSize(10);	// set any buffersize > 0
			outputChannel->SetSampleRate(inputChannel->GetSampleRate());
			outputChannel->SetColor(inputChannel->GetColor());

			// bin frequency as channel name
			mTempString.Format("%.1fHz", spectrum.CalcFrequency(minBinIndex + b));	// respect bin offset
			outputChannel->SetName(mTempString.AsChar());
		}
	}

	// 3) connect channels to output ports
	if (mUseMultiChannel == true)
	{
		// make sure we did everything correctly
		CORE_ASSERT(GetNumOutputPorts() == numInputChannels);
		
		// clear multichannels and add all single channels
		for (uint32 i = 0; i < numInputChannels; ++i)
		{
			MultiChannel* channels = GetOutputPort(i).GetChannels();
			channels->Clear();

			// add all output channels 
			for (uint32 b = 0; b < numBins; ++b)
				channels->AddChannel(&mOutputChannels[i*numBins + b]);
		}
	}
	else
	{
		// one port per single channel

		const uint32 numOutputPorts = GetNumOutputPorts();
		CORE_ASSERT(GetNumOutputPorts() == numBins);
	
		// connect channels
		for (uint32 p = 0; p < numOutputPorts; ++p)
		{ 
			// clear multichannel first
			MultiChannel* channels = GetOutputPort(p).GetChannels();
			channels->Clear();
			
			for (uint32 i = 0; i < numInputChannels; ++i)
			{
				// index safety
				const uint32 index = i*numBins + p;
				if (index < numOutputChannels)
					channels->AddChannel(&mOutputChannels[index]);
			}
		}
	}
}


void BinSelectorNode::DeleteOutputChannels()
{
	// delete all output channels	
	mOutputChannels.Clear();

	// remove channel references from all output ports
	const uint32 numPorts = GetNumOutputPorts();
	for (uint32 i = 0; i < numPorts; ++i)
	{
		Port& outPort = GetOutputPort(i);
		MultiChannel* outputSet = outPort.GetChannels();
		outputSet->Clear();
	}
}

// find number of valid input channels, falls back to 1 or 0, depending on case of failure
uint32 BinSelectorNode::FindNumInputChannels()
{
	Port& inport = GetInputPort(INPUTPORT_SPECTRUM);
	// no connection
	if (inport.HasConnection() == false || inport.GetChannels() == NULL)
		return 0;
	
	// have connection, but multichannel has no channels
	if (inport.GetChannels()->GetNumChannels() == 0)
		return 0;

	// only one input channel (simple case, spectra don't have to be checked)
	else if (inport.GetChannels()->GetNumChannels() == 1)
		if (inport.GetChannels()->GetChannel(0)->IsEmpty())
			return 0;	// no sample yet..
		else
			return 1;

	// make sure all channels have at least one spectrum, and that their sizes match
	else
	{
		// no sample yet...
		if (inport.GetChannels()->GetChannel(0)->IsEmpty() == true)
			return 0;

		// get spectrum from first channel
		const Spectrum& spectrum = inport.GetChannels()->GetChannel(0)->AsType<Spectrum>()->GetLastSample();

		// compare all other spectra against it 
		const uint32 numChannels = inport.GetChannels()->GetNumChannels();
		for (uint32 i = 0; i < numChannels; ++i)
		{
			// ne channel is empty (but the first one isnt.. try again later)
			if (inport.GetChannels()->GetChannel(i)->IsEmpty() == true)
				return 0;

			const Spectrum& otherSpectrum = inport.GetChannels()->GetChannel(i)->AsType<Spectrum>()->GetLastSample();

			// in case spectra don't match up, we default to single channel
			// NOTE we should show an error in this case
			if (otherSpectrum.GetMaxFrequency() != spectrum.GetMaxFrequency() || otherSpectrum.GetNumBins() != spectrum.GetNumBins())
				return 1;
		}

		// success, we can use all channels
		return numChannels;
	}
}
