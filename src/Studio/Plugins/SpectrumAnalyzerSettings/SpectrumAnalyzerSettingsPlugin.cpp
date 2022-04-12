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
#include "SpectrumAnalyzerSettingsPlugin.h"
#include "../../MainWindow.h"
#include "../../AppManager.h"
#include <Core/LogManager.h>
#include <QHBoxLayout>
#include <QPushButton>
#include <QIcon>

using namespace Core;


// constructor
SpectrumAnalyzerSettingsPlugin::SpectrumAnalyzerSettingsPlugin() : Plugin(GetStaticTypeUuid())
{
	LogDetailedInfo("Constructing spectrogram analyzer settings plugin ...");
	mSettingsWidget			= NULL;
}


// destructor
SpectrumAnalyzerSettingsPlugin::~SpectrumAnalyzerSettingsPlugin()
{
	LogDetailedInfo("Destructing spectrogram analyzer settings plugin ...");
}


// clone the plugin
Plugin* SpectrumAnalyzerSettingsPlugin::Clone()
{
	return new SpectrumAnalyzerSettingsPlugin();
}


// init after the parent dock window has been created
bool SpectrumAnalyzerSettingsPlugin::Init()
{
	LogDetailedInfo("Initializing spectrogram analyzer settings plugin ...");

	// initialize the plugin
	ReInit();

	LogDetailedInfo("Spectrogram analyzer settings plugin successfully initialized");

	return true;
}


// reinitialize the window
void SpectrumAnalyzerSettingsPlugin::ReInit()
{
	// main vertical layout
	QWidget* mainWidget = new QWidget();
	mainWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	mainWidget->setFocusPolicy(Qt::StrongFocus);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainWidget->setLayout( mainLayout );

	mSettingsWidget = new SpectrumAnalyzerSettingsWidget( mainWidget, this );
	mainLayout->addWidget(mSettingsWidget);

	// add spacer widget
	QWidget* spacerWidget = new QWidget();
	spacerWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
	mainLayout->addWidget(spacerWidget);

	// prepare the dock window
	mDock->SetContents(mainWidget);
}
