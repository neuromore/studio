/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "LayoutManager.h"
#include <Core/LogManager.h>
#include <Core/Array.h>
#include <Core/Timer.h>
#include <EngineManager.h>
#include "PluginSystem/PluginManager.h"
#include "Windows/EnterLabelWindow.h"
#include "QtBaseManager.h"
#include "LayoutComboBox.h"
#include "LayoutMenu.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QByteArray>
#include <QDockWidget>
#include <QMessageBox>
#include <QSettings>
#include <QDir>


using namespace Core;

// constructor
LayoutManager::LayoutManager()
{
	LogDetailedInfo("Constructing layout manager ...");

	mActiveLayout			= NULL;
	mComboBox				= NULL;
	mMenu					= NULL;
	mSwitchHappening		= false;
	mCustomizingEnabled		= true;
}


// destructor
LayoutManager::~LayoutManager()
{
	LogDetailedInfo("Destructing layout manager ...");
	Clear();
}


// get the local folder where all custom layouts are stored
String LayoutManager::GetLocalCustomLayoutsFolder() const
{
	QString result = GetQtBaseManager()->GetAppDataFolder().AsChar();
	result += "Layouts";

#ifdef NEUROMORE_PLATFORM_WINDOWS
	result += "\\";
#endif
#ifdef NEUROMORE_PLATFORM_OSX
    result += "/";
#endif
#ifdef NEUROMORE_PLATFORM_LINUX
	result += "/";
#endif

	result = QDir::toNativeSeparators( result );

	// make sure our physiological data folder exists
	QDir appDataFolder(result);
	appDataFolder.mkpath(result);

	return FromQtString(result);
}


String LayoutManager::ConstructLocalCustomLayoutFilename(const char* layoutName)
{
	String result = GetLocalCustomLayoutsFolder();
	result += layoutName;
	result += ".layout";

	return result;
}


// NOTE: mThreadLock must be locked by caller
void LayoutManager::RegisterLayout(Layout* layout)
{
	String layoutRuleName = layout->GetRuleName();

	// backend layout security check
	User* user = GetEngine()->GetUser();
	if (user->ReadAllowed(layoutRuleName.AsChar()) == false)
	{
		// detroy the layout to avoid memory leaks
		delete layout;

		// NOTE:
		// do not log anything here as this could be used for possible crack attempts, silently return instead
		// don't register the layout
		return;
	}

	// note: locked by the caller ReInit()
	// add the layout
	mLayouts.Add(layout);
}


void LayoutManager::Clear()
{
	mThreadLock.Lock();

	// destruct all layouts
	const uint32 numLayouts = mLayouts.Size();
	for (uint32 i=0; i<numLayouts; ++i)
		delete mLayouts[i];

	mLayouts.Clear();

	mThreadLock.Unlock();
}


