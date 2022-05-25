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

// include precompiled header
#include <Studio/Precompiled.h>

// include required files
#include "BluetoothService.h"

using namespace Core;

// constructor
BluetoothService::BluetoothService(QObject* parent, BluetoothDevice* btDevice, const QBluetoothUuid& serviceUuid, const QBluetoothUuid& characteristicUuid, QLowEnergyController* controller) : QObject(parent)
{
    mBtDevice           = btDevice;
	mController			= controller;
	mServiceUuid		= serviceUuid;
	mCharacteristicUuid	= characteristicUuid;

	// connect to service
	mService = mController->createServiceObject( serviceUuid, this );
	if (mService == NULL)
	{
		LogError( "Bluetooth LE service '%s' not found.", mServiceUuid.toString().toLatin1().data() );
		return;
	}
    
    LogDetailedInfo( "Connecting to Bluetooth LE service '%s' ...", mService->serviceName().toLatin1().data() );

	connect( mService, SIGNAL(stateChanged(QLowEnergyService::ServiceState)), this, SLOT(OnServiceStateChanged(QLowEnergyService::ServiceState)) );
	connect( mService, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)), this, SLOT(OnCharacteristicChanged(QLowEnergyCharacteristic,QByteArray)) );
	connect( mService, SIGNAL(descriptorWritten(QLowEnergyDescriptor,QByteArray)), this, SLOT(OnConfirmedDescriptorWrite(QLowEnergyDescriptor,QByteArray)) );

	mService->discoverDetails();
}


// destructor
BluetoothService::~BluetoothService()
{
}


// service state change callback
void BluetoothService::OnServiceStateChanged(QLowEnergyService::ServiceState state)
{
	switch (state)
	{
		// service discovered
		case QLowEnergyService::ServiceDiscovered:
		{
            LogDetailedInfo( "Bluetooth LE service '%s' discovered ...", mService->serviceName().toLatin1().data() );
            
			// check for the given characteristic
			const QLowEnergyCharacteristic characteristic = mService->characteristic( mCharacteristicUuid );
			if (characteristic.isValid() == false)
			{
				LogError( "Bluetooth LE characteristic '%s' (UUID=%s) not found.", characteristic.name().toLatin1().data(), mCharacteristicUuid.toString().toLatin1().data() );
				break;
			}
            
			// TODO: HACK: this sucks! Solve on some nice way! Somehow the descriptor receive for the battery level always fails - why???
            if (mCharacteristicUuid == QBluetoothUuid(QBluetoothUuid::BatteryLevel))
            {
                OnCharacteristicChanged(characteristic, characteristic.value());
               /* QByteArray data = characteristic.value();
                if (data.count() > 0)
                {
                    int batteryLevel = *data.data();
                    LogInfo("Battery Level: %i", batteryLevel);
                    mBtDevice->GetHeartRateDevice()->SetBatteryChargeLevel( (float)batteryLevel * 0.01f );
                }*/
            }

			const QLowEnergyDescriptor descriptor = characteristic.descriptor( QBluetoothUuid::ClientCharacteristicConfiguration );
			if (descriptor.isValid() == true)
			{
                LogDetailedInfo( "Bluetooth LE characteristic '%s' (UUID=%s) found. Writing descriptor.", characteristic.name().toLatin1().data(), mCharacteristicUuid.toString().toLatin1().data() );
				mService->writeDescriptor( descriptor, QByteArray::fromHex("0100") );
			}
            else
            {
                LogError( "Bluetooth LE characteristic descriptor '%s' (UUID=%s) not valid.", characteristic.name().toLatin1().data(), mCharacteristicUuid.toString().toLatin1().data() );
            }

			break;
		}

		default:
		{
			break;
		}
	}
}


// descriptor write confirmation
void BluetoothService::OnConfirmedDescriptorWrite(const QLowEnergyDescriptor& descriptor, const QByteArray& value)
{
    if (descriptor.isValid() && /*descriptor == m_notificationDesc &&*/ value == QByteArray("0000"))
	{
        //disabled notifications -> assume disconnect intent
        mController->disconnectFromDevice();
        delete mService;
        mService = NULL;

		LogError( "Bluetooth LE descriptor write for characteristic '%s' failed.", mCharacteristicUuid.toString().toLatin1().data() );
    }
	else
	{
		LogDetailedInfo( "Bluetooth LE descriptor write for characteristic '%s' confirmed.", mCharacteristicUuid.toString().toLatin1().data() );
	}
}


void BluetoothService::OnCharacteristicChanged(const QLowEnergyCharacteristic& characteristic, const QByteArray& value)
{
	// make sure we're dealing with the correct characteristic
	if (characteristic.uuid() != mCharacteristicUuid)
		return;

	emit CharacteristicChanged(characteristic, value);
}
