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
#include "SpectrogramPlugin.h"
#include <Core/LogManager.h>
#include <EngineManager.h>
#include <QtBaseManager.h>
#include <QTimer>

#ifdef USE_QTDATAVISUALIZATION

#include <QtDataVisualization/q3dsurface.h>
#include <QtDataVisualization/qlogvalue3daxisformatter.h>


using namespace Core;

// constructor
SpectrogramPlugin::SpectrogramPlugin(const char* typeUuid) : DataVisualizationPlugin(typeUuid)
{
	LogDetailedInfo("Constructing spectrogram plugin ...");

	mAttributeSetGridWidget = NULL;

	mDefaultTheme = NULL;
	mGraphWidget = NULL;
	mTopLeftWidget = NULL;

	mMaxFrequencySlider = NULL;
	mMinFrequencySlider = NULL;

	mMinValueSlider = NULL;
	mMaxValueSlider = NULL;

	mAutoScalingCheckBox = NULL;
	mUseAutoScaling = false;
	mCurrentMinValue = DBL_MAX;
	mCurrentMaxValue = -DBL_MAX;
	mAutoScalingHysteresis = 0;

	mValueAxisType = ValueAxisType::LINEAR;

	mMinValue = 0;
	mMaxValue = 1;

	mMinFrequency = 0;
	mMaxFrequency = 50;
}


// destructor
SpectrogramPlugin::~SpectrogramPlugin()
{
	LogDetailedInfo("Destructing spectrogram plugin ...");
}


// init base class stuff
bool SpectrogramPlugin::Init(QAbstract3DGraph* graph, QWidget* topLeftWidget, DataProxy* dataProxy)
{
	LogDetailedInfo("Initializing spectrogram plugin ...");

	mGraph = graph;
	mTopLeftWidget = topLeftWidget;
	mDataProxy = dataProxy;

	// FPS measurement
	mGraph->setMeasureFps(true);
	connect(mGraph, SIGNAL(currentFpsChanged(qreal)), this, SLOT(OnCurrentFpsChanged(qreal)));

	// shadows
	graph->setShadowQuality(QAbstract3DGraph::ShadowQualityMedium);
	
	// selection mode
	mGraph->setSelectionMode(QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionItemRowAndColumn);

	// create value axis
	mValueAxis = CreateValueAxis(mValueAxisType);
	UpdateValueAxis();

	// set default theme
	Q3DTheme *theme = GetDefaultTheme();
	mGraph->addTheme(theme);
	mGraph->setActiveTheme(theme);

	// create graph widget
	mGraphWidget = QWidget::createWindowContainer(mGraph);

	// init settings widgets (they won't be created if the derived class deteltes them before calling AddSettingsnot to be created)
	mMinValueSlider			  = new QSlider(Qt::Horizontal);
	mMaxValueSlider			  = new QSlider(Qt::Horizontal);
	mMinFrequencySlider		  = new QSlider(Qt::Horizontal);
	mMaxFrequencySlider		  = new QSlider(Qt::Horizontal);
	mAutoScalingCheckBox	  = new QCheckBox();

	// init plugin layout
	bool result = DataVisualizationPlugin::InitLayout(mGraphWidget, mTopLeftWidget);

	// init values
	SetCameraOrthoView( GetCameraOrthoView() );
	SetCameraPresetIndex( GetCameraPresetIndex() );
	SetValueAxisTypeIndex( GetValueAxisTypeIndex() );

	LogDetailedInfo("Spectrogram plugin successfully initialized");

	return result;
}


void SpectrogramPlugin::OnAttributeChangedBaseClass(Property* property)
{
	const String& propertyInternalName = property->GetAttributeSettings()->GetInternalNameString();

	// camera preset
	if (propertyInternalName.IsEqual("cameraPreset") == true)
		SetCameraPresetIndex( GetCameraPresetIndex() );

	// value axis type
	if (propertyInternalName.IsEqual("valueAxisType") == true)
		SetValueAxisTypeIndex( GetValueAxisTypeIndex() );
}


