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
#include "BrainquiryDriver.h"
#include "BrainquirySerialHandler.h"
#include <EngineManager.h>
#include <System/SerialPort.h>
#include <System/BluetoothHelpers.h>
#include <QApplication>

#ifdef INCLUDE_DEVICE_BRAINQUIRY

// link to the PET libraries
#pragma comment(lib, "BQPetDLL.lib")

using namespace Core;

// constructor
BrainquiryDriver::BrainquiryDriver() : DeviceDriver(), EventHandler()
{
	LogInfo("Constructing Brainquiry device driver ...");


	AddSupportedDevice(BrainquiryDevice::TYPE_ID);

	mAutoDetection = NULL;
	mAutoDetectionTimer = NULL;
	mAutoDetectionThread = NULL;

	LogDetailedInfo("Brainquiry device driver constructed");
}


// destructor
BrainquiryDriver::~BrainquiryDriver()
{
	LogInfo("Destructing Brainquiry device driver ...");

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
	// NOTE: not sure why this happens from time to time...
	//CORE_ASSERT(mSerialHandlerThreads.Size() == 0 && mDevices.Size() == 0);

	//// stop and delete serial handler threads (in case some remain, which means the device manager did not call RemoveDevice())
	//const uint32 numThreads = mSerialHandlerThreads.Size();
	//for (uint32 i = 0; i < numThreads; ++i)
	//{
	//	mSerialHandlerThreads[i]->quit();
	//	mSerialHandlerThreads[i]->wait();
	//	mSerialHandlerThreads[i]->deleteLater();
	//}


	LogDetailedInfo("Brainquiry device driver destructed");
}


// system initialization
bool BrainquiryDriver::Init()
{
	LogInfo("Initializing Brainquiry device driver");

	mAutoDetection = new BrainquiryAutoDetection(this);
	mAutoDetectionTimer = new QTimer(this);
	mAutoDetectionThread = new QThread(this);

	QObject::connect(mAutoDetectionTimer, SIGNAL(timeout()), mAutoDetection, SLOT(DetectDevices()));
	mAutoDetectionTimer->setInterval(5000);
	mAutoDetectionTimer->start();
	mAutoDetectionTimer->moveToThread(mAutoDetectionThread);
	mAutoDetection->moveToThread(mAutoDetectionThread);

	// register event handler for reacting to device removal
	CORE_EVENTMANAGER.AddEventHandler(this);

	return true;
}


void BrainquiryDriver::Update(const Time& elapsed, const Time& delta)
{
}


// start stop auto detection
void BrainquiryDriver::SetAutoDetectionEnabled(bool enable)
{
	DeviceDriver::SetAutoDetectionEnabled(enable);

	if (enable == true)
	{
		LogDetailedInfo("Starting Brainquiry auto detection ...");
		mAutoDetectionThread->start();
		mAutoDetection->Start();
	}
	else
	{
		LogDetailedInfo("Stopping Brainquiry auto detection ...");
		mAutoDetection->Stop();
	}
}


bool BrainquiryDriver::IsDetectionRunning() const
{
	if (mAutoDetectionThread->isRunning() == false)
		return false;

	return mAutoDetection->IsSearching();
}


// detect devices once
void BrainquiryDriver::DetectDevices()
{
	if (mIsEnabled == false)
		return;

	// make sure thread is running
	mAutoDetectionThread->start();
	mAutoDetection->DetectDevicesOnce();
}


Device* BrainquiryDriver::CreateDevice(uint32 deviceTypeID)
{
	// check if its the correct device
	CORE_ASSERT(IsDeviceSupported(deviceTypeID));

	switch (deviceTypeID)
	{
		case BrainquiryDevice::TYPE_ID: 	return new BrainquiryDevice(this);
		default:  /* does not happen*/	return NULL;	
	}
}


void BrainquiryDriver::AddDevice(BrainquiryDevice* device, int handle, Array<int> channelNumbers, const char* serialPortName)
{
	mDevices.Add(device);

	// create serial thread for the device (uses the serial handler)
	BrainquirySerialThread* serialThread = new BrainquirySerialThread(device, handle, channelNumbers, serialPortName);
	serialThread->start();

	mSerialHandlerThreads.Add(serialThread);
}


void BrainquiryDriver::OnRemoveDevice(Device* device)
{
	if (device->GetType() != BrainquiryDevice::TYPE_ID)
		return;

	BrainquiryDevice* brainquiryDevice = static_cast<BrainquiryDevice*>(device); 

	// find index 
	uint32 index = mDevices.Find(brainquiryDevice);

	// device does not belong to driver (should not happen)
	if (index == CORE_INVALIDINDEX32)
		return;

	// remove device from device list
	mDevices.RemoveByValue(brainquiryDevice);

	// stop and remove remove serial thread
	BrainquirySerialThread* serialThread = mSerialHandlerThreads[index];
	serialThread->quit();
	serialThread->wait();
	mSerialHandlerThreads.Remove(index);
	delete serialThread;
}


