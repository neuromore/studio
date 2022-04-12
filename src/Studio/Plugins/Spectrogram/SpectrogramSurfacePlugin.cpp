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
#include "SpectrogramSurfacePlugin.h"

#ifdef USE_QTDATAVISUALIZATION

#include <QtDataVisualization/q3dsurface.h>

//#define LOG_PERFORMANCEINFO

using namespace Core;

// constructor
SpectrogramSurfacePlugin::SpectrogramSurfacePlugin() : SpectrogramPlugin(GetStaticTypeUuid())
{
	LogDetailedInfo("Constructing spectrogram surface plugin ...");

	mChannelSelectionWidget		= NULL;
	mIntervalLengthSlider		= NULL;
	mIntervalLengthLabel		= NULL;

	// allow for 60 seconds of spectrum
	mMaxIntervalLength			= 60;
	mIntervalLength				= 10;
}


// constructor
SpectrogramSurfacePlugin::SpectrogramSurfacePlugin(const char* typeUuid) : SpectrogramPlugin(typeUuid)
{
	LogDetailedInfo("Constructing spectrogram surface plugin ...");

	mChannelSelectionWidget		= NULL;
	mIntervalLengthSlider		= NULL;
	mIntervalLengthLabel		= NULL;	

	// allow for 60 seconds of spectrum
	mMaxIntervalLength			= 60;
	mIntervalLength				= 10;
}


// destructor
SpectrogramSurfacePlugin::~SpectrogramSurfacePlugin()
{
	LogDetailedInfo("Destructing spectrogram surface plugin ...");
}


