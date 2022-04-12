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
#include "EEGElectrodePlacementPlugin.h"
#include <AttributeWidgets/AttributeSetGridWidget.h>

using namespace Core;

// constructor
EEGElectrodePlacementPlugin::EEGElectrodePlacementPlugin() : Plugin(GetStaticTypeUuid())
{
	LogDetailedInfo("Constructing EEG Electrode Placement plugin ...");
	mElectrodesWidget = NULL;
}


// destructor
EEGElectrodePlacementPlugin::~EEGElectrodePlacementPlugin()
{
	LogDetailedInfo("Destructing EEG Electrode Placement plugin ...");
}


// initialize
bool EEGElectrodePlacementPlugin::Init()
{
	LogDetailedInfo("Initializing EEG Electrode Placement plugin ...");

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

	mElectrodesWidget = new EEGElectrodesWidget(mainWidget);
	SetRealtimeWidget( mElectrodesWidget );

	mElectrodesWidget->SetDevice(GetEngine()->GetActiveBci());
	
	///////////////////////////////////////////////////////////////////////////
	// Fill everything
	///////////////////////////////////////////////////////////////////////////
	FillLayouts(mainWidget, mainLayout, toolbarWidgets, "Settings", "Gear", mElectrodesWidget);
	
	LogDetailedInfo("Example EEG Electrode Placement successfully initialized");

	return true;
}


// register attributes and create the default values
void EEGElectrodePlacementPlugin::RegisterAttributes()
{
	// register base class attributes
	Plugin::RegisterAttributes();

	// show feedback control widget
	AttributeSettings* attributeSettings = RegisterAttribute("Multi view", "multiView", "Here comes the description of the value, this will become the tooltip.", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeSettings->SetDefaultValue( AttributeBool::Create(false) );

	// create default attribute values
	CreateDefaultAttributeValues();
}
