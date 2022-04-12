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
#include "AbmDriver.h"
#include <Core/LogManager.h>
#include <QtBaseManager.h>
#include "AbmThreadHandler.h"

#ifdef INCLUDE_DEVICE_ADVANCEDBRAINMONITORING

using namespace Core;

// constructor
AbmDriver::AbmDriver() : DeviceDriver(false)
{
	LogDetailedInfo("Constructing Advanced Brain Monitoring driver ...");

	mAbmThreadHandler = NULL;
	mIsSearching = false;
	mDevice = NULL;
	mHasPreviousDeviceInfo = false;

	AddSupportedDevice(AbmX10Device::TYPE_ID);
	AddSupportedDevice(AbmX24Device::TYPE_ID);
}


// destructor
AbmDriver::~AbmDriver()
{
	LogDetailedInfo("Destructing Advanced Brain Monitoring driver ...");

	// stop and remove the thread
	delete mThread;

	// remove event handler
	CORE_EVENTMANAGER.RemoveEventHandler(this);
}


// initialization
bool AbmDriver::Init()
{
	LogInfo("Initializing Advanced Brain Monitoring driver ...");

	// register event handler for reacting to device removal
	CORE_EVENTMANAGER.AddEventHandler(this);

	// init thread
	mAbmThreadHandler = new AbmThreadHandler(this);
	mThread = new Thread(mAbmThreadHandler, "Advanced Brain Monitoring Driver Thread");

	LogDetailedInfo("Advanced Brain Monitoring driver initialized ...");
	return true;
}


// update the driver
void AbmDriver::Update(const Time& elapsed, const Time& delta)
{
	// do not detect devices if system is disabled
	if (IsEnabled() == false)
		return;
}


// create the ABM device
Device* AbmDriver::CreateDevice(uint32 deviceTypeID)
{
	switch (deviceTypeID)
	{
		case AbmX10Device::TYPE_ID:		return new AbmX10Device(this);
		case AbmX24Device::TYPE_ID:		return new AbmX24Device(this);
		default: break;
	}
	return NULL;
}

// start stop auto detection
void AbmDriver::SetAutoDetectionEnabled(bool enable)
{
   DeviceDriver::SetAutoDetectionEnabled(enable);
}

void AbmDriver::StartAutoDetection()
{
   LogDetailedInfo("Starting ABM auto detection ...");
   mThread->Start();
}

void AbmDriver::StopAutoDetection()
{
   LogDetailedInfo("Stopping ABM auto detection ...");
   // NOTE: thread handler loop uses engine autodetection setting and stops searching for device
}


void AbmDriver::DetectDevices()
{
	if (mIsEnabled == false)
		return;

	mThread->Start();
	if (mAbmThreadHandler != NULL)
		mAbmThreadHandler->DetectDevicesOnce();
}


// event handler
void AbmDriver::OnRemoveDevice(Device* device)
{
	if (mDevice != device)
		return;

	mThread->Stop();

	// remove pointer
	mDevice = NULL;
}


// log ABM device information
void AbmDriver::LogDeviceInfo(_DEVICE_INFO* deviceInfo)
{
	if (deviceInfo == NULL)
		return;

	LogInfo("ABM Device Info:");

	// device name
	String deviceName = deviceInfo->chDeviceName;
	LogInfo("   Device Name: %s", deviceName.AsChar());

	// device id
	String deviceId = deviceInfo->chDeviceID;
	LogInfo("   Device ID: %s", deviceId.AsChar());

	// device handle
	int32 deviceHandle = deviceInfo->nDeviceHandle;
	LogInfo("   Device Handle: %i", deviceHandle);

	// COM port
	int32 comPort = deviceInfo->nCommPort;
	LogInfo("   COM Port: %i", comPort);

	// number of channels
	uint32 numChannels = deviceInfo->nNumberOfChannel;
	LogInfo("   Num Channels: %i", numChannels);

	// ECG pos
	int32 ecgPos = deviceInfo->nECGPos;
	LogInfo("   ECG Pos: %i", ecgPos);
}



// map SDK channel index to device sensors
Sensor* AbmDriver::GetRawSensorByIndex(AbmDevice* abmDevice, uint32 index)
{
	switch (abmDevice->GetType())
	{
		case AbmX10Device::TYPE_ID:
		{
			// first channel is EKG, rest is the sensors in the order given by device
			if (index == 0)
				return abmDevice->GetEKGSensor();
			else if (index < abmDevice->GetNumNeuroSensors() + 1)
				return abmDevice->GetNeuroSensor(index - 1);

		} break;

		case AbmX24Device::TYPE_ID:
		{
			AbmX24Device* device = static_cast<AbmX24Device*>(abmDevice);
			const uint32 numElectrodes = abmDevice->GetNumNeuroSensors();

			// first come the raw sensors as given by device, after that we have EKG followed by AUX1-AUX3
			if (index < numElectrodes)
				return abmDevice->GetNeuroSensor(index);
			else if (index == numElectrodes)
				return device->GetEKGSensor();
			else if (index == numElectrodes + 1)
				return device->GetAux1Sensor();
			else if (index == numElectrodes + 2)
				return device->GetAux2Sensor();
			else if (index == numElectrodes + 3)
				return device->GetAux3Sensor();
		} break;

	}

	return NULL;
}



