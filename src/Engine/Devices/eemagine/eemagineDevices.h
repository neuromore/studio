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

protected:
   bool CreateDefaultElectrodes(const uint32_t count);
};

//////////////////////////////////////////////////////////////////////////////////////////////

// eego EE-211
class ENGINE_API eemagineEE211Device : public eemagineDevice
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_EEMAGINE_EE211 };

   // constructor & destructor
   eemagineEE211Device(DeviceDriver* driver = NULL);
   ~eemagineEE211Device();

   Device* Clone() override { return new eemagineEE211Device(); }

   // overloaded
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetUuid() const override { return "ac5bef84-7160-4ac6-88b6-7e96d0cbd11d"; }
   const char* GetTypeName() const override { return "eegoEE211"; }
   const char* GetHardwareName() const override { return "eego EE-211"; }

   void CreateElectrodes() override;
   void CreateSensors() override;

private:
};

//////////////////////////////////////////////////////////////////////////////////////////////

// eego EE-212
class ENGINE_API eemagineEE212Device : public eemagineDevice
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_EEMAGINE_EE212 };

   // constructor & destructor
   eemagineEE212Device(DeviceDriver* driver = NULL);
   ~eemagineEE212Device();

   Device* Clone() override { return new eemagineEE212Device(); }

   // overloaded
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetUuid() const override { return "c2559b7e-a7f3-4cbf-a19f-ae413fcbf24d"; }
   const char* GetTypeName() const override { return "eegoEE212"; }
   const char* GetHardwareName() const override { return "eego EE-212"; }

   void CreateElectrodes() override;
   void CreateSensors() override;

private:
};

//////////////////////////////////////////////////////////////////////////////////////////////

// eego EE-213
class ENGINE_API eemagineEE213Device : public eemagineDevice
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_EEMAGINE_EE213 };

   // constructor & destructor
   eemagineEE213Device(DeviceDriver* driver = NULL);
   ~eemagineEE213Device();

   Device* Clone() override { return new eemagineEE213Device(); }

   // overloaded
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetUuid() const override { return "a33b82a0-86ff-4990-a594-26dacbf15e71"; }
   const char* GetTypeName() const override { return "eegoEE213"; }
   const char* GetHardwareName() const override { return "eego EE-213"; }

   void CreateElectrodes() override;
   void CreateSensors() override;

private:
};

//////////////////////////////////////////////////////////////////////////////////////////////

// eego EE-214
class ENGINE_API eemagineEE214Device : public eemagineDevice
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_EEMAGINE_EE214 };

   // constructor & destructor
   eemagineEE214Device(DeviceDriver* driver = NULL);
   ~eemagineEE214Device();

   Device* Clone() override { return new eemagineEE214Device(); }

   // overloaded
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetUuid() const override { return "e0f71c80-40cc-42fb-959f-a9a7842613ff"; }
   const char* GetTypeName() const override { return "eegoEE214"; }
   const char* GetHardwareName() const override { return "eego EE-214"; }

   void CreateElectrodes() override;
   void CreateSensors() override;

private:
};

//////////////////////////////////////////////////////////////////////////////////////////////

// eego EE-215
class ENGINE_API eemagineEE215Device : public eemagineDevice
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_EEMAGINE_EE215 };

   // constructor & destructor
   eemagineEE215Device(DeviceDriver* driver = NULL);
   ~eemagineEE215Device();

   Device* Clone() override { return new eemagineEE215Device(); }

   // overloaded
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetUuid() const override { return "d4100456-42cb-4b33-ba38-5c26952beebc"; }
   const char* GetTypeName() const override { return "eegoEE215"; }
   const char* GetHardwareName() const override { return "eego EE-215"; }

   void CreateElectrodes() override;
   void CreateSensors() override;

private:
};

//////////////////////////////////////////////////////////////////////////////////////////////

// eego EE-221
class ENGINE_API eemagineEE221Device : public eemagineDevice
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_EEMAGINE_EE221 };

   // constructor & destructor
   eemagineEE221Device(DeviceDriver* driver = NULL);
   ~eemagineEE221Device();

   Device* Clone() override { return new eemagineEE221Device(); }

   // overloaded
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetUuid() const override { return "b2b765cd-2dca-4762-9764-ca43c5983b75"; }
   const char* GetTypeName() const override { return "eegoEE221"; }
   const char* GetHardwareName() const override { return "eego EE-221"; }

   void CreateElectrodes() override;
   void CreateSensors() override;

private:
};

//////////////////////////////////////////////////////////////////////////////////////////////

