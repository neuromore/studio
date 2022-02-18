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

// the BrainMaster driver class
class BrainMasterDriver : public DeviceDriver, Core::EventHandler
{
public:
   enum { TYPE_ID = DeviceTypeIDs::DRIVER_TYPEID_BRAINMASTER };

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

   // autodetection of local devices
   bool HasAutoDetectionSupport() const override final { return true; }
   void SetAutoDetectionEnabled(bool enable = true) override;

private:
   void Cleanup();

   DiscoveryDevice* mDevice;
};

#endif

#endif
