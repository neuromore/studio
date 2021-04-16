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
#include "Spectrogram2DPlugin.h"
#include <AttributeWidgets/AttributeSetGridWidget.h>
#include <Core/LogManager.h>


using namespace Core;

// constructor
Spectrogram2DPlugin::Spectrogram2DPlugin() : Plugin(GetStaticTypeUuid())
{
	LogDetailedInfo("Constructing raw spectrum plugin ...");

	mChannelSelectionWidget	= NULL;
	mSpectrumWidget			= NULL;
}


// destructor
Spectrogram2DPlugin::~Spectrogram2DPlugin()
{
	LogDetailedInfo("Destructing raw spectrum plugin ...");
	
	// dealloc spectrum analyzers (simple solution)
	const uint32 numAnalyzers = mSpectrumAnalyzers.Size();
	for (uint32 i=0; i<numAnalyzers; ++i)
		delete mSpectrumAnalyzers[i];

	mSpectrumAnalyzers.Clear();
	mAverageSpectra.Clear();
}


// register attributes and create the default values
void Spectrogram2DPlugin::RegisterAttributes()
{
	// register base class attributes
	Plugin::RegisterAttributes();

	// multi view
	AttributeSettings* attributeSettings = RegisterAttribute("Multi View", "multiView", "", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeSettings->SetDefaultValue( AttributeBool::Create(false) );

	// show zero Hz bin
	attributeSettings = RegisterAttribute("Show 0Hz Bin", "show0HzBin", "", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeSettings->SetDefaultValue( AttributeBool::Create(true) );

	// average interval
	attributeSettings = RegisterAttribute("Average Interval", "averageInterval", "", ATTRIBUTE_INTERFACETYPE_FLOATSLIDER);
	attributeSettings->SetDefaultValue( AttributeFloat::Create(1.0f) );
	attributeSettings->SetMinValue( AttributeFloat::Create(0.0f) );
	attributeSettings->SetMaxValue( AttributeFloat::Create(10.0f) );

	// create default attribute values
	CreateDefaultAttributeValues();
}


// called in case any of the attributes got changed
void Spectrogram2DPlugin::OnAttributeChanged(Property* property)
{
	const String& propertyInternalName = property->GetAttributeSettings()->GetInternalNameString();

	// color mapping
	if (propertyInternalName.IsEqual("averageInterval") == true)
		SetAverageInterval( property->AsFloat() );

	// multi view
	if (propertyInternalName.IsEqual("multiView") == true)
		SetMultiView( GetMultiView() );
}


// initialize the plugin
bool Spectrogram2DPlugin::Init()
{
	LogDetailedInfo("Initializing spectrogram plugin ...");

	QWidget*		mainWidget		= NULL;
	QHBoxLayout*	mainLayout		= NULL;
	CreateDockMainWidget( &mainWidget, &mainLayout );

	///////////////////////////////////////////////////////////////////////////
	// Toolbar (top-left)
	///////////////////////////////////////////////////////////////////////////

	Core::Array<QWidget*> toolbarWidgets;

	// init checkbox widget
	mChannelSelectionWidget = new ChannelMultiSelectionWidget(mainWidget);
	toolbarWidgets.Add(mChannelSelectionWidget);
	connect(mChannelSelectionWidget, SIGNAL(ChannelSelectionChanged()), this, SLOT(OnChannelSelectionChanged()));
	mChannelSelectionWidget->SetAutoSelectType(ChannelMultiSelectionWidget::AutoSelectType::SELECT_ALL);
	mChannelSelectionWidget->SetShowNeuroChannelsOnly(true);
	mChannelSelectionWidget->Init();

	///////////////////////////////////////////////////////////////////////////
	// Settings
	///////////////////////////////////////////////////////////////////////////

	// create the attribute set grid widget 
	AttributeSetGridWidget* attributeSetGridWidget = new AttributeSetGridWidget( GetDockWidget() );
	attributeSetGridWidget->ReInit(this);
	connect( attributeSetGridWidget->GetPropertyManager(), SIGNAL(ValueChanged(Property*)), this, SLOT(OnAttributeChanged(Property*)) );

	SetSettingsWidget( attributeSetGridWidget );

	///////////////////////////////////////////////////////////////////////////
	// Add render widget at the end
	///////////////////////////////////////////////////////////////////////////

	mSpectrumWidget = new Spectrogram2DWidget( this, mainWidget );
	
	///////////////////////////////////////////////////////////////////////////
	// Fill everything
	///////////////////////////////////////////////////////////////////////////
	FillLayouts(mainWidget, mainLayout, toolbarWidgets, "Settings", "Gear", mSpectrumWidget);

	// reinitialize
	ReInit();
	
	LogDetailedInfo("Spectrogram plugin successfully initialized");
	return true;
}


void Spectrogram2DPlugin::RealtimeUpdate()
{
	// must always update spectrum analyzers
	const uint32 numAnalyzers = mSpectrumAnalyzers.Size();
	for (uint32 i = 0; i < numAnalyzers; ++i)
		mSpectrumAnalyzers[i]->Update();

	if (mSpectrumWidget != NULL && mSpectrumWidget->isVisible() == true)
		mSpectrumWidget->update();
}


void Spectrogram2DPlugin::ReInit()
{
	// clear channel list
	mChannels.Clear();

	// dealloc spectrum analyzers
	const uint32 numAnalyzers = mSpectrumAnalyzers.Size();
	for (uint32 i = 0; i < numAnalyzers; ++i)
		delete mSpectrumAnalyzers[i];
	mSpectrumAnalyzers.Clear();

	mAverageSpectra.Clear();

	Array<Channel<double>*> channels = mChannelSelectionWidget->GetSelectedChannels();
	const FFTProcessor::FFTSettings& settings = GetEngine()->GetSpectrumAnalyzerSettings()->GetFFTSettings();

	// put each channel in a separate chartform
	const int numSelected = channels.Size();
	for (int i = 0; i < numSelected; i++)
	{
		// get channel and create the spectrum analyzer, add it to waveform
		Channel<double>* channel = channels[i];
		FFTProcessor* analyzer = new FFTProcessor();

		// connect and configure FFT
		analyzer->SetInput(channel);
		analyzer->Setup(settings);
		analyzer->SetUseZeroPadding(true);
		analyzer->ReInit();

		// store everythin in the lists
		mChannels.Add(channel);
		mSpectrumAnalyzers.Add(analyzer);
		mAverageSpectra.AddEmpty();
	}


	if (mSpectrumWidget != NULL)
	{
		if (mSpectrumElements.Size() != numSelected)
			mSpectrumElements.Resize(numSelected);

		for (int i = 0; i < numSelected; i++)
		{
			Channel<double>* channel = channels[i];

			mSpectrumElements[i] = Spectrogram2DWidget::SpectrumElement(channel->GetName(), channel->GetColor());
		}

		mSpectrumWidget->ReInit(mSpectrumElements);
	}

	// remember frequency resolution
	mNumBins = GetEngine()->GetSpectrumAnalyzerSettings()->GetNumFFTBins();

	// remember window shift
	mNumShiftSamples = GetEngine()->GetSpectrumAnalyzerSettings()->GetNumWindowShiftSamples();

	// remember window function
	mWindowFunctionType = GetEngine()->GetSpectrumAnalyzerSettings()->GetWindowFunction()->GetType();

	// set the current channel spectrum analyzer statistic size
	SetAverageInterval( GetAverageInterval() );

	SetMultiView( GetMultiView() );
}


void Spectrogram2DPlugin::UpdateInterface()
{
	// force reinit if FFT settings have changed
	SpectrumAnalyzerSettings* settings = GetEngine()->GetSpectrumAnalyzerSettings();
	if (settings->GetNumFFTBins() != mNumBins || settings->GetNumWindowShiftSamples() != mNumShiftSamples || settings->GetWindowFunction()->GetType() != mWindowFunctionType )
		ReInit();

	const uint32 numAnalyzers = mSpectrumAnalyzers.Size();

	Array<Spectrum*> mViewedSpectrums;
	mViewedSpectrums.Resize(numAnalyzers);

	// update spectrum analyzers and calculate averages
	for (uint32 i=0; i<numAnalyzers; ++i)
	{
		mSpectrumAnalyzers[i]->Update();

		// calculate average and (uses output channel spectrum directly)
		Channel<Spectrum>* spectra = mSpectrumAnalyzers[i]->GetOutput()->AsType<Spectrum>();
		spectra->CalculateAverage(&mAverageSpectra[i]);

		mSpectrumWidget->UpdateSpectrum(i, &mAverageSpectra[i]);
		mViewedSpectrums[i] = &mAverageSpectra[i];
	}
}


void Spectrogram2DPlugin::OnChannelSelectionChanged()
{
	ReInit();
}


void Spectrogram2DPlugin::SetAverageInterval(double length)
{
	const uint32 numChannels = mChannels.Size();
	for (uint32 i = 0; i < numChannels; i++)
	{
		FFTProcessor* analyzer = mSpectrumAnalyzers[i];
		const uint32 sampleRate = analyzer->GetOutput()->GetSampleRate();
		
		uint32 numSamples = length * sampleRate;
		if (length == 0)
			numSamples = 1;

		// set buffer size of fft processor output channel to the number of samples we want to average
		ChannelBase* output = mSpectrumAnalyzers[i]->GetOutput();
		
		// resize the output channel which holds the samples for averaging
		output->Reset();
		output->Clear(true);					// deallocate, so SetBufferSize() will shrink the array
		output->SetBufferSize(numSamples);		// resize the buffer to the interval
		output->Reset();
	}
}
