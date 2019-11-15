/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "PluginMenu.h"
#include "PluginManager.h"
#include "../QtBaseManager.h"


using namespace Core;

// constructor
PluginMenu::PluginMenu(QWidget* parent) : QMenu("&Windows", GetQtBaseManager()->GetMainWindow())
{
	// add the create window menu
	mCreateMenu = addMenu( GetQtBaseManager()->FindIcon("Images/Icons/Plus.png"), "Add" );
	
	// add the remove window menu
	mRemoveMenu = addMenu( GetQtBaseManager()->FindIcon("Images/Icons/Minus.png"), "Remove" );

	ReInit();
}


// destructor
PluginMenu::~PluginMenu()
{
}


// update the layouts menu
void PluginMenu::ReInit()
{
	LogDebug("Updating dynamic window menu ...");

	// clearing old menu entries
	mCreateMenu->clear();
	mRemoveMenu->clear();

	// get the plugin manager
	PluginManager* pluginManager = GetPluginManager();

	//////////////////////////////////////////////////////////////////////////////////////////
	// Create
	//////////////////////////////////////////////////////////////////////////////////////////

	// for all registered plugins, create a menu item
	const uint32 numRegisteredPlugins = pluginManager->GetNumRegisteredPlugins();
	for (uint32 i=0; i<numRegisteredPlugins; ++i)
	{
		Plugin*		plugin = pluginManager->GetRegisteredPlugin(i);
		QAction*	action = mCreateMenu->addAction( plugin->GetName() );

		// only single instance allowed and already active?
		if (plugin->AllowMultipleInstances() == false && pluginManager->FindFirstActivePluginByType(plugin->GetTypeUuid()) != NULL)
		{
			action->setVisible(false);
		}
		else
		{
			action->setVisible(true);
			action->setData( plugin->GetTypeUuid() );
			connect( action, SIGNAL(triggered()), this, SLOT(OnCreatePlugin()) );
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Visibility
	//////////////////////////////////////////////////////////////////////////////////////////

	//// add the visibility window menu
	//mVisibilityMenu = addMenu( GetQtBaseManager()->FindIcon("Images/Icons/Eye.png"), "Visibility" );

	//// for all active plugins, create a menu item
	//const uint32 numActivePlugins = pluginManager->GetNumActivePlugins();
	//for (uint32 i=0; i<numActivePlugins; ++i)
	//{
	//	Plugin*		plugin = pluginManager->GetActivePlugin(i);
	//	QAction*	action = mVisibilityMenu->addAction( plugin->GetName() );

	//	action->setCheckable(true);
	//	action->setChecked( plugin->IsVisible() );
	//	action->setData( plugin->GetId() );
	//	connect( action, SIGNAL(triggered()), this, SLOT(OnChangePluginVisibility()) );
	//}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Remove
	//////////////////////////////////////////////////////////////////////////////////////////

	
	// for all active plugins, create a menu item
	const uint32 numActivePlugins = pluginManager->GetNumActivePlugins();
	for (uint32 i=0; i<numActivePlugins; ++i)
	{
		Plugin*		plugin = pluginManager->GetActivePlugin(i);
		QAction*	action = mRemoveMenu->addAction( plugin->GetName() );
		
		action->setData( plugin->GetId() );
		connect( action, SIGNAL(triggered()), this, SLOT(OnRemovePlugin()) );
	}

	// clear all action
	mRemoveMenu->addSeparator();
	QAction* clearAction = mRemoveMenu->addAction( "Remove All" );
	clearAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Clear.png") );
	connect( clearAction, SIGNAL(triggered()), this, SLOT(OnClearPlugins()) );
}


// create a new given plugin
void PluginMenu::OnCreatePlugin()
{
	QAction* action = qobject_cast<QAction*>( sender() );
	String pluginTypeUuid = FromQtString( action->data().toString() );

	// create the new window
	Plugin* plugin = GetPluginManager()->CreatePlugin( pluginTypeUuid.AsChar() );
	
	// set as floating fixed size window in center of main window
	plugin->GetDockWidget()->setFloating(true);

	int width = 400;
	int height = 300;

	plugin->GetDockWidget()->resize(width, height);
	plugin->GetDockWidget()->move( GetQtBaseManager()->GetMainWindow()->rect().center() - QPoint(width/2, height/2) );
}


// remove the given plugin
void PluginMenu::OnRemovePlugin()
{
	QAction*	 action	  = qobject_cast<QAction*>( sender() );
	const uint32 pluginId = action->data().toInt();

	// try to find the active plugin index by id
	const uint32 pluginIndex = GetPluginManager()->FindActivePluginIndexById( pluginId );
	if (pluginIndex == CORE_INVALIDINDEX32)
	{
		LogError( "PluginMenu::OnRemovePlugin(): Plugin with id %i does not exist. Cannot remove plugin.", pluginId );
		return;
	}

	// remove the plugin
	GetPluginManager()->RemoveActivePlugin( pluginIndex );
}


// switch visibility of the given plugin
void PluginMenu::OnChangePluginVisibility()
{
	QAction*	 action	  = qobject_cast<QAction*>( sender() );
	const uint32 pluginId = action->data().toInt();

	// try to find the active plugin by id
	Plugin* plugin = GetPluginManager()->FindActivePluginById( pluginId );
	if (plugin == NULL)
	{
		LogError( "PluginMenu::OnRemovePlugin(): Plugin with id %i does not exist. Cannot change visibility of plugin.", pluginId );
		return;
	}

	// swap visibility
	plugin->SetVisible( !plugin->IsVisible() );

	// manually call reinit as we directly called SetVisible() on the plugin
	ReInit();
}


// remove all plugins
void PluginMenu::OnClearPlugins()
{
	// try to create the new window
	GetPluginManager()->ClearActivePlugins();
}