void LayoutManager::ReInit()
{
	// remember the name of the active layout (before clearing them all)
	String activeLayoutName;
	if (mActiveLayout != NULL)
		activeLayoutName = mActiveLayout->GetName();

	// clear all layouts (has its own thread lock)
	Clear();

	mThreadLock.Lock();

	///////////////////////////////////////////////////////////////////////////////////
	// Qt resource based layouts
	///////////////////////////////////////////////////////////////////////////////////

	User* user = GetEngine()->GetUser();
	if (user != nullptr && user->FindRule("ROLE_ClinicPatient") != nullptr) {
		RegisterLayout(new Layout(Layout::BUILTIN, "Default", ":/Layouts/PatientUI.layout", "LAYOUT_BiofeedbackUser", ""));
	}
	else {

#if defined(NEUROMORE_PLATFORM_WINDOWS) || defined(NEUROMORE_PLATFORM_LINUX)
  #ifdef CUSTOM_DEFAULT_LAYOUT
		RegisterLayout(new Layout(Layout::BUILTIN, "Default", CUSTOM_DEFAULT_LAYOUT, "LAYOUT_Default", ""));
  #else
		RegisterLayout(new Layout(Layout::BUILTIN, "Default", ":/Layouts/Default.layout", "LAYOUT_Default", ""));
  #endif
		RegisterLayout(new Layout(Layout::BUILTIN, "Classifier Designer", ":/Layouts/ClassifierDesigner.layout", "LAYOUT_ClassifierDesigner", ""));
		RegisterLayout(new Layout(Layout::BUILTIN, "State Machine Designer", ":/Layouts/StateMachineDesigner.layout", "LAYOUT_StateMachineDesigner", ""));
		RegisterLayout(new Layout(Layout::BUILTIN, "Experience Designer", ":/Layouts/ExperienceDesigner.layout", "LAYOUT_ExperienceDesigner", ""));
		RegisterLayout(new Layout(Layout::BUILTIN, "Experience Selection", ":/Layouts/ExperienceSelection.layout", "LAYOUT_ExperienceSelection", ""));
  #ifdef NEUROMORE_BRANDING_ANT
		RegisterLayout(new Layout(Layout::BUILTIN, "Experience Trainer", ":/Layouts/ANT_ExperiencePlayer.layout", "LAYOUT_ExperiencePlayer", ""));
		RegisterLayout(new Layout(Layout::BUILTIN, "Experience Signal", ":/Layouts/EEG.layout", "LAYOUT_EEG", ""));
  #else
		RegisterLayout(new Layout(Layout::BUILTIN, "Experience Player", ":/Layouts/ExperiencePlayer.layout", "LAYOUT_ExperiencePlayer", ""));
		RegisterLayout(new Layout(Layout::BUILTIN, "EEG", ":/Layouts/EEG.layout", "LAYOUT_EEG", ""));
  #endif
#endif


#ifdef NEUROMORE_PLATFORM_OSX
  #ifdef CUSTOM_DEFAULT_LAYOUT
		RegisterLayout(new Layout(Layout::BUILTIN, "Default", CUSTOM_DEFAULT_LAYOUT, "LAYOUT_Default", ""));
  #else
		RegisterLayout(new Layout(Layout::BUILTIN, "Default", ":/Layouts/OSX_Default.layout", "LAYOUT_Default", ""));
  #endif
		RegisterLayout(new Layout(Layout::BUILTIN, "Classifier Designer", ":/Layouts/OSX_ClassifierDesigner.layout", "LAYOUT_ClassifierDesigner", ""));
		RegisterLayout(new Layout(Layout::BUILTIN, "State Machine Designer", ":/Layouts/OSX_StateMachineDesigner.layout", "LAYOUT_StateMachineDesigner", ""));
		RegisterLayout(new Layout(Layout::BUILTIN, "Experience Designer", ":/Layouts/OSX_ExperienceDesigner.layout", "LAYOUT_ExperienceDesigner", ""));
		RegisterLayout(new Layout(Layout::BUILTIN, "Experience Selection", ":/Layouts/ExperienceSelection.layout", "LAYOUT_ExperienceSelection", ""));
		RegisterLayout(new Layout(Layout::BUILTIN, "Experience Player", ":/Layouts/OSX_ExperiencePlayer.layout", "LAYOUT_ExperiencePlayer", ""));
		RegisterLayout(new Layout(Layout::BUILTIN, "EEG", ":/Layouts/OSX_EEG.layout", "LAYOUT_EEG", ""));
#endif
	}

	///////////////////////////////////////////////////////////////////////////////////
	// Local layouts
	///////////////////////////////////////////////////////////////////////////////////

	if (mCustomizingEnabled == true)
	{
		// parse the AppData/neuromore/NMStudio/Layouts/ folder
		QString folderPath = GetLocalCustomLayoutsFolder().AsChar();
		QDir dir( folderPath );
		dir.setFilter(QDir::Files | QDir::NoSymLinks);
		dir.setSorting(QDir::Name);

		// iterate through the files
		QFileInfoList list = dir.entryInfoList();
		String filename, name;
		for (int i=0; i<list.size(); ++i)
		{
			// get the filename
			QFileInfo fileInfo = list.at(i);
			FromQtString( fileInfo.absoluteFilePath(), &filename );

			// only add files with the .layout extension
			if (filename.ExtractFileExtension().Lowered() == "layout")
			{
				FromQtString( fileInfo.fileName(), &name );
				name.RemoveFileExtension();

				mLayouts.Add( new Layout(Layout::LOCAL, name.AsChar(), filename.AsChar(), "", "") );
			}
		}
	}

	// reset the active layout
	mActiveLayout = FindLayoutByName( activeLayoutName.AsChar() );

	mThreadLock.Unlock();

	///////////////////////////////////////////////////////////////////////////////////
	// ReInit combobox and menu
	///////////////////////////////////////////////////////////////////////////////////

	if (mComboBox != NULL)	mComboBox->ReInit();
	if (mMenu != NULL)		mMenu->ReInit();
}


// save the current layout into a user selected file
void LayoutManager::OnSaveLayoutAs()
{
	EnterLabelWindow nameWindow( "Please enter a layout name", "", "", GetQtBaseManager()->GetMainWindow() );
	if (nameWindow.exec() == QDialog::Rejected)
		return;

	// get the layout name and construct a filename out of it
	QString layoutName	= nameWindow.GetLabelText().AsChar();
	QString filename	= GetLocalCustomLayoutsFolder() + layoutName + ".layout";

	// if the file already exists, ask to overwrite or not
	if (QFile::exists(filename) == true)
	{
		QMessageBox msgBox(QMessageBox::Warning, "Warning", "The layout with the given name already exists.<br>Do you want to overwrite it?", QMessageBox::Yes | QMessageBox::No, GetQtBaseManager()->GetMainWindow());
		msgBox.setTextFormat( Qt::RichText );
		if (msgBox.exec() != QMessageBox::Yes)
			 return;
	}

	// try to save the layout to a file
	if (Layout::Save(FromQtString(filename).AsChar()) == true)
	{
		// reinit the layout manager
		ReInit();
	}
	else
		LogError( "Failed to save layout to file '%s'", FromQtString(filename).AsChar() );
}