// eego EE-222
class ENGINE_API eemagineEE222Device : public eemagineDevice
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_EEMAGINE_EE222 };

   // constructor & destructor
   eemagineEE222Device(DeviceDriver* driver = NULL);
   ~eemagineEE222Device();

   Device* Clone() override { return new eemagineEE222Device(); }

   // overloaded
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetUuid() const override { return "8e3643d9-1d64-408e-b990-fd0bc2684a85"; }
   const char* GetTypeName() const override { return "eegoEE222"; }
   const char* GetHardwareName() const override { return "eego EE-222"; }

   void CreateElectrodes() override;
   void CreateSensors() override;

private:
};

//////////////////////////////////////////////////////////////////////////////////////////////

// eego EE-223
class ENGINE_API eemagineEE223Device : public eemagineDevice
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_EEMAGINE_EE223 };

   // constructor & destructor
   eemagineEE223Device(DeviceDriver* driver = NULL);
   ~eemagineEE223Device();

   Device* Clone() override { return new eemagineEE223Device(); }

   // overloaded
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetUuid() const override { return "bbdc6b4e-02d7-42d5-8ead-d5f2832d470f"; }
   const char* GetTypeName() const override { return "eegoEE223"; }
   const char* GetHardwareName() const override { return "eego EE-223"; }

   void CreateElectrodes() override;
   void CreateSensors() override;

private:
};

//////////////////////////////////////////////////////////////////////////////////////////////

// eego EE-224
class ENGINE_API eemagineEE224Device : public eemagineDevice
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_EEMAGINE_EE224 };

   // constructor & destructor
   eemagineEE224Device(DeviceDriver* driver = NULL);
   ~eemagineEE224Device();

   Device* Clone() override { return new eemagineEE224Device(); }

   // overloaded
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetUuid() const override { return "bbdc6b4e-02d7-42d5-8ead-d5f2832d470f"; }
   const char* GetTypeName() const override { return "eegoEE224"; }
   const char* GetHardwareName() const override { return "eego EE-224"; }

   void CreateElectrodes() override;
   void CreateSensors() override;

private:
};

//////////////////////////////////////////////////////////////////////////////////////////////

// eego EE-225
class ENGINE_API eemagineEE225Device : public eemagineDevice
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_EEMAGINE_EE225 };

   // constructor & destructor
   eemagineEE225Device(DeviceDriver* driver = NULL);
   ~eemagineEE225Device();

   Device* Clone() override { return new eemagineEE225Device(); }

   // overloaded
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetUuid() const override { return "a7b22c0b-bcb1-47ab-863c-5693ac13110a"; }
   const char* GetTypeName() const override { return "eegoEE225"; }
   const char* GetHardwareName() const override { return "eego EE-225"; }

   void CreateElectrodes() override;
   void CreateSensors() override;

private:
};

//////////////////////////////////////////////////////////////////////////////////////////////

// eego EE-410
class ENGINE_API eemagineEE410Device : public eemagineDevice
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_EEMAGINE_EE410 };

   // constructor & destructor
   eemagineEE410Device(DeviceDriver* driver = NULL);
   ~eemagineEE410Device();

   Device* Clone() override { return new eemagineEE410Device(); }

   // overloaded
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetUuid() const override { return "0f21e1c2-3efa-435b-8b24-f2e9dfc090f8"; }
   const char* GetTypeName() const override { return "eegoEE410"; }
   const char* GetHardwareName() const override { return "eego EE-410"; }

   void CreateElectrodes() override;
   void CreateSensors() override;

private:
};

//////////////////////////////////////////////////////////////////////////////////////////////

// eego EE-411
class ENGINE_API eemagineEE411Device : public eemagineDevice
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_EEMAGINE_EE411 };

   // constructor & destructor
   eemagineEE411Device(DeviceDriver* driver = NULL);
   ~eemagineEE411Device();

   Device* Clone() override { return new eemagineEE411Device(); }

   // overloaded
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetUuid() const override { return "bfbffef7-7d7b-4043-95f3-4262ddde3687"; }
   const char* GetTypeName() const override { return "eegoEE411"; }
   const char* GetHardwareName() const override { return "eego EE-411"; }

   void CreateElectrodes() override;
   void CreateSensors() override;

private:
};

//////////////////////////////////////////////////////////////////////////////////////////////

// eego EE-430
class ENGINE_API eemagineEE430Device : public eemagineDevice
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_EEMAGINE_EE430 };

   // constructor & destructor
   eemagineEE430Device(DeviceDriver* driver = NULL);
   ~eemagineEE430Device();

   Device* Clone() override { return new eemagineEE430Device(); }

   // overloaded
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetUuid() const override { return "cc36eab0-848f-4504-a527-718e091dbd89"; }
   const char* GetTypeName() const override { return "eegoEE430"; }
   const char* GetHardwareName() const override { return "eego EE-430"; }

   void CreateElectrodes() override;
   void CreateSensors() override;

private:
};
#endif

#endif
