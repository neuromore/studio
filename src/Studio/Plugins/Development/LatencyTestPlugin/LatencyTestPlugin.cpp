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
#include "LatencyTestPlugin.h"
#include <Core/LogManager.h>
#include <AttributeWidgets/AttributeSetGridWidget.h>


using namespace Core;

// constructor
LatencyTestPlugin::LatencyTestPlugin() : Plugin(GetStaticTypeUuid())
{
	LogDetailedInfo("Constructing Latency Test plugin ...");
	mLatencyWidget = NULL;
}


// destructor
LatencyTestPlugin::~LatencyTestPlugin()
{
	LogDetailedInfo("Destructing Latency Test plugin ...");
}


// initialize
bool LatencyTestPlugin::Init()
{
	LogDetailedInfo("Initializing Latency Test plugin ...");

	QWidget*		mainWidget		= NULL;
	QHBoxLayout*	mainLayout		= NULL;
	CreateDockMainWidget( &mainWidget, &mainLayout );

	///////////////////////////////////////////////////////////////////////////
	// Toolbar (top-left)
	///////////////////////////////////////////////////////////////////////////

	Core::Array<QWidget*> toolbarWidgets;

	// TODO: add toolbar widgets to this array, they will be automatically added to the top left (left of the settings gear icon) of the plugin

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

	mLatencyWidget = new LatencyTestWidget(this, mainWidget);
	SetRealtimeWidget( mLatencyWidget );
	
	///////////////////////////////////////////////////////////////////////////
	// Fill everything
	///////////////////////////////////////////////////////////////////////////
	FillLayouts(mainWidget, mainLayout, toolbarWidgets, "Settings", "Gear", mLatencyWidget);
	
	LogDetailedInfo("Latency Test plugin successfully initialized");

	return true;
}


// register attributes and create the default values
void LatencyTestPlugin::RegisterAttributes()
{
	// register base class attributes
	Plugin::RegisterAttributes();

	// threshold
	AttributeSettings* attributeSettings = RegisterAttribute("Threshold", "threshold", "", ATTRIBUTE_INTERFACETYPE_FLOATSLIDER);
	attributeSettings->SetDefaultValue( AttributeFloat::Create(0.5) );
	attributeSettings->SetMinValue( AttributeFloat::Create(-FLT_MAX) );
	attributeSettings->SetMaxValue( AttributeFloat::Create(+FLT_MAX) );

	// create default attribute values
	CreateDefaultAttributeValues();
}
