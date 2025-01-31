/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

 // include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "PluginManager.h"
#include <Core/LogManager.h>
#include <EngineManager.h>
#include <QMainWindow>
#include <QDir>
#include <QTime>
#include <QVariant>
#include <QApplication>


using namespace Core;

// constructor
PluginManager::PluginManager()
{
	LogDetailedInfo("Constructing plugin manager ...");

	mMenu = NULL;
	mCustomizingEnabled = true;
	
	mRegisteredPlugins.Reserve( 64 );
	mActivePlugins.Reserve( 64 );
}


// destructor
PluginManager::~PluginManager()
{
	LogDetailedInfo( "Destructing plugin manager ..." );
	Clear();
}


// register the plugin
void PluginManager::RegisterPlugin(Plugin* plugin)
{
	// backend plugin security check
	User* user = GetEngine()->GetUser();
	if (user->ReadAllowed(plugin->GetRuleName()) == false)
	{
		// NOTE:
		// do not log anything here as this could be used for possible crack attempts, silently kill the plugin prototype and return instead
		// delete the prototype and don't register it
		delete plugin;
		return;
	}

	LogDetailedInfo( "Registering plugin ..." );
	mRegisteredPlugins.Add( plugin );
	LogDebug( "Plugin '%s' registered ...", plugin->GetName() );
}


// create a new active plugin from a given type
Plugin* PluginManager::CreatePlugin(const char* pluginTypeUuid, const char* objectName, bool updateMenu, Core::Json* json, Core::Json::Item* pluginItem)
{
	// try to locate the registered plugin index based on the given uuid
	const uint32 pluginIndex = FindRegisteredPluginIndexByTypeUuid( pluginTypeUuid );
	if (pluginIndex == CORE_INVALIDINDEX32)
	{
		LogCritical("PluginManager::CreatePlugin(): Cannot find plugin with UUID '%s' in registered plugins.", pluginTypeUuid);
		return NULL;
	}

	// create a plugin clone from the registered plugin
	LogDetailedInfo( "Creating plugin ..." );
	Plugin* newPlugin = mRegisteredPlugins[pluginIndex]->Clone();

	// initialize the new plugin instance
	LogInfo( "Initializing plugin '%s' ...", newPlugin->GetName() );

	LogDetailedInfo("Registering plugin attributes ...");
	newPlugin->RegisterAttributes();

	// let the plugin read the data
	if (json != NULL && pluginItem != NULL)
	{
		if (newPlugin->LoadSettings(*json, *pluginItem) == false)
			LogCritical("PluginManager::CreatePlugin(): Error loading plugin settings for plugin '%s'.", newPlugin->GetName());
	}

	LogDetailedInfo("Creating base interface ...");
	newPlugin->CreateBaseInterface( objectName );

	newPlugin->Init();
	connect( newPlugin, SIGNAL(RemovePlugin()), this, SLOT(OnRemovePluginSignal()) );

	if (json != NULL && pluginItem != NULL)
	{
		if (newPlugin->LoadUiConfigs(*json, *pluginItem) == false)
			LogCritical("PluginManager::CreatePlugin(): Error loading plugin UI configs for plugin '%s'.", newPlugin->GetName());
	}

	// if layout customization is disabled, lock plugin so it can't be moved
	newPlugin->SetLocked(mCustomizingEnabled == false);

	// register as active plugin and return it
	mActivePlugins.Add( newPlugin );
	LogDetailedInfo( "Plugin '%s' successfully initialized ...", newPlugin->GetName() );

	// show the plugin
	newPlugin->SetVisible(true);

	// reinitialize the plugin menu
	if (updateMenu == true)
		mMenu->ReInit();

	return newPlugin;
}


// find the first active plugin of a given type
Plugin* PluginManager::FindFirstActivePluginByType(const char* typeUuid)
{
	const uint32 numActivePlugins = mActivePlugins.Size();
	for (uint32 i=0; i<numActivePlugins; ++i)
	{
		if (mActivePlugins[i]->GetTypeUuidString().IsEqual(typeUuid) == true)
			return mActivePlugins[i];
	}

	return NULL;
}


// find plugin by identification number
Plugin* PluginManager::FindActivePluginById(uint32 id)
{
	// get the number of active plugins and iterate through them
	const uint32 numActivePlugins = mActivePlugins.Size();
	for (uint32 i=0; i<numActivePlugins; ++i)
	{
		if (mActivePlugins[i]->GetId() == id)
			return mActivePlugins[i];
	}

	return NULL;
}