// detect neurosky devices
void BrainquiryAutoDetection::DetectDevices()
{
	if (mIsRunning == false && mDetectOnce == false)
		return;

	// tells driver detection is running now
	mIsSearching = true;

	// try to detect new neurosky devices by trying all ports
	Array<String> portNames;


	// find all bluetooth Brainquiry devices
	Array<BluetoothHelpers::DeviceInfo> devices = BluetoothHelpers::FindDevicesByName("BlueRadios", false);
	const uint32 numDevices = devices.Size();
	for (uint32 i = 0; i < numDevices; ++i)
	{
		// try to get the first registered serialport that belongs to the device
		Array<String> devicePorts = BluetoothHelpers::FindSerialPorts(devices[i]);
		if (devicePorts.IsEmpty() == false)
			portNames.Add(devicePorts);
	}

	// try to connect to those ports and create headset if successfull
	const uint32 numPorts = portNames.Size();
	for (uint32 i = 0; i < numPorts; ++i)
	{
		if (mBreak == true)
			break;

		const char* portName = portNames[i].AsChar();

		// configure port
		SerialPort port(portName); 
		port.Init();
		port.SetNumDataBits(8);
		port.SetNumStopBits(1);
		port.SetParity(SerialPort::NO_PARITY);
		port.SetBaudRate(115200);
		port.Reset();

		// skip port if it is already in use
		if (GetEngine()->GetSerialPortManager()->IsLocked(portName) == true)
			continue;

		// aquire serial port
		GetEngine()->GetSerialPortManager()->AcquireSerialPort(portName);


		// get port number (stupid..)
		const uint32 portNumber = SerialPort::ExtractPortNumber(portName);
	
		//
		// Step 1: connect to device
		//


		// try to connect
		const int baudRate = 250000;
		int handle = BQConnect(portNumber, baudRate);
		LogDebug("handle = %i", handle); 
		if (handle == 0)
		{
			LogWarning("Brainquiry Device: Could not connect to port %s", portName);
			continue;
		}

		// TODO check if BQEEGCreate creates incremental numbers or not
	
		// try to configure 2 EEG channels
		Array<int> channelNumbers;
		channelNumbers.Add( BQEEGCreate(handle, 2, 1, 64, 256) );
		channelNumbers.Add( BQEEGCreate(handle, 3, 1, 64, 256) );
		channelNumbers.Add( BQEEGCreate(handle, 4, 1, 64, 256) );
		channelNumbers.Add( BQEEGCreate(handle, 5, 1, 64, 256) ); 
		if (channelNumbers.Contains(0) == true)
		{
			LogWarning("Brainquiry Device: Could not configure EEG channels (err=%i)", BQGetError(handle));
			BQClose(handle);
			continue;
		}

		// upload channel config
		if (BQUpdate(handle) == 0)
		{
			LogWarning("Brainquiry Device: Could not upload config (err=%i)", BQGetError(handle));
			BQClose(handle);
			continue;
		}

		// start sampling
		// TODO always do this _after_ adding the device and add device uninitialized (can't test this)
		if (BQStart(handle) == 0)
		{
			LogWarning("Brainquiry Device: Could not start sampling (err=%i)", BQGetError(handle));
			BQClose(handle);
			continue;
		}

		//
		// Step 2: create the device object
		//

		// create settings, set serial port name

		// create new headset
		BrainquiryDevice* device = new BrainquiryDevice(mDriver);
	
		// add device to driver, it will create a serial handler thread for it 
		mDriver->AddDevice(device, handle, channelNumbers, portName);

		// transfer ownership of headset to device manager
		GetDeviceManager()->AddDeviceAsync(device);
	}
	
	// detect only once? -> stop
	if (mDetectOnce == true)
		mDetectOnce = false;

	// reset flag
	mIsSearching = false;
}


void BrainquirySerialThread::run()
{
	// try to connect to the headset
	BrainquirySerialHandler mSerialHandler(mDevice, mHandle, mChannelNumbers, this);

	// start event loop
	exec();

	// stop sampling and close connection after loop exited
	if (BQStop(mHandle) == 0)
		LogWarning("Brainquiry Device: Could not stop sampling (err=%i)", BQGetError(mHandle));

	if (BQClose(mHandle) == 0)
		LogWarning("Brainquiry Device: Could not close connection (err=%i)", BQGetError(mHandle));


	// release port again
	GetEngine()->GetSerialPortManager()->ReleaseSerialPort(mSerialPortName);
}


#endif
