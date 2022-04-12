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
#include <Studio/Precompiled.h>

// include required headers
#include "SpectrogramBandsPlugin.h"

#ifdef USE_QTDATAVISUALIZATION

#include <QtDataVisualization/q3dsurface.h>

using namespace Core;

// constructor
SpectrogramBandsPlugin::SpectrogramBandsPlugin() : SpectrogramPlugin(GetStaticTypeUuid())
{
	LogDetailedInfo("Constructing spectrogram bands plugin ...");

	mDataProxy						= NULL;
	mGraph							= NULL;
	mSeries							= NULL;

	mChannelMultiSelectionWidget	= NULL;
}


// destructor
SpectrogramBandsPlugin::~SpectrogramBandsPlugin()
{
	LogDetailedInfo("Destructing spectrogram bands plugin ...");
}


// init after the parent dock window has been created
bool SpectrogramBandsPlugin::Init()
{
	LogDetailedInfo("Initializing spectrogram bands plugin ...");

	// create graph and widget
	mGraph = new Q3DBars( &GetOpenGLManager()->GetDefaultSurfaceFormat(), NULL );

	// data proxy
	mDataProxy = new ChannelBandsDataProxy();

	// setup axis
	mGraph->setRowAxis(CreateChannelAxis());
	mGraph->setColumnAxis(CreateBandsAxis());
	
	// create surface series 
	mSeries = new QBar3DSeries(mDataProxy);

	// add series to graph
	mGraph->addSeries( mSeries );

	// channel switcher widgets
	mChannelMultiSelectionWidget = new ChannelMultiSelectionWidget();
	connect(mChannelMultiSelectionWidget, SIGNAL(ChannelSelectionChanged()), this, SLOT(OnChannelSelectionChanged()));
	mChannelMultiSelectionWidget->SetAutoSelectType(ChannelMultiSelectionWidget::AutoSelectType::SELECT_ALL);
	mChannelMultiSelectionWidget->SetShowNeuroChannelsOnly(true);
	mChannelMultiSelectionWidget->Init();

	// channel selection layout (topleft)
	QHBoxLayout* layout = new QHBoxLayout();
	QWidget* topLeftWidget = new QWidget();
	topLeftWidget->setLayout(layout);
	layout->setMargin(3);
	layout->setSpacing(10);
	layout->setAlignment(Qt::AlignLeft);
	layout->addWidget(mChannelMultiSelectionWidget);
	
	// add channels
	OnChannelSelectionChanged();

	// setup base class
	SpectrogramPlugin::SetAutoScalingHysteresis(0.75);

	// call base class init
	bool result = SpectrogramPlugin::Init(mGraph, topLeftWidget, mDataProxy);
	LogDetailedInfo("Spectrogram bands plugin successfully initialized");

	connect( mAttributeSetGridWidget->GetPropertyManager(), SIGNAL(ValueChanged(Property*)), this, SLOT(OnAttributeChanged(Property*)) );

	SetColorMapping( GetColorMapping() );

	return result;
}


void SpectrogramBandsPlugin::RealtimeUpdate()
{
	mDataProxy->Update();
	SpectrogramPlugin::RealtimeUpdate();
}


// attach value axis to bar graph
void SpectrogramBandsPlugin::UpdateValueAxis()
{
	mGraph->setValueAxis(GetValueAxis());
	switch (mValueAxisType)
	{
		case DEZIBEL:
			mDataProxy->SetConvertToDezibel(true);
			break;
		case LINEAR:
		case LOGARITHMIC:
			mDataProxy->SetConvertToDezibel(false);
	}
}

// register attributes and create the default values
void SpectrogramBandsPlugin::RegisterAttributes()
{
	// base class call
	SpectrogramPlugin::RegisterAttributes();

	// add additional attributes

	// color mapping
	AttributeSettings* attributeSettings = RegisterAttribute("Color Mapping", "colorMapping", "", ATTRIBUTE_INTERFACETYPE_COLORMAPPING);
	attributeSettings->SetDefaultValue( AttributeInt32::Create(0) );

	// create default attribute values
	CreateDefaultAttributeValues();
}