// save the current layout
void LayoutManager::OnSaveLayout()
{
	if (mActiveLayout == NULL)
	{
		LogError("Cannot save active layout. No layout present.");
		return;
	}

	// try to save the layout to a file
	if (Layout::Save(mActiveLayout->GetFilename()) == true)
	{
		// reinit the layout manager
		ReInit();
	}
	else
		LogError( "Failed to save layout to file '%s'", mActiveLayout->GetFilename() );
}


// find the index for the layout with the given name
uint32 LayoutManager::FindLayoutIndexByName(const char* layoutName)
{
	mThreadLock.Lock();

	// get the number of layouts and iterate through them
	const uint32 numLayouts = mLayouts.Size();
	for (uint32 i=0; i<numLayouts; ++i)
	{
		Layout* layout = GetLayout(i);
		if (layout->GetNameString().IsEqual(layoutName) == true)
		{
			mThreadLock.Unlock();
			return i;
		}
	}

	mThreadLock.Unlock();

	// return failure, not found
	return CORE_INVALIDINDEX32;
}


// find the layout with the given name
// NOTE: mThreadLock must be locked by caller
Layout* LayoutManager::FindLayoutByName(const char* layoutName)
{
	// get the number of layouts and iterate through them
	const uint32 numLayouts = mLayouts.Size();
	for (uint32 i=0; i<numLayouts; ++i)
	{
		Layout* layout = GetLayout(i);
		if (layout->GetNameString().IsEqual(layoutName) == true)
			return layout;
	}
	// 
	// return failure, not found
	return NULL;
}


// switch layout to layout with the given name
void LayoutManager::SwitchToLayoutByName(const char* layoutName)
{
	uint32 layoutIndex = FindLayoutIndexByName(layoutName);
	SwitchToLayoutByIndex(layoutIndex);
}


// switch layout to layout at the given index
void LayoutManager::SwitchToLayoutByIndex(uint32 index)
{
	if (index >= mLayouts.Size())
		return;

	if (mSwitchHappening == true)
		return;

	Layout* layout = GetLayout(index);

	mThreadLock.Lock();
	mSwitchHappening = true;
	GetQtBaseManager()->SetPauseInterface(true);

	switch (layout->GetType())
	{
		case Layout::BACKEND:
		{
			// TODO: load from cloud
			break;
		}

		default:
		{
			// try to load it
			if (layout->Load(layout->GetFilename()) == false)
			{
				LogError( "Failed to load layout from file '%s'", layout->GetFilename() );
				mActiveLayout = NULL;
			}
			else
			{
				mActiveLayout = layout;
			}

			break;
		}
	}

	User* user = GetEngine()->GetUser();
	if (user != nullptr && user->FindRule("ROLE_ClinicPatient") == nullptr) {
		mComboBox->SilentChangeCurrentIndex(index);
	}

	mMenu->UpdateInterface();

	GetQtBaseManager()->SetPauseInterface(false);
	mSwitchHappening = false;
	mThreadLock.Unlock();
}


// remove a given layout
void LayoutManager::OnRemoveLayout(int layoutIndex)
{
	Layout* layout = GetLayout(layoutIndex);

	if (mActiveLayout == layout)
		mActiveLayout = NULL;

	String title, message;
	title.Format( "Remove Layout?" );
	message.Format( "Are you sure you want to remove the given layout '%s'?", layout->GetName() );

	// make sure we really want to remove it
	if (QMessageBox::warning(GetQtBaseManager()->GetMainWindow(), title.AsChar(), message.AsChar(), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
		 return;

	// try to remove the file
	QFile file( layout->GetFilename() );
	if (file.remove() == false)
	{
		LogError( "Failed to remove layout file '%s'", layout->GetFilename() );
		return;
	}
	else
		LogInfo( "Successfully removed layout file '%s'", layout->GetFilename() );

	// reinit the layout manager
	ReInit();
}


const QIcon& LayoutManager::GetHddIcon()
{
	return GetQtBaseManager()->FindIcon("Images/Icons/HDD.png");
}


const QIcon& LayoutManager::GetCloudIcon()
{
	return GetQtBaseManager()->FindIcon("Images/Icons/Cloud.png");
}


const QIcon& LayoutManager::GetBoxIcon()
{
	return GetQtBaseManager()->FindIcon("Images/Icons/Box.png");
}


LayoutMenu* LayoutManager::ConstructMenu()
{
	if (mMenu == NULL)
		mMenu = new LayoutMenu();

	mMenu->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Layout.png"));

	mMenu->EnableCustomizing(mCustomizingEnabled);
	
	return mMenu;
}


void LayoutManager::EnableCustomizing(bool enableCustomizing)
{
	mCustomizingEnabled = enableCustomizing;

	if (mMenu != NULL)
		mMenu->EnableCustomizing(mCustomizingEnabled);

	ReInit();
}
