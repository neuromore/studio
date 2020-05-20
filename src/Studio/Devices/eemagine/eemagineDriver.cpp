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
#include "eemagineDriver.h"
#include <Core/LogManager.h>

#ifdef INCLUDE_DEVICE_EEMAGINE

#include <eemagine/sdk/wrapper.cc>

using namespace Core;

// constructor
eemagineDriver::eemagineDriver() : DeviceDriver(false), mMode(EMode::MODE_IDLE), mDevice(NULL), mAmplifier(NULL), mStream(NULL), mLastDetect(0)
{
   LogDetailedInfo("Constructing eemagine driver ...");

   // add devices of this driver
   AddSupportedDevice(eemagineEE211Device::TYPE_ID);
   AddSupportedDevice(eemagineEE212Device::TYPE_ID);
   AddSupportedDevice(eemagineEE213Device::TYPE_ID);
   AddSupportedDevice(eemagineEE214Device::TYPE_ID);
   AddSupportedDevice(eemagineEE215Device::TYPE_ID);
   AddSupportedDevice(eemagineEE221Device::TYPE_ID);
   AddSupportedDevice(eemagineEE222Device::TYPE_ID);
   AddSupportedDevice(eemagineEE223Device::TYPE_ID);
   AddSupportedDevice(eemagineEE224Device::TYPE_ID);
   AddSupportedDevice(eemagineEE225Device::TYPE_ID);
   AddSupportedDevice(eemagineEE410Device::TYPE_ID);
   AddSupportedDevice(eemagineEE411Device::TYPE_ID);
   AddSupportedDevice(eemagineEE430Device::TYPE_ID);
}


// destructor
eemagineDriver::~eemagineDriver()
{
   LogDetailedInfo("Destructing eemagine driver ...");

   // remove event handler
   CORE_EVENTMANAGER.RemoveEventHandler(this);

   // cleanup
   Cleanup();
}


// initialization
bool eemagineDriver::Init()
{
   LogInfo("Initializing eemagine driver...");

   // register event handler
   CORE_EVENTMANAGER.AddEventHandler(this);

   LogDetailedInfo("eemagine driver initialized ...");
   return true;
}


void eemagineDriver::Update(const Time& elapsed, const Time& delta)
{
   // driver not enabled
   if (!mIsEnabled)
      return;

   // try find amplifier regularly if got none and autodetect is enabled (takes < 1ms, no need to offload)
   if (!mAmplifier && IsAutoDetectionEnabled() && (Core::Time::Now() - mLastDetect) > Core::Time(3.0))
      DetectDevices();

   // no device available/found
   if (!mAmplifier || !mStream || !mDevice)
      return;

   try
   {
      // try to read data from device stream
      mBuffer = mStream->getData();
   }
   catch (const eemagine::sdk::exceptions::notConnected&)
   {
      // log connection loss
      LogInfo("eemagine: Lost connection to device %s", mDevice->GetHardwareName());

      // remove device from manager (will delete device instance)
      GetDeviceManager()->RemoveDeviceAsync(mDevice);

      // do already? (also done in callback for remove)
      Cleanup();

      // don't go on
      return;
   }
   catch (const eemagine::sdk::exceptions::incorrectValue& e)
   {
      // log and don't go on
      LogWarning("eemagine: %s", e.what());
      return;
   }

   // how many samples and sensors
   const uint32_t numSamples = mBuffer.getSampleCount();
   const uint32_t numSensors = mDevice->GetNumNeuroSensors();

   // must keep the device alive in impedance mode (no samples)
   if (mMode == EMode::MODE_IMPEDANCE)
      mDevice->KeepAlive();

   // loop samples
   for (uint32_t s = 0; s < numSamples; s++)
   {
      // loop electrodes (neuro sensors)
      for (uint32_t e = 0; e < numSensors; e++)
      {
         if (Sensor* sensor = mDevice->GetNeuroSensor(e))
         {
            // get hw channel index for sensor from hardcoded mapping
            const int32 idx = sensor->GetHardwareChannel();

            // add sample for sensor
            if (idx >= 0 && idx < (int)mBuffer.getChannelCount())
            {
               // get value
               const double& v = mBuffer.getSample((uint32)idx, s);

               // add as sample or impedance
               if      (mMode == EMode::MODE_STREAM)    sensor->AddQueuedSample(v);
               else if (mMode == EMode::MODE_IMPEDANCE) sensor->SetImpedance(v * 0.001); // Ohm -> kOhm
            }
         }
      }
   }
}

