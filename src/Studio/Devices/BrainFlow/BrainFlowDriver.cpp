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
#include "BrainFlowDriver.h"
#include "../DeviceHelpers.h"
#include <Devices/BrainFlow/BrainFlowDevices.h>
#include <EngineManager.h>
#include <System/SerialPort.h>
#include <QApplication>

#ifdef INCLUDE_DEVICE_BRAINFLOW

using namespace Core;


// destructor
BrainFlowDriverBase::~BrainFlowDriverBase ()
{
	LogInfo("Destructing BrainFlow device driver ...");
	// remove event handler
	CORE_EVENTMANAGER.RemoveEventHandler(this);
}


void BrainFlowDriverBase::AddDevice(BrainFlowDeviceBase* device)
{
	mDevices.Add(device);
	BrainFlowInputParams params = GetParams();
	device->CreateBoardShim(params);
	BoardShim* boardShim = device->GetBoardShim();
	if (boardShim != NULL)
	{
		try
		{
			boardShim->prepare_session();
			// boardShim->start_stream();
			boardShim->start_stream(450000, "file://brainflow_raw_data.csv:w");
		}
		catch (const BrainFlowException& err)
		{
			LogError(err.what());
		}
	}
}


void BrainFlowDriverBase::OnRemoveDevice(Device* device)
{
	BrainFlowDeviceBase* brainFlowDevice = static_cast<BrainFlowDeviceBase*>(device);

	// find index 
	uint32 index = mDevices.Find(brainFlowDevice);

	// device does not belong to driver (should not happen)
	if (index == CORE_INVALIDINDEX32)
		return;
	
	// release brainflow session
	BoardShim* boardShim = brainFlowDevice->GetBoardShim();
	if (boardShim != NULL)
	{
		try
		{
			boardShim->release_session();
		}
		catch (const BrainFlowException& err)
		{
			LogError (err.what ());
		}
		brainFlowDevice->ReleaseBoardShim();
	}

	// remove device from device listr
	mDevices.RemoveByValue(brainFlowDevice);
}

BrainFlowDriverCyton::BrainFlowDriverCyton() : BrainFlowDriverBase(), mTimeSinceDeviceCheck(0)
{}

Device* BrainFlowDriverCyton::CreateDevice(uint32 deviceTypeID)
{
	// check if its the correct device
	CORE_ASSERT(IsDeviceSupported(deviceTypeID));

	switch (deviceTypeID)
	{
	case BrainFlowDeviceCyton::TYPE_ID: 	return new BrainFlowDeviceCyton(this);
	default:  /* does not happen*/	return NULL;
	}
}

bool BrainFlowDriverCyton::Init()
{
	return true; 
}

void BrainFlowDriverCyton::Update(const Core::Time& delta, const Core::Time& elapsed)
{
	if (IsEnabled() == false)
		return;

	mTimeSinceDeviceCheck += delta.InSeconds();
	if (mTimeSinceDeviceCheck > 1.0)
	{
		// add one test headset in case no other device is connected
		const uint32 numTestHeadsets = GetDeviceManager()->FindNumDevicesByType(BrainFlowDeviceCyton::TYPE_ID);
		if (numTestHeadsets == 0)
		{
			if (GetDeviceManager()->GetNumDevices() == 0)
			{
				Device* testHeadset = new BrainFlowDeviceCyton(this);
				GetDeviceManager()->AddDeviceAsync(testHeadset);
			}
		}
		else
		{
			// remove the test headset as soon as a real device gets connected
			if (GetDeviceManager()->GetNumDevices() > numTestHeadsets)
			{
				Device* testHeadset = GetDeviceManager()->FindDeviceByType(BrainFlowDeviceCyton::TYPE_ID, 0);
				GetDeviceManager()->RemoveDeviceAsync(testHeadset);
			}
		}

		mTimeSinceDeviceCheck = 0.0;
	}
}

BrainFlowInputParams BrainFlowDriverCyton::GetParams()
{
	BrainFlowInputParams params;
	params.serial_port = "COM3"; // temp hardcode, get it from user input or via autodiscovery
	return params;
}

#endif
