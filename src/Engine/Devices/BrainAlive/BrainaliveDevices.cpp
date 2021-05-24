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

#include "../../EngineManager.h"
#include "../../Core/LogManager.h"
#include "BrainaliveDevices.h"

#ifdef INCLUDE_DEVICE_BRAINALIVE

using namespace Core;
extern String data_1, data_2, data_3, data_4, data_5, data_6, data_7, data_8;

//base class commons
void BrainAliveDeviceBase::CreateSensors()
{
	BciDevice::CreateSensors();
	mRed = new Sensor("fNIRS", 250);
	mRed->GetChannel()->SetMinValue(-2000.0);
	mRed->GetChannel()->SetMaxValue(1996.1);
	mRed->GetChannel()->SetUnit("mm/s^2");
	AddSensor(mRed);
	mRed = new Sensor("Red", 250);
	mRed->GetChannel()->SetMinValue(-2000.0);
	mRed->GetChannel()->SetMaxValue(1996.1);
	mRed->GetChannel()->SetUnit("mm/s^2");
	AddSensor(mRed);
	mIr = new Sensor("IR", 250);
	mIr->GetChannel()->SetMinValue(-2000.0);
	mIr->GetChannel()->SetMaxValue(1996.1);
	mIr->GetChannel()->SetUnit("mm/s^2");
	AddSensor(mIr);
	mGreen = new Sensor("Green", 250);
	mGreen->GetChannel()->SetMinValue(-2000.0);
	mGreen->GetChannel()->SetMaxValue(1996.1);
	mGreen->GetChannel()->SetUnit("mm/s^2");
	AddSensor(mGreen);
	

	Axl = new Sensor("AXL", 250);
	Axl->GetChannel()->SetMinValue(-2000.0);
	Axl->GetChannel()->SetMaxValue(1996.1);
	Axl->GetChannel()->SetUnit("mm/s^2");
	AddSensor(Axl);

	mAccForwardSensor = new Sensor("X", 250);
	mAccForwardSensor->GetChannel()->SetMinValue(-2000.0);
	mAccForwardSensor->GetChannel()->SetMaxValue(1996.1);
	mAccForwardSensor->GetChannel()->SetUnit("mm/s^2");
	AddSensor(mAccForwardSensor);

	mAccUpSensor = new Sensor("Y", 250);
	mAccUpSensor->GetChannel()->SetMinValue(-2000.0);
	mAccUpSensor->GetChannel()->SetMaxValue(1996.1);
	mAccUpSensor->GetChannel()->SetUnit("mm/s^2");
	AddSensor(mAccUpSensor);

	mAccLeftSensor = new Sensor("Z", 250);
	mAccLeftSensor->GetChannel()->SetMinValue(-2000.0);
	mAccLeftSensor->GetChannel()->SetMaxValue(1996.1);
	mAccLeftSensor->GetChannel()->SetUnit("mm/s^2");
	AddSensor(mAccLeftSensor);
}


//
// OpenBCI without Daisy module
//

// constructor
BrainAliveDevice::BrainAliveDevice(DeviceDriver* driver) : BrainAliveDeviceBase(driver)
{
	LogDetailedInfo("Constructing Brainalive headset ...");

	// create all sensors
	CreateSensors();
}


// destructor
BrainAliveDevice::~BrainAliveDevice()
{
	LogDetailedInfo("Destructing Brainalive headset ...");
}


// get the available electrodes of the neuro headset
void BrainAliveDevice::CreateElectrodes()
{
	mElectrodes.Clear();
	mElectrodes.Reserve(8);
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID(data_1));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID(data_2));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID(data_3));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID(data_4));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID(data_5));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID(data_6));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID(data_7));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID(data_8));
}


//
// OpenBCI + Daisy module
//

// constructor
BrainAliveDaisyDevice::BrainAliveDaisyDevice(DeviceDriver* driver) : BrainAliveDeviceBase(driver)
{
	LogDetailedInfo("Constructing BrainAlive + Daisy device ...");

	// create all sensors
	CreateSensors();
}


// destructor
BrainAliveDaisyDevice::~BrainAliveDaisyDevice()
{
	LogDetailedInfo("Destructing BrainAlive + Daisy device...");
}


// default: ultracortex positions
void BrainAliveDaisyDevice::CreateElectrodes()
{
	mElectrodes.Clear();
	mElectrodes.Reserve(16);

	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P3"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P4"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Oz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T5"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T6"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F3"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F4"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T3"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T4"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C3"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C4"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Cz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O1"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O2"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Pz"));
}


#endif
