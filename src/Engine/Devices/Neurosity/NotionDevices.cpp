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

//
// Neurosity Notion 1
//

//constructor
Notion1Device::Notion1Device(DeviceDriver* driver) : BciDevice(driver)
{
	LogDetailedInfo("Constructing Notion 1 headset...");
}

// destructor
Notion1Device::~Notion1Device()
{
	LogDetailedInfo("Destructing Notion 1 headset ...");
}

// get the available electrodes of the headset
void Notion1Device::CreateElectrodes()
{
    mElectrodes.Clear();
    mElectrodes.Reserve(8);

    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP6"));
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F6"));
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C4"));
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP4"));
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP3"));
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F5"));
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C3"));
    mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP5"));
}

// notion1 OSC streaming format
void Notion1Device::ProcessMessage(OscMessageParser* message)
{
    // do nothing is device was disabled
    if (IsEnabled() == false)
        return;

    // raw 8 channel eeg
    if (message->MatchAddress("/neurosity/notion/*/raw") == true)
    {
        if (message->GetNumArguments() != 8)
            return;

        // sensor order is: CP6, F6, C4, CP4, CP3, F5, C3, CP5
        for (uint32 i = 0; i < 8; ++i)
        {
            // float compressedValue; (*message) >> compressedValue;
            float rawValue; (*message) >> rawValue;

            // add sample to each channel
            GetSensor(i)->AddQueuedSample(rawValue);
        }
    }
}


//
// Neurosity Notion 2
//

//constructor
Notion2Device::Notion2Device(DeviceDriver* driver) : BciDevice(driver)
{
	LogDetailedInfo("Constructing Notion 2 headset...");
}

// destructor
Notion2Device::~Notion2Device()
{
	LogDetailedInfo("Destructing Notion 2 headset ...");
}

// get the available electrodes of the headset
void Notion2Device::CreateElectrodes()
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

// notion1 OSC streaming format
void Notion2Device::ProcessMessage(OscMessageParser* message)
{
    // do nothing is device was disabled
    if (IsEnabled() == false)
        return;

    // raw 8 channel eeg
    if (message->MatchAddress("/neurosity/notion/*/raw") == true)
    {
        if (message->GetNumArguments() != 8)
            return;

        // sensor order is: CP5, F5, C3, CP3, CP6, F6, C4, CP4
        for (uint32 i = 0; i < 8; ++i)
        {
            // float compressedValue; (*message) >> compressedValue;
            float rawValue; (*message) >> rawValue;

            // add sample to each channel
            GetSensor(i)->AddQueuedSample(rawValue);
        }
    }
}

#endif