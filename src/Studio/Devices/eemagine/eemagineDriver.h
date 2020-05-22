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

#ifndef __NEUROMORE_EEMAGINEDRIVER_H
#define __NEUROMORE_EEMAGINEDRIVER_H

// include required headers
#include <DeviceDriver.h>
#include <EngineManager.h>
#include <Devices/eemagine/eemagineDevices.h>

#ifdef INCLUDE_DEVICE_EEMAGINE

// Select between linking their dynamic lib at compile time (EEGO_SDK_BIND_STATIC)
// and loading their dynamic lib at runtime (EEGO_SDK_BIND_DYNAMIC).
#define EEGO_SDK_BIND_DYNAMIC

#if defined(NEUROMORE_PLATFORM_WINDOWS)
#define EEGO_LIB_FILENAME "eego-SDK.dll"
#elif defined(NEUROMORE_PLATFORM_LINUX)
#define EEGO_LIB_FILENAME "libeego-SDK.so"
#endif

#include <eemagine/sdk/factory.h>

// the eemagine driver class
class eemagineDriver : public DeviceDriver, Core::EventHandler
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DRIVER_TYPEID_EEMAGINE };

   enum EMode
   {
      MODE_IDLE,        // init/default mode
      MODE_STREAM,      // normal EEG data stream
      MODE_IMPEDANCE    // impedance test 
   };

   // constructor & destructor
   eemagineDriver();
   virtual ~eemagineDriver();

   const char* GetName() const override { return "eemagine Devices"; }
   uint32      GetType() const override { return TYPE_ID; }

   bool Init() override;
   void Update(const Core::Time& elapsed, const Core::Time& delta) override;
   void DetectDevices() override;
   Device* CreateDevice(uint32 deviceTypeID) override;
   void OnRemoveDevice(Device* device) override;
   void OnDeviceAdded(Device* device) override;

   // impedance test
   void StartTest(Device* device) override;
   void StopTest(Device* device) override;
   bool IsTestRunning(Device* device) override;

   void StartStreaming();
   void StopStreaming();

   // autodetection of local devices
   bool HasAutoDetectionSupport() const override final { return true; }
   void SetAutoDetectionEnabled(bool enable = true) override;

private:
   void Cleanup();

   EMode                     mMode;
   eemagineDevice*           mDevice;
   eemagine::sdk::buffer     mBuffer;
   eemagine::sdk::factory    mFactory;
   eemagine::sdk::amplifier* mAmplifier;
   eemagine::sdk::stream*    mStream;
   Core::Time                mLastDetect;
};

#endif

#endif
