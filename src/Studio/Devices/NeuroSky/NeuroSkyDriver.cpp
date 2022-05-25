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
#include "NeuroSkyDriver.h"
#include "NeuroSkySerialHandler.h"

#ifdef INCLUDE_DEVICE_NEUROSKY_MINDWAVE

using namespace Core;

// constructor
NeuroSkyDriver::NeuroSkyDriver() : DeviceDriver(false), EventHandler()
{
	LogInfo("Constructing NeuroSky device driver ...");

	AddSupportedDevice(NeuroSkyDevice::TYPE_ID);

	mAutoDetection = NULL;
	mAutoDetectionTimer = NULL;
	mAutoDetectionThread = NULL;


	// IMPORTANT
	// disable this device by default, it clashes with muse
	mIsEnabled = false;

	LogDetailedInfo("NeuroSky device driver constructed");
}


// destructor
NeuroSkyDriver::~NeuroSkyDriver()
{
	LogInfo("Destructing NeuroSky device driver ...");

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
	// NOTE: not sure why this happens from time to time... only with neurosky
	CORE_ASSERT(mSerialHandlerThreads.Size() == 0 && mDevices.Size() == 0);

	//// stop and delete serial handler threads (in case some remain, which means the device manager did not call RemoveDevice())
	//const uint32 numThreads = mSerialHandlerThreads.Size();
	//for (uint32 i = 0; i < numThreads; ++i)
	//{
	//	mSerialHandlerThreads[i]->quit();
	//	mSerialHandlerThreads[i]->wait();
	//	mSerialHandlerThreads[i]->deleteLater();
	//}


	LogDetailedInfo("NeuroSky device driver destructed");
}


