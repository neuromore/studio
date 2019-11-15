/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_PLUGINMANAGER_H
#define __NEUROMORE_PLUGINMANAGER_H

// include required headers
#include <Core/Array.h>
#include <Core/String.h>
#include "../QtBaseConfig.h"
#include "Plugin.h"
#include "PluginMenu.h"
#include <QObject>


class QTBASE_API PluginManager : public QObject
{
	friend class Plugin;
	Q_OBJECT
	public:
		// constructor & destructor
		PluginManager();
		~PluginManager();

		// registering (happens once per available plugin type during application startup)
		void RegisterPlugin(Plugin* plugin);

		// create new active plugin
		Plugin* CreatePlugin(const char* pluginTypeUuid, const char* objectName=NULL, bool updateMenu=true, Core::Json* json=NULL, Core::Json::Item* pluginItem=NULL);

		// menu item
		PluginMenu* ConstructMenu()									{ if (mMenu == NULL) mMenu = new PluginMenu(); return mMenu; }
		void ReInitMenu()											{ if (mMenu != NULL) mMenu->ReInit(); }

		inline uint32 GetNumActivePlugins() const					{ return mActivePlugins.Size(); }
		inline Plugin* GetActivePlugin(const uint32 index)			{ return mActivePlugins[index]; }

		// search helpers
		Plugin* FindFirstActivePluginByType(const char* typeUuid);
		Plugin* FindActivePluginById(uint32 id);
		uint32 FindActivePluginIndexById(uint32 id);
		uint32 FindActivePluginIndex(Plugin* plugin);
		
		// active plugin removal helpers
		bool RemoveActivePlugin(uint32 index, bool updateMenu=true);
		void ClearActivePlugins(bool updateMenu=true);

		// toggle docking mode of active plugin windows
		void LockActivePlugins (bool locked = true);

		// calls post authentication init for all active plugins
		void OnPostAuthenticationInit();

		inline uint32 GetNumRegisteredPlugins() const						{ return mRegisteredPlugins.Size(); }
		inline Plugin* GetRegisteredPlugin(const uint32 index)				{ return mRegisteredPlugins[index]; }

		void EnableCustomizing(bool enable)									{ mCustomizingEnabled = enable; }
		bool IsCustomizingEnabled() const									{ return mCustomizingEnabled; }

	private slots:
		void OnRemovePluginSignal();

	private:
		void Clear();
		uint32 FindRegisteredPluginIndexByTypeUuid(const char* typeUuid) const;

		Core::Array<Plugin*>				mRegisteredPlugins;
		Core::Array<Plugin*>				mActivePlugins;
		PluginMenu*							mMenu;

		bool								mCustomizingEnabled;
};


#endif