void SpectrogramBandsPlugin::OnAttributeChanged(Property* property)
{
	const String& propertyInternalName = property->GetAttributeSettings()->GetInternalNameString();

	// color mapping
	if (propertyInternalName.IsEqual("colorMapping") == true)
		SetColorMapping( GetColorMapping() );
}


void SpectrogramBandsPlugin::AddSettings(DataVisualizationSettingsWidget* settingsWidget)
{
	delete mMinFrequencySlider;
	mMinFrequencySlider = NULL;
	delete mMaxFrequencySlider;
	mMaxFrequencySlider = NULL;

	// add base class settings 
	SpectrogramPlugin::AddSettings(settingsWidget);
}


// check channels for consistency 
void SpectrogramBandsPlugin::ChannelBandsDataProxy::ReInit()
{
	const int numChannels = mChannels.Size();
	if (numChannels == 0)
		return;

	int sampleRate = -1;

	// re-init data array
	mData->clear();

	// clear spectrum analyzers (better would be: reuse them)
	const uint32 numAnalyzers = mSpectrumAnalyzers.Size();
	for (uint32 i=0; i<numAnalyzers; i++)
		delete mSpectrumAnalyzers[i];
	mSpectrumAnalyzers.Clear();

	// remember frequency resolution
	mNumBins = GetEngine()->GetSpectrumAnalyzerSettings()->GetNumFFTBins();

	// remember window shift
	mNumShiftSamples = GetEngine()->GetSpectrumAnalyzerSettings()->GetNumWindowShiftSamples();

	// remember window function
	mWindowFunctionType = GetEngine()->GetSpectrumAnalyzerSettings()->GetWindowFunction()->GetType();

	// for all channels ...
	for (int c = 0; c < numChannels; c++)
	{
		Channel<double>* channel = mChannels[c];

		// check sampling rate
		if (sampleRate == -1)
		{
			sampleRate = channel->GetSampleRate();
		}
		else if (sampleRate != channel->GetSampleRate())
		{
			LogError("Samplerate of the channels do not match while initializing ChannelBandsDataProxy!");
		}

		// create a spectrum analyzer
		FFTProcessor* analyzer = new FFTProcessor();
		analyzer->SetInput(channel);
		analyzer->Setup(GetEngine()->GetSpectrumAnalyzerSettings()->GetFFTSettings());
		analyzer->ReInit();

		mSpectrumAnalyzers.Add(analyzer);

		// init data array row by row
		QBarDataRow* row = new QBarDataRow(GetNumBins());
		*mData << row;
	}

	resetArray(mData);
}


// data proxy update : fill visualization arrays with data
void SpectrogramBandsPlugin::ChannelBandsDataProxy::Update()
{
	// nothing to update
	const int numChannels = mChannels.Size();
	if (numChannels == 0)
		return;

	// force reinit if FFT settings have changed
	SpectrumAnalyzerSettings* settings = GetEngine()->GetSpectrumAnalyzerSettings();
	if (settings->GetNumFFTBins() != mNumBins || settings->GetNumWindowShiftSamples() != mNumShiftSamples || settings->GetWindowFunction()->GetType() != mWindowFunctionType)
		ReInit();

	// update spectrum analyzer
	const uint32 numAnalyzers = mSpectrumAnalyzers.Size();
	for (uint32 i=0; i<numAnalyzers; i++)
		mSpectrumAnalyzers[i]->Update();

	// reset to extremes
	mMinValue = DBL_MAX;
	mMaxValue = -DBL_MAX;

	// copy over the spectrum magnitudes of each channel and find the min/max values
	for (int c = 0; c < numChannels; c++)
	{
		Channel<double>* channel = mChannels[c];
		Channel<Spectrum>* output = mSpectrumAnalyzers[c]->GetOutput()->AsType<Spectrum>();
		
		if (output->GetNumSamples() == 0)
			continue; 

		const Spectrum* spectrum = &output->GetLastSample();

		QBarDataRow* row = mData->at(c);
		
		// calculate magnitude for each band in the channel
		float bandMagnitude;
		const uint32 numBins = GetNumBins();
		for (uint32 b = 0; b < GetNumBins(); b++)
		{
			FrequencyBand* band = GetEngine()->GetSpectrumAnalyzerSettings()->GetFrequencyBand(b);

			if (mConvertToDezibel == true)
				bandMagnitude = spectrum->GetFrequencyDecibels(band->CalcMagnitude(spectrum) / numBins);
			else
				bandMagnitude = band->CalcMagnitude(spectrum) / numBins;

			(*row)[b].setValue(bandMagnitude);

			// find min/max value
			mMinValue = fmin(mMinValue, bandMagnitude);
			mMaxValue = fmax(mMaxValue, bandMagnitude);
		}
	}

	resetArray( mData );
}


