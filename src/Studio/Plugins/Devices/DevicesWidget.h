/****************************************************************************
**
** Copyright 2019 neuromore co
** Contact: https://neuromore.com/contact
**
** Commercial License Usage
** Licensees holding valid commercial neuromore licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and neuromore. For licensing terms
** and conditions see https://neuromore.com/licensing. For further
** information use the contact form at https://neuromore.com/contact.
**
** neuromore Public License Usage
** Alternatively, this file may be used under the terms of the neuromore
** Public License version 1 as published by neuromore co with exceptions as 
** appearing in the file neuromore-class-exception.md included in the 
** packaging of this file. Please review the following information to 
** ensure the neuromore Public License requirements will be met: 
** https://neuromore.com/npl
**
****************************************************************************/

#ifndef __NEUROMORE_DEVICESWIDGET_H
#define __NEUROMORE_DEVICESWIDGET_H

// include required headers
#include "../../Config.h"
#include "DeviceWidget.h"
#include <ImageButton.h>
#include <QWidget>
#include <QVBoxLayout>
#include <QKeyEvent>


class DevicesWidget : public QWidget
{
	Q_OBJECT
	public:
		DevicesWidget(QWidget* parent=NULL);
		virtual ~DevicesWidget();
	
		void ReInit();
		void UpdateInterface();

		void Clear();
		void AddWidgetForDevice(Device* device);
		void RemoveWidgetByDevice(Device* device);

		uint32 GetNumDevices() const				{ return mDeviceWidgets.Size(); }

	private slots:
		void OnSearchButton();

	private:
		//void keyPressEvent(QKeyEvent* event);

		QVBoxLayout*				mVLayout;
		Core::Array<DeviceWidget*>	mDeviceWidgets;

		ImageButton*				mSearchButton;
		QLabel*						mSearchLabel;
		QMovie*						mWaitingMovie;
		QLabel*						mWaitingMovieLabel;
};

#endif
