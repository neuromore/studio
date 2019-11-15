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

// include required headers
#include "EegDeviceNode.h"

// required for detecting used channels in combination with selector node
#include "ChannelSelectorNode.h"

using namespace Core;
	
// replaces DeviceInputNode::Init
// initialize the node
void EegDeviceNode::Init()
{
	// init base class first
	SPNode::Init();

	// has only one port for EEG multichannel
	InitOutputPorts(1);
	GetOutputPort(OUTPUTPORT_EEG).Setup("EEG", "eeg", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_EEG);

	// ATTRIBUTES

	// list of supported devices
	AttributeSettings* attribAllowedDevices = RegisterAttribute("Allowed Devices", "allowedDevices", "List of the allowed EEG devices by name or type ID. The first matching device will be used (leave empty to match all EEGs)", ATTRIBUTE_INTERFACETYPE_STRINGARRAY);
	attribAllowedDevices->SetDefaultValue( AttributeStringArray::Create("") );

	// TODO?: handle device IDs (= id to identify multiple devices of the same type)
}


// replaces DeviceInputNode::Start
// connect sensor channels to output ports
void EegDeviceNode::Start(const Time& elapsed)
{
	// Note: do not call SPNode::Start, as the output channels do not belong to device and must not be aligned by the node
	
	// device not found?
	if (mCurrentDevice == NULL)
		return;

	//const uint32 numSensors = mCurrentDevice->GetNumSensors();

	// connect EEG channels to multi channel port
	const BciDevice* headset = static_cast<const BciDevice*>(mCurrentDevice);
	const uint32 numNeuroSensors = headset->GetNumNeuroSensors();

	// rename the port with device name
	GetOutputPort(OUTPUTPORT_EEG).SetName(headset->GetName());

	MultiChannel* channels = GetOutputPort(OUTPUTPORT_EEG).GetChannels();

	// bundle neuro channels in one port (raw or synced channel)
	for (uint32 i = 0; i < numNeuroSensors; ++i)
		channels->AddChannel(headset->GetNeuroSensor(i)->GetChannel());
}


// replaces DeviceInputNode::OnAttributesChanged()
void EegDeviceNode::OnAttributesChanged()
{
	// nothing at all...
}


Device* EegDeviceNode::FindDevice()
{
	// go through string list attribute and return the first connected device the user is allowed to use
	Array<String> allowedDeviceList = GetStringArrayAttribute(ATTRIB_ALLOWEDDEVICES, Array<String>());

	// check all supplied entries
	uint32 numAllowed = allowedDeviceList.Size();
	if (numAllowed > 0)
	{
		for (uint32 i=0; i<numAllowed; ++i)
		{
			const uint32 numDevices = GetDeviceManager()->GetNumDevices();
			for (uint32 j=0; j<numDevices; ++j)
			{
				Device* device = GetDeviceManager()->GetDevice(j);

				// MUST be a BciDevice
				if (device->GetBaseType() != BciDevice::BASE_TYPE_ID)
					continue;
			
				// 1) try to match device ID (hex)
				int32 typeID;
				if (sscanf(allowedDeviceList[i].AsChar(), "%x", &typeID) == 1)
					if ((int32)device->GetType() == typeID)
						return device;

				// 2) try to match name (substring)
				String deviceName = device->GetHardwareName();
				deviceName.ToLower();
				allowedDeviceList[i].ToLower();
				if (deviceName.Contains(allowedDeviceList[i]) == true)
					return device;

				// nothing matched if we arrive here -> try next name
			}
		}
	}
	else
	{
		// in case the list is empty: allow all devices
		const uint32 numDevices = GetDeviceManager()->GetNumDevices();
		for (uint32 j=0; j<numDevices; ++j)
		{
			Device* device = GetDeviceManager()->GetDevice(j);

			// MUST be a BciDevice
			if (device->GetBaseType() == BciDevice::BASE_TYPE_ID)
				return device;
		}
	}

	// no matching device was found
	return NULL;
}


// replaces DeviceInputNode::GetNumSensors
uint32 EegDeviceNode::GetNumSensors() const 
{
	if (mCurrentDevice == NULL)
		return 0;
	
	BciDevice* device = static_cast<BciDevice*>(mCurrentDevice);

	return device->GetNumNeuroSensors();
}


// replaces DeviceInputNode::GetSensor
Sensor* EegDeviceNode::GetSensor(uint32 index)  
{
	if (mCurrentDevice == NULL)
		return NULL;


	// note: device can never be NOT a BCI
	BciDevice* device = static_cast<BciDevice*>(mCurrentDevice);

	return device->GetNeuroSensor(index);
}


// replaces DeviceInputNode::IsSensorUsed
bool EegDeviceNode::IsSensorUsed(uint32 index)
{
	if (mCurrentDevice == NULL)
		return false;

	MultiChannel* eegChannels = GetOutputPort(OUTPUTPORT_EEG).GetChannels();
		
	// EEG port is used -> check if the second node is a channel selection node and get the selected channels from there
	// Note: there can only be one connection if we want to avoid ambiguity
	if (GetOutputPort(OUTPUTPORT_EEG).GetNumConnection() == 1 && GetOutputPort(OUTPUTPORT_EEG).GetConnection(0)->GetTargetNode()->GetType() ==  ChannelSelectorNode::TYPE_ID)
	{
		const ChannelSelectorNode* selectorNode = static_cast<const ChannelSelectorNode*>(GetOutputPort(OUTPUTPORT_EEG).GetConnection(0)->GetTargetNode());
		const ChannelBase* channel = eegChannels->GetChannel(index);

		// channel is used by selector node
		if (selectorNode->IsChannelSelected(channel) == true)
			return true;
	}
	else if (GetOutputPort(OUTPUTPORT_EEG).GetNumConnection() > 0) // if no selector node is connected, consider all channels as 'used' 
	{
		return true;
	}

	return false;
}
