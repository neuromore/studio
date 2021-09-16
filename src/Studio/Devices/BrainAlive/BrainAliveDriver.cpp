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
#include "./BrainAliveDriver.h"
#include "./BrainAliveSerialHandler.h"
#include "../DeviceHelpers.h"
#include "./Devices/BrainAlive/BrainAliveDevices.h"
#include <EngineManager.h>
#include <QApplication>


#ifdef INCLUDE_DEVICE_BRAINALIVE

using namespace Core;
using namespace std;


extern bool mDevice_connected;

BrainAliveDeviceBase* device;
BrainAliveAutoDetection* mAutoDetection_2;




// constructor
BrainAliveDriver::BrainAliveDriver() : DeviceDriver(false), EventHandler()
{
	
	LogInfo("Constructing BrainAlive device driver ...");

	AddSupportedDevice(BrainAliveDevice::TYPE_ID);

	mAutoDetection_2 = NULL;
	mAutoDetectionTimer_2 = NULL;
	mAutoDetectionThread_2 = NULL;
	LogDetailedInfo("BrainAlive driver constructed");
}


// destructor
BrainAliveDriver::~BrainAliveDriver()
{
	LogInfo("Destructing BrainAlive device driver ...");

	// remove event handler
	CORE_EVENTMANAGER.RemoveEventHandler(this);

	// stop thread
	if (mAutoDetectionThread_2 != NULL)
		if (mAutoDetectionThread_2->isRunning())
		{
			mAutoDetection_2->Break();
			mAutoDetectionThread_2->quit();
			mAutoDetectionThread_2->wait();
		}
	delete mAutoDetection_2;
	delete mAutoDetectionTimer_2;
	delete mAutoDetectionThread_2;

	// NOTE: devices and serial threads are usually removed by the device manager. We check this, maybe some something went wrong (e.g. we fucked up the order):
	CORE_ASSERT(mSerialHandlerThreads.Size() == 0 && mDevices.Size() == 0);

	//// stop and delete serial handler threads (in case some remain, which means the device manager did not call RemoveDevice())
	//const uint32 numThreads = mSerialHandlerThreads.Size();
	//for (uint32 i = 0; i < numThreads; ++i)
	//{
	// mSerialHandlerThreads[i]->quit();
	// mSerialHandlerThreads[i]->wait();
	// mSerialHandlerThreads[i]->deleteLater();
	//}


	 LogDetailedInfo("BrainAlive device driver destructed");
}


// system initialization
bool BrainAliveDriver::Init()
{
	LogInfo("Initializing BrainAlive device driver");

	mAutoDetection_2 = new BrainAliveAutoDetection(this);
	mAutoDetectionTimer_2 = new QTimer(NULL);
	mAutoDetectionThread_2 = new QThread(this);

	QObject::connect(mAutoDetectionTimer_2, &QTimer::timeout, mAutoDetection_2, &BrainAliveAutoDetection::DetectDevices);
	// lower frequency for this device to optimize detection (we wait most of the time on the serial port)
	mAutoDetectionTimer_2->setInterval(1000);
	mAutoDetectionTimer_2->start();
	mAutoDetectionTimer_2->moveToThread(mAutoDetectionThread_2);
	mAutoDetection_2->moveToThread(mAutoDetectionThread_2);

	// register event handler for reacting to device removal
	CORE_EVENTMANAGER.AddEventHandler(this);

	return true;
}

void BrainAliveDriver::Update(const Time& elapsed, const Time& delta)
{
}


// start stop auto detection
void BrainAliveDriver::SetAutoDetectionEnabled(bool enable)
{
	DeviceDriver::SetAutoDetectionEnabled(enable);

	if (enable == true)
	{
		LogDetailedInfo("Starting BrainAlive auto detection ...");
		mAutoDetectionThread_2->start();
		mAutoDetection_2->Start();
	}
	else
	{
		LogDetailedInfo("Stopping BrainAlive auto detection ...");
		mAutoDetection_2->Stop();
	}
}


bool BrainAliveDriver::IsDetectionRunning() const
{
	if (mAutoDetectionThread_2->isRunning() == false)
		return false;

	return mAutoDetection_2->IsSearching();
}


