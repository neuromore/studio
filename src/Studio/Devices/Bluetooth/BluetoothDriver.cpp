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

// include required files
#include "BluetoothDriver.h"
#include <Core/EventManager.h>
#include <QtBaseManager.h>


using namespace Core;

// constructor
BluetoothDriver::BluetoothDriver() : DeviceDriver(false), EventHandler()
{
	LogInfo("Constructing Bluetooth device driver ...");

	mBluetoothDeviceDiscoveryAgent = NULL;
	mDetectOnce = false;
	mIsSearching = false;

	LogDetailedInfo("Bluetooth device driver constructed");
}


// destructor
BluetoothDriver::~BluetoothDriver()
{
	LogInfo("Destructing Bluetooth device driver ...");

	// remove event handler
	CORE_EVENTMANAGER.RemoveEventHandler(this);

	LogDetailedInfo("Bluetooth device driver destructed");
}


// system initialization
bool BluetoothDriver::Init()
{
	LogInfo("Initializing Bluetooth device driver");

	// register event handler for reacting to device removal
	CORE_EVENTMANAGER.AddEventHandler(this);

	// create the bluetooth device discovery agent
	mBluetoothDeviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

	connect( mBluetoothDeviceDiscoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)), this, SLOT(OnDeviceDiscovered(const QBluetoothDeviceInfo&)) );
	connect( mBluetoothDeviceDiscoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)), this, SLOT(OnDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error)) );
	connect( mBluetoothDeviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(OnDeviceScanFinished()) );

	// autodetect timer
	mAutodetectTimer = new QTimer(this);
	connect(mAutodetectTimer, SIGNAL(timeout()), this, SLOT(OnDetectDevices()));

	return true;
}


// called when the bluetooth device discovery agent found a new device
void BluetoothDriver::OnDeviceDiscovered(const QBluetoothDeviceInfo& deviceInfo)
{
	// are we dealing with a Bluetooth LE device?
	if (deviceInfo.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
	{
		LogInfo(" - Bluetooth LE device discovered:");
		LogInfo("    + Name: %s", deviceInfo.name().toLatin1().data());
        LogInfo("    + Adress: %s", BluetoothDevice::GetDeviceInfoAddress(deviceInfo).toLatin1().data());

		mDiscoveredDeviceInfos.Add( deviceInfo );
        
        // don't connec to the device here directly, else we might miss one or the other LE device and connect to a wrong one
	}
}


// called when the bluetooth device discovery agent finished searching for devices
void BluetoothDriver::OnDeviceScanFinished()
{
	LogInfo("Scan for Bluetooth LE devices finished.");
    
    const uint32 numDeviceInfos = mDiscoveredDeviceInfos.Size();
    
/*    if (numDeviceInfos == 1)
        Connect(mDiscoveredDeviceInfos[0]);
    else if (numDeviceInfos > 1)*/
    if (numDeviceInfos > 0)
    {
        BluetoothDeviceSelectorDialog selectionDialog( mDiscoveredDeviceInfos, GetQtBaseManager()->GetMainWindow() );
        selectionDialog.exec();
        
        const uint32 connectToDeviceIndex = selectionDialog.GetIndex();
        if (connectToDeviceIndex < numDeviceInfos)
            Connect(mDiscoveredDeviceInfos[connectToDeviceIndex]);
    }
    
	mIsSearching = false;
	mDetectOnce = false;
}


// called in case an error in the bluetooth device discovery agent happened
void BluetoothDriver::OnDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
        LogError("The Bluetooth adaptor is powered off, power it on before doing discovery.");
    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
        LogError("Writing or reading from the device resulted in an error.");
    else
        LogError("An unknown error has occurred.");
    
    mIsSearching = false;
}


void BluetoothDriver::Connect(const QBluetoothDeviceInfo& deviceInfo)
{
    BluetoothDevice* device = FindDevice(deviceInfo);
    
    // stop the scan before connecting to a new device
    mBluetoothDeviceDiscoveryAgent->stop();
    
    // connect to this device if there is no active connection yet
    if (device == NULL)
    {
        // create a new Bluetooth device and add it to the device array
        device = new BluetoothDevice(this, deviceInfo);
        //connect( device, SIGNAL(Finished()), this, SLOT(OnConnectNextDevice()) );
        mDevices.Add(device);
    }
    
    if (device->IsConnected() == false && device->IsConnecting() == false)
        device->Connect();
}


// start connecting to the next device
/*void BluetoothDriver::OnConnectNextDevice()
{
    // get the number of discovered Bluetooth devices and iterate through them
    const uint32 numDevices = mDiscoveredDeviceInfos.Size();
    for (uint32 i=0; i<numDevices; ++i)
    {
        QBluetoothDeviceInfo deviceInfo = mDiscoveredDeviceInfos[i];
        
        BluetoothDevice* device = FindDevice(deviceInfo);
        
        // connect to this device if there is no active connection yet
        if (device == NULL)
        {
            // create a new Bluetooth device and add it to the device array
            device = new BluetoothDevice(this, deviceInfo);
            connect( device, SIGNAL(Finished()), this, SLOT(OnConnectNextDevice()) );
            mDevices.Add(device);
        }
        
        if (device->IsConnected() == false && device->IsConnecting() == false)
        {
            device->Connect();

            // as soon as we found the next device we haven't connected to, return to avoid scanning multiple devices
            return;
        }
    }
}*/