// system initialization
bool NeuroSkyDriver::Init()
{
	LogInfo("Initializing NeuroSky device driver");

	mAutoDetection = new NeuroSkyAutoDetection(this);
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


void NeuroSkyDriver::Update(const Time& elapsed, const Time& delta)
{
}


// start stop auto detection
void NeuroSkyDriver::SetAutoDetectionEnabled(bool enable)
{
   DeviceDriver::SetAutoDetectionEnabled(enable);
}

void NeuroSkyDriver::StartAutoDetection()
{
   LogDetailedInfo("Starting NeuroSky auto detection ...");
   mAutoDetectionThread->start();
   mAutoDetection->Start();
}

void NeuroSkyDriver::StopAutoDetection()
{
   LogDetailedInfo("Stopping NeuroSky auto detection ...");
   mAutoDetection->Stop();
}

bool NeuroSkyDriver::IsDetectionRunning() const
{
	if (mAutoDetectionThread->isRunning() == false)
		return false;

	return mAutoDetection->IsSearching();
}


// detect devices once
void NeuroSkyDriver::DetectDevices()
{
	if (mIsEnabled == false)
		return;

	// make sure thread is running
	mAutoDetectionThread->start();
	mAutoDetection->DetectDevicesOnce();
}


Device* NeuroSkyDriver::CreateDevice(uint32 deviceTypeID)
{
	// check if its the correct device
	CORE_ASSERT(IsDeviceSupported(deviceTypeID));

	switch (deviceTypeID)
	{
		case NeuroSkyDevice::TYPE_ID: 	return new NeuroSkyDevice(this);
		default:  /* does not happen*/	return NULL;	
	}
}


void NeuroSkyDriver::AddDevice(NeuroSkyDevice* device, int connectionID, const char* serialPortName)
{
	mDevices.Add(device);

	// create serial thread for the device (uses the serial handler)
	NeuroSkySerialThread* serialThread = new NeuroSkySerialThread(device, connectionID, serialPortName);
	serialThread->start();

	mSerialHandlerThreads.Add(serialThread);
}


void NeuroSkyDriver::OnRemoveDevice(Device* device)
{
	if (device->GetType() != NeuroSkyDevice::TYPE_ID)
		return;

	NeuroSkyDevice* neuroSkyDevice = static_cast<NeuroSkyDevice*>(device); 

	// find index 
	uint32 index = mDevices.Find(neuroSkyDevice);

	// device does not belong to driver (should not happen)
	if (index == CORE_INVALIDINDEX32)
		return;

	// remove device from device list
	mDevices.RemoveByValue(neuroSkyDevice);

	// stop and remove remove serial thread
	NeuroSkySerialThread* serialThread = mSerialHandlerThreads[index];
	serialThread->quit();
	serialThread->wait();
	mSerialHandlerThreads.Remove(index);
	delete serialThread;
}


// detect neurosky devices
void NeuroSkyAutoDetection::DetectDevices()
{
	if (mIsRunning == false && mDetectOnce == false)
		return;

	// tells driver detection is running now
	mIsSearching = true;

	// try to detect new neurosky devices by trying all ports
	Array<String> portNames;

	// find all bluetooth NeuroSky devices
	Array<String> names;
	names.Add("MindWave");
	names.Add("BrainBand");
	Array<BluetoothHelpers::DeviceInfo> devices = BluetoothHelpers::FindDevicesByName(names, false);
	const uint32 numDevices = devices.Size();
	for (uint32 i = 0; i < numDevices; ++i)
	{
		// try to get the first registered serialport that belongs to the device
		String port = BluetoothHelpers::FindSerialPort(devices[i]);
		if (port.IsEmpty() == false)
			portNames.Add(port);
	}

	// try to connect to those ports and create headset if successfull
	const uint32 numPorts = portNames.Size();
	for (uint32 i = 0; i < numPorts; ++i)
	{
		if (mBreak == true)
			break;

		const char* portName = portNames[i].AsChar();

		// skip port if it is already in use
		if (GetEngine()->GetSerialPortManager()->IsLocked(portName) == true)
			continue;

		//
		// Step 1: connect to device
		//

		// Get a connection ID handle to ThinkGear
		int connectionID = TG_GetNewConnectionId();
		if (connectionID < 0)
		{
			LogWarning("NeuroSky: Either too many connections have been created without beeing freed or there is not enough memory to allocate to a new ThinkGear connection.");

			// release port again
			TG_FreeConnection(connectionID);
			GetEngine()->GetSerialPortManager()->ReleaseSerialPort(portName);
			continue;
		}

		mTempString = portName;

#ifdef NEUROMORE_PLATFORM_WINDOWS
		// if port name has two digits, convert it to windows format by prefexing it with "\\.\"
		if (mTempString.GetLength() == 5) // double digit COM port name is always 5 chars long
		{
			mTempString = "\\\\.\\";	// NOTE escape chars: this is actually "\\.\" in bytes
			mTempString += portName;
		}
#endif

		// try to connect
		int errorCode = TG_Connect(connectionID, mTempString.AsChar(), TG_BAUD_9600, TG_STREAM_PACKETS);

		// connection failed
		if (errorCode < 0)
		{
			LogDetailedInfo("NeuroSky: Could not connect (thinkgear error %i), errorCode.", errorCode);

			// release port again
			TG_FreeConnection(connectionID);
			GetEngine()->GetSerialPortManager()->ReleaseSerialPort(portName);
			continue;
		}

		// try to read first packet
		if (TG_ReadPackets(connectionID, 1) == 0)
		{
			LogDetailedInfo("NeuroSky: Device did not respond.");

			// release port again
			TG_FreeConnection(connectionID);
			GetEngine()->GetSerialPortManager()->ReleaseSerialPort(portName);
			continue;
		}

		// enable blink detection
		//TG_EnableBlinkDetection(connectionID, true);

		//
		// Step 2: create the device object
		//

		// create settings, set serial port name

		// create new headset
		NeuroSkyDevice* device = new NeuroSkyDevice(mDriver);
	
		// add device to driver, it will create a serial handler thread for it 
		mDriver->AddDevice(device, connectionID, portName);

		// transfer ownership of headset to device manager
		GetDeviceManager()->AddDeviceAsync(device);
	}

	
	// detect only once? -> stop
	if (mDetectOnce == true)
		mDetectOnce = false;

	// reset flag
	mIsSearching = false;
}


void NeuroSkySerialThread::run()
{
	// try to connect to the headset
	NeuroSkySerialHandler mSerialHandler(mBciDevice, mConnectionID, this);

	// start event loop
	exec();

	// free connection after loop exited
	TG_FreeConnection(mConnectionID);

	// release port again
	GetEngine()->GetSerialPortManager()->ReleaseSerialPort(mSerialPortName);
}


#endif
