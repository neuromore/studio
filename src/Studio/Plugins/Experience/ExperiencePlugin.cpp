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
#include "ExperiencePlugin.h"
#include <EngineManager.h>
#include <QtBaseManager.h>
#include "../../AppManager.h"
#include <Core/LogManager.h>
#include <Core/EventManager.h>


using namespace Core;

// constructor
ExperiencePlugin::ExperiencePlugin() : Plugin(GetStaticTypeUuid())
{
	LogDetailedInfo("Constructing experience plugin ...");
}


// destructor
ExperiencePlugin::~ExperiencePlugin()
{
	CORE_EVENTMANAGER.RemoveEventHandler(this);
	LogDetailedInfo("Destructing experience plugin ...");
}

 
// init after the parent dock window has been created
bool ExperiencePlugin::Init()
{
	LogDetailedInfo("Initializing experience plugin ...");

	CORE_EVENTMANAGER.AddEventHandler(this);

	QWidget*		mainWidget		= NULL;
	QHBoxLayout*	mainLayout		= NULL;
	CreateDockMainWidget( &mainWidget, &mainLayout );

	///////////////////////////////////////////////////////////////////////////
	// Toolbar (top-left)
	///////////////////////////////////////////////////////////////////////////

	Core::Array<QWidget*> toolbarWidgets;

	/////////////////////////////////////////////////////////////////////////////
	//// Settings
	/////////////////////////////////////////////////////////////////////////////

	// don't show settings in case there aren't any
	mForceEnableSettings = false;

	///////////////////////////////////////////////////////////////////////////
	// Add render widget at the end
	///////////////////////////////////////////////////////////////////////////

	mExperienceWidget = new ExperienceWidget(mainWidget);
	
	///////////////////////////////////////////////////////////////////////////
	// Fill everything
	///////////////////////////////////////////////////////////////////////////
	FillLayouts(mainWidget, mainLayout, toolbarWidgets, "Settings", "Gear", mExperienceWidget);

	LogDetailedInfo("Experience plugin successfully initialized...");

	return true;
}


// update interface information
void ExperiencePlugin::UpdateInterface()
{
}


// called when the experience changed
void ExperiencePlugin::OnActiveExperienceChanged(Experience* experience)
{
}
