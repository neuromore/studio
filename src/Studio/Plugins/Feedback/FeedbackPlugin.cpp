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
#include "FeedbackPlugin.h"
#include <Core/LogManager.h>
#include <EngineManager.h>
#include <QtBaseManager.h>
#include "../../AppManager.h"
#include "../../MainWindow.h"
#include <AttributeWidgets/AttributeSetGridWidget.h>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QApplication>
#include <QDesktopWidget>


using namespace Core;

// constructor
FeedbackPlugin::FeedbackPlugin() : Plugin(GetStaticTypeUuid())
{
	LogDetailedInfo("Constructing feedback plugin ...");
	mHistoryWidget			= NULL;
}


// destructor
FeedbackPlugin::~FeedbackPlugin()
{
	LogDetailedInfo("Destructing feedback plugin ...");
	CORE_EVENTMANAGER.RemoveEventHandler(this);
}


// init after the parent dock window has been created
bool FeedbackPlugin::Init()
{
	LogDetailedInfo("Initializing feedback plugin ...");

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

	// add the history widget
	mHistoryWidget = new FeedbackHistoryWidget(this, vWidget);
	SetRealtimeWidget( mHistoryWidget );
	vLayout->addWidget( mHistoryWidget );
	UpdateInterface();
	
	///////////////////////////////////////////////////////////////////////////
	// Fill everything
	///////////////////////////////////////////////////////////////////////////
	FillLayouts(mainWidget, mainLayout, toolbarWidgets, "Settings", "Gear", vWidget);

	vWidget->show();
	
	CORE_EVENTMANAGER.AddEventHandler(this);

	LogDetailedInfo("Feedback plugin successfully initialized");

	return true;
}


// register attributes and create the default values
void FeedbackPlugin::RegisterAttributes()
{
	// register base class attributes
	Plugin::RegisterAttributes();

	// displayed interval duration
	AttributeSettings* attributeTime = RegisterAttribute("Time Range (s)", "timeRange", "Length of the displayed interval in seconds.", ATTRIBUTE_INTERFACETYPE_FLOATSLIDER);
	attributeTime->SetDefaultValue( AttributeFloat::Create(10.0f) );
	attributeTime->SetMinValue( AttributeFloat::Create(1.0f) );
	attributeTime->SetMaxValue( AttributeFloat::Create(120.0f) );

	// visual sample style
	AttributeSettings* attributeStyle = RegisterAttribute("Style", "style", "The visual appearance of the chart.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attributeStyle->AddComboValue("Boxes");
	attributeStyle->AddComboValue("Bars");
	attributeStyle->AddComboValue("Lollipops");
	attributeStyle->AddComboValue("Dots");
	attributeStyle->AddComboValue("Lines");
	attributeStyle->SetDefaultValue( AttributeInt32::Create(0) );

	// show latency marker checkbox
	AttributeSettings* attributeShowLatencyMarker = RegisterAttribute("Show Latency", "showLatencyMarker", "Show a latency indicator marking the average latent sample.", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeShowLatencyMarker->SetDefaultValue( AttributeBool::Create(false) );

	// create default attribute values
	CreateDefaultAttributeValues();
}
