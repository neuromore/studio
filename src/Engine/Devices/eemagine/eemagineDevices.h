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

#ifndef __NEUROMORE_EEMAGINEDEVICE_H
#define __NEUROMORE_EEMAGINEDEVICE_H

// include required headers
#include "../../Config.h"
#include "../../BciDevice.h"

#ifdef INCLUDE_DEVICE_EEMAGINE

// eego amplifier base class
class ENGINE_API eemagineDevice : public BciDevice
{
public:
   // constructor & destructor
   eemagineDevice(DeviceDriver* driver = NULL);
   virtual ~eemagineDevice();

   // overloaded
   double GetExpectedJitter() const override { return 0.2; }
   double GetSampleRate() const override { return 500; }
   bool IsWireless() const override { return false; }
   bool HasTestMode() const override { return true; }
   static const char* GetRuleName() { return "DEVICE_eemagine"; }
   void CreateSensors() override;

   void StartTest() override { mDeviceDriver->StartTest(this); }
   void StopTest() override { mDeviceDriver->StopTest(this); }
   bool IsTestRunning() override { return mDeviceDriver->IsTestRunning(this); }

   // impedance test provides contact quality
   bool HasEegContactQualityIndicator() override { return IsTestRunning(); }
   double GetImpedance(uint32 neuroSensorIndex) override;
};

//////////////////////////////////////////////////////////////////////////////////////////////

// eego with 8 channels
class ENGINE_API eemagine8Device : public eemagineDevice
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_EEMAGINE_8CH };

   // constructor & destructor
   eemagine8Device(DeviceDriver* driver = NULL);
   ~eemagine8Device();

   Device* Clone() override { return new eemagine8Device(); }

   // overloaded
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetUuid() const override { return "b3ee17de-4dea-46a8-a9bf-ac3ea9d28fb2"; }
   const char* GetTypeName() const override { return "eego8ch"; }
   const char* GetHardwareName() const override { return "eego 8ch"; }

   void CreateElectrodes() override;
   void CreateSensors() override;
};

// eego with 32 channels
class ENGINE_API eemagine32Device : public eemagineDevice
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_EEMAGINE_32CH };

   // constructor & destructor
   eemagine32Device(DeviceDriver* driver = NULL);
   ~eemagine32Device();

   Device* Clone() override { return new eemagine32Device(); }

   // overloaded
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetUuid() const override { return "c1a40e09-adcc-41d5-945e-5abfe249dcb0"; }
   const char* GetTypeName() const override { return "eego32ch"; }
   const char* GetHardwareName() const override { return "eego 32ch"; }

   void CreateElectrodes() override;
   void CreateSensors() override;
};

// eego with 64 channels
class ENGINE_API eemagine64Device : public eemagineDevice
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_EEMAGINE_64CH };

   // constructor & destructor
   eemagine64Device(DeviceDriver* driver = NULL);
   ~eemagine64Device();

   Device* Clone() override { return new eemagine64Device(); }

   // overloaded
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetUuid() const override { return "9d07803b-f913-44e9-bc3a-52bfb5cbe5f2"; }
   const char* GetTypeName() const override { return "eego64ch"; }
   const char* GetHardwareName() const override { return "eego 64ch"; }

   void CreateElectrodes() override;
   void CreateSensors() override;
};

#endif

#endif
