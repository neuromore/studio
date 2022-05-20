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

// include precompiled header
#include <Engine/Precompiled.h>

// include required files
#include "BrainMasterDevices.h"
#include "../../EngineManager.h"
#include "../../Core/LogManager.h"

#ifdef INCLUDE_DEVICE_BRAINMASTER

using namespace Core;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// base class
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DiscoveryDevice::DiscoveryDevice(DeviceDriver* driver) : BciDevice(driver)
{
   // start in connected state
   mState = STATE_IDLE;
}

DiscoveryDevice::~DiscoveryDevice()
{
}

void DiscoveryDevice::CreateSensors()
{
   // create EEG electrode sensors first
   BciDevice::CreateSensors();

   // add custom aux inputs
   mAUX23 = AddSensor(Device::ESensorDirection::SENSOR_INPUT, "AUX23");
   mAUX24 = AddSensor(Device::ESensorDirection::SENSOR_INPUT, "AUX24");

   // use device default samplerate on them
   mAUX23->SetSampleRate(GetSampleRate());
   mAUX24->SetSampleRate(GetSampleRate());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Discovery with 20 channels
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Discovery20Device::Discovery20Device(DeviceDriver* driver) : DiscoveryDevice(driver), mSensorMode(0)
{
   LogDetailedInfo("Constructing Discovery with 20 channels ...");
   CreateSensors();

   // set powersupplytype to line
   SetPowerSupplyType(EPowerSupplyType::POWERSUPPLY_LINE);
}

Discovery20Device::~Discovery20Device()
{
   LogDetailedInfo("Destructing Discovery with 20 channels ...");
}

void Discovery20Device::CreateElectrodes()
{
   mElectrodes.Clear();
   mElectrodes.Reserve(22);

   // thist must exactly match the ordering of the device channels
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp1")); // ch01
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F3"));  // ch02
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C3"));  // ch03
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P3"));  // ch04
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O1"));  // ch05
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F7"));  // ch06
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T3"));  // ch07
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T5"));  // ch08
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fz"));  // ch09
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp2")); // ch10
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F4"));  // ch11
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C4"));  // ch12
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P4"));  // ch13
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O2"));  // ch14
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F8"));  // ch15
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T4"));  // ch16
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T6"));  // ch17
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Cz"));  // ch18
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Pz"));  // ch19
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("A2"));  // ch20 rather:A2-A1
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fpz")); // ch21
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Oz"));  // ch22

   //mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("AUX23")); // ch23
   //mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("AUX24")); // ch24
}

void Discovery20Device::CreateSensors()
{
   DiscoveryDevice::CreateSensors();

   // create additional sensors
   mSensorMode = AddSensor(Device::ESensorDirection::SENSOR_INPUT, "Mode");
   mSensorMode->SetSampleRate(this->GetSampleRate());

   // extend buffers so they can hold up to 60s of samples
   uint32_t numSensors = GetNumSensors();
   for (uint32_t i = 0; i < numSensors; i++)
   {
      Sensor* sensor = GetSensor(i);
      sensor->GetInput()->SetBufferSizeInSeconds(60.0);
      sensor->GetOutput()->SetBufferSizeInSeconds(60.0);
   }
}

double Discovery20Device::GetImpedance(uint32 neuroSensorIndex)
{
   if (neuroSensorIndex < GetNumNeuroSensors())
      return GetNeuroSensor(neuroSensorIndex)->GetChannel()->GetLastSample();

   return 0.0;
}

#endif
