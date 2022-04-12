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
#include "BrainMasterDriver.h"

#ifdef INCLUDE_DEVICE_BRAINMASTER

// TODO: Switch this for branding
#if true
#define DEFAULT_DRIVER_ENABLED true
#else
#define DEFAULT_DRIVER_ENABLED false
#endif

using namespace Core;

// constructor
BrainMasterDriver::BrainMasterDriver() : DeviceDriver(DEFAULT_DRIVER_ENABLED), mSDK(*this), mDevice(0)
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

   // no device available/found
   if (!mDevice)
      return;

   //WIP
   mSDK.update();
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

   if (mSDK.find())
   {
      if (mSDK.connect())
      {
         mDevice = static_cast<Discovery20Device*>(CreateDevice(Discovery20Device::TYPE_ID));
         GetDeviceManager()->AddDeviceAsync(mDevice);
      }
   }
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

   // start data streaming, TODO: handle false
   if (!mSDK.start())
   {

   }
}

void BrainMasterDriver::Cleanup()
{
   mSDK.disconnect();
   mDevice = NULL;
}

void BrainMasterDriver::SetAutoDetectionEnabled(bool enable)
{
   DeviceDriver::SetAutoDetectionEnabled(enable);
}

// SDK CALLBACKS

void BrainMasterDriver::onLoadSDKSuccess(HMODULE h) 
{
   LogInfo("DISCOVERY20: Loaded bmrcm.dll successfully.");
}
void BrainMasterDriver::onLoadSDKFail()
{
   LogError("DISCOVERY20: Failed to load bmrcm.dll.");
}
void BrainMasterDriver::onDeviceFound(int32_t port)
{
   LogInfo("DISCOVERY20: Found device on COM%i.", port);
}
void BrainMasterDriver::onDeviceConnected()
{
   LogInfo("DISCOVERY20: Device connected.");
}
void BrainMasterDriver::onDeviceDisconnected()
{
   LogInfo("DISCOVERY20: Device disconnected.");
}
void BrainMasterDriver::onDeviceTimeout()
{
   LogError("DISCOVERY20: Device timeout.");
   if (mDevice)
      GetDeviceManager()->RemoveDeviceAsync(mDevice);
}
void BrainMasterDriver::onSyncStart(uint8_t c1, uint8_t c2)
{
   LogDebug("DISCOVERY20: Sync started on pair (%i, %i)", c1, c2);
}
void BrainMasterDriver::onSyncSuccess() 
{
   LogInfo("DISCOVERY20: Sync successful.");
}
void BrainMasterDriver::onSyncFail(uint8_t expected, uint8_t received)
{
   LogDebug("DISCOVERY20: Sync fail. Expected: %i Received: %i.", expected, received);
}
void BrainMasterDriver::onSyncLost()
{
   LogWarning("DISCOVERY20: Sync lost.");
}
void BrainMasterDriver::onFrame(const Discovery20::Frame& f, const Discovery20::Channels& c) 
{
   if (!mDevice)
      return;

   const uint32 numSensors = mDevice->GetNumInputSensors();
   if (numSensors > Discovery20::Channels::SIZE)
      return;

   // this requires identical ordering of sensors and frame channels on first 22
   for (uint32_t i = 0; i < numSensors; i++)
   {
      Sensor* s = mDevice->GetInputSensor(i);
      s->AddQueuedSample(c.data[i]);
   }
}
#endif
