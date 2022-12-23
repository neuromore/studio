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
#include "NatusDevice.h"
#include "../../EngineManager.h"
#include "../../Core/LogManager.h"
#include <iostream>

#ifdef INCLUDE_DEVICE_NATUS

#define BUFFERSIZE_IN_SECONDS 60.0

using namespace Core;

//constructor
NatusDevice::NatusDevice(DeviceDriver* driver) : BciDevice(driver)
{
    LogDetailedInfo("Constructing Natus Device...");
    mOscPathPattern = "/natus/natus/*/*";
    CreateSensors();
}

// destructor
NatusDevice::~NatusDevice()
{
    LogDetailedInfo("Destructing Natus Device ...");
}

// get the available electrodes of the headset
void NatusDevice::CreateElectrodes()
{
    mElectrodes.Clear();
    mElectrodes.Reserve(32);

    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C3")); // 0
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C4"));  // 1
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Cz"));  // 2
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F3")); // 3
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F4")); // 4
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F7"));  // 5
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F8"));  // 6
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fz")); // 7
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp1")); // 8
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp2"));  // 9
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fpz"));  // 10
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O1")); // 11
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O2")); // 12
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P3"));  // 13
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P4"));  // 14
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Pz")); // 15
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T3")); // 16
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T4"));  // 17
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T5"));  // 18
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T6")); // 19
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("AUX1")); // 20
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("AUX2"));  // 21
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("AUX3"));  // 22
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("AUX4")); // 23
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("AUX5")); // 24
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("AUX6"));  // 25
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("AUX7"));  // 26
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("AUX8")); // 27
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("PG1")); // 28
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("PG2"));  // 29
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("A1"));  // 30
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("A2")); // 31
}

void NatusDevice::CreateSensors()
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

void NatusDevice::ProcessMessage(OscMessageParser* message)
{
    // do nothing is device was disabled or not enough sensors for code
    if (IsEnabled() == false || GetNumSensors() < 8)
        return;

    // raw 8 channel eeg
    if (message->MatchAddress("/natus/natus/*/raw") == true)
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
        const float v9 = it->AsFloat(); it++;
        const float v10 = it->AsFloat(); it++;
        const float v11 = it->AsFloat(); it++;
        const float v12 = it->AsFloat(); it++;
        const float v13 = it->AsFloat(); it++;
        const float v14 = it->AsFloat(); it++;
        const float v15 = it->AsFloat(); it++;
        const float v16 = it->AsFloat(); it++;
        const float v17 = it->AsFloat(); it++;
        const float v18 = it->AsFloat(); it++;
        const float v19 = it->AsFloat(); it++;
        const float v20 = it->AsFloat(); it++;
        const float v21 = it->AsFloat(); it++;
        const float v22 = it->AsFloat(); it++;
        const float v23 = it->AsFloat(); it++;
        const float v24 = it->AsFloat(); it++;
        const float v25 = it->AsFloat(); it++;
        const float v26 = it->AsFloat(); it++;
        const float v27 = it->AsFloat(); it++;
        const float v28 = it->AsFloat(); it++;
        const float v29 = it->AsFloat(); it++;
        const float v30 = it->AsFloat(); it++;
        const float v31 = it->AsFloat(); it++;
        const float v32 = it->AsFloat(); it++;
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
        GetSensor(8)->AddQueuedSample(v9); // v1 = CP3
        GetSensor(9)->AddQueuedSample(v10); // v2 = C3
        GetSensor(10)->AddQueuedSample(v11); // v3 = F5
        GetSensor(11)->AddQueuedSample(v12); // v4 = PO3
        GetSensor(12)->AddQueuedSample(v13); // v5 = PO4
        GetSensor(13)->AddQueuedSample(v14); // v6 = F6
        GetSensor(14)->AddQueuedSample(v15); // v7 = C4
        GetSensor(15)->AddQueuedSample(v16); // v8 = CP4
        GetSensor(16)->AddQueuedSample(v17); // v1 = CP3
        GetSensor(17)->AddQueuedSample(v18); // v2 = C3
        GetSensor(18)->AddQueuedSample(v19); // v3 = F5
        GetSensor(19)->AddQueuedSample(v20); // v4 = PO3
        GetSensor(20)->AddQueuedSample(v21); // v5 = PO4
        GetSensor(21)->AddQueuedSample(v22); // v6 = F6
        GetSensor(22)->AddQueuedSample(v23); // v7 = C4
        GetSensor(23)->AddQueuedSample(v24); // v8 = CP4
        GetSensor(24)->AddQueuedSample(v25); // v1 = CP3
        GetSensor(25)->AddQueuedSample(v26); // v2 = C3
        GetSensor(26)->AddQueuedSample(v27); // v3 = F5
        GetSensor(27)->AddQueuedSample(v28); // v4 = PO3
        GetSensor(28)->AddQueuedSample(v29); // v5 = PO4
        GetSensor(29)->AddQueuedSample(v30); // v6 = F6
        GetSensor(30)->AddQueuedSample(v31); // v7 = C4
        GetSensor(31)->AddQueuedSample(v32); // v8 = CP4
    }
    else if (message->MatchAddress("/natus/natus/*/info") == true)
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

    }
}

void NatusDevice::SetDeviceId(uint32 deviceId)
{
    mDeviceID = deviceId;
}

void NatusDevice::SetDeviceString(const Core::String& deviceString)
{
    Device::SetDeviceString(deviceString);
    mOscAddress = "/natus/natus/" + deviceString + "/*";
}

int32 NatusDevice::GetOscPathDeviceId(const Core::String& address) const
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

Core::String NatusDevice::GetOscPathDeviceString(const Core::String& address) const
{
    // e.g. returns 'local7cca794fb5f4675a69371e949b2'
    // for '/natus/natus/local7cca794fb5f4675a69371e949b2/raw'
    const Array<String> elements = address.Split(StringCharacter::forwardSlash);
    return (elements.Size() >= 4) ? elements[3] : "";
}

#endif