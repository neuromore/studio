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
#include "BrainquiryDevice.h"
#include "../../EngineManager.h"
#include "../../Core/LogManager.h"

#ifdef INCLUDE_DEVICE_BRAINQUIRY

using namespace Core;

// constructor
BrainquiryDevice::BrainquiryDevice(DeviceDriver* driver) : BciDevice(driver)
{
	LogDetailedInfo("Constructing Brainquiry PET 4.0 ...");

	mState = STATE_IDLE;
	mPowerSupplyType = POWERSUPPLY_BATTERY;

	// create all sensors
	CreateSensors();
}


// destructor
BrainquiryDevice::~BrainquiryDevice()
{
	LogDetailedInfo("Destructing Brainquiry PET 4.0 ...");
}


// get the available electrodes of the neuro headset
void BrainquiryDevice::CreateElectrodes()
{
	mElectrodes.Clear();
	mElectrodes.Reserve(4);

	mElectrodes.Add( GetEngine()->GetEEGElectrodes()->GetElectrodeByID("F3") ); 
	mElectrodes.Add( GetEngine()->GetEEGElectrodes()->GetElectrodeByID("F4") ); 
	mElectrodes.Add( GetEngine()->GetEEGElectrodes()->GetElectrodeByID("Fz") );
	mElectrodes.Add( GetEngine()->GetEEGElectrodes()->GetElectrodeByID("Pz") );
}



#endif
