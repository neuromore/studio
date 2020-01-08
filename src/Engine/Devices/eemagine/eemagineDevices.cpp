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
#include "eemagineDevices.h"
#include "../../EngineManager.h"
#include "../../Core/LogManager.h"

#ifdef INCLUDE_DEVICE_EEMAGINE

using namespace Core;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// base class
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

eemagineDevice::eemagineDevice(DeviceDriver* driver) : BciDevice(driver)
{
   // start in connected state
   mState = STATE_IDLE;
}

eemagineDevice::~eemagineDevice()
{
}

void eemagineDevice::CreateSensors()
{
   // create EEG sensors first
   BciDevice::CreateSensors();

   // iterate neuro sensors, set buffer size to 10s
   // TODO: Implement this more generic as new
   // SetBufferSizeInSeconds() in class Channel
   const uint32_t numSensors = GetNumNeuroSensors();
   for (uint32_t e = 0; e < numSensors; e++)
   {
      if (Sensor* sensor = GetNeuroSensor(e))
      {
         sensor->GetInput()->SetBufferSize((uint32_t)(GetSampleRate() * 10.0));
         sensor->GetOutput()->SetBufferSize((uint32_t)(GetSampleRate() * 10.0));
      }
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EE-214
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

eemagineEE214Device::eemagineEE214Device(DeviceDriver* driver) : eemagineDevice(driver)
{
   LogDetailedInfo("Constructing eemagine EE-214 ...");

   // create all sensors
   CreateSensors();
}

eemagineEE214Device::~eemagineEE214Device()
{
   LogDetailedInfo("Destructing eemagine EE-214 ...");
}

void eemagineEE214Device::CreateElectrodes()
{
   mElectrodes.Clear();

   mElectrodes.Reserve(32);
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp1"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fpz"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp2"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F7"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F3"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fz"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F4"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F8"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC5"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC1"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC2"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC6"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("A1")); // = M1 ?
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T7"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C3"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Cz"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C4"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T8"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("A2")); // = M2 ?
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP5"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP1"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP2"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP6"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P7"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P3"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Pz"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P4"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P8"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("POz"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O1"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Oz"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O2"));
}

void eemagineEE214Device::CreateSensors()
{
   eemagineDevice::CreateSensors();
}

int eemagineEE214Device::GetHwChannelIndexForElectrode(const char* electrode)
{
   // channelidx of the electrode in SDK calls to getSample(channelidx, sampleidx)

   if (String::SafeCompare(electrode, "Fp1") == 0)    return 0;
   if (String::SafeCompare(electrode, "Fpz") == 0)    return 1;
   if (String::SafeCompare(electrode, "Fp2") == 0)    return 2;
   if (String::SafeCompare(electrode, "F7") == 0)     return 3;
   if (String::SafeCompare(electrode, "F3") == 0)     return 4;
   if (String::SafeCompare(electrode, "Fz") == 0)     return 5;
   if (String::SafeCompare(electrode, "F4") == 0)     return 6;
   if (String::SafeCompare(electrode, "F8") == 0)     return 7;
   if (String::SafeCompare(electrode, "FC5") == 0)    return 8;
   if (String::SafeCompare(electrode, "FC1") == 0)    return 9;
   if (String::SafeCompare(electrode, "FC2") == 0)    return 10;
   if (String::SafeCompare(electrode, "FC6") == 0)    return 11;
   if (String::SafeCompare(electrode, "A1") == 0)     return 12;
   if (String::SafeCompare(electrode, "T7") == 0)     return 13;
   if (String::SafeCompare(electrode, "C3") == 0)     return 14;
   if (String::SafeCompare(electrode, "Cz") == 0)     return 15;
   if (String::SafeCompare(electrode, "C4") == 0)     return 16;
   if (String::SafeCompare(electrode, "T8") == 0)     return 17;
   if (String::SafeCompare(electrode, "A2") == 0)     return 18;
   if (String::SafeCompare(electrode, "CP5") == 0)    return 19;
   if (String::SafeCompare(electrode, "CP1") == 0)    return 20;
   if (String::SafeCompare(electrode, "CP2") == 0)    return 21;
   if (String::SafeCompare(electrode, "CP6") == 0)    return 22;
   if (String::SafeCompare(electrode, "P7") == 0)     return 23;
   if (String::SafeCompare(electrode, "P3") == 0)     return 24;
   if (String::SafeCompare(electrode, "Pz") == 0)     return 25;
   if (String::SafeCompare(electrode, "P4") == 0)     return 26;
   if (String::SafeCompare(electrode, "P8") == 0)     return 27;
   if (String::SafeCompare(electrode, "POz") == 0)    return 28;
   if (String::SafeCompare(electrode, "O1") == 0)     return 29;
   if (String::SafeCompare(electrode, "Oz") == 0)     return 30;
   if (String::SafeCompare(electrode, "O2") == 0)     return 31;
   return -1;
}

#endif
