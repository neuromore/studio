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

#include "ActiChampDriver.h"
#include <Core/LogManager.h>
#include <Core/ThreadHandler.h>
#include <QMessageBox>
#include <QtBaseConfig.h>
#include <QtBaseManager.h>

#ifdef INCLUDE_DEVICE_ACTICHAMP

using namespace Core;

// constructor
ActiChampDriver::ActiChampDriver() : DeviceDriver(false)
{
	LogDetailedInfo("Constructing Mitsar Driver ...");

	mDevice = NULL;

	AddSupportedDevice(ActiChampDevice::TYPE_ID);
}


// destructor
ActiChampDriver::~ActiChampDriver()
{
	LogDetailedInfo("Destructing Mitsar Driver ...");

	// remove event handler
	CORE_EVENTMANAGER.RemoveEventHandler(this);
}


// initialization
bool ActiChampDriver::Init()
{
	LogInfo("Initializing Mitsar Driver... ");

	// register event handler for reacting to device removal
	CORE_EVENTMANAGER.AddEventHandler(this);

	LogDetailedInfo("Mitsar Driver initialized.");
	return true;
}


// update the emotiv manager
void ActiChampDriver::Update(const Time& elapsed, const Time& delta)
{
	// do not detect devices if system is disabled
	if (IsEnabled() == false)
		return;
}


void ActiChampDriver::SetAutoDetectionEnabled(bool enable)
{
	DeviceDriver::SetAutoDetectionEnabled(enable);

}

bool ActiChampDriver::IsDetectionRunning() const
{
	return (mState == STATE_SEARCHING);
}


// detect once
void ActiChampDriver::DetectDevices()
{
	if (mIsEnabled == false)
		return;

}


Device* ActiChampDriver::CreateDevice(uint32 deviceTypeID)
{
	CORE_ASSERT (IsDeviceSupported(deviceTypeID) == true);
	if (IsDeviceSupported(deviceTypeID) == false)
		return NULL;

	switch (deviceTypeID)
	{
		case ActiChampDevice::TYPE_ID: 	return new ActiChampDevice(this);
		// TODO more devices
		default: return NULL;
	}
}

// mitsar should be removed -> thread has to be stopped
void ActiChampDriver::OnRemoveDevice(Device* device) 
{
	// terminate thread if our device was removed
	if (device == mDevice && mUseAutoDetection == true)
	{}
}


// mitsar device was added
void ActiChampDriver::OnDeviceAdded(Device* device)
{
	if (IsDeviceSupported(device->GetType()) == false)
		return;

	mDevice = device;
}


void ActiChampDriver::StartTest(Device* device)
{
	if (device != mDevice)
		return;

}


void ActiChampDriver::StopTest(Device* device)
{
	if (device != mDevice)
		return;

}


bool ActiChampDriver::IsTestRunning(Device* device)
{
	if (device != mDevice)
		return false;

	return NULL;
}


#endif
