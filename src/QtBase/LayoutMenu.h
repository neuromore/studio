/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __QTBASE_LAYOUTMENU_H
#define __QTBASE_LAYOUTMENU_H

// include required headers
#include "QtBaseConfig.h"
#include <QMenu>


class QTBASE_API LayoutMenu : public QMenu
{
	Q_OBJECT
	public:
		LayoutMenu(QWidget* parent=NULL);
		virtual ~LayoutMenu();

		void EnableCustomizing(bool enableCustomizing);
		bool IsCustomizingEnabled() const						{ return mCustomizingEnabled; }

		void ReInit();
		void UpdateInterface();

	public slots:
		void OnLayoutClicked();
		void OnRemoveLayout();

	private:
		bool		mCustomizingEnabled;
		QAction*	mSaveAction;
};


#endif
