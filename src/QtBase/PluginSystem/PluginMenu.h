/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __QTBASE_PLUGINMENU_H
#define __QTBASE_PLUGINMENU_H

// include required headers
#include "../QtBaseConfig.h"
#include <QMenu>


class QTBASE_API PluginMenu : public QMenu
{
	Q_OBJECT
	public:
		PluginMenu(QWidget* parent=NULL);
		virtual ~PluginMenu();

		void ReInit();

		QMenu* GetCreateMenu()				{ return mCreateMenu; }
		QMenu* GetRemoveMenu()				{ return mRemoveMenu; }

	private slots:
		void OnCreatePlugin();
		void OnRemovePlugin();
		void OnClearPlugins();
		void OnChangePluginVisibility();

	private:
		QMenu* mCreateMenu;
		QMenu* mRemoveMenu;
		//QMenu* mVisibilityMenu;
};


#endif
