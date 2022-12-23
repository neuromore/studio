/****************************************************************************
**
** Copyright 2022 neuromore co
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

#ifndef __NEUROMORE_NATUS_H
#define __NEUROMORE_NATUS_H

// include required headers
#include "../../BciDevice.h"

#ifdef INCLUDE_DEVICE_NATUS

// forward declaration
class NotionSystem;


class ENGINE_API NatusDevice : public BciDevice
{
    public:
        enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_NATUS };

        // CONSTRUCTOR AND DESTRUCTOR
        NatusDevice(DeviceDriver* driver = NULL);
        ~NatusDevice();

        Device* Clone() override                            { return new NatusDevice(); }

        // overloaded
        uint32 GetType() const override                     { return TYPE_ID; }
        double GetSampleRate() const override               { return 256; }
        const char* GetHardwareName() const override        { return "Natus Headbox"; }
        const char* GetUuid() const override                { return "24b358c9-7af2-4fab-9c3c-0d47635458ac"; }
        const char* GetTypeName() const override            { return "natus"; }
        double GetLatency() const override                  { return 0.1; }
        double GetExpectedJitter() const override           { return 0.2; }
        bool IsWireless() const override                    { return false; /* we do not know the connection quality of the muse; this removes the connection quality icon in the device manager */ }
        bool HasEegContactQualityIndicator() override       { return true; }
        static const char* GetRuleName()                    { return "DEVICE_Natus"; }

        int32 GetOscPathDeviceId(const Core::String& address) const override;
        Core::String GetOscPathDeviceString(const Core::String& address) const override;

        void CreateElectrodes() override;
        void CreateSensors() override;

        void SetDeviceId(uint32 deviceId) override;
        void SetDeviceString(const Core::String& deviceString) override;
        void ProcessMessage(OscMessageParser* message) override;
};

#endif

#endif