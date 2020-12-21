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
#include "NotionDevices.h"
#include "../../EngineManager.h"
#include "../../Core/LogManager.h"

#ifdef INCLUDE_DEVICE_NEUROSITY_NOTION

using namespace Core;

//constructor
NotionDevice::NotionDevice(DeviceDriver* driver) : BciDevice(driver)
{
    LogDetailedInfo("Constructing Notion 2 headset...");
    mOscPathPattern = "/neurosity/notion/*/raw";
    CreateSensors();
}

// destructor
NotionDevice::~NotionDevice()
{
    LogDetailedInfo("Destructing Notion 2 headset ...");
}

// get the available electrodes of the headset
void NotionDevice::CreateElectrodes()
{
    mElectrodes.Clear();
    mElectrodes.Reserve(8);

    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP5"));
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F5"));
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C3"));
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP3"));
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP6"));
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F6"));
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C4"));
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP4"));
}

void NotionDevice::CreateSensors()
{
    // create EEG sensors first
    BciDevice::CreateSensors();
}

void NotionDevice::ProcessMessage(OscMessageParser* message)
{
    // do nothing is device was disabled
    if (IsEnabled() == false)
        return;

    // raw 8 channel eeg
    if (message->MatchAddress("/neurosity/notion/*/raw") == true)
    {
        if (message->GetNumArguments() < 8)
            return;

        // sensor order is: CP5, F5, C3, CP3, CP6, F6, C4, CP4
        for (uint32 i = 0; i < 8; ++i)
        {
            float rawValue; (*message) >> rawValue;

            // add sample to each channel
            GetSensor(i)->AddQueuedSample(rawValue);
        }
    }
}

void NotionDevice::SetDeviceId(uint32 deviceId)
{
    mDeviceID = deviceId;
}

void NotionDevice::SetDeviceString(const Core::String& deviceString)
{
    Device::SetDeviceString(deviceString);
    mOscAddress = "/neurosity/notion/" + deviceString + "/raw";
}

int32 NotionDevice::GetOscPathDeviceId(const Core::String& address) const
{
    const Array<String> elements = address.Split(StringCharacter::forwardSlash);
    if (elements.Size() >= 4)
    {
        // should look like: 'local7cca794fb5f4675a69371e949b2'
        ::std::string s = elements[3];

        // must have at least 4 symbols
        if (s.length() >= 4)
        {
            // get last 4 hex symbols (16-bit)
            s = s.substr(s.length() - 4, 4);

            // try hex to bin
            int32 x;
            std::stringstream ss;
            ss << std::hex << s;
            ss >> x;

            // x = 0 if non-hex symbol
            return x != 0 ? x : -1;
        }
    }

    // fail
    return -1;
}

Core::String NotionDevice::GetOscPathDeviceString(const Core::String& address) const
{
    // e.g. returns 'local7cca794fb5f4675a69371e949b2'
    // for '/neurosity/notion/local7cca794fb5f4675a69371e949b2/raw'
    const Array<String> elements = address.Split(StringCharacter::forwardSlash);
    return (elements.Size() >= 4) ? elements[3] : "";
}

#endif