// find plugin index by identification number
uint32 PluginManager::FindActivePluginIndexById(uint32 id)
{
	// get the number of active plugins and iterate through them
	const uint32 numActivePlugins = mActivePlugins.Size();
	for (uint32 i=0; i<numActivePlugins; ++i)
	{
		if (mActivePlugins[i]->GetId() == id)
			return i;
	}

	return CORE_INVALIDINDEX32;
}


// find plugin index
uint32 PluginManager::FindActivePluginIndex(Plugin* plugin)
{
	// get the number of active plugins and iterate through them
	const uint32 numActivePlugins = mActivePlugins.Size();
	for (uint32 i=0; i<numActivePlugins; ++i)
	{
		if (mActivePlugins[i] == plugin)
			return i;
	}

	return CORE_INVALIDINDEX32;
}


void PluginManager::OnRemovePluginSignal()
{
	Plugin* plugin = qobject_cast<Plugin*>( sender() );

	// try to find the active plugin index
	const uint32 pluginIndex = FindActivePluginIndex( plugin );
	if (pluginIndex == CORE_INVALIDINDEX32)
	{
		LogError( "PluginManager::OnRemovePluginSignal(): Plugin 0x%x does not exist. Cannot remove plugin.", plugin );
		return;
	}

	// remove the plugin
	RemoveActivePlugin( pluginIndex );
}


// remove the plugin with the given index
bool PluginManager::RemoveActivePlugin(uint32 index, bool updateMenu)
{
	if (mActivePlugins.IsValidIndex(index) == false)
	{
		LogError("PluginManager::RemoveActivePlugin(): Index %i not valid. Cannot remove plugin. ", index);
		return false;
	}

	LogDetailedInfo("Removing active plugin ...");

	Plugin* plugin = mActivePlugins[index];

	// remove it from the active plugins
	mActivePlugins.Remove(index);

	// safe delete
	plugin->deleteLater();

	// reinitialize the plugin menu
	if (updateMenu == true)
		mMenu->ReInit();

	return true;
}


// delete all active plugins
void PluginManager::ClearActivePlugins(bool updateMenu)
{
	LogDetailedInfo("Clearing active plugins ...");

	// delete all active plugins
	const uint32 numActivePlugins = mActivePlugins.Size();
	for (uint32 i=0; i<numActivePlugins; ++i)
	{
		Plugin* plugin = mActivePlugins[i];
		LogDetailedInfo( "Unloading plugin '%s' ...", plugin->GetName() );

		// safe delete
		//plugin->deleteLater();
		delete plugin;
	}
	mActivePlugins.Clear();

	// reinitialize the plugin menu
	if (updateMenu == true)
		mMenu->ReInit();
}


// unload the plugin libraries
void PluginManager::Clear()
{
	// unload active plugins
	ClearActivePlugins(false);

	// delete all registered plugins
	const uint32 numRegisteredPlugins = mRegisteredPlugins.Size();
	for (uint32 i=0; i<numRegisteredPlugins; ++i)
	{
		Plugin* plugin = mRegisteredPlugins[i];
		LogDetailedInfo( "Unregistering plugin '%s' ...", plugin->GetName() );

		// safe delete
		//plugin->deleteLater();
		delete plugin;
	}
	mRegisteredPlugins.Clear();
}


// find a given plugin by its name (type string)
uint32 PluginManager::FindRegisteredPluginIndexByTypeUuid(const char* pluginTypeUuid) const
{
	const uint32 numPlugins = mRegisteredPlugins.Size();
	for (uint32 i=0; i<numPlugins; ++i)
		if (mRegisteredPlugins[i]->GetTypeUuidString().IsEqual(pluginTypeUuid) == true)
			return i;

	return CORE_INVALIDINDEX32;
}


void PluginManager::LockActivePlugins (bool locked)
{
	const uint32 numPlugins = mActivePlugins.Size();
	for (uint32 i=0; i<numPlugins; ++i)
		mActivePlugins[i]->SetLocked(locked);
}


// call post-authentication init for all plugins
void PluginManager::OnPostAuthenticationInit()
{
	LogDetailedInfo("Plugin manager post-authentication initialization ...");

	const uint32 numPlugins = mActivePlugins.Size();
	for (uint32 i=0; i<numPlugins; ++i)
		mActivePlugins[i]->OnPostAuthenticationInit();
}