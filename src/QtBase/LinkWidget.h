/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_LINKWIDGET_H
#define __NEUROMORE_LINKWIDGET_H

// include required headers
#include "QtBaseConfig.h"
#include <QWidget>
#include <QToolButton>


class QTBASE_API LinkWidget : public QToolButton
{
	Q_OBJECT
	public:
		LinkWidget(const char* text="", QWidget* parent=NULL);
		virtual ~LinkWidget();

	signals:
		void Clicked();

	private slots:
		void OnClicked();
};


#endif
