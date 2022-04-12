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
#include "BackendFileSystemPlugin.h"
#include "../../MainWindow.h"

using namespace Core;

// constructor
BackendFileSystemPlugin::BackendFileSystemPlugin() : Plugin(GetStaticTypeUuid())
{
	LogDetailedInfo("Constructing backend file system plugin ...");
	mFileSystemWidget = NULL;
}


// destructor
BackendFileSystemPlugin::~BackendFileSystemPlugin()
{
	LogDetailedInfo("Destructing backend file system plugin ...");

	CORE_EVENTMANAGER.RemoveEventHandler(this);
}


// init after the parent dock window has been created
bool BackendFileSystemPlugin::Init()
{
	LogDetailedInfo("Initializing backend file system plugin ...");
	
	mFileSystemWidget = new BackendFileSystemWidget( mDock, this );
	mFileSystemWidget->Refresh();
	
	mDock->SetContents(mFileSystemWidget);

	// attach to event system
	LogDebug("Attaching backend file system plugin to event system ...");
	CORE_EVENTMANAGER.AddEventHandler(this);

	LogDetailedInfo("Backend file system plugin successfully initialized");
	return true;
}
