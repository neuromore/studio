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
#include "DeviceInputNode.h"
#include "../EngineManager.h"
#include "../DSP/AttributeChannels.h"

// required for detecting used channels in combination with selector node
#include "ChannelSelectorNode.h"

using namespace Core;

// constructor
DeviceInputNode::DeviceInputNode(Graph* graph, uint32 deviceType) : InputNode(graph)
{
	mDeviceType = deviceType;
	mCurrentDevice = NULL;
	mRawOutputEnabled = false;
}


// destructor
DeviceInputNode::~DeviceInputNode()
{
}


// init nodes
void DeviceInputNode::Init()
{
	// PORTS

	// get device prototype from manager
	const Device* prototype = GetDeviceManager()->GetRegisteredDeviceType(mDeviceType);

	CORE_ASSERT(prototype != NULL);
	CORE_ASSERT(prototype->IsInputDevice() == true && prototype->IsOutputDevice() == false);
	
	// setup the output ports
	RegisterDeviceSensorsAsPorts(prototype);
	UseChannelColoring();

	// ATTRIBUTES

	// baseclass attributes
	InputNode::Init();

	// device ID
	AttributeSettings* deviceIDAttrib = RegisterAttribute("Device Number", "ID", "In case you have connected multiple devices of the same Type, use this ID to specify which one this node uses", ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	deviceIDAttrib->SetDefaultValue( AttributeInt32::Create(1) );
	deviceIDAttrib->SetMinValue( AttributeInt32::Create(1) );
	deviceIDAttrib->SetMaxValue( AttributeInt32::Create(CORE_INT32_MAX) );

	// raw output setting
	AttributeSettings* rawOutputAttrib = RegisterAttribute("Raw Output", "RawOutput", "Output the unsynced raw streams of the device and bypass the drift correction. The channel's clock will be driven by the device.", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	rawOutputAttrib->SetDefaultValue(AttributeBool::Create(mRawOutputEnabled));

	// show upload checkbox
	GetAttributeSettings(ATTRIB_UPLOAD)->SetVisible(true);
}


// reset node
void DeviceInputNode::Reset()
{
	// skip InputNode:Reset()
	SPNode::Reset();

	// remove device channel references from outputs
	const uint32 numPorts = GetNumOutputPorts();
	for (uint32 i = 0; i<numPorts; ++i)
		GetOutputPort(i).GetChannels()->Clear();

	// TODO no locks for input devices right now, it doesn't work right on so many levels
	// free device lock
//	if (mCurrentDevice != NULL)
//		mCurrentDevice->ReleaseLock(this);

	mCurrentDevice = NULL;
}


// check if device is present and usable
void DeviceInputNode::ReInit(const Time& elapsed, const Time& delta)
{
	// Note: Skip InputNode::ReInit()
	SPNode::ReInit(elapsed, delta);

	// get device and stop node if there is not a running one
	mCurrentDevice = NULL;
	Device* device = FindDevice();
	if (device != NULL && device->IsEnabled())
	{
		// NOTE: disabled locking mechanism for input device for now

		//// make sure we're the only one who has the lock
		//if (device->AcquireLock(this) == true)
		//{
		//	// try to connect, if not already
		//	if (device->IsConnected() == false)
		//		device->Connect();

			if (device->IsStreaming() == true)
				mCurrentDevice = device;
		//}
		//else
		//{
		//	SetHasError("Device is already in use.");
		//}
	}

	// have no device
	if (mCurrentDevice == NULL)
	{
		mIsInitialized = false;
		SetError(ERROR_DEVICE_NOT_FOUND, "Device not connected");
	}
	else
		ClearError(ERROR_DEVICE_NOT_FOUND);

	PostReInit(elapsed, delta);
}


// connect sensor channels to output ports
void DeviceInputNode::Start(const Time& elapsed)
{
	// Note: do not call SPNode::Start, as the output channels do not belong to device and must not be aligned by the node
	
	// device not found?
	if (mCurrentDevice == NULL)
		return;

	const uint32 numSensors = mCurrentDevice->GetNumSensors();
	const bool rawOutputEnabled = GetBoolAttribute(ATTRIB_RAWOUTPUT);

	// connect EEG channels to multi channel port if its a neuro headset
	if (mCurrentDevice->GetBaseType() == BciDevice::BASE_TYPE_ID)
	{
		const BciDevice* headset = static_cast<const BciDevice*>(mCurrentDevice);
		const uint32 numNeuroSensors = headset->GetNumNeuroSensors();

		MultiChannel* channels = GetOutputPort(0).GetChannels();
		// bundle neuro channels in one port (raw or synced channel)
		if (rawOutputEnabled == true)
			for (uint32 i = 0; i < numNeuroSensors; ++i)
				channels->AddChannel(headset->GetNeuroSensor(i)->GetInput());
		else
			for (uint32 i = 0; i < numNeuroSensors; ++i)
				channels->AddChannel(headset->GetNeuroSensor(i)->GetChannel());

		// show individual neuro sensor ports?
		if (headset->ShowNeuroChannels() == true)
		{
			// forward all sensors channels to ports
			for (uint32 i = 0; i < numSensors; ++i)
			{
				// get access to the current sensor
				Sensor* sensor = headset->GetSensor(i);

				// set node output channel, raw or synced
				if (rawOutputEnabled == true)
					GetOutputPort(i + 1).GetChannels()->AddChannel(sensor->GetInput());
				else
					GetOutputPort(i + 1).GetChannels()->AddChannel(sensor->GetChannel());

				// update port name
				GetOutputPort(i + 1).SetName(sensor->GetName());
			}
		}
		else
		{
			// forward all non-neuro sensor channels to ports (they come first in the list)
			int portIndex = 1;
			for (uint32 i = numNeuroSensors; i < numSensors; ++i)
			{
				// get access to the current sensor
				Sensor* sensor = headset->GetSensor(i);

				// set node output channel, raw or synced
				if (rawOutputEnabled == true)
					GetOutputPort(portIndex).GetChannels()->AddChannel(sensor->GetInput());
				else
					GetOutputPort(portIndex).GetChannels()->AddChannel(sensor->GetChannel());
				
				// update port name
				GetOutputPort(portIndex).SetName(sensor->GetName());

				portIndex++;
			}

		}
	}
	else // device is not a neuro headset
	{
		// forward all sensors channels to ports
		for (uint32 i = 0; i<numSensors; ++i)
		{
			// get access to the current sensor
			Sensor* sensor = mCurrentDevice->GetSensor(i);

			// set node output channel, raw or synced
			if (mRawOutputEnabled == true)
				GetOutputPort(i).GetChannels()->AddChannel(sensor->GetInput());
			else
				GetOutputPort(i).GetChannels()->AddChannel(sensor->GetChannel());

			// update port name
			GetOutputPort(i).SetName(sensor->GetName());
		}
	}

}


// update the node
void DeviceInputNode::Update(const Time& elapsed, const Time& delta)
{
	// shared base update helper
	if (BaseUpdate(elapsed, delta) == false)
		return;

	if (mIsInitialized == false)
		return;

	// NOTE: do not update InputNode or SPNode base class here, because the output channels are owned by the device
	mInputReader.Update();

	if (mCurrentDevice->GetBatteryChargeLevel() < 0.1f)
		SetWarning(WARNING_DEVICE_DEVICE_BATTERY_LOW, "Battery low.");
	else
		ClearWarning(WARNING_DEVICE_DEVICE_BATTERY_LOW);

}


// make each sensor of the given device to an output port
void DeviceInputNode::RegisterDeviceSensorsAsPorts(const Device* device)
{
	const uint32 numSensors = device->GetNumSensors();
	String internalPortName;

	// register multi channel EEG port if the device is a neuro headset
	if (device->GetBaseType() == BciDevice::BASE_TYPE_ID)
	{
		const BciDevice* headset = static_cast<const BciDevice*>(device);
		const uint32 numNeuroSensors = headset->GetNumNeuroSensors();

		// show individual neuro sensors only if there are few of them
		if (headset->ShowNeuroChannels() == true)
			InitOutputPorts(numSensors + 1);
		else
			InitOutputPorts( (numSensors-numNeuroSensors) + 1);
		
		// setup EEG output port
		GetOutputPort(0).Setup("EEG", "eegsensors", AttributeChannels<double>::TYPE_ID, 0);

		// setup the other sensor ports (with or without neuro sensors)
		if (headset->ShowNeuroChannels() == true)
		{
			// add all sensors including neuro sensors
			for (uint32 i = 0; i < numSensors; ++i)
			{
				Sensor* sensor = headset->GetSensor(i);
				
				const uint32 portIndex = i + 1;

				// set port name
				internalPortName.Format("y%i", portIndex);
				GetOutputPort(portIndex).Setup(sensor->GetName(), internalPortName.AsChar(), AttributeChannels<double>::TYPE_ID, portIndex);
			}

		}
		else
		{
			// add all sensors excluding neuro sensors (they come first in the list, so we can simply skip them)
			for (uint32 i = numNeuroSensors; i < numSensors; ++i)
			{
				Sensor* sensor = headset->GetSensor(i);

				const uint32 portIndex = i - numNeuroSensors + 1;

				// set port name
				internalPortName.Format("y%i", portIndex);
				GetOutputPort(portIndex).Setup(sensor->GetName(), internalPortName.AsChar(), AttributeChannels<double>::TYPE_ID, portIndex);
			}
		}
	}
	else // non-eeg device
	{
		// just create one port for each sensor
		InitOutputPorts(numSensors);

		// name the ports
		for (uint32 i = 0; i<numSensors; ++i)
		{
			// get access to the current sensor
			Sensor* sensor = device->GetSensor(i);

			// set port name
			internalPortName.Format("y%i", i+1); //y1 is first port
			GetOutputPort(i).Setup(sensor->GetName(), internalPortName.AsChar(), AttributeChannels<double>::TYPE_ID, i);
		}
	}
}


void DeviceInputNode::OnAttributesChanged()
{
	// baseclass call
	InputNode::OnAttributesChanged();

	// reset node if output mode was changed
	const bool rawOutputEnabled = GetBoolAttribute(ATTRIB_RAWOUTPUT);
	if (rawOutputEnabled != mRawOutputEnabled)
	{
		mRawOutputEnabled = rawOutputEnabled;
		ResetAsync();
	}
}


// find the first corresponding device
Device* DeviceInputNode::FindDevice()
{
	uint32 deviceID = GetInt32AttributeByName("ID");
	if (deviceID == 0)
		deviceID = 1;
	
	Device* device = GetDeviceManager()->FindDeviceByType(mDeviceType, deviceID-1); // 0-indexed
	return device;
}


uint32 DeviceInputNode::GetNumSensors() const 
{
	if (mCurrentDevice == NULL)
		return 0;

	return mCurrentDevice->GetNumSensors();
}


Sensor* DeviceInputNode::GetSensor(uint32 index)  
{
	if (mCurrentDevice == NULL)
		return NULL;

	return mCurrentDevice->GetSensor(index);
}


// color all output channels that do not belong to eeg sensors
void DeviceInputNode::UpdateOutputChannelColors()
{
	if (mCurrentDevice == NULL)
		return;

	// find the starting index so we can skip all EEG electrodes
	uint32 startIndex = 0;

	// skip the EEG channel
	startIndex = 1;

	// skip the individual EEG channels
	if (mCurrentDevice->GetBaseType() == BciDevice::BASE_TYPE_ID)
	{
		const BciDevice* headset = static_cast<const BciDevice*>(mCurrentDevice);
		if (headset->ShowNeuroChannels() == true)
			startIndex += headset->GetNumNeuroSensors();
	}

	// iterate over all remaining output channels and set a unique color
	const uint32 numOutputs = GetNumOutputPorts();
	CORE_ASSERT(startIndex <= numOutputs);

	uint32 uniqueIndex = 0;

	// for all channels of all ports.. set unique color
	for (uint32 p = startIndex; p < numOutputs; ++p)
	{
		Port& outPort = GetOutputPort(p);

		MultiChannel* channels = outPort.GetChannels();
		const uint32 numChannels = channels->GetNumChannels();
		for (uint32 c = 0; c < numChannels; ++c)
		{
			channels->GetChannel(c)->GetColor().SetUniqueColor(uniqueIndex);
			uniqueIndex++;
		}
	}
}


bool DeviceInputNode::IsSensorUsed(uint32 index)
{
	if (mCurrentDevice == NULL)
		return false;

	if (mCurrentDevice->GetBaseType() == BciDevice::BASE_TYPE_ID)
	{
		MultiChannel* eegChannels = GetOutputPort(0).GetChannels();
		//const uint32 numEegChannels = eegChannels->GetNumChannels();
		
		// index is ouf of range
		if (index >= GetNumSensors()+1)
			return false;

		// SPECIAL CASE CODE (kind of..)
		// EEG port is used -> check if the second node is a channel selection node and get the selected channels from there
		// Note: there can only be one connection if we want to avoid ambiguity
		if (GetOutputPort(0).GetNumConnection() == 1 && GetOutputPort(0).GetConnection(0)->GetTargetNode()->GetType() ==  ChannelSelectorNode::TYPE_ID)
		{
			const ChannelSelectorNode* selectorNode = static_cast<const ChannelSelectorNode*>(GetOutputPort(0).GetConnection(0)->GetTargetNode());
			const ChannelBase* channel = eegChannels->GetChannel(index);

			// channel is used by selector node
			if (selectorNode->IsChannelSelected(channel) == true)
				return true;
		}
		else if (GetOutputPort(0).GetNumConnection() > 0) // if no selector node is connected, consider all channels as 'used' 
		{
			return true;
		}

		// at last, check if the single-channel port has a connection
		return GetOutputPort(index + 1).HasConnection();
	}	
	else // device is not a neuro headset, has no EEG port
	{
		return GetOutputPort(index).HasConnection();
	}
}