QCategory3DAxis* SpectrogramBandsPlugin::CreateChannelAxis()
{
	QCategory3DAxis* axis = new QCategory3DAxis();
	axis->setAutoAdjustRange(true);
	axis->setTitle("Channels");

	axis->setLabels(GetChannelAxisLabels());

	return axis;
}


QCategory3DAxis* SpectrogramBandsPlugin::CreateBandsAxis()
{
	QCategory3DAxis* axis = new QCategory3DAxis();
	axis->setAutoAdjustRange(true);
	axis->setTitle("Frequency Bands");

	axis->setLabels(GetBandsAxisLabels());
	
	return axis;
}


QStringList SpectrogramBandsPlugin::GetChannelAxisLabels()
{
	const uint32 numChannels = mDataProxy->GetNumChannels();

	QStringList labels;
	for (uint32 i = 0; i < numChannels; i++)
		labels << mDataProxy->GetChannel(i)->GetName();

	return labels;
}


QStringList SpectrogramBandsPlugin::GetBandsAxisLabels()
{
	const uint32 numBins = mDataProxy->GetNumBins();

	QStringList labels;
	for (uint32 i = 0; i < numBins; i++)
		labels << GetEngine()->GetSpectrumAnalyzerSettings()->GetFrequencyBand(i)->GetName();

	return labels;
}


void SpectrogramBandsPlugin::OnChannelSelectionChanged()
{
	// remove channels from dataproxy
	mDataProxy->Clear();

	double maxFrequency = -1;
	Array<Channel<double>*> channels = mChannelMultiSelectionWidget->GetSelectedChannels();

	// iterate over channels, add them one by one
	const int numSelected = channels.Size();
	for (int i = 0; i < numSelected; i++)
		mDataProxy->AddChannel(channels[i]);

	// update axis channel labels
	mGraph->rowAxis()->setLabels(GetChannelAxisLabels());

	// re-init data proxy
	mDataProxy->ReInit();
	// check that max frequency is consitent for all channels
	for (int i = 0; i < numSelected; i++)
	{
		const double currentMaxFrequency = mDataProxy->GetSpectrumAnalyzer(i)->GetOutput()->GetSampleRate() / 2.0;
		if (maxFrequency == -1)
		{
			maxFrequency = currentMaxFrequency;
		}
		else if (currentMaxFrequency != maxFrequency)
		{
			LogError("Maximum Frequency of the Bands Spectrogram Channels do not match!");
		}
	}

	SetFrequencyRange(0, maxFrequency);
}


// called when the color mapping got changed
void SpectrogramBandsPlugin::SetColorMapping(ColorMapper::EColorMapping colorMapping)
{
	mColorMapper.SetColorMapping(colorMapping);
	
	QBar3DSeries* series = mGraph->seriesList()[0];
	series->setBaseGradient(GetColorGradient(colorMapping));
	series->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
	series->setSingleHighlightColor(QColor(0, 159, 227));
	series->setMultiHighlightGradient(GetColorGradient(colorMapping));
}

#endif
