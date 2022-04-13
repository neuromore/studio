/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

 // include precompiled header
#include <QtBase/Precompiled.h>

#include "DockWidget.h"
#include <QHBoxLayout>


DockWidget::DockWidget(const QString& name, QWidget* parent) : QDockWidget(name, parent)
{
	mContentsWidget = NULL;
}


DockWidget::~DockWidget()
{
}


void DockWidget::SetContents(QWidget* contents)
{
	if (mContentsWidget != NULL)
	{
		mContentsWidget->hide();
		mContentsWidget->deleteLater();
	}

	mContentsWidget = new QWidget(this);
	mContentsWidget->hide();
	mContentsWidget->setObjectName("DockWidget");

	QHBoxLayout* layout = new QHBoxLayout(mContentsWidget);
	layout->addWidget( contents );
	layout->setMargin(1);
	mContentsWidget->setLayout( layout );
	setWidget( mContentsWidget );
}