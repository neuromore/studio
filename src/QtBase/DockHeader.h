/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __QTBASE_DOCKHEADER_H
#define __QTBASE_DOCKHEADER_H

// include the required headers
#include <Core/StandardHeaders.h>
#include "QtBaseConfig.h"
#include <QWidget>
#include <QLabel>
#include <QDockWidget>
#include <QPushButton>


class QTBASE_API DockHeaderFillWidget : public QWidget
{
	Q_OBJECT
	public:
		DockHeaderFillWidget(QWidget* parent) : QWidget(parent)				{}
		~DockHeaderFillWidget()												{}

		virtual void paintEvent(QPaintEvent* event) override;
};


class DockHeader : public QWidget
{
	Q_OBJECT
	public:
		DockHeader(QDockWidget* dockWidget);
		~DockHeader();

		void SetTitle(const char* title)						{ mTitleLabel->setText(title); }

	signals:
		void RemovePlugin();

	public slots:
		void OnUpdate();

	protected slots:
		void OnDockButton();
		void OnCloseButton();
		void OnMaximizeButton();
		void OnTopLevelChanged(bool isFloating);
	
	private:
		QDockWidget*	mDockWidget;
		QPushButton*	mMinimizeButton;
		QPushButton*	mMaximizeButton;
		QPushButton*	mCloseButton;
		QPushButton*	mDockButton;
		QPushButton*	mUndockButton;
		QLabel*			mTitleLabel;
};


#endif
