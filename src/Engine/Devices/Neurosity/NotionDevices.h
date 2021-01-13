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

#ifndef __NEUROMORE_NOTION_H
#define __NEUROMORE_NOTION_H

// include required headers
#include "../../BciDevice.h"

#ifdef INCLUDE_DEVICE_NEUROSITY_NOTION

// forward declaration
class NotionSystem;


// the Neurosity Notion 1 and 2 headset class
class ENGINE_API NotionDevice : public BciDevice
{
    public:
        enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_NEUROSITY_NOTION };

        // There are two revisions of the Notion
        enum SubType {
            NOTION1 = 1,
            NOTION2 = 2
        };

        // CONSTRUCTOR AND DESTRUCTOR
        NotionDevice(DeviceDriver* driver = NULL);
        ~NotionDevice();

        Device* Clone() override                            { return new NotionDevice(); }

        // overloaded
        uint32 GetType() const override                     { return TYPE_ID; }
        double GetSampleRate() const override               { return 250; }
        const char* GetHardwareName() const override        { return "Neurosity Notion"; }
        const char* GetUuid() const override                { return "1cd1e229-b6ac-4d70-92c6-9e987f49e9cf"; }
        const char* GetTypeName() const override            { return "notion"; }
        double GetLatency() const override                  { return 0.1; }
        double GetExpectedJitter() const override           { return 0.2; }
        bool IsWireless() const override                    { return false; /* we do not know the connection quality of the muse; this removes the connection quality icon in the device manager */ }
        bool HasEegContactQualityIndicator() override       { return true; }
        static const char* GetRuleName()                    { return "DEVICE_NeurosityNotion"; }

        int32 GetOscPathDeviceId(const Core::String& address) const override;
        Core::String GetOscPathDeviceString(const Core::String& address) const override;

        void CreateElectrodes() override; 
        void CreateSensors() override;

        void SetDeviceId(uint32 deviceId) override;
        void SetDeviceString(const Core::String& deviceString) override;
        void ProcessMessage(OscMessageParser* message) override;

        inline SubType getSubType() { return mSubType; }
        inline void setSubType(const SubType type) { mSubType = type; }

   protected:
      SubType mSubType;
};

#endif

#endif