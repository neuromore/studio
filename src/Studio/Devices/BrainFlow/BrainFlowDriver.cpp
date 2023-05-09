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
#include "BrainFlowDriver.h"
#include <Devices/BrainFlow/BrainFlowDevices.h>
#include <Devices/BrainFlow/BrainFlowNodes.h>

#ifdef INCLUDE_DEVICE_BRAINFLOW

using namespace Core;

BrainFlowDriver::BrainFlowDriver() : DeviceDriver(true) 
{
	CORE_EVENTMANAGER.AddEventHandler(this);
	AddSupportedDevice(BrainFlowDevice::TYPE_ID);
}

void BrainFlowDriver::DetectDevices() {
	if (!mIsEnabled)
		return;

	if (auto* device = GetDeviceManager()->FindDeviceByType(BrainFlowDevice::TYPE_ID, 0))
	{
		GetDeviceManager()->RemoveDeviceAsync(device);
	}
	else
	{
		device = CreateDevice(BoardIds::SYNTHETIC_BOARD, BrainFlowInputParams());
		GetDeviceManager()->AddDeviceAsync(device);
	}
}

const char* BrainFlowDriver::GetName() const 
{
	return "BrainFlow Driver"; 
}

uint32 BrainFlowDriver::GetType() const 
{
	return DeviceTypeIDs::DRIVER_TYPEID_BRAINFLOW;
}

bool BrainFlowDriver::Init() 
{
	return true; 
}

void BrainFlowDriver::OnDeviceAdded(Device* device)
{
	if (IsDeviceSupported(device->GetType()))
		device->Connect();
}

void BrainFlowDriver::OnRemoveDevice(Device* device)
{
	if (IsDeviceSupported(device->GetType()))
		device->Disconnect();
}


void BrainFlowDriver::OnRemoveNode(Graph* graph, Node* node)
{
	if (auto* brainFlowNode = dynamic_cast<BrainFlowNode*>(node))
		if (auto* currentDevice = brainFlowNode->GetCurrentDevice())
			GetDeviceManager()->RemoveDeviceAsync(currentDevice);
}

Device* BrainFlowDriver::CreateDevice(BoardIds boardId, BrainFlowInputParams params)
{
	return new BrainFlowDevice(boardId, std::move(params), this);
}
#endif
