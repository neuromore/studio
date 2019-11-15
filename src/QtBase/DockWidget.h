/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __QTBASE_DOCKWIDGET_H
#define __QTBASE_DOCKWIDGET_H

#include <QDockWidget>


class DockWidget : public QDockWidget
{
	Q_OBJECT
	public:
		DockWidget(const QString& name, QWidget* parent=NULL);
		virtual ~DockWidget();

		void SetContents(QWidget* contents);

	protected:
		QWidget* mContentsWidget;
};


#endif
