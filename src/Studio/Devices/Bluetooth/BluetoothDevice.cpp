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
#include "BluetoothDevice.h"

using namespace Core;

// constructor
BluetoothDevice::BluetoothDevice(QObject* parent, const QBluetoothDeviceInfo& deviceInfo) : QObject(parent)
{
	mParent                     = parent;
	mDeviceInfo                 = deviceInfo;
	mController                 = NULL;
	mHeartRateService           = NULL;
	mBatteryService             = NULL;
    mBatteryLevel               = 0.5;
    mIsConnecting               = false;
    mIsConnected                = false;
    
    mHeartRateDevice            = NULL;
}


// destructor
BluetoothDevice::~BluetoothDevice()
{
    // do nothing here, heart rate device will get removed with the disconnect event
}


// connect to the bluetooth device
void BluetoothDevice::Connect()
{
    if (IsConnected() == true)
    {
        LogWarning( "BluetoothDevice::Connect(): Trying to connect to Bluetooth LE device '%s' while it is already connected. Skipping...", mDeviceInfo.name().toLatin1().data() );
        return;
    }
    
    mIsConnecting = true;
    
	// get rid of the old Bluetooth LE controller
	if (mController != NULL)
	{
		mController->disconnectFromDevice();
		delete mController;
		mController = NULL;
	}
    
    LogInfo( "Connecting to Bluetooth LE device '%s' (Address: %s) ...", mDeviceInfo.name().toLatin1().data(), GetDeviceInfoAddress(mDeviceInfo).toLatin1().data() );

	// create the Bluetooth LE controller
	mController = new QLowEnergyController( mDeviceInfo, this );
	connect( mController, SIGNAL(serviceDiscovered(QBluetoothUuid)), this, SLOT(OnServiceDiscovered(QBluetoothUuid)));
	connect( mController, SIGNAL(discoveryFinished()), this, SLOT(OnServiceScanDone()));
	connect( mController, SIGNAL(error(QLowEnergyController::Error)), this, SLOT(OnControllerError(QLowEnergyController::Error)));
	connect( mController, SIGNAL(connected()),	this, SLOT(OnDeviceConnected()));
	connect( mController, SIGNAL(disconnected()), this, SLOT(OnDeviceDisconnected()));

	// connect to the Bluetooth LE device
	mController->connectToDevice();
}


// device got connected
void BluetoothDevice::OnDeviceConnected()
{
    LogInfo( "Connected to Bluetooth LE Device '%s'", mDeviceInfo.name().toLatin1().data() );
    LogInfo( "Discovering services for LE Device '%s' ...", mDeviceInfo.name().toLatin1().data() );
    mController->discoverServices();
}


// device got disconnected
void BluetoothDevice::OnDeviceDisconnected()
{
    LogInfo( "Failed to connect to Bluetooth LE Device '%s'", mDeviceInfo.name().toLatin1().data() );
    
    if (mHeartRateDevice != NULL)
    {
        GetDeviceManager()->RemoveDeviceAsync(mHeartRateDevice);
        mHeartRateDevice = NULL;
    }
    
    mIsConnected = false;
    mIsConnecting = false;
    
    emit Finished();
}


void BluetoothDevice::OnControllerError(QLowEnergyController::Error error)
{
    LogInfo( "Cannot connect to Bluetooth LE Device '%s'.", mDeviceInfo.name().toLatin1().data() );

    if (mHeartRateDevice != NULL)
    {
        GetDeviceManager()->RemoveDeviceAsync(mHeartRateDevice);
        mHeartRateDevice = NULL;
    }
    
    mIsConnected = false;
    mIsConnecting = false;
    
    emit Finished();
}


// called when a Bluetooth service got discovered for the given device
void BluetoothDevice::OnServiceDiscovered(const QBluetoothUuid& gatt)
{
	mDiscoveredServiceUuids.Add( gatt );
	LogDebug( "Bluetooth LE service '%s' discovered.", gatt.toString().toLatin1().data() );
}


