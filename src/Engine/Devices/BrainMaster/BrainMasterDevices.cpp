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
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Discovery with 20 channels
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Discovery20Device::Discovery20Device(DeviceDriver* driver) : DiscoveryDevice(driver)
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

   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp1"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp2"));

   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F7"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F3"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fz"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F4"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F8"));

   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T3"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C3"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Cz"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C4"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T4"));

   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T5"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P3"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Pz"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P4"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T6"));

   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O1"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O2"));

   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("A1"));
   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("A2"));

   mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("GND"));
}

void Discovery20Device::CreateSensors()
{
   DiscoveryDevice::CreateSensors();
}

#endif
