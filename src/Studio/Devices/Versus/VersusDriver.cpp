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
#include <Devices/Versus/VersusDevice.h>
#include "VersusDriver.h"
#include "VersusSerialHandler.h"

using namespace Core;

// constructor
VersusDriver::VersusDriver() : DeviceDriver(false), EventHandler()
{
	LogInfo("Constructing Versus device driver ...");

	AddSupportedDevice(VersusDevice::TYPE_ID);

	mAutoDetection = NULL;
	mAutoDetectionTimer = NULL;
	mAutoDetectionThread = NULL;

	LogDetailedInfo("Versus device driver constructed");
}


// destructor
VersusDriver::~VersusDriver()
{
	LogInfo("Destructing Versus device driver ...");

	// remove event handler
	CORE_EVENTMANAGER.RemoveEventHandler(this);

	// stop thread
	if (mAutoDetectionThread != NULL)
		if (mAutoDetectionThread->isRunning())
		{
			mAutoDetection->Break();
			mAutoDetectionThread->quit();
			mAutoDetectionThread->wait();
		}
	delete mAutoDetection;
	delete mAutoDetectionTimer;
	delete mAutoDetectionThread;

	// NOTE: devices and serial threads are usually removed by the device manager. We check this, maybe some something went wrong (e.g. we fucked up the order):
	CORE_ASSERT(mSerialHandlerThreads.Size() == 0 && mDevices.Size() == 0);

	//// stop and delete serial handler threads (in case some remain, which means the device manager did not call RemoveDevice())
	//const uint32 numThreads = mSerialHandlerThreads.Size();
	//for (uint32 i = 0; i < numThreads; ++i)
	//{
	//	mSerialHandlerThreads[i]->quit();
	//	mSerialHandlerThreads[i]->wait();
	//	mSerialHandlerThreads[i]->deleteLater();
	//}


	LogDetailedInfo("Versus device driver destructed");
}


// system initialization
bool VersusDriver::Init()
{
	LogInfo("Initializing Versus device driver");

	mAutoDetection = new VersusAutoDetection(this);
	mAutoDetectionTimer = new QTimer(this);
	mAutoDetectionThread = new QThread(this);

	QObject::connect(mAutoDetectionTimer, SIGNAL(timeout()), mAutoDetection, SLOT(DetectDevices()));

	// lower frequency for this device to optimize detection (we wait most of the time on the serial port)
	mAutoDetectionTimer->setInterval(1000);
	mAutoDetectionTimer->start();
	mAutoDetectionTimer->moveToThread(mAutoDetectionThread);
	mAutoDetection->moveToThread(mAutoDetectionThread);

	// register event handler for reacting to device removal
	CORE_EVENTMANAGER.AddEventHandler(this);

	return true;
}


void VersusDriver::Update(const Time& elapsed, const Time& delta)
{
}


// start stop auto detection
void VersusDriver::SetAutoDetectionEnabled(bool enable)
{
	DeviceDriver::SetAutoDetectionEnabled(enable);

	if (enable == true && mIsEnabled)
	{
		LogDetailedInfo("Starting Versus auto detection ...");
		mAutoDetectionThread->start();
		mAutoDetection->Start();
	}
	else
	{
		LogDetailedInfo("Stopping Versus auto detection ...");
		mAutoDetection->Stop();
	}
}


bool VersusDriver::IsDetectionRunning() const
{
	if (mAutoDetectionThread->isRunning() == false)
		return false;

	return mAutoDetection->IsSearching();
}


// detect devices once
void VersusDriver::DetectDevices()
{
	if (mIsEnabled == false)
		return;

	// make sure thread is running
	mAutoDetectionThread->start();
	mAutoDetection->DetectDevicesOnce();
}


Device* VersusDriver::CreateDevice(uint32 deviceTypeID)
{
	// check if its the correct device
	CORE_ASSERT(IsDeviceSupported(deviceTypeID));

	switch (deviceTypeID)
	{
		case VersusDevice::TYPE_ID: 	return new VersusDevice(this);
		default:  /* does not happen*/	return NULL;	
	}
}


void VersusDriver::AddDevice(VersusDevice* device, const char* serialPort)
{
	mDevices.Add(device);

	// create serial thread for the device (uses the serial handler)
	VersusSerialThread* serialThread = new VersusSerialThread(device, serialPort);
	serialThread->start();

	mSerialHandlerThreads.Add(serialThread);
}


