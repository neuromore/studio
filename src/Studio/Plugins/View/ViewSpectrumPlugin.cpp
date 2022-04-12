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
#include "ViewSpectrumPlugin.h"
#include <Studio/MainWindow.h>
#include <AttributeWidgets/AttributeSetGridWidget.h>

using namespace Core;

// constructor
ViewSpectrumPlugin::ViewSpectrumPlugin() : Plugin(GetStaticTypeUuid())
{
	LogDetailedInfo("Constructing Spectrum View plugin ...");
	mViewWidget			= NULL;
}


// destructor
ViewSpectrumPlugin::~ViewSpectrumPlugin()
{
	LogDetailedInfo("Destructing Spectrum View plugin ...");
	CORE_EVENTMANAGER.RemoveEventHandler(this);
}


// init after the parent dock window has been created
bool ViewSpectrumPlugin::Init()
{
	LogDetailedInfo("Initializing Spectrum View plugin ...");

	QWidget*		mainWidget		= NULL;
	QHBoxLayout*	mainLayout		= NULL;
	CreateDockMainWidget( &mainWidget, &mainLayout );

	///////////////////////////////////////////////////////////////////////////
	// Toolbar (top-left)
	///////////////////////////////////////////////////////////////////////////

	Core::Array<QWidget*> toolbarWidgets;

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

	QWidget* vWidget = new QWidget(mainWidget);
	vWidget->hide();
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->setMargin(0);
	vLayout->setSpacing(0);
	vWidget->setLayout(vLayout);

	// add the view widget
	mViewWidget = new ViewSpectrumWidget(this, vWidget);
	SetRealtimeWidget( mViewWidget );
	vLayout->addWidget( mViewWidget );
	UpdateInterface();
	
	///////////////////////////////////////////////////////////////////////////
	// Fill everything
	///////////////////////////////////////////////////////////////////////////
	FillLayouts(mainWidget, mainLayout, toolbarWidgets, "Settings", "Gear", vWidget);

	vWidget->show();
	
	CORE_EVENTMANAGER.AddEventHandler(this);

	LogDetailedInfo("Spectrum View plugin successfully initialized");

	return true;
}


// register attributes and create the default values
void ViewSpectrumPlugin::RegisterAttributes()
{
	// register base class attributes
	Plugin::RegisterAttributes();

	AttributeSettings* attributeMinFrequency = RegisterAttribute("Min Frequency", "minFrequency", "The minimum frequency of the spectrum display.", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeMinFrequency->SetMinValue(AttributeFloat::Create(0.0f));
	attributeMinFrequency->SetMaxValue(AttributeFloat::Create(FLT_MAX));

	AttributeSettings* attributeMaxFrequency = RegisterAttribute("Max Frequency", "maxFrequency", "The minimum frequency of the spectrum display.", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeMaxFrequency->SetMinValue(AttributeFloat::Create(0.0f));
	attributeMaxFrequency->SetMaxValue(AttributeFloat::Create(FLT_MAX));

#ifdef NEUROMORE_BRANDING_ANT
	attributeMinFrequency->SetDefaultValue(AttributeFloat::Create(1.0f));
	attributeMaxFrequency->SetDefaultValue(AttributeFloat::Create(45.0f));
#else
	attributeMinFrequency->SetDefaultValue(AttributeFloat::Create(0.0f));
	attributeMaxFrequency->SetDefaultValue(AttributeFloat::Create(50.0f));
#endif

	// create default attribute values
	CreateDefaultAttributeValues();
}


uint32 ViewSpectrumPlugin::GetNumMultiChannels()
{
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	if (classifier == NULL)
		return 0;

	return classifier->GetNumViewSpectrumMultiChannels();
}


const MultiChannel& ViewSpectrumPlugin::GetMultiChannel(uint32 index)
{
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	CORE_ASSERT(classifier);

	return classifier->GetViewSpectrumMultiChannel(index);
}


Core::Color ViewSpectrumPlugin::GetChannelColor(uint32 multichannel, uint32 index)
{
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	CORE_ASSERT(classifier);

	const ViewNode& node = classifier->GetViewNodeForSpectrumMultiChannel(multichannel);
	if (node.CustomColor() == true)
		return node.GetCustomColor();
	
	return classifier->GetViewSpectrumMultiChannel(multichannel).GetChannel(index)->GetColor();
}

