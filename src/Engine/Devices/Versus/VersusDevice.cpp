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
#include "VersusDevice.h"
#include "../../EngineManager.h"
#include "../../Core/LogManager.h"

#ifdef INCLUDE_DEVICE_SENSELABS_VERSUS

using namespace Core;


// constructor
VersusDevice::VersusDevice(DeviceDriver* driver) : BciDevice(driver)
{
	LogDetailedInfo("Constructing Versus headset ...");

	mState = STATE_IDLE;
	mPowerSupplyType = POWERSUPPLY_BATTERY;

	// create all sensors
	CreateSensors();
}


// destructor
VersusDevice::~VersusDevice()
{
	LogDetailedInfo("Destructing Versus headset ...");
}


// get the available electrodes of the neuro headset
void VersusDevice::CreateElectrodes()
{
	mElectrodes.Clear();

	mElectrodes.Reserve(5);
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Pz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C4"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Cz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C3"));
}

#endif