void VersusDriver::OnRemoveDevice(Device* device)
{
	if (device->GetType() != VersusDevice::TYPE_ID)
		return;

	VersusDevice* openBCIDevice = static_cast<VersusDevice*>(device); 

	// find index 
	uint32 index = mDevices.Find(openBCIDevice);

	// device does not belong to driver (should not happen)
	if (index == CORE_INVALIDINDEX32)
		return;

	// remove device from device list
	mDevices.RemoveByValue(openBCIDevice);

	// stop and remove remove serial thread
	VersusSerialThread* serialThread = mSerialHandlerThreads[index];
	serialThread->quit();
	serialThread->wait();
	mSerialHandlerThreads.Remove(index);
	delete serialThread;
}


// run autodetection once
void VersusAutoDetection::DetectDevices()
{ 
	if (mIsRunning == false && mDetectOnce == false)
		return;

	// tells driver detection is running now
	mIsSearching = true;

	// list of ports that belong to a versus device
	Array<String> portNames;

	// collect all device configs and their serial ports (if any)
	Array<const Device::DeviceConfig*> deviceConfigs = GetDeviceManager()->FindDeviceConfigsByType(VersusDevice::TYPE_ID);
	const uint32 numConfigs = deviceConfigs.Size();
	for (uint32 i=0; i<numConfigs; ++i)
		portNames.Add( DeviceHelpers::FindSerialPorts(*deviceConfigs[i]) );

	// find all USB connected versus devices
	portNames.Add(SerialPort::FindPortsByDescription("Freescale CDC Device"));

	// find all bluetooth versus devices
	Array<BluetoothHelpers::DeviceInfo> devices = BluetoothHelpers::FindDevicesByName("VERSUS", false);
	const uint32 numDevices = devices.Size();
	for (uint32 i = 0; i<numDevices; ++i)
	{
		// try to get the first registered serialport that belongs to the device
		String port = BluetoothHelpers::FindSerialPort(devices[i]);
		if (port.IsEmpty() == false)
			portNames.Add(port);
	}

#ifndef NEUROMORE_PLATFORM_WINDOWS
	// use all ports till Qt Bluetooth is implemented on OSX
	portNames = SerialPort::FindAllPorts();
#endif

	const uint32 numPorts = portNames.Size();
	for (uint32 i = 0; i<numPorts; ++i)
	{
		// exit early if thread is to be stopped
		if (mBreak == true)
			break;

		LogDetailedInfo("VersusAutoDetection: trying port %s ...", portNames[i].AsChar());

		// check if the com port is accessible before creating the device
		SerialPort serialPort(portNames[i].AsChar());
		if (serialPort.Init() == false)
			return;

		// try to configure and open the port
		bool success = serialPort.SetBaudRate(115200) &&
			serialPort.SetParity(SerialPort::NO_PARITY) &&
			serialPort.SetNumDataBits(8) &&
			serialPort.SetNumStopBits(1);

		success = success && serialPort.Open();

		if (success == false)
			LogDetailedInfo("VersusAutoDetection: ... failed (not configurable / openable)", portNames[i].AsChar());

		for (uint32 s = 0; s<100 && mBreak == false; ++s)
			Thread::Sleep(10);

		// try to send data to device
		if (serialPort.IsWriteable())
		{
			serialPort.Write("\r", 1);
			serialPort.Flush();

			success = success && serialPort.WaitForWrite(500);
			if (success == false)
				LogDetailedInfo("VersusAutoDetection: ... failed (not writeable) ", portNames[i].AsChar());

		}

		// free serial port before creating serialhandler
		serialPort.Reset();

		// serial port cannot be used, the device behind the virtual serial port is probably not connected
		if (success == false)
		{
			continue;
		}

		LogDetailedInfo("VersusAutoDetection: ... success", portNames[i].AsChar());

		for (uint32 s = 0; s<100 && mBreak == false; ++s)
			Thread::Sleep(10);

		if (mBreak == true)
			break;

		//
		// Create Device
		//

		// create new versus headset
		VersusDevice* device = static_cast<VersusDevice*>(mDriver->CreateDevice(VersusDevice::TYPE_ID));
		
		// add device to driver, it will create a serial handler thread for it 
		mDriver->AddDevice(device, portNames[i].AsChar());

		// add device to device manager
		GetDeviceManager()->AddDeviceAsync(device);
	}


	// detect only once? -> stop
	if (mDetectOnce == true)
		mDetectOnce = false;

	// reset flag
	mIsSearching = false;
}


void VersusSerialThread::run()
{
	// creat serial port
	SerialPort serialPort(mPortName, this);

	// try to connect to the versus
	VersusSerialHandler serialHandler(&serialPort, mDevice, this);

	const bool success = serialHandler.Connect();

	if (success == false)
		LogError("%s device failed to initialize", mDevice->GetName().AsChar());
	else
	{
		LogDetailedInfo("%s device successfully initialized", mDevice->GetName().AsChar());

		// start event loop
		exec();
	}
}