// detect devices once
void BrainAliveDriver::DetectDevices()
{
	if (mIsEnabled == false)
		return;

	// make sure thread is running
	mAutoDetectionThread_2->start();
	mAutoDetection_2->DetectDevicesOnce();
}


Device* BrainAliveDriver::CreateDevice(uint32 deviceTypeID)
{
	// check if its the correct device
	CORE_ASSERT(IsDeviceSupported(deviceTypeID));

	switch (deviceTypeID)
	{
	case BrainAliveDevice::TYPE_ID: return new BrainAliveDevice(this);
	default: /* does not happen*/ return NULL;
	}
}


void BrainAliveDriver::AddDevice(BrainAliveDeviceBase* device)
{
	mDevices.Add(device);
	// create serial thread for the device (uses the serial handler)
	BrainAliveSerialThread* serialThread = new BrainAliveSerialThread(device);
	serialThread->start();

	mSerialHandlerThreads.Add(serialThread);
}


void BrainAliveDriver::OnRemoveDevice(Device* device)
{
	if (device->GetType() != BrainAliveDevice::TYPE_ID)
		return;

	BrainAliveDeviceBase* BrainAliveDevice = static_cast<BrainAliveDeviceBase*>(device);

	// find index
	uint32 index = mDevices.Find(BrainAliveDevice);

	// device does not belong to driver (should not happen)
	if (index == CORE_INVALIDINDEX32)
		return;

	// remove device from device listr
	mDevices.RemoveByValue(BrainAliveDevice);

	// stop and remove remove serial thread
	BrainAliveSerialThread* serialThread = mSerialHandlerThreads[index];
	serialThread->quit();
	serialThread->wait();
	mSerialHandlerThreads.Remove(index);
	delete serialThread;
}


// run autodetection once
void BrainAliveAutoDetection::DetectDevices()
{
	/*m_bleInterface = new BLEInterface(this);*/
	if (mIsRunning == false && mDetectOnce == false)
		return;
	// tells driver detection is running now
	mIsSearching = true;

	// list with all serial ports we want to try
	/*Array<String> portNames;*/



	// collect all device configs and their serial ports (if any)
	Array<const Device::DeviceConfig*> deviceConfigs;
	deviceConfigs.Add(GetDeviceManager()->FindDeviceConfigsByType(BrainAliveDevice::TYPE_ID));

	//const uint32 numConfigs = deviceConfigs.Size();




	if (mDevice_connected == true)
	{

		if (deviceConfigs.Size() > 0)
			deviceConfigs.Add(deviceConfigs[0]); // use first device config, if any (so we can configure electrodes of auto detected devices etc)
		else
			deviceConfigs.Add(NULL);

		uint32 deviceType = 0;
		uint32 reportedDeviceType = 0;
		uint32 configuredDeviceType = 0;

		// if device config is specified, just go with it
		const Device::DeviceConfig* config = deviceConfigs[0];
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

			reportedDeviceType = BrainAliveDevice::TYPE_ID;

			// if daisy is reported, but configuration requested the 8 channel device, then disable the daisy module
			if (configuredDeviceType == BrainAliveDevice::TYPE_ID)
			{
				deviceType = BrainAliveDevice::TYPE_ID;

			}
			else
			{
				deviceType = reportedDeviceType;
			}
		}
		
		//
		// Create Device
		//
		// create new  BrainAlive headset
	
			device = static_cast<BrainAliveDeviceBase*>(mDriver->CreateDevice(deviceType));
			/*	device->Connect();
				device->KeepAlive();*/
				//device->KeepAlive();
				// configure device
			if (config != NULL)
			{
				CORE_ASSERT(deviceType == config->mDeviceType);
				device->Configure(*config);
			}

			// add device to driver, it will create a serial handler thread for it
			mDriver->AddDevice(device);

			// add device to device manager

			GetDeviceManager()->AddDeviceAsync(device);
	
		//mDetectOnce = true;
		mDriver->mAutoDetectionTimer_2->stop();
		//mDriver->mAutoDetectionThread_2->exit();
	}



	// detect only once? -> stop
	if (mDetectOnce == true)
		mDetectOnce = false;

	// reset flag
	mIsSearching = false;
}


void BrainAliveSerialThread::run()
{
	// creat serial port
	//SerialPort serialPort(mPortName, this);

	//// try to connect to the versus
	BrainAliveSerialHandler serialHandler(mDevice, this);

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