// called after all Bluetooth services for the device got discovered
void BluetoothDevice::OnServiceScanDone()
{
	// heart rate service
	if (mDiscoveredServiceUuids.Find(QBluetoothUuid(QBluetoothUuid::HeartRate)) != CORE_INVALIDINDEX32)
	{
		mHeartRateService = new BluetoothService( this, this, QBluetoothUuid(QBluetoothUuid::HeartRate), QBluetoothUuid(QBluetoothUuid::HeartRateMeasurement), mController );
		connect( mHeartRateService, SIGNAL(CharacteristicChanged(QLowEnergyCharacteristic,QByteArray)), this, SLOT(OnUpdateHeartRate(QLowEnergyCharacteristic,QByteArray)) );
	}

	// battery service
	if (mDiscoveredServiceUuids.Find(QBluetoothUuid(QBluetoothUuid::BatteryService)) != CORE_INVALIDINDEX32)
	{
		mBatteryService = new BluetoothService( this, this, QBluetoothUuid(QBluetoothUuid::BatteryService), QBluetoothUuid(QBluetoothUuid::BatteryLevel), mController );
		connect( mBatteryService, SIGNAL(CharacteristicChanged(QLowEnergyCharacteristic,QByteArray)), this, SLOT(OnUpdateBatteryLevel(QLowEnergyCharacteristic,QByteArray)) );
	}

    mIsConnected = true;
    mIsConnecting = false;

    emit Finished();
}


void BluetoothDevice::OnUpdateBatteryLevel(const QLowEnergyCharacteristic& characteristic, const QByteArray& value)
{
    const char *data = value.constData();
    int offset = 0;
    
    quint8* batteryLevel = (quint8*)&data[offset];
    LogInfo("Battery Level: %i", *batteryLevel);
    mBatteryLevel = (float)(*batteryLevel) * 0.01f;
}


void BluetoothDevice::OnUpdateHeartRate(const QLowEnergyCharacteristic& characteristic, const QByteArray& value)
{
    // check if we have already created the device
    if (mHeartRateDevice == NULL)
    {
        mHeartRateDevice = new HeartRateDevice();
        GetDeviceManager()->AddDeviceAsync(mHeartRateDevice);
    }
    
	const char *data = value.constData();
	quint8 flags = data[0];
    int offset = sizeof(uint8);

	// heart rate
	if (flags & 0x01)
	{
		// HR 16 bit? otherwise 8 bit
		quint16* heartRate = (quint16*)&data[offset];
		//LogInfo("16-bit Heart Rate: %i", *heartRate);
        mHeartRateDevice->GetHeartRateSensor()->AddQueuedSample(*heartRate);

        offset += sizeof(uint16);
	}
    else
	{
		quint8* heartRate = (quint8*)&data[offset];
		//LogInfo("8-bit Heart Rate: %i", *heartRate);
        mHeartRateDevice->GetHeartRateSensor()->AddQueuedSample(*heartRate);
        
		offset += sizeof(uint8);
	}
    
    // energy expended
    if (flags & 0x08)
    {
		//quint16* energy = (quint16*)&data[offset];
        //LogInfo("Energy expended: %i", *energy);
        
		offset += sizeof(uint16);
    }
    
    // RR interval
    if (flags & 0x10)
    {
		quint16* rrInterval = (quint16*)&data[offset];
        //LogInfo("RR Interval: %i", *rrInterval);

        mHeartRateDevice->GetRRIntervalSensor()->AddQueuedSample(*rrInterval);

        offset += sizeof(uint16);
    }
    
    // update the battery level
	if (mBatteryService == NULL)
		mHeartRateDevice->SetPowerSupplyType(Device::POWERSUPPLY_UNKNOWN);
	else 
		mHeartRateDevice->SetPowerSupplyType(Device::POWERSUPPLY_BATTERY);

    mHeartRateDevice->SetBatteryChargeLevel( mBatteryLevel );
}


QString BluetoothDevice::GetDeviceInfoAddress(const QBluetoothDeviceInfo& deviceInfo)
{
#ifdef Q_OS_MAC
    // workaround for Core Bluetooth:
    return deviceInfo.deviceUuid().toString();
#else
    return deviceInfo.address().toString();
#endif
}