void SpectrogramPlugin::RealtimeUpdate()
{
	const float perfTimeDelta = perfTimer.GetTimeDelta().InSeconds();

	Q3DCamera* camera = GetGraph()->scene()->activeCamera();

	// camera animation
	if (IsCameraAutoRotating() == true)
	{
		const float rot = camera->xRotation();
		camera->setXRotation(rot + 360 / 30 * perfTimeDelta);		// 30 seconds per revolution 
	}

	// ortho view
	const bool orthoView = GetCameraOrthoView();
	if (mGraph != NULL && mGraph->isOrthoProjection() != orthoView)
		mGraph->setOrthoProjection(orthoView);

	// autoscale axis
	if (mUseAutoScaling)
	{
		// uses a hysteresis for shrinking the range, but not for increasing it
		AutoScale();
	}
}


// autoscale the axis with a simple hysteresis
void SpectrogramPlugin::AutoScale()
{
	// uses a hysteresis for shrinking the range, but not for increasing it
	const double newMinValue = mDataProxy->GetMinValue();
	const double newMaxValue = mDataProxy->GetMaxValue();

	// linear axis
	if (mValueAxisType == ValueAxisType::LINEAR || mValueAxisType == ValueAxisType::DEZIBEL)
	{
		mCurrentMinValue = 0;

		if (newMaxValue > mCurrentMaxValue)
			mCurrentMaxValue = newMaxValue;
		else if (newMaxValue < mCurrentMaxValue * (1 - mAutoScalingHysteresis))
			mCurrentMaxValue = newMaxValue;


		if (newMinValue < mCurrentMinValue)
			mCurrentMinValue = newMinValue;
		else if (newMinValue > mCurrentMinValue * (1 - mAutoScalingHysteresis))
			mCurrentMinValue = newMinValue;
	}
	else if (mValueAxisType == ValueAxisType::LOGARITHMIC)
	{
		// number of powers-of-10 between current min and max value
		const float logRange = log10(mCurrentMaxValue) - log10(mCurrentMinValue);

		if (newMinValue < mCurrentMinValue)
			mCurrentMinValue = newMinValue;
		else if (log10(newMinValue) > log10(mCurrentMinValue) + logRange * mAutoScalingHysteresis)
			mCurrentMinValue = newMinValue;

		if (newMaxValue > mCurrentMaxValue)
			mCurrentMaxValue = newMaxValue;
		else if (log10(newMaxValue) < log10(mCurrentMaxValue) - logRange * mAutoScalingHysteresis)
			mCurrentMaxValue = newMaxValue;
	}

	// set new range
	mValueAxis->setRange(mCurrentMinValue, mCurrentMaxValue);
}


