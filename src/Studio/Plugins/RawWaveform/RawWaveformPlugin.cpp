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
#include "RawWaveformPlugin.h"

using namespace Core;

// constructor
RawWaveformPlugin::RawWaveformPlugin() : Plugin(GetStaticTypeUuid())
{
	LogDetailedInfo("Constructing Raw Waveform plugin ...");
	mWaveformWidget			= NULL;
	mChannelSelectionWidget	= NULL;
}


// destructor
RawWaveformPlugin::~RawWaveformPlugin()
{
	LogDetailedInfo("Destructing Raw Waveform plugin ...");
}


// initialize
bool RawWaveformPlugin::Init()
{
	LogDetailedInfo("Initializing Raw Waveform plugin ...");

	QWidget*		mainWidget		= NULL;
	QHBoxLayout*	mainLayout		= NULL;
	CreateDockMainWidget( &mainWidget, &mainLayout );

	///////////////////////////////////////////////////////////////////////////
	// Toolbar (top-left)
	///////////////////////////////////////////////////////////////////////////

	Core::Array<QWidget*> toolbarWidgets;

	// init checkbox widget
	mChannelSelectionWidget = new ChannelMultiSelectionWidget();
	mChannelSelectionWidget->SetAutoSelectType((ChannelMultiSelectionWidget::AutoSelectType)Branding::DefaultAutoSelectType);
	mChannelSelectionWidget->SetShowNeuroChannelsOnly(true);
	mChannelSelectionWidget->Init();
	connect( mChannelSelectionWidget, SIGNAL(ShowUsedCheckboxToggled(int)), this, SLOT(OnShowUsedCheckboxToggled(int)) );

	toolbarWidgets.Add(mChannelSelectionWidget);

	///////////////////////////////////////////////////////////////////////////
	// Settings
	///////////////////////////////////////////////////////////////////////////

	// create the attribute set grid widget
	AttributeSetGridWidget* attributeSetGridWidget = new AttributeSetGridWidget( GetDockWidget() );
	attributeSetGridWidget->ReInit(this);

	SetSettingsWidget( attributeSetGridWidget );

	///////////////////////////////////////////////////////////////////////////
	// Add render widget at the end
	///////////////////////////////////////////////////////////////////////////

	mWaveformWidget = new RawWaveformWidget(this, mainWidget);
	SetRealtimeWidget( mWaveformWidget );
	
	///////////////////////////////////////////////////////////////////////////
	// Fill everything
	///////////////////////////////////////////////////////////////////////////
	FillLayouts(mainWidget, mainLayout, toolbarWidgets, "Settings", "Gear", mWaveformWidget);

	// use attribute to check showUsed checkbox
	mChannelSelectionWidget->SetShowUsedChecked(GetBoolAttributeByName("showUsedChannels", true));

	LogDetailedInfo("Raw Waveform plugin successfully initialized");

	return true;
}


// register attributes and create the default values
void RawWaveformPlugin::RegisterAttributes()
{
	// register base class attributes
	Plugin::RegisterAttributes();

	// y scale
	AttributeSettings* attributeSettings = RegisterAttribute("uV/Div", "uV/Div", "Amplitude scale in microvolts per division.", ATTRIBUTE_INTERFACETYPE_FLOATSLIDER);
	attributeSettings->SetDefaultValue( AttributeFloat::Create(300.0f) );
	attributeSettings->SetMinValue( AttributeFloat::Create(0.01f) );
	attributeSettings->SetMaxValue( AttributeFloat::Create(1000.0f) );

	// autoscale amplitude
	attributeSettings = RegisterAttribute("Autoscale", "autoScale", "", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeSettings->SetDefaultValue( AttributeBool::Create(true) );

	// x scale
	attributeSettings = RegisterAttribute("Time (s)", "time", "Length of the displayed Waveform Interval in seconds.", ATTRIBUTE_INTERFACETYPE_FLOATSLIDER);
	attributeSettings->SetDefaultValue( AttributeFloat::Create(10.0f) );
	attributeSettings->SetMinValue( AttributeFloat::Create(1.0f) );
	attributeSettings->SetMaxValue( AttributeFloat::Create(1200.0f) );

	// show voltages
	attributeSettings = RegisterAttribute("Show Voltages", "showVoltages", "", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeSettings->SetDefaultValue( AttributeBool::Create(false) );

	// show times
	attributeSettings = RegisterAttribute("Show Times", "showTimes", "", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeSettings->SetDefaultValue( AttributeBool::Create(true) );

	// internal "show used" checkbox in channel selection widget
	attributeSettings = RegisterAttribute("", "showUsedChannels", "", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeSettings->SetDefaultValue( AttributeBool::Create(false) );
	attributeSettings->SetVisible(false);

	// create default attribute values
	CreateDefaultAttributeValues();
}


void RawWaveformPlugin::UpdateInterface()
{
	// hide and uncheck all unused sensors
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	Device* device = mChannelSelectionWidget->GetSelectedDevice();

	bool channelsChanged = false;
	bool hasUsedChannels = false;
	bool hasDisabledChannels = false;

	// hide checkbxoes if neuro sensors are unused
	if (device != NULL)
	{
		const uint32 numSensors = device->GetNumSensors();
		for (uint32 i = 0; i < numSensors; i++)
		{
			Sensor* sensor = device->GetSensor(i);
			Channel<double>* channel = sensor->GetChannel();
			bool isUsed = sensor->IsEnabled(); // sensor must be used by classifier and also enabled to count as 'active'
			if (isUsed && classifier != NULL)
				isUsed = classifier->IsSensorUsed(sensor);

			if (mChannelSelectionWidget->IsChannelUsed(channel) != isUsed)
			{
				mChannelSelectionWidget->SetChannelAsUsed(channel, isUsed);
				channelsChanged = true;
			}

			// remember if at least one channel is used
			if (isUsed == true)
				hasUsedChannels = true;
			else
				hasDisabledChannels = true;
		}
	}

	if (hasUsedChannels == false)
		mChannelSelectionWidget->ClearUsedChannels();

	if (channelsChanged == true)
		mChannelSelectionWidget->ReInit();

	// show/hide "used" checkbox
	mChannelSelectionWidget->SetShowUsedVisible(hasDisabledChannels);
}


void RawWaveformPlugin::OnShowUsedCheckboxToggled(int state)
{
	if (state == Qt::Checked)
		SetBoolAttribute("showUsedChannels", true, false);
	else
		SetBoolAttribute("showUsedChannels", false, false);
}