Device* eemagineDriver::CreateDevice(uint32 deviceTypeID)
{
   CORE_ASSERT(IsDeviceSupported(deviceTypeID) == true);

   if (!IsDeviceSupported(deviceTypeID))
      return NULL;

   switch (deviceTypeID)
   {
   case eemagineEE211Device::TYPE_ID:  return new eemagineEE211Device(this);
   case eemagineEE212Device::TYPE_ID:  return new eemagineEE212Device(this);
   case eemagineEE213Device::TYPE_ID:  return new eemagineEE213Device(this);
   case eemagineEE214Device::TYPE_ID:  return new eemagineEE214Device(this);
   case eemagineEE215Device::TYPE_ID:  return new eemagineEE215Device(this);
   case eemagineEE221Device::TYPE_ID:  return new eemagineEE221Device(this);
   case eemagineEE222Device::TYPE_ID:  return new eemagineEE222Device(this);
   case eemagineEE223Device::TYPE_ID:  return new eemagineEE223Device(this);
   case eemagineEE224Device::TYPE_ID:  return new eemagineEE224Device(this);
   case eemagineEE225Device::TYPE_ID:  return new eemagineEE225Device(this);
   case eemagineEE410Device::TYPE_ID:  return new eemagineEE410Device(this);
   case eemagineEE411Device::TYPE_ID:  return new eemagineEE411Device(this);
   case eemagineEE430Device::TYPE_ID:  return new eemagineEE430Device(this);
   default: return NULL;
   }
}

void eemagineDriver::DetectDevices()
{
   // remember when last detection was
   mLastDetect = Core::Time::Now();

   // not enabled or already got one
   if (!mIsEnabled || mAmplifier)
      return;

   try
   {
      // get first amplifier or raise exception
      mAmplifier = mFactory.getAmplifier();

      // get type
      const std::string& type = mAmplifier->getType();

      // log found device
      LogInfo("eemagine: found amplifier of type %s, checking compatilibty...", type.c_str());

      // create device for type if supported
      if      (type == "EE211") { mDevice = static_cast<eemagineEE211Device*>(CreateDevice(eemagineEE211Device::TYPE_ID)); }
      else if (type == "EE212") { mDevice = static_cast<eemagineEE212Device*>(CreateDevice(eemagineEE212Device::TYPE_ID)); }
      else if (type == "EE213") { mDevice = static_cast<eemagineEE213Device*>(CreateDevice(eemagineEE213Device::TYPE_ID)); }
      else if (type == "EE214") { mDevice = static_cast<eemagineEE214Device*>(CreateDevice(eemagineEE214Device::TYPE_ID)); }
      else if (type == "EE215") { mDevice = static_cast<eemagineEE215Device*>(CreateDevice(eemagineEE215Device::TYPE_ID)); }
      else if (type == "EE221") { mDevice = static_cast<eemagineEE221Device*>(CreateDevice(eemagineEE221Device::TYPE_ID)); }
      else if (type == "EE222") { mDevice = static_cast<eemagineEE222Device*>(CreateDevice(eemagineEE222Device::TYPE_ID)); }
      else if (type == "EE223") { mDevice = static_cast<eemagineEE223Device*>(CreateDevice(eemagineEE223Device::TYPE_ID)); }
      else if (type == "EE224") { mDevice = static_cast<eemagineEE224Device*>(CreateDevice(eemagineEE224Device::TYPE_ID)); }
      else if (type == "EE225") { mDevice = static_cast<eemagineEE225Device*>(CreateDevice(eemagineEE225Device::TYPE_ID)); }
      else if (type == "EE410") { mDevice = static_cast<eemagineEE410Device*>(CreateDevice(eemagineEE410Device::TYPE_ID)); }
      else if (type == "EE411") { mDevice = static_cast<eemagineEE411Device*>(CreateDevice(eemagineEE411Device::TYPE_ID)); }
      else if (type == "EE430") { mDevice = static_cast<eemagineEE430Device*>(CreateDevice(eemagineEE430Device::TYPE_ID)); }

      // supported device
      if (mDevice)
      {
         // get defined sampling rate for this device and supported rates
         const int rateDefined = (int)mDevice->GetSampleRate();
         const auto rates = mAmplifier->getSamplingRatesAvailable();

         // defined rate must be supported (should be!)
         if (std::find(rates.begin(), rates.end(), rateDefined) != rates.end())
         {
            // start streaming
            StartStreaming();

            // add device to manager
            GetDeviceManager()->AddDeviceAsync(mDevice);
         }
         else
            LogInfo("eemagine: error requested sampling rate is not supported by device.");
      }
      else
         LogWarning("eemagine: error amplifier of type %s is not supported", type.c_str());
   }
   catch (const eemagine::sdk::exceptions::notFound&)
   {
      LogInfo("eemagine: device scan finished without results...");
   }
   catch (const std::exception&)
   {
      LogWarning("eemagine: unexpected error");
   }
}

