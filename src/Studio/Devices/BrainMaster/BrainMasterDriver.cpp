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

#ifdef NEUROMORE_BRANDING_STARRBASE
#define DEFAULT_DRIVER_ENABLED true
#else
#define DEFAULT_DRIVER_ENABLED false
#endif

using namespace Core;

// constructor
BrainMasterDriver::BrainMasterDriver() : DeviceDriver(DEFAULT_DRIVER_ENABLED), mSDK(*this), mMode(EMode::MODE_IDLE), mDevice(0)
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

void BrainMasterDriver::StartTest(Device* device)
{
   if (device != mDevice || !mDevice || mMode == EMode::MODE_IMPEDANCE)
      return;

   // log
   LogInfo("DISCOVERY20: Switching to Impedance Mode");

   // set mode to impedance
   mMode = EMode::MODE_IMPEDANCE;

   // reset the device
   mDevice->Reset();
}

void BrainMasterDriver::StopTest(Device* device)
{
   if (device != mDevice || mMode != EMode::MODE_IMPEDANCE)
      return;

   // log
   LogInfo("DISCOVERY20: Switching to Streaming Mode");

   // set mode to idle
   mMode = EMode::MODE_STREAM;

   // reset the device
   mDevice->Reset();
}

bool BrainMasterDriver::IsTestRunning(Device* device)
{
   if (device != mDevice)
      return false;

   return mMode == EMode::MODE_IMPEDANCE;
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

   // must have according device with drivers installed
   if (!mSDK.find())
      return;

   // must have credentials
   if (mSerial.empty() || mCodeKey.empty() || mPassKey.empty())
      return;

   // try connect
   if (!mSDK.connect(mCodeKey.c_str(), mSerial.c_str(), mPassKey.c_str()))
      return;

   // success, create device and add it
   mDevice = static_cast<Discovery20Device*>(CreateDevice(Discovery20Device::TYPE_ID));
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

   // start data streaming
   if (!mSDK.start())
   {
      mSDK.disconnect();
      mMode = EMode::MODE_IDLE;
      return;
   }

   mMode = EMode::MODE_STREAM;
}

void BrainMasterDriver::Cleanup()
{
   mSDK.disconnect();
   mMode = EMode::MODE_IDLE;
   mDevice = NULL;
}

void BrainMasterDriver::SetAutoDetectionEnabled(bool enable)
{
   DeviceDriver::SetAutoDetectionEnabled(enable);
}

// SDK CALLBACKS

void BrainMasterDriver::onLoadSDKSuccess(Discovery20& d, HMODULE h)
{
   LogInfo("DISCOVERY20: Loaded bmrcm.dll successfully.");
}
void BrainMasterDriver::onLoadSDKFail(Discovery20& d)
{
   LogError("DISCOVERY20: Failed to load bmrcm.dll.");
}
void BrainMasterDriver::onDeviceFound(Discovery20& d, int32_t port)
{
   LogInfo("DISCOVERY20: Found device on COM%i.", port);
}
void BrainMasterDriver::onDeviceConnected(Discovery20& d)
{
   LogInfo("DISCOVERY20: Device connected.");
}
void BrainMasterDriver::onDeviceDisconnected(Discovery20& d)
{
   LogInfo("DISCOVERY20: Device disconnected.");
}
void BrainMasterDriver::onDeviceTimeout(Discovery20& d)
{
   LogError("DISCOVERY20: Device timeout.");
   if (mDevice)
      GetDeviceManager()->RemoveDeviceAsync(mDevice);
}
void BrainMasterDriver::onSyncStart(Discovery20& d, uint8_t c1, uint8_t c2)
{
   LogDebug("DISCOVERY20: Sync started on pair (%i, %i)", c1, c2);
}
void BrainMasterDriver::onSyncSuccess(Discovery20& d)
{
   LogInfo("DISCOVERY20: Sync successful.");
}
void BrainMasterDriver::onSyncFail(Discovery20& d, uint8_t expected, uint8_t received)
{
   LogDebug("DISCOVERY20: Sync fail. Expected: %i Received: %i.", expected, received);
}
void BrainMasterDriver::onSyncLost(Discovery20& d)
{
   LogWarning("DISCOVERY20: Sync lost.");
}
void BrainMasterDriver::onFrame(Discovery20& d, const Discovery20::Frame& f, const Discovery20::Channels& c)
{
   if (!mDevice)
      return;

   const uint32 numSensors = mDevice->GetNumInputSensors();
   if (numSensors != Discovery20::Channels::SIZE + 1) // + Mode
      return;

   if (mMode == EMode::MODE_IMPEDANCE)
   {
      const auto& impAct = d.getActiveImpedances();
      const auto& impRef = d.getReferenceImpedances();

      // TODO: Use impedance values here instead
      for (uint32_t i = 0; i < numSensors - 1; i++)
      {
         Sensor* s = mDevice->GetInputSensor(i);
         s->AddQueuedSample(impAct.data[i]);
      }

      // output mode (impedance=1.0)
      if (Sensor* sensor = mDevice->GetSensorMode())
         sensor->AddQueuedSample(1.0);
   }
   else
   {
      // channel values
      for (uint32_t i = 0; i < numSensors - 1; i++)
      {
         Sensor* s = mDevice->GetInputSensor(i);
         s->AddQueuedSample(c.data[i]);
      }

      // output mode (non-impedance=0.0)
      if (Sensor* sensor = mDevice->GetSensorMode())
         sensor->AddQueuedSample(0.0);
   }
}
#endif