// register attributes and create the default values
void SpectrogramSurfacePlugin::RegisterAttributes()
{
	// base class call
	SpectrogramPlugin::RegisterAttributes();

	// add additional attributes

	// color mapping
	AttributeSettings* attributeSettings = RegisterAttribute("Color Mapping", "colorMapping", "", ATTRIBUTE_INTERFACETYPE_COLORMAPPING);
	attributeSettings->SetDefaultValue( AttributeInt32::Create(0) );

	// show zero Hz bin
	attributeSettings = RegisterAttribute("Show 0Hz Bin", "show0HzBin", "", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeSettings->SetDefaultValue( AttributeBool::Create(false) );

	// create default attribute values
	CreateDefaultAttributeValues();
}


void SpectrogramSurfacePlugin::OnAttributeChanged(Property* property)
{
	const String& propertyInternalName = property->GetAttributeSettings()->GetInternalNameString();

	// color mapping
	if (propertyInternalName.IsEqual("colorMapping") == true)
		SetColorMapping( GetColorMapping() );

	// show 0Hz bin
	if (propertyInternalName.IsEqual("show0HzBin") == true)
		SetShow0HzBin( GetShow0HzBin() );
}


// init after the parent dock window has been created
bool SpectrogramSurfacePlugin::Init()
{
	LogDetailedInfo("Initializing spectrogram surface plugin ...");

	mIntervalLengthSlider		= new QSlider(Qt::Horizontal);

	////////////////////////////////////////////////

	// create graph and widget
	mGraph = new SpectrogramSurfaceWidget (&GetOpenGLManager()->GetDefaultSurfaceFormat(), NULL);

	// data proxy
	mDataProxy = new SurfaceDataProxy(mIntervalLength);

	// setup axis
	mGraph->setAxisX(CreateXAxis());
	mGraph->setAxisZ(CreateZAxis());
	mGraph->setHorizontalAspectRatio(1);
	UpdateTimeAxis();
	//float margin = mGraph->margin();

	// create surface series 
	mSeries = new QSurface3DSeries(mDataProxy);
	mSeries->setDrawMode(QSurface3DSeries::DrawSurface);

	mSeries->setFlatShadingEnabled(false);
	mSeries->setMeshSmooth(false);

	// add series to graph
	mGraph->addSeries(mSeries);

	// channel switcher widgets (raw channels only)
	mChannelSelectionWidget = new ChannelSelectionWidget();
	connect(mChannelSelectionWidget, SIGNAL(ChannelSelectionChanged(Channel<double>*)), this, SLOT(OnChannelSelectionChanged(Channel<double>*)));
	mChannelSelectionWidget->SetShowNeuroChannelsOnly(true);
	mChannelSelectionWidget->Init();

	// channel selection widget (topleft)
	QHBoxLayout* layout = new QHBoxLayout();
	QWidget* topLeftWidget = new QWidget();
	topLeftWidget->setLayout(layout);
	layout->setMargin(3);
	layout->setSpacing(10);
	layout->setAlignment(Qt::AlignLeft);
	layout->addWidget(mChannelSelectionWidget);

	// add channels
	Channel<double>* channel = mChannelSelectionWidget->GetSelectedChannel();
	if (channel != NULL)
		OnChannelSelectionChanged(channel);

	// setup base class 
	SpectrogramPlugin::SetAutoScalingHysteresis(0.1);

	// call base class init
	bool result = SpectrogramPlugin::Init(mGraph, topLeftWidget, mDataProxy);

	connect( mAttributeSetGridWidget->GetPropertyManager(), SIGNAL(ValueChanged(Property*)), this, SLOT(OnAttributeChanged(Property*)) );

	// update settings
	SetColorMapping( GetColorMapping() );
	SetShow0HzBin( GetShow0HzBin() );

	LogDetailedInfo("Spectrogram surface plugin successfully initialized");

	return result;
}


void SpectrogramSurfacePlugin::RealtimeUpdate()
{
	// update data proxy
#ifdef LOG_PERFORMANCEINFO
	Timer proxyUpdateTimer;
#endif
		mDataProxy->Update();
#ifdef LOG_PERFORMANCEINFO
	const float proxyUpdateTime = proxyUpdateTimer.GetTime();
#endif

	// update time axis
#ifdef LOG_PERFORMANCEINFO
	Timer timeAxisUpdateTimer;
#endif
	UpdateTimeAxis();
#ifdef LOG_PERFORMANCEINFO
	const float timeAxisUpdateTime = timeAxisUpdateTimer.GetTime();
#endif

	// call baseclass update (camera animation and stuff)
#ifdef LOG_PERFORMANCEINFO
	Timer baseUpdateTimer;
#endif
	SpectrogramPlugin::RealtimeUpdate();
#ifdef LOG_PERFORMANCEINFO
	const float baseUpdateTime = baseUpdateTimer.GetTime();
	LogDebug("SpectrogramSurfacePlugin: proxyUpdateTime=%.2f, timeAxisUpdateTime=%.2f, baseUpdateTimer=%.2f", proxyUpdateTime*1000.0f, timeAxisUpdateTime*1000.0f, baseUpdateTime*1000.0f);
#endif
}


void SpectrogramSurfacePlugin::AddSettings(DataVisualizationSettingsWidget* settingsWidget)
{
	// add base class settings 
	SpectrogramPlugin::AddSettings(settingsWidget);

	// sample interval length / duration
	if (mIntervalLengthSlider != NULL)
	{
		mIntervalLengthSlider->adjustSize();
		mIntervalLengthSlider->setMinimum(0);
		mIntervalLengthSlider->setMaximum(60);
		mIntervalLengthSlider->setPageStep(5);
		mIntervalLengthSlider->setTickInterval(5);
		connect(mIntervalLengthSlider, SIGNAL(valueChanged(int)), this, SLOT(OnIntervalLengthSliderChanged(int)));
	
		QHBoxLayout* hLayout = new QHBoxLayout();
		hLayout->addWidget(mIntervalLengthSlider);
		mIntervalLengthLabel = new QLabel();
		hLayout->addWidget(mIntervalLengthLabel);
		settingsWidget->AddLayout("Duration ", hLayout);
	}

	// set and apply default values 
	mIntervalLengthSlider->setValue(mIntervalLength); //FIXME? one time this failed because there was no device present
}


//////////////////////////////////////////////////
//
// DATA PROXY IMPLEMENTATIONS
//

void SpectrogramSurfacePlugin::SurfaceDataProxy::Clear()
{
	// clear arrays
	mData->clear();
}


void SpectrogramSurfacePlugin::SurfaceDataProxy::ReInit()
{
	// nothing to do (yet)
	if (mChannel == NULL)
		return;

	// clear render array
	Clear();

	// remember frequency resolution
	mNumBins = GetEngine()->GetSpectrumAnalyzerSettings()->GetNumFFTBins();

	// remember window shift
	mNumShiftSamples = GetEngine()->GetSpectrumAnalyzerSettings()->GetNumWindowShiftSamples();

	// remember window function
	mWindowFunctionType = GetEngine()->GetSpectrumAnalyzerSettings()->GetWindowFunction()->GetType();

	// remember sampling rate
	const double inputSampleRate = mSpectrumAnalyzer.GetOutput()->GetSampleRate();
	mSampleRate = inputSampleRate;

	// calculate number of (downsampled) spectrums to keep in the display buffer
	mNumSamples = (uint32)(mSampleRate * mDuration) + 1;

	// configure spectrum analyzer
	mSpectrumAnalyzer.SetInput(mChannel);
	mSpectrumAnalyzer.GetOutput()->SetBufferSize(2*mNumSamples);
	mSpectrumAnalyzer.Setup(GetEngine()->GetSpectrumAnalyzerSettings()->GetFFTSettings());
	mSpectrumAnalyzer.ReInit();

	
	// pre-allocate surface data array
	//for (uint32 i = 0; i < mNumSamples; i++)
	//	mData->append(new QSurfaceDataRow(mNumBins));
}


// data proxy update : fill visualization arrays with data
void SpectrogramSurfacePlugin::SurfaceDataProxy::Update()
{
	// nothing to update
	if (mChannel == NULL)
		return;

	mSpectrumAnalyzer.Update();
	Channel<Spectrum>* output = mSpectrumAnalyzer.GetOutput()->AsType<Spectrum>();
	SpectrumAnalyzerSettings* settings = GetEngine()->GetSpectrumAnalyzerSettings();

	// force reinit if FFT settings have changed
	if (settings->GetNumFFTBins() != mNumBins || output->GetSampleRate() != mSampleRate || settings->GetNumWindowShiftSamples() != mNumShiftSamples || settings->GetWindowFunction()->GetType() != mWindowFunctionType)
		ReInit();

	// current number of rows (time axis)
	uint32 numRows = (uint32)mData->size();

	// data array still growing?
	bool isGrowing = (numRows < mNumSamples) || mNumSamples == 0;

	// FIXME use a channelreader instead of this code!

	// calculate number of new spectrums we have to copy over
	const uint64 currentSpectrumIndex = output->GetContinuousSampleCounter();
	uint64 numToAdd = currentSpectrumIndex - mLastIndex;

	// we can add only as many spectrums we have in the buffer, not more
	if (numToAdd > output->GetNumSamples())
		numToAdd = output->GetNumSamples();
	
	// nothing to do (yet) ..
	if (numToAdd > 0)
	{
		// copy over the new spectrums and find min/max values
		for (int s = 0; s < numToAdd; s++)
		{
			const int spectrumIndex = output->GetMaxSampleIndex()+1 - (numToAdd - s);
			Spectrum* spectrum = output->GetSampleRef(spectrumIndex);
		
			// get first data row, remove it from list, change values, append to back of array
			QSurfaceDataRow* row;
			if (isGrowing == true || mData->size() == 0)
				row = new QSurfaceDataRow(mNumBins);
			else
			{
				row = mData->takeLast();
				row->resize(mNumBins);
			}

			// FIXME use actual number number of bins of input spectrum (accesses out of bounds in spectrum->GetBin(f) when using layout F5 and increasing FFT order.
			// note: has maybe something to do with input reader changes of FFT processor? not sure.
			const uint32 numBins = Min(spectrum->GetNumBins(), mNumBins); // was: numBins = mNumBins

			// convert to dezibel or not? (we do not want this to be in the most inner loop)
			if (mConvertToDezibel == true)
			{
				// copy over band magnitude values and min/max values
				for (uint32 f = 0; f < numBins; f++)
				{
					const double value = spectrum->GetFrequencyDecibels(spectrum->GetBin(f));
					const double time = spectrum->GetTime();
					(*row)[f].setPosition(QVector3D(spectrum->CalcFrequency(f), value, time));
				}

			}
			else
			{
				// copy over band magnitude values and min/max values
				for (uint32 f = 0; f < numBins; f++)
				{
					const double value = spectrum->GetBin(f);
					const double time = spectrum->GetTime();
					(*row)[f].setPosition(QVector3D(spectrum->CalcFrequency(f), value, time));
				}

			}

			// add row in front
			mData->prepend(row);

		}
	}

	// calculate min/max value over all bins of all spectra
	if (output->GetNumSamples() > 0)
	{
		// we only calculate the min/max value between the frequency band range that is shown (even though we copied all values to the visualization array)
		const double frequencyOfHighestBin = output->GetSampleRate() / 2.0;
		const Spectrum* lastSpectrum = &output->GetLastSample();
	
		const double numBins = lastSpectrum->GetNumBins();
		const uint32 minFrequencyBinIndex = lastSpectrum->CalcBinIndex(mMinFrequency);
		const uint32 maxFrequencyBinIndex = lastSpectrum->CalcBinIndex(mMaxFrequency);

		// require valid indices
		if (minFrequencyBinIndex != CORE_INVALIDINDEX32 && maxFrequencyBinIndex != CORE_INVALIDINDEX32)
		{

			// init w/ extreme values
			double minVal = +DBL_MAX;
			double maxVal = -DBL_MAX;

			const double numSpectrums = mData->length();
			for (uint32 i = 0; i <numSpectrums; i++)
			{
				QSurfaceDataRow* row = mData->at(i);
				for (uint32 f = minFrequencyBinIndex; f <= maxFrequencyBinIndex; f++)
				{ 
					if (f >= (uint32)row->length() || f < 0)
						continue;

					const double value = (*row)[f].y();
					maxVal = fmax(maxVal, value);
					minVal = fmin(minVal, value);
				}
			}

			mMaxValue = maxVal;
			mMinValue = minVal;

			// do not allow an empty z range
			if (mMinValue == mMaxValue)
				mMaxValue = mMinValue+1;

			// remember current sample time and running index
			mLastTime = lastSpectrum->GetTime();
			mLastIndex = currentSpectrumIndex;
		}
	}


	// tell graph the data has changed
	if (numToAdd > 0)
	{
		// fix time values of last row to remove yitter (aligning the mesh to the graph border)
		//if (isGrowing == false)
		if (false){
			QSurfaceDataRow* lastRow = mData->last();
			const uint32 numBins = lastRow->length();
			for (uint32 f = 0; f < numBins; f++)
				(*lastRow)[f].setZ(mLastTime - mDuration);
		}

		resetArray(mData);
	}
}


//////////////////////////////////////////////////
//
// AXIS HELPERS
//

// update value axis, set dB conversion in dataproxy if required by the axis type
void SpectrogramSurfacePlugin::UpdateValueAxis()
{
	mGraph->setAxisY(GetValueAxis());

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


// update range of time axis
void SpectrogramSurfacePlugin::UpdateTimeAxis()
{
	if (mGraph == NULL)
		return;

	float currentTime = 0.0;
	if (mDataProxy->GetChannel() != NULL)
		if (mDataProxy->GetSpectrumAnalyzer()->GetOutput()->GetNumSamples() > 0)
			currentTime = mDataProxy->GetSpectrumAnalyzer()->GetOutput()->AsType<Spectrum>()->GetLastSample().GetTime();

	mGraph->axisZ()->setRange(currentTime - mIntervalLength, currentTime);
}


QValue3DAxis* SpectrogramSurfacePlugin::CreateXAxis()
{
	QValue3DAxis* axis = new QValue3DAxis();
	//TimeAxisFormatter* formatter = new TimeAxisFormatter();
	//axis->setFormatter( formatter );
	axis->setLabelFormat( "%.1f Hz" );
	axis->setTitle( QStringLiteral( "Frequency" ) );
  
	// apply default style
	StyleValueAxis( axis );
	return axis;
}


QValue3DAxis* SpectrogramSurfacePlugin::CreateZAxis()
{
	QValue3DAxis* axis = new QValue3DAxis();	
	
	//TimeAxisFormatter* formatter = new TimeAxisFormatter();
	//axis->setFormatter( formatter );
	axis->setLabelFormat( "%.1f s" );
	axis->setTitle( QStringLiteral( "Time" ) );

	// reverse axis
	axis->setReversed( true );

	// apply default style
	StyleValueAxis( axis );

	return axis;
}


//////////////////////////////////////////////////
//
// SETTINGS WIDGET SLOTS
//

void SpectrogramSurfacePlugin::OnMinFrequencyChanged(double value)
{
	mGraph->axisX()->setMin(value); 
	mDataProxy->SetFrequencyRange(mMinFrequency, mMaxFrequency);
}

void SpectrogramSurfacePlugin::OnMaxFrequencyChanged(double value)
{
	mGraph->axisX()->setMax(value); 
	mDataProxy->SetFrequencyRange(mMinFrequency, mMaxFrequency);
}

void SpectrogramSurfacePlugin::OnChannelSelectionChanged(Channel<double>* channel)
{
	mDataProxy->SetChannel(channel);
	// update max frequency in base class
	SetFrequencyRange(0, mDataProxy->GetSpectrumAnalyzer()->GetInput()->GetSampleRate() / 2.0);
}


void SpectrogramSurfacePlugin::OnIntervalLengthSliderChanged(int value)
{
	mTempString.Format("%i s", value);
	mIntervalLengthLabel->setText(mTempString.AsChar());
	mIntervalLength = (double)value;
	mDataProxy->SetDuration(mIntervalLength);
	UpdateTimeAxis();
}


// called when the color mapping got changed
void SpectrogramSurfacePlugin::SetColorMapping(ColorMapper::EColorMapping colorMapping)
{
	mColorMapper.SetColorMapping( colorMapping );

	QSurface3DSeries* series = mGraph->seriesList()[0];
	series->setBaseGradient(GetColorGradient(colorMapping));
	series->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}


void SpectrogramSurfacePlugin::SetShow0HzBin(bool show)
{
	// update min frequency slider minimum value
	if (show == true)
	{
		mMinFrequencySlider->setMinimum(0);

		// if slider value is 1, set it back to zero; otherwise the checkbox behaviour does not feel right
		if (mMinFrequencySlider->value() == 1)
			mMinFrequencySlider->setValue(0);
	} 
	else
	{
		mMinFrequencySlider->setMinimum(1);
	}
}

#endif
