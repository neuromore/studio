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

#ifndef __NEUROMORE_DEVICEWIDGET_H
#define __NEUROMORE_DEVICEWIDGET_H

// include required headers
#include "../../Config.h"
#include "../../Widgets/BatteryStatusWidget.h"
#include "../../Widgets/SignalQualityWidget.h"
#include <Device.h>
#include <QWidget>
#include <QLabel>
#include <QTreeWidget>
#include <QBoxLayout>
#include <QPushButton>


class DeviceWidget : public QWidget
{
	Q_OBJECT
	public:
		DeviceWidget(Device* device, QWidget* parent = NULL);
		virtual ~DeviceWidget();

		inline void SetDevice(Device* device)		{ mDevice = device;  }
		Device* GetDevice() const					{ return mDevice;  }

		// init widgets
		virtual void Init();
		
		// update widget
		virtual void UpdateInterface();

		// init device info tree widget
		void InitDeviceInfoWidget();
		void InitDeviceTestWidget();

		// overwrite this in superclass, if device has test functionality
		virtual QWidget* CreateDeviceTestWidget()	{ CORE_ASSERT(false); return NULL; }
	
	private slots:
		void OnDeviceInfoButtonPressed();
		void OnDeviceTestButtonPressed();
		
	private:

		// create / update  tree items
		virtual void AddDeviceItems();
		virtual void UpdateDeviceItems();
		virtual void AddSensorItems(QTreeWidgetItem* parent);
		virtual void UpdateSensorItems(QTreeWidgetItem* parent);

		// UI elements
		QGridLayout*			mLayout;						// main layout
		QHBoxLayout*			mPrimaryDeviceInfoLayout;		// the right half of the device status panel
		QHBoxLayout*			mSecondaryDeviceInfoLayout;		// the area on the left under the device icon
		QLabel*					mDeviceIcon;					// photographic icon (large size)
		QPushButton*			mDeviceInfoButton;				// show/hide button for device information tree
		QPushButton*			mDeviceTestButton;				// show/hide button for device information test
		QTreeWidget*			mDeviceInfoTree;				// device information in tree form
		QTreeWidgetItem*		mDeviceInfoSensors;				// sensor item of device info tree
		
		// UI State
		bool					mShowInfoWidget;

		// general device widgets
		BatteryStatusWidget*	mBatteryStatusWidget;			// battery status info widget
		
	protected:
		Device*					mDevice;						// device for which the information should be displayed
		
		// shared ui elements
		QWidget*				mDeviceTestWidget;

		inline QHBoxLayout* GetPrimaryDeviceInfoLayout()				{ return mPrimaryDeviceInfoLayout; }
		inline QHBoxLayout* GetSecondaryDeviceInfoLayout()				{ return mSecondaryDeviceInfoLayout; }
		void AddDeviceInfo (const char* name, QWidget* value);

		// helpers
		Core::String			mTempString;
};


#endif
