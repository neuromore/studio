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

    // create default OSC address
    mOscPathPattern = "/neurosity/notion/*/raw";
    // mOscPathPattern = "/notion2/*/raw";

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
        auto num_args = message->GetNumArguments();
        if (message->GetNumArguments() != 13)
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

/*
void NotionDevice::SetDeviceId(uint32 deviceId)
{
    mDeviceId = deviceId;

    // update osc address
    mOscAddress.Format("/neurosity/notion/%i/raw");
}
*/

int32 NotionDevice::GetOscPathDeviceId(const Core::String& address) const
{
    int32 deviceId = 0;
    Array<String> elements = address.Split(StringCharacter::forwardSlash);

    if (elements.Size() > 3)
    {
        // pull elements[3] from the message and store it to a variable (likely string)
        
        // convert string to integer

        // store integer to deviceId
        deviceId = 1;

    }
    
    return deviceId;
}



#endif