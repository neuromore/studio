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

#include "../../Config.h"
#include "../../Device.h"
#include "../../DeviceDriver.h"
#include "TobiiEyeXDevice.h"


#ifdef INCLUDE_DEVICE_TOBIIEYEX

// constructor and destructor
TobiiEyeXDevice::TobiiEyeXDevice(DeviceDriver* driver) : Device(driver)
{

	mSampleRate = 55;
	// create sensors
	CreateSensors();

	// start in idle state
	mState = STATE_IDLE;
	mPowerSupplyType = POWERSUPPLY_BATTERY;
}


TobiiEyeXDevice::~TobiiEyeXDevice()
{

}


void TobiiEyeXDevice::CreateSensors()
{
	mLeftEyeXSensor = new Sensor("Left Eye X", mSampleRate);
	mLeftEyeXSensor->GetChannel()->SetBufferSize(5 * mSampleRate);
	mLeftEyeXSensor->SetDriftCorrectionEnabled(false);
	AddSensor(mLeftEyeXSensor, SENSOR_INPUT);

	mLeftEyeYSensor = new Sensor("Left Eye Y", mSampleRate);
	mLeftEyeYSensor->GetChannel()->SetBufferSize(5 * mSampleRate);
	mLeftEyeYSensor->SetDriftCorrectionEnabled(false);
	AddSensor(mLeftEyeYSensor, SENSOR_INPUT);

	mLeftEyeZSensor = new Sensor("Left Eye Z", mSampleRate);
	mLeftEyeZSensor->GetChannel()->SetBufferSize(5 * mSampleRate);
	mLeftEyeZSensor->SetDriftCorrectionEnabled(false);
	AddSensor(mLeftEyeZSensor, SENSOR_INPUT);

	mLeftEyeXNormalizedSensor = new Sensor("Left Eye X Normalized", mSampleRate);
	mLeftEyeXNormalizedSensor->GetChannel()->SetBufferSize(5 * mSampleRate);
	mLeftEyeXNormalizedSensor->SetDriftCorrectionEnabled(false);
	AddSensor(mLeftEyeXNormalizedSensor, SENSOR_INPUT);

	mLeftEyeYNormalizedSensor = new Sensor("Left Eye Y Normalized", mSampleRate);
	mLeftEyeYNormalizedSensor->GetChannel()->SetBufferSize(5 * mSampleRate);
	mLeftEyeYNormalizedSensor->SetDriftCorrectionEnabled(false);
	AddSensor(mLeftEyeYNormalizedSensor, SENSOR_INPUT);

	mLeftEyeZNormalizedSensor = new Sensor("Left Eye Z Normalized", mSampleRate);
	mLeftEyeZNormalizedSensor->GetChannel()->SetBufferSize(5 * mSampleRate);
	mLeftEyeZNormalizedSensor->SetDriftCorrectionEnabled(false);
	AddSensor(mLeftEyeZNormalizedSensor, SENSOR_INPUT);

	mRightEyeXSensor = new Sensor("Right Eye X", mSampleRate);
	mRightEyeXSensor->GetChannel()->SetBufferSize(5 * mSampleRate);
	mRightEyeXSensor->SetDriftCorrectionEnabled(false);
	AddSensor(mRightEyeXSensor, SENSOR_INPUT);

	mRightEyeYSensor = new Sensor("Right Eye Y", mSampleRate);
	mRightEyeYSensor->GetChannel()->SetBufferSize(5 * mSampleRate);
	mRightEyeYSensor->SetDriftCorrectionEnabled(false);
	AddSensor(mRightEyeYSensor, SENSOR_INPUT);

	mRightEyeZSensor = new Sensor("Right Eye Z", mSampleRate);
	mRightEyeZSensor->GetChannel()->SetBufferSize(5 * mSampleRate);
	mRightEyeZSensor->SetDriftCorrectionEnabled(false);
	AddSensor(mRightEyeZSensor, SENSOR_INPUT);

	mRightEyeXNormalizedSensor = new Sensor("Right Eye X Normalized", mSampleRate);
	mRightEyeXNormalizedSensor->GetChannel()->SetBufferSize(5 * mSampleRate);
	mRightEyeXNormalizedSensor->SetDriftCorrectionEnabled(false);
	AddSensor(mRightEyeXNormalizedSensor, SENSOR_INPUT);

	mRightEyeYNormalizedSensor = new Sensor("Right Eye Y Normalized", mSampleRate);
	mRightEyeYNormalizedSensor->GetChannel()->SetBufferSize(5 * mSampleRate);
	mRightEyeYNormalizedSensor->SetDriftCorrectionEnabled(false);
	AddSensor(mRightEyeYNormalizedSensor, SENSOR_INPUT);

	mRightEyeZNormalizedSensor = new Sensor("Right Eye Z Normalized", mSampleRate);
	mRightEyeZNormalizedSensor->GetChannel()->SetBufferSize(5 * mSampleRate);
	mRightEyeZNormalizedSensor->SetDriftCorrectionEnabled(false);
	AddSensor(mRightEyeZNormalizedSensor, SENSOR_INPUT);

	mGazeXSensor = new Sensor("Gaze X", mSampleRate);
	mGazeXSensor->GetChannel()->SetBufferSize(5 * mSampleRate);
	mGazeXSensor->SetDriftCorrectionEnabled(false);
	AddSensor(mGazeXSensor, SENSOR_INPUT);

	mGazeYSensor = new Sensor("Gaze Y", mSampleRate);
	mGazeYSensor->GetChannel()->SetBufferSize(5 * mSampleRate);
	mGazeYSensor->SetDriftCorrectionEnabled(false);
	AddSensor(mGazeYSensor, SENSOR_INPUT);
}


#endif