void eemagineDriver::OnRemoveDevice(Device* device)
{
   // not our device
   if (device != mDevice)
      return;

   // else cleanup
   Cleanup();
}

void eemagineDriver::OnDeviceAdded(Device* device)
{
   // not our device
   if (device != mDevice)
      return;
}

void eemagineDriver::Cleanup()
{
   // delete sdk instances
   if (mStream)    delete mStream;
   if (mAmplifier) delete mAmplifier;

   // reset pointers
   mStream    = NULL;
   mAmplifier = NULL;
   mDevice    = NULL;
   mMode      = EMode::MODE_IDLE;
}

void eemagineDriver::StartTest(Device* device)
{
   if (device != mDevice || mMode == EMode::MODE_IMPEDANCE || !mAmplifier)
      return;

   // log
   LogInfo("eemagine: Switching to Impedance Mode");

   // set mode to impedance
   mMode = EMode::MODE_IMPEDANCE;

   // delete old stream if any
   if (mStream) 
      delete mStream;

   // open imepdance stream
   mStream = mAmplifier->OpenImpedanceStream();

   // get channels
   const auto channels = mStream->getChannelList();

   // build channel info for log
   LogInfo("eemagine: Found %i Impedance Channels:", channels.size());

   // log channel details
   for (auto channel : channels)
   {
      std::stringstream s;
      s << channel; // implemented in sdk
      LogInfo(s.str().c_str());
   }
}

void eemagineDriver::StartStreaming()
{
   if (mMode == EMode::MODE_STREAM || !mAmplifier)
      return;

   // log
   LogInfo("eemagine: Switching to Streaming Mode");

   // set mode to streaming
   mMode = EMode::MODE_STREAM;

   // delete old stream if any
   if (mStream)
      delete mStream;

   // open data stream on amplifier
   mStream = mAmplifier->OpenEegStream((int)mDevice->GetSampleRate());

   // get channels
   const auto channels = mStream->getChannelList();

   // build channel info for log
   LogInfo("eemagine: Found %i Streaming Channels:", channels.size());

   // log channel details
   for (auto channel : channels)
   {
      std::stringstream s;
      s << channel; // implemented in sdk
      LogInfo(s.str().c_str());
   }
}

void eemagineDriver::StopTest(Device* device)
{
   if (device != mDevice || mMode != EMode::MODE_IMPEDANCE)
      return;

   // set mode to idle
   mMode = EMode::MODE_IDLE;

   // delete old stream if any
   if (mStream)
      delete mStream;

   // reset ref
   mStream = NULL;

   // directly switch to streaming mode at the end of tests
   StartStreaming();
}

void eemagineDriver::StopStreaming()
{
   if (mMode != EMode::MODE_STREAM)
      return;

   // set mode to streaming
   mMode = EMode::MODE_IDLE;

   // delete old stream if any
   if (mStream)
      delete mStream;

   // reset ref
   mStream = NULL;
}

bool eemagineDriver::IsTestRunning(Device* device)
{
   if (device != mDevice)
      return false;

   return mMode == EMode::MODE_IMPEDANCE;
}

void eemagineDriver::SetAutoDetectionEnabled(bool enable)
{
   DeviceDriver::SetAutoDetectionEnabled(enable);

}
#endif
