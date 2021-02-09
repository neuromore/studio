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
#include "ExperienceSelectionPlugin.h"
#include <Core/LogManager.h>
#include <EngineManager.h>
#include "../../MainWindow.h"
#include "../../AppManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QIcon>
#include <QSplitter>
#include <QFileDialog>


using namespace Core;

// constructor
ExperienceSelectionPlugin::ExperienceSelectionPlugin() : Plugin(GetStaticTypeUuid())
{
	LogDetailedInfo("Constructing experience selection plugin ...");
	mWidget = NULL;
}


// destructor
ExperienceSelectionPlugin::~ExperienceSelectionPlugin()
{
	LogDetailedInfo("Destructing experience selection plugin ...");
	CORE_EVENTMANAGER.RemoveEventHandler(this);
}


// init after the parent dock window has been created
bool ExperienceSelectionPlugin::Init()
{
	LogDetailedInfo("Initializing experience selection plugin ...");
	
	mWidget = new ExperienceSelectionWidget( mDock, this );
	mWidget->AsyncLoadFromBackend("");
	
	mDock->SetContents(mWidget);

	// attach to event system
	LogDebug("Attaching Experience selection pluginplugin to event system ...");
	CORE_EVENTMANAGER.AddEventHandler(this);

	LogDetailedInfo("Experience selection plugin successfully initialized");
	return true;
}


// called after switching layout
void ExperienceSelectionPlugin::OnAfterLoadLayout()
{
	mWidget->AsyncLoadFromBackend("");
}
