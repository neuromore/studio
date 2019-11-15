/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_GRAPHOBJECTVIEWWIDGET_H
#define __NEUROMORE_GRAPHOBJECTVIEWWIDGET_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/EventHandler.h>
#include <Graph/GraphObject.h>
#include <QGridLayout>
#include <QWidget>


class QTBASE_API GraphObjectViewWidget : public QWidget
{
	Q_OBJECT
	public:
		GraphObjectViewWidget(QWidget* parent=NULL);
		virtual ~GraphObjectViewWidget();

		void ReInit(GraphObject* object);
		void Clear();

	protected:

		GraphObject*			mObject;
		Core::Array<QWidget*>	mWidgets;
		QGridLayout*			mGridLayout;
};


#endif