// find the Bluetooth device info based on the address
QBluetoothDeviceInfo* BluetoothDriver::FindDeviceInfo(QString address)
{
	// get the number of discovered Bluetooth devices and iterate through them
	const uint32 numDevices = mDiscoveredDeviceInfos.Size();
	for (uint32 i=0; i<numDevices; ++i)
	{
		// compare the device address and return on success
		if (BluetoothDevice::GetDeviceInfoAddress(mDiscoveredDeviceInfos[i]) == address)
			return &mDiscoveredDeviceInfos[i];
	}

	// no device with the given address found, return NULL
	return NULL;
}


// find device based on device info
BluetoothDevice* BluetoothDriver::FindDevice(const QBluetoothDeviceInfo& deviceInfo)
{
    // get the number of Bluetooth devices and iterate through them
    const uint32 numDevices = mDevices.Size();
    for (uint32 i=0; i<numDevices; ++i)
    {
        // compare the device info and return on success
        if (mDevices[i]->GetDeviceInfo() == deviceInfo)
            return mDevices[i];
    }
    
    // no device with the given address found, return NULL
    return NULL;
}


// find bluetooth device index by engine device object
uint32 BluetoothDriver::FindBluetoothDeviceIndex(Device* device)
{
    // get the number of Bluetooth devices and iterate through them
    const uint32 numDevices = mDevices.Size();
    for (uint32 i=0; i<numDevices; ++i)
    {
        // compare the device and return on success
        if (mDevices[i]->GetHeartRateDevice() == device)
            return i;
    }
    
    // no device with the given address found, return invalid index
    return CORE_INVALIDINDEX32;
}


// find bluetooth device index by device info
uint32 BluetoothDriver::FindBluetoothDeviceIndex(const QBluetoothDeviceInfo& deviceInfo)
{
    // get the number of Bluetooth devices and iterate through them
    const uint32 numDevices = mDevices.Size();
    for (uint32 i=0; i<numDevices; ++i)
    {
        // compare the device info and return on success
        if (mDevices[i]->GetDeviceInfo() == deviceInfo)
            return i;
    }
    
    // no device with the given address found, return invalid index
    return CORE_INVALIDINDEX32;
}


// check if any of the devices is currently connecting
bool BluetoothDriver::IsDeviceConnecting() const
{
    const uint32 numDevices = mDevices.Size();
    for (uint32 i=0; i<numDevices; ++i)
    {
        if (mDevices[i]->IsConnecting() == true)
            return true;
    }
    
    return false;
}


// start stop auto detection thread
void BluetoothDriver::SetAutoDetectionEnabled(bool enable)
{
	// baseclass (sets flags)
	DeviceDriver::SetAutoDetectionEnabled(enable);
	
	mDetectOnce = false;

	// if true and device has autodetection support: start thread
	if (enable == true && mIsEnabled)
	{
		LogDetailedInfo("Starting Bluetooth LE device auto detection ...");
		mAutodetectTimer->start(5000);
	}
	else
	{
		LogDetailedInfo("Stopping Bluetooth LE device auto detection ...");
		mAutodetectTimer->stop();

        // clear discovered devices and stop searching for new ones
        mDiscoveredDeviceInfos.Clear();
        mBluetoothDeviceDiscoveryAgent->stop();
	}
}


void BluetoothDriver::DetectDevices()
{
    if (mIsEnabled == false)
		return;

	mDetectOnce = true;
	OnDetectDevices();
}


// search for devices once
void BluetoothDriver::OnDetectDevices()
{
    // skip directly in case we're already searching or if any bluetooth device is currently already connecting
    if (mIsSearching == true || IsDeviceConnecting() == true)
        return;
    
	LogInfo("Scanning for Bluetooth LE devices ...");

	// clear discovered devices and start searching for new ones
	mDiscoveredDeviceInfos.Clear();
	mBluetoothDeviceDiscoveryAgent->start();

	mIsSearching = true;
}


// create device
Device* BluetoothDriver::CreateDevice(uint32 deviceTypeID)
{
	// check if its the correct device
	CORE_ASSERT(IsDeviceSupported(deviceTypeID));

	switch (deviceTypeID)
	{
		case HeartRateDevice::TYPE_ID: 	return new HeartRateDevice(this);
		default:  /* does not happen*/	return NULL;	
	}
}


// async remove device
void BluetoothDriver::OnRemoveDevice(Device* device)
{
    uint32 btDeviceIndex = FindBluetoothDeviceIndex(device);
    if (btDeviceIndex == CORE_INVALIDINDEX32)
        return;
    
    BluetoothDevice* btDevice = mDevices[btDeviceIndex];
    
    btDevice->deleteLater();
    mDevices.Remove(btDeviceIndex);
}
