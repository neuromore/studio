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
#include <Engine/Precompiled.h>

// include required headers
#include "DeviceOutputNode.h"
#include "../EngineManager.h"
#include "../DSP/AttributeChannels.h"

using namespace Core;

// constructor
DeviceOutputNode::DeviceOutputNode(Graph* graph, uint32 deviceType) : SPNode(graph)
{
	mDeviceType = deviceType;
	mCurrentDevice = NULL;
}


// destructor
DeviceOutputNode::~DeviceOutputNode()
{
	// free device lock and disconnect
	if (mCurrentDevice != NULL)
	{
		// TODO : if studio is closed the device is removed before the node so this lock fails!
		if (mCurrentDevice->ReleaseLock(this) == true)
			mCurrentDevice->Disconnect();

		mCurrentDevice = NULL;
	}
}


// init nodes
void DeviceOutputNode::Init()
{
	// SPNODE
	RequireInputConnection();
	RequireConstantSampleRate();
	RequireMatchingSampleRates();
	RequireSyncedInput();

	// PORTS

	// get device prototype from manager
	const Device* prototype = GetDeviceManager()->GetRegisteredDeviceType(mDeviceType);

	CORE_ASSERT(prototype != NULL);
	CORE_ASSERT(prototype->IsOutputDevice() == true && prototype->IsInputDevice() == false);
	
	// setup the output ports
	RegisterDeviceSensorsAsPorts(prototype);

	// ATTRIBUTES

	// baseclass attributes
	SPNode::Init();

	// device ID
	AttributeSettings* deviceIDAttrib = RegisterAttribute("Device ID", "ID", "In case you have connected multiple devices of the same type, use this ID to specify which one this node uses", ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	deviceIDAttrib->SetDefaultValue( AttributeInt32::Create(1) );
	deviceIDAttrib->SetMinValue( AttributeInt32::Create(1) );
	deviceIDAttrib->SetMaxValue( AttributeInt32::Create(CORE_INT32_MAX) );
}


// reset node
void DeviceOutputNode::Reset()
{
	// skip InputNode:Reset()
	SPNode::Reset();

	// free device lock and disconnect
	if (mCurrentDevice != NULL)
	{
		if (mCurrentDevice->ReleaseLock(this) == true)
			mCurrentDevice->Disconnect();

		mCurrentDevice = NULL;
	}
}


// check if device is present and usable
void DeviceOutputNode::ReInit(const Time& elapsed, const Time& delta)
{
	// shared base reinit helper
	if (BaseReInit(elapsed, delta) == false)
		return;

	SPNode::ReInit(elapsed, delta);

	// skip if node base cannot init
	if (mIsInitialized == true)
	{
		const uint32 numPorts = GetNumInputPorts();
		for (uint32 i = 0; i < numPorts; ++i)
		{
			if (GetInputPort(i).HasConnection() == false ||
				(GetInputPort(i).HasConnection() == true && GetInputPort(i).GetChannels()->GetNumChannels() != 1))
			{
				SetError(ERROR_WRONG_INPUT, "Input requires single channels.");
				mIsInitialized = false;
			}
		}
	}

	ClearError(ERROR_WRONG_INPUT);

	// skip device search if node cannot init
	if (mIsInitialized == true)
	{
		// get device and stop node if there is not a running one
		Device* device = FindDevice();
		if (device != NULL && device->IsEnabled())
		{
			// make sure we're the only one who has the lock
			if (device->AcquireLock(this) == true)
			{
				if (device->IsConnected() == false)
					device->Connect();

				if (device->IsConnected() == true)
					mCurrentDevice = device;

				ClearError(ERROR_DEVICE_LOCKED);
			}
			else
			{
				SetError(ERROR_DEVICE_LOCKED, "Device is already in use.");
			}
		}
	}

	// have no device
	if (mCurrentDevice == NULL)
	{
		mIsInitialized = false;
		SetError(ERROR_DEVICE_NOT_FOUND, "Device not connected.");
	}
	else
		ClearError(ERROR_DEVICE_NOT_FOUND);

	PostReInit(elapsed, delta);
}


// connect sensor channels to output ports
void DeviceOutputNode::Start(const Time& elapsed)
{
	SPNode::Start(elapsed);
}


// update the node
void DeviceOutputNode::Update(const Time& elapsed, const Time& delta)
{
	// shared base update helper
	if (BaseUpdate(elapsed, delta) == false)
		return;

	SPNode::Update(elapsed, delta);

	if (mIsInitialized == false)
		return;

	if (mCurrentDevice->GetBatteryChargeLevel() < 0.1f)
		SetWarning(WARNING_DEVICE_DEVICE_BATTERY_LOW, "Battery low.");
	else
		ClearWarning(WARNING_DEVICE_DEVICE_BATTERY_LOW);

	// foward new data to device sensors
	const uint32 numSensors = mCurrentDevice->GetNumSensors();
	const uint32 numSamples = mInputReader.GetMinNumNewSamples();

	// forward all input samples to the sensors
	for (uint32 i = 0; i<numSensors; ++i)
	{
		ChannelReader* channelReader = mInputReader.GetReader(i);
		for (uint32 j = 0; j < numSamples; ++j)
		{
			// get access to the current sensor
			Sensor* sensor = mCurrentDevice->GetSensor(i);
			sensor->AddQueuedSample(channelReader->PopOldestSample<double>());
		}
	}
}


// make each sensor of the given device to an output port
void DeviceOutputNode::RegisterDeviceSensorsAsPorts(const Device* device)
{
	const uint32 numSensors = device->GetNumSensors();
	String internalPortName;

	// just create one port for each sensor
	InitInputPorts(numSensors);

	// name the ports
	for (uint32 i = 0; i<numSensors; ++i)
	{
		// get access to the current sensor
		Sensor* sensor = device->GetSensor(i);

		// set port name
		internalPortName.Format("y%i", i+1); //y1 is first port
		GetInputPort(i).Setup(sensor->GetName(), internalPortName.AsChar(), AttributeChannels<double>::TYPE_ID, i);
	}
}


void DeviceOutputNode::OnAttributesChanged()
{
	// baseclass call
	SPNode::OnAttributesChanged();

}


// find the first corresponding device
Device* DeviceOutputNode::FindDevice()
{
	uint32 deviceID = GetInt32AttributeByName("ID");
	if (deviceID == 0)
		deviceID = 1;
	
	Device* device = GetDeviceManager()->FindDeviceByType(mDeviceType, deviceID-1); // 0-indexed
	return device;
}


uint32 DeviceOutputNode::GetNumSensors() const 
{
	if (mCurrentDevice == NULL)
		return 0;

	return mCurrentDevice->GetNumSensors();
}


Sensor* DeviceOutputNode::GetSensor(uint32 index)  
{
	if (mCurrentDevice == NULL)
		return NULL;

	return mCurrentDevice->GetSensor(index);
}
