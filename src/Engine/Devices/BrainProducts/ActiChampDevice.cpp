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
#include "ActiChampDevice.h"
#include "../../EngineManager.h"
#include "../../Core/LogManager.h"

#ifdef INCLUDE_DEVICE_ACTICHAMP

using namespace Core;

ActiChampDevice::ActiChampDevice(DeviceDriver* driver) : BciDevice(driver)
{
	mState = STATE_IDLE;
	CreateSensors();
}


ActiChampDevice::~ActiChampDevice()
{
}


void ActiChampDevice::CreateSensors()
{
	BciDevice::CreateSensors();
}


void ActiChampDevice::CreateElectrodes()
{
	mElectrodes.Clear();

	mElectrodes.Reserve(33);
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp1"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fpz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp2"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F7"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F3"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F4"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F8"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FT9"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC5"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC1"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC2"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC6"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FT10"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T7"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C3"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Cz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C4"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T8"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("TP9"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP5"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP1"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP2"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP6"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("TP10"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P7"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P3"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Pz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P4"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P8"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O1"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Oz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O2"));
}

#endif
