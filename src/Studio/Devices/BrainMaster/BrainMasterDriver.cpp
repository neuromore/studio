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
#include <algorithm>
#include "BrainMasterDriver.h"
#include <Core/LogManager.h>

#ifdef INCLUDE_DEVICE_BRAINMASTER

// TODO: Switch this for branding
#if true
#define DEFAULT_DRIVER_ENABLED true
#else
#define DEFAULT_DRIVER_ENABLED false
#endif

using namespace Core;

// constructor
BrainMasterDriver::BrainMasterDriver() : DeviceDriver(DEFAULT_DRIVER_ENABLED), mDevice(0)
{
   LogDetailedInfo("Constructing BrainMaster driver ...");

   // add devices of this driver
   AddSupportedDevice(Discovery20Device::TYPE_ID);
}


// destructor
BrainMasterDriver::~BrainMasterDriver()
{
   LogDetailedInfo("Destructing BrainMaster driver ...");

   // remove event handler
   CORE_EVENTMANAGER.RemoveEventHandler(this);
}


// initialization
bool BrainMasterDriver::Init()
{
   LogInfo("Initializing BrainMaster driver...");

   // register event handler
   CORE_EVENTMANAGER.AddEventHandler(this);

   LogDetailedInfo("BrainMaster driver initialized ...");
   return true;
}


void BrainMasterDriver::Update(const Time& elapsed, const Time& delta)
{
   // driver not enabled
   if (!mIsEnabled)
      return;

   //TODO: If autodection is enabled, an attempt to discover the according
   // device could be made here. But careful: This is the mainthread.

   // no device available/found
   if (!mDevice)
      return;

   // TODO: Implement real data pulling from device
   // This simply adds 0.0 with given sample rate
   const double expectedsamples = mDevice->GetSampleRate() * delta.InSeconds();
   const uint32 numsamples = (uint32)expectedsamples;
   const uint32 numSensors = mDevice->GetNumNeuroSensors();
   for (uint32_t e = 0; e < numSensors; e++)
      if (Sensor* sensor = mDevice->GetNeuroSensor(e))
         for (uint32_t s = 0; s < numsamples; s++)
            sensor->AddQueuedSample(0.0);
}

Device* BrainMasterDriver::CreateDevice(uint32 deviceTypeID)
{
   CORE_ASSERT(IsDeviceSupported(deviceTypeID) == true);

   if (!IsDeviceSupported(deviceTypeID))
      return NULL;

   switch (deviceTypeID)
   {
   case Discovery20Device::TYPE_ID:  return new Discovery20Device(this);
   default: return NULL;
   }
}

void BrainMasterDriver::DetectDevices()
{
   // not enabled or already got one
   if (!mIsEnabled || mDevice)
      return;

   // TODO: This just claims one is connected and creates it
   // Replace with actual SDK call/detection/initiation
   mDevice = static_cast<Discovery20Device*>(CreateDevice(Discovery20Device::TYPE_ID));

   // add device to manager
   GetDeviceManager()->AddDeviceAsync(mDevice);
}

void BrainMasterDriver::OnRemoveDevice(Device* device)
{
   // not our device
   if (device != mDevice)
      return;

   // else cleanup
   Cleanup();
}

void BrainMasterDriver::OnDeviceAdded(Device* device)
{
   // not our device
   if (device != mDevice)
      return;
}

void BrainMasterDriver::Cleanup()
{
   mDevice = NULL;
}

void BrainMasterDriver::SetAutoDetectionEnabled(bool enable)
{
   DeviceDriver::SetAutoDetectionEnabled(enable);
}

#endif
