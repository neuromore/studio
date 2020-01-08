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
   bool IsWireless() const override { return false; }
   bool HasTestMode() const override { return false; }
   static const char* GetRuleName() { return "DEVICE_eemagine"; }
   bool HasEegContactQualityIndicator() override { return false; }
   void CreateSensors() override;

   virtual int GetHwChannelIndexForElectrode(const char* electrode) = 0;

protected:
};

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
   double GetSampleRate() const override { return 500; }
   const char* GetUuid() const override { return "e0f71c80-40cc-42fb-959f-a9a7842613ff"; }
   const char* GetTypeName() const override { return "eegoEE214"; }
   const char* GetHardwareName() const override { return "eego EE-214"; }

   void CreateElectrodes() override;
   void CreateSensors() override;

   int GetHwChannelIndexForElectrode(const char* electrode) override;

private:
};

#endif

#endif