// register attributes and create the default values
void SpectrogramPlugin::RegisterAttributes()
{
	// register base class attributes
	Plugin::RegisterAttributes();

	// camera animation
	AttributeSettings* attributeSettings = RegisterAttribute("Camera Animation", "cameraAnimation", "", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeSettings->SetDefaultValue( AttributeBool::Create(false) );

	// orthogonal view
	attributeSettings = RegisterAttribute("Orthogonal View", "orthogonalView", "", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeSettings->SetDefaultValue( AttributeBool::Create(false) );

	// camera preset
	attributeSettings = RegisterAttribute( "Camera Preset", "cameraPreset", "", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX );
	attributeSettings->SetDefaultValue( Core::AttributeInt32::Create( 0 ) );
	const uint32 numCameraPresets = 5;
	attributeSettings->ResizeComboValues( numCameraPresets );
	attributeSettings->SetComboValue( 0, "Isometric Right" );
	attributeSettings->SetComboValue( 1, "Isometric Right High" );
	attributeSettings->SetComboValue( 2, "Front" );
	attributeSettings->SetComboValue( 3, "Front High" );
	attributeSettings->SetComboValue( 4, "Directly Above" );

	// fill preset list indices
	mCameraPresetList.Add(Q3DCamera::CameraPresetIsometricRight);
	mCameraPresetList.Add(Q3DCamera::CameraPresetIsometricRightHigh);
	mCameraPresetList.Add(Q3DCamera::CameraPresetFront);
	mCameraPresetList.Add(Q3DCamera::CameraPresetFrontHigh);
	mCameraPresetList.Add(Q3DCamera::CameraPresetDirectlyAbove);

	// value axis type
	attributeSettings = RegisterAttribute( "Value Axis Type", "valueAxisType", "", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX );
	attributeSettings->SetDefaultValue( Core::AttributeInt32::Create( 0 ) );
	const uint32 numValueAxisTypes = 3;
	attributeSettings->ResizeComboValues( numValueAxisTypes );

	QString linearString= QString::fromWCharArray( L"Linear (\u00B5V)" );
	QString logString	= QString::fromWCharArray( L"Logarithmic (\u00B5V)" );
	QString logDbString	= QString::fromWCharArray( L"Logarithmic (dB\u00B5V)" );

	attributeSettings->SetComboValue( 0, linearString.toUtf8().data() );
	attributeSettings->SetComboValue( 1, logString.toUtf8().data() );
	attributeSettings->SetComboValue( 2, logDbString.toUtf8().data() );


	const int minValueSliderValue = -6 * 10; // 10^-6 = 0.000001
	const int maxValueSliderValue = 4 * 10;  // 10^4  = 10000

	// lower value
/*	attributeSettings = RegisterAttribute("Lower Value", "lowerValue", "", ATTRIBUTE_INTERFACETYPE_FLOATSLIDERLABEL);
	attributeSettings->SetDefaultValue( AttributeFloat::Create(0.0) );
	attributeSettings->SetMinValue( AttributeFloat::Create(minValueSliderValue) );
	attributeSettings->SetMaxValue( AttributeFloat::Create(maxValueSliderValue) );

	// upper value value
	attributeSettings = RegisterAttribute("Upper Value", "upperValue", "", ATTRIBUTE_INTERFACETYPE_FLOATSLIDERLABEL);
	attributeSettings->SetDefaultValue( AttributeFloat::Create(0.0) );
	attributeSettings->SetMinValue( AttributeFloat::Create(minValueSliderValue) );
	attributeSettings->SetMaxValue( AttributeFloat::Create(maxValueSliderValue) );*/

	// create default attribute values
	CreateDefaultAttributeValues();
}


void SpectrogramPlugin::SetCameraOrthoView(bool isOrtho)
{
	SetBoolAttribute( "orthogonalView", isOrtho );
}


void SpectrogramPlugin::SetCameraPreset(Q3DCamera::CameraPreset cameraPreset)
{
	Q3DCamera* camera = GetGraph()->scene()->activeCamera();
	camera->setCameraPreset( cameraPreset );
}


void SpectrogramPlugin::SetCameraPresetIndex(int32 index)
{
	QtDataVisualization::Q3DCamera::CameraPreset cameraPreset = (QtDataVisualization::Q3DCamera::CameraPreset)mCameraPresetList[GetCameraPresetIndex()];
	SetCameraPreset( cameraPreset );
}


// fill the settings window
void SpectrogramPlugin::AddSettings(DataVisualizationSettingsWidget* settingsWidget)
{
	// TODO: NEW WAY OF DOING IT! IF ALL IS PORTED COMPLETELY REMOVE THE DATAVIZ SETTINGS WIDGET
	mAttributeSetGridWidget = new AttributeSetGridWidget(settingsWidget);
	mAttributeSetGridWidget->ReInit(this);
	settingsWidget->AddAttributeSetGridWidget(mAttributeSetGridWidget);
	connect( mAttributeSetGridWidget->GetPropertyManager(), SIGNAL(ValueChanged(Property*)), this, SLOT(OnAttributeChangedBaseClass(Property*)) );

	// value slider range
	const int minValueSliderValue = -6 * 10; // 10^-6 = 0.000001
	const int maxValueSliderValue = 4 * 10;  // 10^4  = 10000

	// add min value slider
	if (mMinValueSlider != NULL)
	{
		mMinValueSlider->setMinimum(minValueSliderValue);
		mMinValueSlider->setMaximum(maxValueSliderValue);
		mMinValueLabel = new QLabel();
		QHBoxLayout* hLayout = new QHBoxLayout();
		hLayout->addWidget(mMinValueSlider);
		hLayout->addWidget(mMinValueLabel);
		settingsWidget->AddLayout("Lower Value", hLayout);
		mMinValueSlider->setValue(minValueSliderValue);
		connect(mMinValueSlider, SIGNAL(valueChanged(int)), this, SLOT(OnMinValueSliderChanged(int)));
		OnMinValueSliderChanged(minValueSliderValue);
	}

	// add max value slider
	if (mMaxValueSlider != NULL)
	{
		mMaxValueSlider->setMaximum(minValueSliderValue);
		mMaxValueSlider->setMaximum(maxValueSliderValue);
		mMaxValueLabel = new QLabel();
		QHBoxLayout* hLayout = new QHBoxLayout();
		hLayout->addWidget(mMaxValueSlider);
		hLayout->addWidget(mMaxValueLabel);
		settingsWidget->AddLayout("Upper Value", hLayout);
		mMaxValueSlider->setValue(maxValueSliderValue);
		connect(mMaxValueSlider, SIGNAL(valueChanged(int)), this, SLOT(OnMaxValueSliderChanged(int)));
		OnMaxValueSliderChanged(maxValueSliderValue);

	}

	// autoscale checkbox
	if (mAutoScalingCheckBox != NULL)
	{
		mAutoScalingCheckBox = new QCheckBox();
		settingsWidget->AddWidget("Autoscaling", mAutoScalingCheckBox);
		connect(mAutoScalingCheckBox, SIGNAL(clicked(bool)), this, SLOT(OnAutoScalingCheckBoxClicked(bool)));

		// default option : use autoscale
		mAutoScalingCheckBox->setChecked(true);
		OnAutoScalingCheckBoxClicked(true);
	}

	// 50 Hertz default max freq value
	const uint32 defaultMaxFrequencySliderValue = 50;
	const double maxInitFrequency = fmax(mMaxFrequency, defaultMaxFrequencySliderValue);

	// add min frequency slider + label
	if (mMinFrequencySlider != NULL)
	{
		mMinFrequencySlider->setMinimum(0);
		mMinFrequencySlider->setMaximum((int)maxInitFrequency);
		mMinFrequencyLabel = new QLabel();
		QHBoxLayout* hLayout = new QHBoxLayout();
		hLayout->addWidget(mMinFrequencySlider);
		hLayout->addWidget(mMinFrequencyLabel);
		settingsWidget->AddLayout("Lower Frequency", hLayout);
		mMinFrequencySlider->setValue(0);
		connect(mMinFrequencySlider, SIGNAL(valueChanged(int)), this, SLOT(OnMinFrequencySliderChanged(int)));
	}
	
	// add max frequency slider + label
	if (mMaxFrequencySlider != NULL)
	{
		mMaxFrequencySlider->setMinimum(0);
		mMaxFrequencySlider->setMaximum((int)maxInitFrequency);
		mMaxFrequencyLabel = new QLabel();
		QHBoxLayout* hLayout = new QHBoxLayout();
		hLayout->addWidget(mMaxFrequencySlider);
		hLayout->addWidget(mMaxFrequencyLabel);
		settingsWidget->AddLayout("Upper Frequency", hLayout);
		mMaxFrequencySlider->setValue(defaultMaxFrequencySliderValue);
		connect(mMaxFrequencySlider, SIGNAL(valueChanged(int)), this, SLOT(OnMaxFrequencySliderChanged(int)));
	}

	// set default values
	if (mMinFrequencySlider != NULL)
		OnMinFrequencySliderChanged(0);
	if (mMaxFrequencySlider != NULL)
		OnMaxFrequencySliderChanged(defaultMaxFrequencySliderValue);
}


void SpectrogramPlugin::SetValueAxisTypeIndex(int32 index)
{
	mValueAxisType = (ValueAxisType)index;
	mValueAxis = CreateValueAxis(mValueAxisType);

	// reset autoscaling values if its enabled because both one axis is scale in log, on in lin space
	if (mUseAutoScaling == true)
	{
		mCurrentMinValue = DBL_MAX;
		mCurrentMaxValue = -DBL_MAX;
	}

	UpdateValueAxis();
}

void SpectrogramPlugin::OnMinValueSliderChanged(int value)
{
	// clamp bottom to max-1
	if (value >= mMaxValueSlider->value())
	{
		value = mMaxValueSlider->value() - 1;
		mMinValueSlider->setValue(value);
	}

	// TODO switch calculation depending on log/lin scale
	const double expVal = pow(10, (double)value / 10.0);
#ifdef NEUROMORE_PLATFORM_WINDOWS
	mTmpQString = QString().sprintf("10^%.1f", (double)value / 10.0) + QChar('\u00B5') + QChar('V');
#else
    mTmpQString = QString().sprintf(u8"10^%.1f \u00B5V", (double)value / 10.0);
#endif
	mMinValueLabel->setText(mTmpQString);
	mMinValue = expVal;
	mValueAxis->setRange(mMinValue, mMaxValue);
}


void SpectrogramPlugin::OnMaxValueSliderChanged(int value)
{
	// clamp top to to min+1
	if (value <= mMinValueSlider->value())
	{
		value = mMinValueSlider->value() + 1;
		mMaxValueSlider->setValue(value);
	}

	// TODO switch calculation depending on log/lin scale
	const double expVal = pow(10, (double)value / 10.0);

#ifdef NEUROMORE_PLATFORM_WINDOWS
	mTmpQString = QString().sprintf("10^%.1f", (double)value / 10.0) + QChar('\u00B5') + QChar('V');
#else
    mTmpQString = QString().sprintf(u8"10^%.1f \u00B5V", (double)value / 10.0);
#endif

	mMaxValueLabel->setText(mTmpQString);
	mMaxValue = expVal;
	mValueAxis->setRange(mMinValue, mMaxValue);
}


void SpectrogramPlugin::OnAutoScalingCheckBoxClicked(bool checked)
{
	mUseAutoScaling = checked;

	// enable/disable min/max sliders
	const bool slidersEnabled = !mUseAutoScaling;
	if (mMinValueSlider != NULL) {
		mMinValueSlider->setEnabled(slidersEnabled);
		
		// reset values to that of the sliders
		if (mUseAutoScaling == false)
			OnMinValueSliderChanged(mMinValueSlider->value());
	}

	if (mMaxValueSlider != NULL)
	{
		mMaxValueSlider->setEnabled(slidersEnabled);
		
		// reset values to that of the sliders
		if (mUseAutoScaling == false)
			OnMaxValueSliderChanged(mMaxValueSlider->value());
	}

	// reset min/max values on enabling
	if (mUseAutoScaling == true)
	{
		mCurrentMinValue = DBL_MAX;
		mCurrentMaxValue = -DBL_MAX;
	}
}


void SpectrogramPlugin::OnMinFrequencySliderChanged(int value)
{
	// clamp top to max - 5
	if (value > mMaxFrequencySlider->value() - 5)
	{
		value = mMaxFrequencySlider->value() - 5;
		mMinFrequencySlider->setValue(value);
	}

	mMinFrequency = value;

	mTempString.Format("%.1f Hz", mMinFrequency);
	mMinFrequencyLabel->setText(mTempString.AsChar());
	OnMinFrequencyChanged(mMinFrequency);
}


void SpectrogramPlugin::OnMaxFrequencySliderChanged(int value)
{
	// clamp top to min + 5
	if (value <= mMinFrequencySlider->value() + 5)
	{
		value = mMinFrequencySlider->value() + 5;
		mMaxFrequencySlider->setValue(value);
	}
	mMaxFrequency = value;

	mTempString.Format("%.1f Hz", mMaxFrequency);
	mMaxFrequencyLabel->setText(mTempString.AsChar());
	OnMaxFrequencyChanged(mMaxFrequency);
}


QValue3DAxis* SpectrogramPlugin::CreateValueAxis(ValueAxisType type)
{
	QValue3DAxis* axis = new QValue3DAxis();

	// apply default style
	StyleValueAxis(axis);

	// remember axis so the settings can modify it in the settings slots
	mValueAxis = axis;

	// use logarithmic formatter ( = log grid)
	if (type == ValueAxisType::LOGARITHMIC)
	{ 
		QValue3DAxisFormatter* logFormatter = new QLogValue3DAxisFormatter();
		axis->setFormatter(logFormatter);
	}

	// label shows dB?
	if (type == ValueAxisType::DEZIBEL)
		axis->setLabelFormat(QString::fromWCharArray(L"%.3f dB\u00B5V"));
	else
		axis->setLabelFormat(QString::fromWCharArray(L"%.3f \u00B5V"));

	// axis title
	axis->setTitle(QStringLiteral("Magnitude"));
	
	// set axis range
	axis->setRange(mMinValue, mMaxValue);

	return axis;
}


// update frequency slider range
void SpectrogramPlugin::SetFrequencyRange(double minFrequency, double maxFrequency)
{
	// set range of min freq slider
	if (mMinFrequencySlider != NULL)
	{
		const int sliderValue = mMinFrequencySlider->value();
		mMinFrequencySlider->setMinimum((int)minFrequency);
		mMinFrequencySlider->setMaximum((int)ceil(maxFrequency));		// round in the correct direction so no data is ommitted in rendering
		mMinFrequencySlider->setValue(sliderValue);

		// adjust current slider value only if the current one is now outside the range
		if (minFrequency > mMinFrequency)
		{
			const int value = minFrequency;
			mMinFrequencySlider->setValue(value);
			OnMinFrequencySliderChanged(value);
		}
	}

	// set range of max freq slider
	if (mMaxFrequencySlider != NULL)
	{
		const int sliderValue = mMaxFrequencySlider->value();
		mMaxFrequencySlider->setMinimum((int)minFrequency);	
		mMaxFrequencySlider->setMaximum((int)ceil(maxFrequency));		// round in the correct direction so no data is ommitted in rendering
		mMaxFrequencySlider->setValue(sliderValue);

		// adjust current slider value only if the current one is now outside the range
		if (maxFrequency < mMaxFrequency)
		{
			const int value = maxFrequency;
			mMaxFrequencySlider->setValue(value);
			OnMaxFrequencySliderChanged(value);
		}
	}

	// remember slider range internally
	mMinFrequency = minFrequency;
	mMaxFrequency = maxFrequency;
}

Q3DTheme* SpectrogramPlugin::GetDefaultTheme()
{
	if (mDefaultTheme == NULL)
	{
		const QColor windowBGColor	= QColor(40,40,40);
		const QColor gridBGColor	= QColor(55,55,55);
		const QColor gridColor		= QColor(157,157,156);

		mDefaultTheme = new Q3DTheme(Q3DTheme::ThemeQt);

		// window background
		mDefaultTheme->setWindowColor(windowBGColor);

		// grid background color
		mDefaultTheme->setBackgroundColor(gridBGColor);
		mDefaultTheme->setBackgroundEnabled(false);

		// grid color
		mDefaultTheme->setGridLineColor(gridColor);

		// label text
		mDefaultTheme->setLabelTextColor(QColor(237,237,237));

		// label border
		mDefaultTheme->setLabelBorderEnabled(false);

		// label background
		mDefaultTheme->setLabelBackgroundEnabled(false);
		mDefaultTheme->setLabelBackgroundColor(gridBGColor);

		mDefaultTheme->setFont(QFont("Arial", 20));
		mDefaultTheme->setColorStyle(Q3DTheme::ColorStyleUniform);

		mDefaultTheme->setLightColor(Qt::white);
		mDefaultTheme->setLightStrength(1.0f);
		mDefaultTheme->setAmbientLightStrength(0.5);
		mDefaultTheme->setHighlightLightStrength(0.0);
	}
	return mDefaultTheme;
}


QLinearGradient SpectrogramPlugin::GetColorGradient(int index)
{
	CORE_ASSERT(index < ColorMapper::COLORMAPPING_NUM);

	ColorMapper colorMapper;
	colorMapper.SetColorMapping((ColorMapper::EColorMapping)index);
	int numColors = colorMapper.GetNumColors();

	// iterate over colors, place them at equal distances
	// FIXME support non-linear gradients and non-uniform color distance
	QLinearGradient gradient;
	for (int i = 0; i < numColors; i++)
	{
		const float colorPos = i / (double)(numColors - 1);

		const Color color = colorMapper.GetColor(i);
		const QColor qcolor = QColor(color.r * 255.0, color.g * 255.0, color.b * 255.0);

		gradient.setColorAt(colorPos, qcolor);
	}

	return gradient;
}


// set the default style of a value axis
void SpectrogramPlugin::StyleValueAxis(QValue3DAxis* inOutAxis)
{
	inOutAxis->setAutoAdjustRange(true);
	inOutAxis->setSegmentCount(5);
	inOutAxis->setSubSegmentCount(2);
}

#endif
