/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "ProgressWindowManager.h"
#include "ProgressWindow.h"
#include "../QtBaseManager.h"
#include <Core/LogManager.h>
#include <QMessageBox>
#include <QDateTime>


using namespace Core;

// constructor
ProgressWindowManager::ProgressWindowManager()
{
	LogDetailedInfo("Constructing process window manager ...");

	mProgressWindow			= NULL;
	mIsProgressWindowActive	= false;

	// create the event handler
	mProgressHandler = new ProgressHandler();

	connect( mProgressHandler, SIGNAL(ProgressStart(bool, bool, bool, bool)), this, SLOT(ShowProgressWindow(bool, bool, bool, bool)) );
	connect( mProgressHandler, SIGNAL(ProgressEnd()), this, SLOT(HideProgressWindow()) );
	connect( mProgressHandler, SIGNAL(ProgressText(Core::String)), this, SLOT(OnProgressText(Core::String)) );
	connect( mProgressHandler, SIGNAL(ProgressValue(float)), this, SLOT(OnProgressValue(float)) );
	connect( mProgressHandler, SIGNAL(SubProgressText(Core::String)), this, SLOT(OnSubProgressText(Core::String)) );
	connect( mProgressHandler, SIGNAL(SubProgressValue(float)), this, SLOT(OnSubProgressValue(float)) );
	
	CORE_EVENTMANAGER.AddEventHandler(mProgressHandler);
}


// destructor
ProgressWindowManager::~ProgressWindowManager()
{
	LogDetailedInfo("Destructing process window manager ...");

	CORE_EVENTMANAGER.RemoveEventHandler(mProgressHandler);
	delete mProgressHandler;
}


void ProgressWindowManager::ShowProgressWindow(bool showProgressText, bool showProgressValue, bool showSubProgressText, bool showSubProgressValue)
{
	// create the progress window
	if (mProgressWindow == NULL)
	{
		mProgressWindow = new ProgressWindow( GetQtBaseManager()->GetMainWindow() );
	}
	else
	{
		const char* errorMsg = "Trying to open a progress window while it is already open.";
		//#ifdef CORE_DEBUG
			//QMessageBox::critical(GetQtBaseManager()->GetMainWindow(), "Progress Window", errorMsg );
		//#endif
		LogError( errorMsg );
	}

	mProgressWindow->OnProgressStart(showProgressText, showProgressValue, showSubProgressText, showSubProgressValue);
	mIsProgressWindowActive = true;
}


void ProgressWindowManager::HideProgressWindow()
{
	if (mProgressWindow != NULL)
	{
		mProgressWindow->OnProgressEnd();
		mProgressWindow->deleteLater();
		mProgressWindow = NULL;
	}

	mIsProgressWindowActive = false;
}


void ProgressWindowManager::OnProgressText(String text)
{ 
	if (mProgressWindow == NULL)
		return; 

	mProgressWindow->OnProgressText(text);
}


void ProgressWindowManager::OnProgressValue(float percentage)
{ 
	if (mProgressWindow == NULL) 
		return; 

	mProgressWindow->OnProgressValue(percentage);
}


void ProgressWindowManager::OnSubProgressText(String text)
{ 
	if (mProgressWindow == NULL) 
		return;

	mProgressWindow->OnSubProgressText(text);
}


void ProgressWindowManager::OnSubProgressValue(float percentage)
{
	if (mProgressWindow == NULL) 
		return;

	mProgressWindow->OnSubProgressValue(percentage);
}
