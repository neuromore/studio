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
#include "OpenBCIDriver.h"
#include "OpenBCISerialHandler.h"
#include "../DeviceHelpers.h"
#include <Devices/OpenBCI/OpenBCIDevices.h>
#include <EngineManager.h>
#include <System/SerialPort.h>
#include <QApplication>

#ifdef INCLUDE_DEVICE_OPENBCI

using namespace Core;

// constructor
OpenBCIDriver::OpenBCIDriver() : DeviceDriver(false), EventHandler()
{
	LogInfo("Constructing OpenBCI device driver ...");

	AddSupportedDevice(OpenBCIDevice::TYPE_ID);
	AddSupportedDevice(OpenBCIDaisyDevice::TYPE_ID);

	mAutoDetection = NULL;
	mAutoDetectionTimer = NULL;
	mAutoDetectionThread = NULL;

	LogDetailedInfo("OpenBCI device driver constructed");
}


// destructor
OpenBCIDriver::~OpenBCIDriver()
{
	LogInfo("Destructing OpenBCI device driver ...");

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


	LogDetailedInfo("OpenBCI device driver destructed");
}


// system initialization
bool OpenBCIDriver::Init()
{
	LogInfo("Initializing OpenBCI device driver");

	mAutoDetection = new OpenBCIAutoDetection(this);
	mAutoDetectionTimer = new QTimer(NULL);
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


void OpenBCIDriver::Update(const Time& elapsed, const Time& delta)
{
}


// start stop auto detection
void OpenBCIDriver::SetAutoDetectionEnabled(bool enable)
{
	DeviceDriver::SetAutoDetectionEnabled(enable);

	if (enable == true)
	{
		LogDetailedInfo("Starting OpenBCI auto detection ...");
		mAutoDetectionThread->start();
		mAutoDetection->Start();
	}
	else
	{
		LogDetailedInfo("Stopping OpenBCI auto detection ...");
		mAutoDetection->Stop();
	}
}


bool OpenBCIDriver::IsDetectionRunning() const
{
	if (mAutoDetectionThread->isRunning() == false)
		return false;

	return mAutoDetection->IsSearching();
}


// detect devices once
void OpenBCIDriver::DetectDevices()
{
	if (mIsEnabled == false)
		return;
	
	// make sure thread is running
	mAutoDetectionThread->start();
	mAutoDetection->DetectDevicesOnce();
}


Device* OpenBCIDriver::CreateDevice(uint32 deviceTypeID)
{
	// check if its the correct device
	CORE_ASSERT(IsDeviceSupported(deviceTypeID));

	switch (deviceTypeID)
	{
		case OpenBCIDevice::TYPE_ID: 		return new OpenBCIDevice(this);
		case OpenBCIDaisyDevice::TYPE_ID: 	return new OpenBCIDaisyDevice(this);
		default:  /* does not happen*/	return NULL;	
	}
}


void OpenBCIDriver::AddDevice(OpenBCIDeviceBase* device, const char* serialPort)
{
	mDevices.Add(device);

	// create serial thread for the device (uses the serial handler)
	OpenBCISerialThread* serialThread = new OpenBCISerialThread(device, serialPort);
	serialThread->start();

	mSerialHandlerThreads.Add(serialThread);
}


void OpenBCIDriver::OnRemoveDevice(Device* device)
{
	if (device->GetType() != OpenBCIDevice::TYPE_ID &&
		device->GetType() != OpenBCIDaisyDevice::TYPE_ID)
		return;

	OpenBCIDeviceBase* openBCIDevice = static_cast<OpenBCIDeviceBase*>(device); 

	// find index 
	uint32 index = mDevices.Find(openBCIDevice);

	// device does not belong to driver (should not happen)
	if (index == CORE_INVALIDINDEX32)
		return;

	// remove device from device listr
	mDevices.RemoveByValue(openBCIDevice);

	// stop and remove remove serial thread
	OpenBCISerialThread* serialThread = mSerialHandlerThreads[index];
	serialThread->quit();
	serialThread->wait();
	mSerialHandlerThreads.Remove(index);
	delete serialThread;
}


// run autodetection once
void OpenBCIAutoDetection::DetectDevices()
{ 
	if (mIsRunning == false && mDetectOnce == false)
		return;

	// tells driver detection is running now
	mIsSearching = true;

	// list with all serial ports we want to try
	Array<String> portNames;

	// collect all device configs and their serial ports (if any)
	Array<const Device::DeviceConfig*> deviceConfigs;
	deviceConfigs.Add ( GetDeviceManager()->FindDeviceConfigsByType(OpenBCIDevice::TYPE_ID) );
	deviceConfigs.Add ( GetDeviceManager()->FindDeviceConfigsByType(OpenBCIDaisyDevice::TYPE_ID) );

	const uint32 numConfigs = deviceConfigs.Size();
	for (uint32 i=0; i<numConfigs; ++i)
	{
		portNames.Add( DeviceHelpers::FindSerialPorts(*deviceConfigs[i]) );
	}
	
	// check the default usb vid/pid pair
	Array<String> defaultPorts = SerialPort::FindPortsByUsbID(0x6015, 0x0403); // default openbci pid/vids (int: 24597, 1027)
	for (uint32 i=0; i<defaultPorts.Size(); ++i)
	{
		// add port if not contained already
		if (portNames.Contains(defaultPorts[i]) == false)
		{
			portNames.Add(defaultPorts[i]);
			if (deviceConfigs.Size() > 0)
				deviceConfigs.Add(deviceConfigs[0]);   // use first device config, if any (so we can configure electrodes of auto detected devices etc)
			else
				deviceConfigs.Add(NULL); 
		}
	}
	
	const uint32 numPorts = portNames.Size();
	for (uint32 i=0; i<numPorts; ++i)
	{
		// exit early if thread is to be stopped
		if (mBreak == true)
			break;

		//
		// Init Serial connection
		//
		
		LogDetailedInfo("OpenBCIAutoDetection: trying port %s ...", portNames[i].AsChar());

		// check if the com port is accessible before creating the device
		SerialPort serialPort(portNames[i].AsChar());
		if (serialPort.Init() == false)
		{
			LogDetailedInfo("OpenBCIAutoDetection: ... could not initialize port %s", portNames[i].AsChar());
			serialPort.Reset();
			continue;
		}

		// try to configure and open the port
		bool success =  serialPort.Open() && 
						serialPort.SetBaudRate(115200) &&
						serialPort.SetParity(SerialPort::NO_PARITY) &&
						serialPort.SetNumDataBits(8) &&
						serialPort.SetNumStopBits(1);

		// could not open or configure port
		if (success == false)
		{
			LogDetailedInfo("OpenBCIAutoDetection: ... failed (port %s not configurable / openable)", portNames[i].AsChar());
			serialPort.Reset();
			continue;
		}

		uint32 deviceType = 0;
		uint32 reportedDeviceType = 0;
		uint32 configuredDeviceType = 0;

		// if device config is specified, just go with it
		const Device::DeviceConfig* config = deviceConfigs[i];
		if (config != NULL)
			configuredDeviceType = config->mDeviceType;

		// FIXME autodetection does not work with 32 bit board, the reset command does nothing from the looks of it (not yet tested with our device)
		// skip autodetection if a config is specified
		// Note: this makes it incompatible with 8 bit deviec
		const bool skipTypeDetection = (configuredDeviceType != 0);
		
		if (skipTypeDetection == true)
		{
			deviceType = configuredDeviceType;
			Thread::Sleep(5000); // safety wait until I can verify it myself with 32 bit board
		}
		else
		{
			//
			// Detect device by reading openbci boot message
			//

			// send stop command first, in case the device is streaming already and was not stopped correctly
			//serialPort.Write("s", 1);

			// in case this is a 32 bit board: send soft reset before waiting for bootscreen
			if (serialPort.Write("v", 1) == 0 || serialPort.WaitForWrite(100) == false)
			{
				LogDetailedInfo("OpenBCIAutoDetection: reset command failed.", portNames[i].AsChar());
				serialPort.Reset();
				continue;
			}

			// give device time to boot
			if (serialPort.WaitForRead(5000) == false)
			{
				LogDetailedInfo("OpenBCIAutoDetection: device did not answer.", portNames[i].AsChar());
				serialPort.Reset();
				continue;
			}
			serialPort.GetNumBytesAvailable();
			String buff = serialPort.ReadAll();
			while (serialPort.WaitForRead(500))
			{
				// QT BUG QT BUG QT BUG
				// HACKHACKHACK
				// unless we call bytesAvailable() on the serial port, readAll doesn't reply with the current data (sometimes old data, but most of the times no data at all)
				serialPort.GetNumBytesAvailable();
				buff += serialPort.ReadAll();
			}

			LogDetailedInfo(buff.AsChar());

			// no marker string -> device did not send the expected startup message
			if (buff.Contains("$$$") == false)
			{
				LogDetailedInfo("OpenBCIAutoDetection: device did not send its version", portNames[i].AsChar());

				serialPort.Reset();
				continue;
			}
		
			// detect daisy from boot message
			if (buff.Contains("Daisy") == true)
				reportedDeviceType = OpenBCIDaisyDevice::TYPE_ID;
			else // assume its a regular 8 channel device
				reportedDeviceType = OpenBCIDevice::TYPE_ID;

			// if daisy is reported, but configuration requested the 8 channel device, then disable the daisy module
			if (reportedDeviceType == OpenBCIDaisyDevice::TYPE_ID && configuredDeviceType == OpenBCIDevice::TYPE_ID)
			{
				deviceType = OpenBCIDevice::TYPE_ID;
				if (serialPort.Write("c",1) != 1)
				{
					LogDetailedInfo("OpenBCIAutoDetection: disabling daisy module failed", portNames[i].AsChar());

					serialPort.Reset();
					continue;
				}
			}
			else
			{
				deviceType = reportedDeviceType;
			}

		}

		// free serial port before creating serialhandler
		serialPort.Clear();
		serialPort.Reset();
		
		LogDetailedInfo("OpenBCIAutoDetection: ... success", portNames[i].AsChar());

		//for (uint32 s=0; s<100 && mBreak == false; ++s)
		//	Thread::Sleep(10);
		
		// now is a good time for a break
		if (mBreak == true)
			break;

		//
		// Create Device
		//

		// create new open bci headset
		OpenBCIDeviceBase* device = static_cast<OpenBCIDeviceBase*>(mDriver->CreateDevice(deviceType));

		// configure device
		if (config != NULL)
		{
			CORE_ASSERT(deviceType == config->mDeviceType);
			device->Configure(*config);
		}

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


void OpenBCISerialThread::run()
{
	// creat serial port
	SerialPort serialPort(mPortName, this);

	// try to connect to the versus
	OpenBCISerialHandler serialHandler(&serialPort, mDevice, this);

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

#endif
