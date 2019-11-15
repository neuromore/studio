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

#ifndef __NEUROMORE_BLUETOOTHSERVICE_H
#define __NEUROMORE_BLUETOOTHSERVICE_H

#include <Config.h>
#include <DeviceDriver.h>
#include <QObject>
#include <QLowEnergyController>
#include <QLowEnergyService>


// forward declaration
class BluetoothDevice;

// bluetooth service class
class BluetoothService : public QObject
{
	Q_OBJECT
	public:
		// constructor & destructor
		BluetoothService(QObject* parent, BluetoothDevice* btDevice, const QBluetoothUuid& serviceUuid, const QBluetoothUuid& characteristicUuid, QLowEnergyController* controller);
		virtual ~BluetoothService();

		bool IsInitialized() const						{ return mService != NULL; }

	signals:
		void CharacteristicChanged(QLowEnergyCharacteristic characteristic, QByteArray data);

	private slots:
		void OnServiceStateChanged(QLowEnergyService::ServiceState state);
		void OnCharacteristicChanged(const QLowEnergyCharacteristic& characteristic, const QByteArray& value);
		void OnConfirmedDescriptorWrite(const QLowEnergyDescriptor& descriptor, const QByteArray& value);

	private:
        BluetoothDevice*            mBtDevice;
		QLowEnergyController*		mController;
		QBluetoothUuid				mServiceUuid;
		QBluetoothUuid				mCharacteristicUuid;
		QLowEnergyService*			mService;
};


#endif
