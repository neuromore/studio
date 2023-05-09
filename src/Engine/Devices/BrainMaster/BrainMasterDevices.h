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

#ifndef __NEUROMORE_BRAINMASTERDEVICES_H
#define __NEUROMORE_BRAINMASTERDEVICES_H

// include required headers
#include "../../Config.h"
#include "../../BciDevice.h"

#ifdef INCLUDE_DEVICE_BRAINMASTER

// discovery amplifier base class
class ENGINE_API DiscoveryDevice : public BciDevice
{
public:
   // constructor & destructor
   DiscoveryDevice(DeviceDriver* driver = NULL);
   virtual ~DiscoveryDevice();

   // overloaded
   double GetExpectedJitter() const override { return 0.1; }
   double GetSampleRate() const override { return 256; }
   bool IsWireless() const override { return false; }
   bool HasTestMode() const override { return false; }
   static const char* GetRuleName() { return "DEVICE_BrainMaster"; }
   void CreateSensors() override;

protected:
   Sensor* mFpz;
   Sensor* mOz;
   Sensor* mAUX23;
   Sensor* mAUX24;
};

//////////////////////////////////////////////////////////////////////////////////////////////

// discovery with 20 channels
class ENGINE_API Discovery20Device : public DiscoveryDevice
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_BRAINMASTER_DISCOVERY20 };

   // constructor & destructor
   Discovery20Device(DeviceDriver* driver = NULL);
   ~Discovery20Device();

   Device* Clone() override { return new Discovery20Device(); }

   // overloaded
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetUuid() const override { return "fd90f87b-5a78-4885-b6b3-7ed82f3cb85a"; }
   const char* GetTypeName() const override { return "discovery20"; }
   const char* GetHardwareName() const override { return "Discovery 20"; }

   void CreateElectrodes() override;
   void CreateSensors() override;

   bool HasTestMode() const override { return true; }
   void StartTest() override { mDeviceDriver->StartTest(this); }
   void StopTest() override { mDeviceDriver->StopTest(this); }
   bool IsTestRunning() override { return mDeviceDriver->IsTestRunning(this); }

   bool HasEegContactQualityIndicator() override { return IsTestRunning(); }
   double GetImpedance(uint32 neuroSensorIndex) override;


   inline Sensor* GetSensorMode() { return mSensorMode; }

   Sensor* mSensorMode;
};

#endif

#endif
