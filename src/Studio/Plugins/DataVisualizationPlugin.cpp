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
#include "DataVisualizationPlugin.h"
#include <Core/LogManager.h>
#include <QtBaseManager.h>
#include <AttributeWidgets/PropertyTreeWidget.h>
#include <QHBoxLayout>

#ifdef USE_QTDATAVISUALIZATION

#ifdef CORE_DEBUG
	#pragma comment(lib, "DataVisualizationd.lib")
#else
	#pragma comment(lib, "DataVisualization.lib")
#endif

using namespace Core;

// constructor
DataVisualizationPlugin::DataVisualizationPlugin(const char* typeUuid) : Plugin(typeUuid)
{
	LogDetailedInfo("Constructing data visualization plugin ...");

	mRenderWidget			= NULL;
	mSettingsWidget			= NULL;
}


// destructor
DataVisualizationPlugin::~DataVisualizationPlugin()
{
	LogDetailedInfo("Destructing data visualization plugin ...");
}


// init after the parent dock window has been created
bool DataVisualizationPlugin::InitLayout(QWidget* renderWidget, QWidget* topLeftWidget)
{
	LogDetailedInfo("Initializing data visualization plugin layout ...");

	QWidget*		mainWidget		= NULL;
	QHBoxLayout*	mainLayout		= NULL;
	CreateDockMainWidget( &mainWidget, &mainLayout );

	///////////////////////////////////////////////////////////////////////////
	// Toolbar (top-left)
	///////////////////////////////////////////////////////////////////////////

	Core::Array<QWidget*> toolbarWidgets;

	// create the top-left widget
	if (topLeftWidget != NULL)
		toolbarWidgets.Add(topLeftWidget);

	// FPS label
	mFpsLabel = new QLabel();
	toolbarWidgets.Add(mFpsLabel);

	///////////////////////////////////////////////////////////////////////////
	// Settings
	///////////////////////////////////////////////////////////////////////////

	mSettingsWidget = new DataVisualizationSettingsWidget(settingsDialogStack, this);
	
	SetSettingsWidget( mSettingsWidget );

	///////////////////////////////////////////////////////////////////////////
	// Fill everything
	///////////////////////////////////////////////////////////////////////////
	FillLayouts(mainWidget, mainLayout, toolbarWidgets, "Settings", "Gear", renderWidget);

	return true;
}


void DataVisualizationPlugin::OnCurrentFpsChanged( double fps )
{
	mTempString.Format("%.1f FPS", fps);
	mFpsLabel->setText(mTempString.AsChar());
}

#endif
