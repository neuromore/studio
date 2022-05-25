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

// include precompiled header
#include <Engine/Precompiled.h>

// include required files
#include "CrownDevice.h"
#include "../../EngineManager.h"
#include "../../Core/LogManager.h"

#define BUFFERSIZE_IN_SECONDS 60.0

using namespace Core;

//constructor
CrownDevice::CrownDevice(DeviceDriver* driver) : BciDevice(driver)
{
    LogDetailedInfo("Constructing Crown headset...");
    mOscPathPattern = "/neurosity/notion/*/*";
    CreateSensors();
}

// destructor
CrownDevice::~CrownDevice()
{
    LogDetailedInfo("Destructing Crown headset ...");
}

// get the available electrodes of the headset
void CrownDevice::CreateElectrodes()
{
    mElectrodes.Clear();
    mElectrodes.Reserve(8);
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP3")); // 0
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C3"));  // 1
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F5"));  // 2
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("PO3")); // 3
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("PO4")); // 4
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F6"));  // 5
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C4"));  // 6
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP4")); // 7
}

void CrownDevice::CreateSensors()
{
    // create EEG sensors first
   BciDevice::CreateSensors();

   const uint32 numSensors = GetNumSensors();
   for (uint32_t i = 0; i < numSensors; i++)
   {
      // extend buffers so they can hold up to 60s of samples
      GetSensor(i)->GetInput()->SetBufferSizeInSeconds(BUFFERSIZE_IN_SECONDS);
      GetSensor(i)->GetOutput()->SetBufferSizeInSeconds(BUFFERSIZE_IN_SECONDS);
   }
}

void CrownDevice::ProcessMessage(OscMessageParser* message)
{
    // do nothing is device was disabled or not enough sensors for code
    if (IsEnabled() == false || GetNumSensors() < 8)
        return;

    // raw 8 channel eeg
    if (message->MatchAddress("/neurosity/notion/*/raw") == true)
    {
        // validate data layout
        if (strcmp(message->GetTypeTags(), "[ffffffff]sis") != 0)
           return;

        // get iterator
        auto it = message->GetOscMessage().ArgumentsBegin();

        // EEG
        it++; // array start
        const float v1 = it->AsFloat(); it++;
        const float v2 = it->AsFloat(); it++;
        const float v3 = it->AsFloat(); it++;
        const float v4 = it->AsFloat(); it++;
        const float v5 = it->AsFloat(); it++;
        const float v6 = it->AsFloat(); it++;
        const float v7 = it->AsFloat(); it++;
        const float v8 = it->AsFloat(); it++;
        it++; // array end

        // Misc/Unused
        const char* a1 = it->AsString(); it++;
        const int   a2 = it->AsInt32();  it++;
        const char* a3 = it->AsString(); it++;

        GetSensor(0)->AddQueuedSample(v1); // v1 = CP3
        GetSensor(1)->AddQueuedSample(v2); // v2 = C3
        GetSensor(2)->AddQueuedSample(v3); // v3 = F5
        GetSensor(3)->AddQueuedSample(v4); // v4 = PO3
        GetSensor(4)->AddQueuedSample(v5); // v5 = PO4
        GetSensor(5)->AddQueuedSample(v6); // v6 = F6
        GetSensor(6)->AddQueuedSample(v7); // v7 = C4
        GetSensor(7)->AddQueuedSample(v8); // v8 = CP4
    }
    else if (message->MatchAddress("/neurosity/notion/*/info") == true)
    {
        // validate data layout
        if (strcmp(message->GetTypeTags(), "ssssssiis") != 0)
            return;

        // parse them
        const char* deviceId       = 0; (*message) >> deviceId;
        const char* deviceNickname = 0; (*message) >> deviceNickname;
        const char* model          = 0; (*message) >> model;
        const char* modelName      = 0; (*message) >> modelName;
        const char* modelVersion   = 0; (*message) >> modelVersion;
        const char* manufacturer   = 0; (*message) >> manufacturer;
        int         samplingRate   = 0; (*message) >> samplingRate;
        int         numChannels    = 0; (*message) >> numChannels;
        const char* channelnames   = 0; (*message) >> channelnames;

        if (strcmp(modelVersion, "3") != 0 ||
            strcmp(modelName, "Crown") != 0 ||
            strcmp(channelnames, "CP3,C3,F5,PO3,PO4,F6,C4,CP4") != 0 ||
            samplingRate != GetSampleRate()) {
                LogError("Crown device is not properly configured");
        }
    }
}

void CrownDevice::SetDeviceId(uint32 deviceId)
{
    mDeviceID = deviceId;
}

void CrownDevice::SetDeviceString(const Core::String& deviceString)
{
    Device::SetDeviceString(deviceString);
    mOscAddress = "/neurosity/notion/" + deviceString + "/*";
}

int32 CrownDevice::GetOscPathDeviceId(const Core::String& address) const
{
    const Array<String> elements = address.Split(StringCharacter::forwardSlash);
    if (elements.Size() >= 4)
    {
        // should look like: 'local7cca794fb5f4675a69371e949b2'
        ::std::string s(elements[3].AsChar());

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

Core::String CrownDevice::GetOscPathDeviceString(const Core::String& address) const
{
    // e.g. returns 'local7cca794fb5f4675a69371e949b2'
    // for '/neurosity/notion/local7cca794fb5f4675a69371e949b2/raw'
    const Array<String> elements = address.Split(StringCharacter::forwardSlash);
    return (elements.Size() >= 4) ? elements[3] : "";
}
