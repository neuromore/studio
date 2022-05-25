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

#include "../../EngineManager.h"
#include "../../Core/LogManager.h"
#include "./BrainAliveDevices.h"

#ifdef INCLUDE_DEVICE_BRAINALIVE

using namespace Core;
String data_1, data_2, data_3, data_4, data_5, data_6, data_7, data_8;

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
	mIR = new Sensor("IR", 250);
	mIR->GetChannel()->SetMinValue(-2000.0);
	mIR->GetChannel()->SetMaxValue(1996.1);
	mIR->GetChannel()->SetUnit("mm/s^2");
	AddSensor(mIR);
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

	mPID = new Sensor("PID", 250);
	mPID->GetChannel()->SetMinValue(0);
	mPID->GetChannel()->SetMaxValue(255);
	mAccLeftSensor->GetChannel()->SetUnit("mm/s^2");
	AddSensor(mPID);
}

// constructor
BrainAliveDevice::BrainAliveDevice(DeviceDriver* driver) : BrainAliveDeviceBase(driver)
{
	LogDetailedInfo("Constructing BrainAlive headset ...");
	// create all sensors
	CreateSensors();
}


// destructor
BrainAliveDevice::~BrainAliveDevice()
{
	LogDetailedInfo("Destructing BrainAlive headset ...");
}
void BrainAliveDevice::set_channel(Core::String data[]) {
  mParams.channel_1 = data[0];
  mParams.channel_2 = data[1];
  mParams.channel_3 = data[2];
  mParams.channel_4 = data[3];
  mParams.channel_5 = data[4];
  mParams.channel_6 = data[5];
  mParams.channel_7 = data[6];
  mParams.channel_8 = data[7];
}
void BrainAliveDevice::CreateElectrodes() {
	mElectrodes.Clear();
	mElectrodes.Reserve(8);
        mElectrodes.Add(
            GetEEGElectrodes()->GetElectrodeByID(mParams.channel_1));
        mElectrodes.Add(
            GetEEGElectrodes()->GetElectrodeByID(mParams.channel_2));
        mElectrodes.Add(
            GetEEGElectrodes()->GetElectrodeByID(mParams.channel_3));
        mElectrodes.Add(
            GetEEGElectrodes()->GetElectrodeByID(mParams.channel_4));
        mElectrodes.Add(
            GetEEGElectrodes()->GetElectrodeByID(mParams.channel_5));
        mElectrodes.Add(
            GetEEGElectrodes()->GetElectrodeByID(mParams.channel_6));
        mElectrodes.Add(
            GetEEGElectrodes()->GetElectrodeByID(mParams.channel_7));
        mElectrodes.Add(
            GetEEGElectrodes()->GetElectrodeByID(mParams.channel_8));
        }

#endif
