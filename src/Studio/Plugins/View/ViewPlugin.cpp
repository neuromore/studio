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
#include "ViewPlugin.h"
#include <Core/LogManager.h>
#include <EngineManager.h>
#include <QtBaseManager.h>
#include "../../AppManager.h"
#include "../../MainWindow.h"
#include <DSP/Epoch.h>
#include <AttributeWidgets/AttributeSetGridWidget.h>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QApplication>
#include <QDesktopWidget>


using namespace Core;

// constructor
ViewPlugin::ViewPlugin() : Plugin(GetStaticTypeUuid())
{
	LogDetailedInfo("Constructing Signal View plugin ...");
	mViewWidget			= NULL;
}


// destructor
ViewPlugin::~ViewPlugin()
{
	LogDetailedInfo("Destructing Signal View plugin ...");
	CORE_EVENTMANAGER.RemoveEventHandler(this);
}


// init after the parent dock window has been created
bool ViewPlugin::Init()
{
	LogDetailedInfo("Initializing Signal View plugin ...");

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
	mViewWidget = new ViewWidget(this, vWidget);
	SetRealtimeWidget( mViewWidget );
	vLayout->addWidget( mViewWidget );
	UpdateInterface();
	
	///////////////////////////////////////////////////////////////////////////
	// Fill everything
	///////////////////////////////////////////////////////////////////////////
	FillLayouts(mainWidget, mainLayout, toolbarWidgets, "Settings", "Gear", vWidget);

	vWidget->show();
	
	CORE_EVENTMANAGER.AddEventHandler(this);

	LogDetailedInfo("Signal View plugin successfully initialized");

	connect(attributeSetGridWidget->GetPropertyManager(), SIGNAL(ValueChanged(Property*)), this, SLOT(OnAttributeChanged(Property*)));


	return true;
}


// register attributes and create the default values
void ViewPlugin::RegisterAttributes()
{
	// register base class attributes
	Plugin::RegisterAttributes();

	// displayed interval duration
	const double defaultViewRange = 10;
	AttributeSettings* attributeTimeRange = RegisterAttribute("Time Range (s)", "timeRange", "Length of the displayed interval in seconds.", ATTRIBUTE_INTERFACETYPE_FLOATSLIDER);
	attributeTimeRange->SetDefaultValue( AttributeFloat::Create(defaultViewRange) );
	attributeTimeRange->SetMinValue( AttributeFloat::Create(1.0f) );
	attributeTimeRange->SetMaxValue( AttributeFloat::Create(120.0f) );

	// set default view duration
	SetViewDuration(defaultViewRange);

	// visual sample style
	AttributeSettings* attributeStyle = RegisterAttribute("Style", "style", "The visual appearance of the chart.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attributeStyle->AddComboValue("Boxes");
	attributeStyle->AddComboValue("Bars");
	attributeStyle->AddComboValue("Lollipops");
	attributeStyle->AddComboValue("Dots");
	attributeStyle->AddComboValue("Lines");
	attributeStyle->SetDefaultValue( AttributeInt32::Create(4) );	// use lines as default style

	// show latency marker checkbox
	AttributeSettings* attributeShowLatencyMarker = RegisterAttribute("Show Latency", "showLatencyMarker", "Show a latency indicator marking the average latent sample.", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeShowLatencyMarker->SetDefaultValue( AttributeBool::Create(false) );

	CreateDefaultAttributeValues();
}


void ViewPlugin::OnAttributeChanged(Property* property)
{
	const String& propertyInternalName = property->GetAttributeSettings()->GetInternalNameString();

	// timerange slider has changed
	if (propertyInternalName.IsEqual("timeRange") == true)
	{
		const double timerange = property->AsFloat();
		SetViewDuration(timerange);
	}
}


uint32 ViewPlugin::GetNumMultiChannels()
{
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	if (classifier == NULL)
		return 0;

	return classifier->GetNumViewMultiChannels();
}


const MultiChannel& ViewPlugin::GetMultiChannel(uint32 index)
{
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	CORE_ASSERT(classifier);

	return classifier->GetViewMultiChannel(index);
}


Core::Color ViewPlugin::GetChannelColor(uint32 multichannel, uint32 index)
{
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	CORE_ASSERT(classifier);

	const ViewNode& node = classifier->GetViewNodeForMultiChannel(multichannel);
	if (node.CustomColor() == true)
		return node.GetCustomColor();
	
	return classifier->GetViewMultiChannel(multichannel).GetChannel(index)->GetColor();
}


void ViewPlugin::SetViewDuration(double seconds)
{

	Classifier* classifier = GetEngine()->GetActiveClassifier();
	if (classifier == NULL)
		return;

	// set view duration of all view nodes in the classifier (always, even if the view mode is different)
	const uint32 numViewNodes = classifier->GetNumViewNodes();
	for (uint32 i=0; i<numViewNodes; ++i)
		classifier->GetViewNode(i)->SetViewDuration(seconds);
}