String AbmDriver::GetAbmErrorAsString(int errorcode)
{
	switch (errorcode)
	{
		case ABM_ERROR_SDK_ACQUISITION_STOPPED			        : return "ABM_ERROR_SDK_ACQUISITION_STOPPED";
		case ABM_ERROR_SDK_NO_DATA_ARRIVING						: return "ABM_ERROR_SDK_NO_DATA_ARRIVING";
		case ABM_ERROR_SDK_CREATE_MAIN_WINDOW_FAILED			: return "ABM_ERROR_SDK_CREATE_MAIN_WINDOW_FAILED";
		case ABM_ERROR_SDK_COULDNT_FIND_DEVICE           		: return "ABM_ERROR_SDK_COULDNT_FIND_DEVICE";
		case ABM_ERROR_SDK_COULDNT_CONNECT_DEVICE				: return "ABM_ERROR_SDK_COULDNT_CONNECT_DEVICE";
		case ABM_ERROR_SDK_EDF_FILE_ERROR						: return "ABM_ERROR_SDK_EDF_FILE_ERROR";
		case ABM_ERROR_SDK_THIRDPARTYBYTES						: return "ABM_ERROR_SDK_THIRDPARTYBYTES";
		case ABM_ERROR_SDK_TEAMING_BEACON_ERROR					: return "ABM_ERROR_SDK_TEAMING_BEACON_ERROR";
		case ABM_ERROR_SDK_COULDNT_START_REALTIME        		: return "ABM_ERROR_SDK_COULDNT_START_REALTIME";
		case ABM_ERROR_SDK_COULDNT_START_SAVING					: return "ABM_ERROR_SDK_COULDNT_START_SAVING";
		case ABM_ERROR_SDK_COULDNT_STOP_REAL_TIME				: return "ABM_ERROR_SDK_COULDNT_STOP_REAL_TIME";
		case ABM_ERROR_SDK_COULDNT_LOAD_CHANNEL_MAP	    		: return "ABM_ERROR_SDK_COULDNT_LOAD_CHANNEL_MAP";
		case ABM_ERROR_SDK_WRONG_SESSION_TYPE					: return "ABM_ERROR_SDK_WRONG_SESSION_TYPE";
		case ABM_ERROR_SDK_WRONG_INPUT_SETTINGS					: return "ABM_ERROR_SDK_WRONG_INPUT_SETTINGS";
		case ABM_ERROR_SDK_WRONG_FILE_PATHS						: return "ABM_ERROR_SDK_WRONG_FILE_PATHS";
		case ABM_ERROR_SDK_CLASSIFICATION_INIT_FAILED    		: return "ABM_ERROR_SDK_CLASSIFICATION_INIT_FAILED";
		case ABM_ERROR_SDK_COULDNT_CLOSE_CONNECTION	    		: return "ABM_ERROR_SDK_COULDNT_CLOSE_CONNECTION";
		case ABM_ERROR_SDK_NOTSET_DEFFILE						: return "ABM_ERROR_SDK_NOTSET_DEFFILE";
		case ABM_ERROR_SDK_WRONG_DESTPATH						: return "ABM_ERROR_SDK_WRONG_DESTPATH";
		case ABM_ERROR_SDK_TOO_LARGE_MISSED_BLOCK				: return "ABM_ERROR_SDK_TOO_LARGE_MISSED_BLOCK";
		case ABM_ERROR_SDK_TOO_MANY_MISSED_BLOCKS        		: return "ABM_ERROR_SDK_TOO_MANY_MISSED_BLOCKS";
		case ABM_ERROR_SDK_COMMAND_IMP_START_FAILED				: return "ABM_ERROR_SDK_COMMAND_IMP_START_FAILED";
		case ABM_ERROR_SDK_COMMAND_IMP_STOP_FAILED				: return "ABM_ERROR_SDK_COMMAND_IMP_STOP_FAILED";
		case ABM_ERROR_SDK_COMMAND_IMP_HIGH_FAILED				: return "ABM_ERROR_SDK_COMMAND_IMP_HIGH_FAILED";
		case ABM_ERROR_SDK_COMMAND_IMP_LOW_FAILED				: return "ABM_ERROR_SDK_COMMAND_IMP_LOW_FAILED";
		case ABM_ERROR_SDK_COMMAND_IMP_START_FAILED_IGNORED     : return "ABM_ERROR_SDK_COMMAND_IMP_START_FAILED_IGNORED";
		default: return "Unknown Error ID";
	}
}




#endif

