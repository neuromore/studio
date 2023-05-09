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

#ifndef __NEUROMORE_BRAINMASTERDRIVER_H
#define __NEUROMORE_BRAINMASTERDRIVER_H

// include required headers
#include <DeviceDriver.h>
#include <EngineManager.h>
#include <Devices/BrainMaster/BrainMasterDevices.h>

#ifdef INCLUDE_DEVICE_BRAINMASTER

#include "Discovery20.h"

// the BrainMaster driver class
class BrainMasterDriver : public DeviceDriver, Core::EventHandler, Discovery20::Callback
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DRIVER_TYPEID_BRAINMASTER };

   enum EMode
   {
      MODE_IDLE,        // init/default mode
      MODE_STREAM,      // normal EEG data stream
      MODE_IMPEDANCE    // impedance test 
   };

   // constructor & destructor
   BrainMasterDriver();
   virtual ~BrainMasterDriver();

   const char* GetName() const override { return "BrainMaster Devices"; }
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

   // autodetection of local devices
   bool HasAutoDetectionSupport() const override final { return true; }
   void SetAutoDetectionEnabled(bool enable = true) override;

   inline const std::string& GetCodeKey() const { return mCodeKey; }
   inline const std::string& GetSerial() const { return mSerial; }
   inline const std::string& GetPassKey() const { return mPassKey; }
   inline void SetCodeKey(const std::string& v) { mCodeKey = v; }
   inline void SetSerial(const std::string& v) { mSerial = v; }
   inline void SetPassKey(const std::string& v) { mPassKey = v; }

private:
   void Cleanup();

   virtual void onLoadSDKSuccess(Discovery20& d, HMODULE h) override;
   virtual void onLoadSDKFail(Discovery20& d) override;
   virtual void onDeviceFound(Discovery20& d, int32_t port)override;
   virtual void onDeviceConnected(Discovery20& d) override;
   virtual void onDeviceDisconnected(Discovery20& d) override;
   virtual void onDeviceTimeout(Discovery20& d) override;
   virtual void onSyncStart(Discovery20& d, uint8_t c1, uint8_t c2) override;
   virtual void onSyncSuccess(Discovery20& d) override;
   virtual void onSyncFail(Discovery20& d, uint8_t expected, uint8_t received) override;
   virtual void onSyncLost(Discovery20& d) override;
   virtual void onFrame(Discovery20& d, const Discovery20::Frame& f, const Discovery20::Channels& c) override;


   EMode              mMode;
   std::string        mCodeKey;
   std::string        mSerial;
   std::string        mPassKey;
   Discovery20        mSDK;
   Discovery20Device* mDevice;
   Core::Time         mLastDetect;
};

#endif

#endif
