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

#ifndef __NEUROMORE_BCIDEVICEWIDGET_H
#define __NEUROMORE_BCIDEVICEWIDGET_H

// include required headers
#include "DeviceWidget.h"
#include "../../Config.h"
#include "../../Widgets/SignalQualityWidget.h"
#include "../../Widgets/EEGElectrodesWidget.h"
#include "../../Widgets/ImpedanceTestWidget.h"
#include <BciDevice.h>
#include <QWidget>
#include <QLabel>
#include <QTreeWidget>


class BciDeviceWidget : public DeviceWidget
{
	Q_OBJECT
	public:
		BciDeviceWidget(BciDevice* device, QWidget* parent = NULL);
		virtual ~BciDeviceWidget();

		void UpdateInterface() override;
		void AddSensorItems(QTreeWidgetItem* parent) override;
		void UpdateSensorItems(QTreeWidgetItem* parent) override;

		void Init() override;

		QWidget* CreateDeviceTestWidget() override			{ return new ImpedanceTestWidget(mBciDevice, this); }
		
	private:
		BciDevice*				mBciDevice;					// reference to BciDevice
		EEGElectrodesWidget*	mEEGElectrodeWidget;		// eeg sensor status
		SignalQualityWidget*	mSignalQualityWidget;
		QWidget*				mImpedanceGridWidget;
		QGridLayout*			mImpedanceGrid;
};

#endif
