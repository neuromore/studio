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

#ifndef __NEUROMORE_BLUETOOTHDEVICE_H
#define __NEUROMORE_BLUETOOTHDEVICE_H

#include <Config.h>
#include <Core/Array.h>
#include <Devices/Generic/GenericDevices.h>
#include <QObject>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include "BluetoothService.h"


// the bluetooth device class
class BluetoothDevice : public QObject
{
	Q_OBJECT
	public:
		// constructor & destructor
		BluetoothDevice(QObject* parent, const QBluetoothDeviceInfo& deviceInfo);
		virtual ~BluetoothDevice();

		void Connect();
    
        const QBluetoothDeviceInfo& GetDeviceInfo() const                   { return mDeviceInfo; }
        HeartRateDevice* GetHeartRateDevice() const                         { return mHeartRateDevice; }
    
        static QString GetDeviceInfoAddress(const QBluetoothDeviceInfo& deviceInfo);

        bool IsConnecting() const                                           { return mIsConnecting; }
        bool IsConnected() const                                            { return mIsConnected; }
    
    signals:
        void Finished(BluetoothDevice* device);

	private slots:
		void OnServiceDiscovered(const QBluetoothUuid& gatt);
		void OnServiceScanDone();

		void OnControllerError(QLowEnergyController::Error error);

		void OnDeviceConnected();
		void OnDeviceDisconnected();

		void OnUpdateHeartRate(const QLowEnergyCharacteristic& characteristic, const QByteArray& value);
        void OnUpdateBatteryLevel(const QLowEnergyCharacteristic& characteristic, const QByteArray& value);

	private:
		QObject*					mParent;
		QBluetoothDeviceInfo		mDeviceInfo;
		QLowEnergyController*		mController;
		Core::Array<QBluetoothUuid>	mDiscoveredServiceUuids;
        bool                        mIsConnecting;
        bool                        mIsConnected;
    
        // Bluetooth services
		BluetoothService*			mBatteryService;
        float                       mBatteryLevel;
		BluetoothService*			mHeartRateService;
    
        // heart rate device
        HeartRateDevice*            mHeartRateDevice;
};


#endif
