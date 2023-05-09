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
#include "AbmDevices.h"
#include "../../EngineManager.h"
#include "../../Core/LogManager.h"

#ifdef INCLUDE_DEVICE_ADVANCEDBRAINMONITORING

using namespace Core;

//
// ABM device base class
//

// constructor
AbmDevice::AbmDevice(DeviceDriver* driver) : BciDevice(driver)
{
	mState = STATE_IDLE;
	mDataMode = DATAMODE_RAW;			// only raw signals
	mFilterMode = FILTERMODE_NONE;		// no filters or decontamination
	mNumRawChannels = 0;
	mPowerSupplyType = POWERSUPPLY_BATTERY;
}


void AbmDevice::CreateSensors()
{
	// create EEG sensors first
	BciDevice::CreateSensors();

	const double ekgSampleRate = GetSampleRate();
	const double brainStateSampleRate = 10.0;
	const double movementSampleRate = 1.0;
	const double tiltSampleRate = GetTiltSampleRate();
	const double heartRateSampleRate = 10;
	//const double deconSampleRate = 10.0;

	// EKG
	mEKGSensor = new Sensor("EKG", ekgSampleRate);
	mEKGSensor->GetChannel()->SetMinValue(-DBL_MAX);
	mEKGSensor->GetChannel()->SetMaxValue(DBL_MAX);
	mEKGSensor->GetChannel()->SetUnit("V");
	AddSensor(mEKGSensor);

	// classification estimate
	mClassificationSensor = new Sensor("Classification", brainStateSampleRate, 0.0);
	mClassificationSensor->GetChannel()->SetMinValue(0.0);
	mClassificationSensor->GetChannel()->SetMaxValue(1.0);
	mClassificationSensor->GetChannel()->SetUnit("");
	AddSensor(mClassificationSensor);
	
	// high engagement
	mHighEngagementSensor = new Sensor("High Engagement", brainStateSampleRate,  0.0);
	mHighEngagementSensor->GetChannel()->SetMinValue(0.0);
	mHighEngagementSensor->GetChannel()->SetMaxValue(1.0);
	mHighEngagementSensor->GetChannel()->SetUnit("");
	AddSensor(mHighEngagementSensor);

	// low engagement
	mLowEngagementSensor = new Sensor("Low Engagement", brainStateSampleRate,  0.0);
	mLowEngagementSensor->GetChannel()->SetMinValue(0.0);
	mLowEngagementSensor->GetChannel()->SetMaxValue(1.0);
	mLowEngagementSensor->GetChannel()->SetUnit("");
	AddSensor(mLowEngagementSensor);

	// distraction
	mDistractionSensor = new Sensor("Distraction", brainStateSampleRate,  0.0);
	mDistractionSensor->GetChannel()->SetMinValue(0.0);
	mDistractionSensor->GetChannel()->SetMaxValue(1.0);
	mDistractionSensor->GetChannel()->SetUnit("");
	AddSensor(mDistractionSensor);

	// drowsy
	mDrowsySensor = new Sensor("Drowsy", brainStateSampleRate,  0.0);
	mDrowsySensor->GetChannel()->SetMinValue(0.0);
	mDrowsySensor->GetChannel()->SetMaxValue(1.0);
	mDrowsySensor->GetChannel()->SetUnit("");
	AddSensor(mDrowsySensor);

	// workload average
	mWorkloadAverageSensor = new Sensor("Workload Average", brainStateSampleRate,  0.0);
	mWorkloadAverageSensor->GetChannel()->SetMinValue(0.0);
	mWorkloadAverageSensor->GetChannel()->SetMaxValue(1.0);
	mWorkloadAverageSensor->GetChannel()->SetUnit("");
	AddSensor(mWorkloadAverageSensor);

	// workload BDS
	mWorkloadBDSSensor = new Sensor("Workload BDS", brainStateSampleRate,  0.0);
	mWorkloadBDSSensor->GetChannel()->SetMinValue(0.0);
	mWorkloadBDSSensor->GetChannel()->SetMaxValue(1.0);
	mWorkloadBDSSensor->GetChannel()->SetUnit("");
	AddSensor(mWorkloadBDSSensor);

	// workload FBDS
	mWorkloadFBDSSensor = new Sensor("Workload FBDS", brainStateSampleRate,  0.0);
	mWorkloadFBDSSensor->GetChannel()->SetMinValue(0.0);
	mWorkloadFBDSSensor->GetChannel()->SetMaxValue(1.0);
	mWorkloadFBDSSensor->GetChannel()->SetUnit("");
	AddSensor(mWorkloadFBDSSensor);

	// movement value
	mMovementValueSensor = new Sensor("Movement Value", movementSampleRate,  0.0);
	mMovementValueSensor->GetChannel()->SetMinValue(-100.0);
	mMovementValueSensor->GetChannel()->SetMaxValue(100.0);
	mMovementValueSensor->GetChannel()->SetUnit("");
	AddSensor(mMovementValueSensor);

	// movement level
	mMovementLevelSensor = new Sensor("Movement Level", movementSampleRate,  0.0);
	mMovementLevelSensor->GetChannel()->SetMinValue(0.0);
	mMovementLevelSensor->GetChannel()->SetMaxValue(5.0);
	mMovementLevelSensor->GetChannel()->SetUnit("");
	AddSensor(mMovementLevelSensor);

	// tilt X
	mTiltXSensor = new Sensor("Tilt X", tiltSampleRate);
	mTiltXSensor->GetChannel()->SetMinValue(-360.0);
	mTiltXSensor->GetChannel()->SetMaxValue(+360.0);
	mTiltXSensor->GetChannel()->SetUnit("deg");
	AddSensor(mTiltXSensor);

	// tilt Y
	mTiltYSensor = new Sensor("Tilt Y", tiltSampleRate);
	mTiltYSensor->GetChannel()->SetMinValue(-360.0);
	mTiltYSensor->GetChannel()->SetMaxValue(+360.0);
	mTiltYSensor->GetChannel()->SetUnit("deg");
	AddSensor(mTiltYSensor);

	// tilt Z
	mTiltZSensor = new Sensor("Tilt Z", tiltSampleRate);
	mTiltZSensor->GetChannel()->SetMinValue(-360.0);
	mTiltZSensor->GetChannel()->SetMaxValue(+360.0);
	mTiltZSensor->GetChannel()->SetUnit("deg");
	AddSensor(mTiltZSensor);

	// EMG
	mHeartRateSensor = new Sensor("Heartrate", heartRateSampleRate,  0.0);
	mHeartRateSensor->GetChannel()->SetMinValue(0.0);
	mHeartRateSensor->GetChannel()->SetMaxValue(200.0);
	mHeartRateSensor->GetChannel()->SetUnit("bpm");
	AddSensor(mHeartRateSensor);

	// eye blinks
	// NOTE: are of event type with SDK callbacks
/*	mEyeBlinkSensor = new Sensor("Eye Blink", mDeconSampleRate);
	mEyeBlinkSensor->GetChannel()->SetMinValue(0.0);
	mEyeBlinkSensor->GetChannel()->SetMaxValue(1.0);
	mEyeBlinkSensor->GetChannel()->SetUnit("");
	AddSensor(mEyeBlinkSensor);

	// saturation
	mSaturationSensor = new Sensor("Saturation", mDeconSampleRate);
	mSaturationSensor->GetChannel()->SetMinValue(0.0);
	mSaturationSensor->GetChannel()->SetMaxValue(1.0);
	mSaturationSensor->GetChannel()->SetUnit("");
	AddSensor(mSaturationSensor);

	// excursion
	mExcursionSensor = new Sensor("Excursion", mDeconSampleRate);
	mExcursionSensor->GetChannel()->SetMinValue(0.0);
	mExcursionSensor->GetChannel()->SetMaxValue(1.0);
	mExcursionSensor->GetChannel()->SetUnit("");
	AddSensor(mExcursionSensor);

	// spike
	mSpikeSensor = new Sensor("Spike", mDeconSampleRate);
	mSpikeSensor->GetChannel()->SetMinValue(0.0);
	mSpikeSensor->GetChannel()->SetMaxValue(1.0);
	mSpikeSensor->GetChannel()->SetUnit("");
	AddSensor(mSpikeSensor);

	// EMG
	mEMGSensor = new Sensor("EMG", mDeconSampleRate);
	mEMGSensor->GetChannel()->SetMinValue(0.0);
	mEMGSensor->GetChannel()->SetMaxValue(1.0);
	mEMGSensor->GetChannel()->SetUnit("");
	AddSensor(mEMGSensor);*/
}


//
// ABM X24
// 

AbmX24Device::AbmX24Device(DeviceDriver* driver) : AbmDevice(driver)
{
	LogDetailedInfo("Constructing Advanced Brain Monitoring B-Alert X24 device ...");

	// create all sensors
	CreateSensors();
}


// destructor
AbmX24Device::~AbmX24Device()
{
	LogDetailedInfo("Destructing Advanced Brain Monitoring B-Alert X24 device ...");
}


//
// ABM X10
// 

AbmX10Device::AbmX10Device(DeviceDriver* driver) : AbmDevice(driver)
{
	LogDetailedInfo("Constructing Advanced Brain Monitoring B-Alert X10 device ...");

	// create all sensors
	CreateSensors();
}


// destructor
AbmX10Device::~AbmX10Device()
{
	LogDetailedInfo("Destructing Advanced Brain Monitoring B-Alert X10 device ...");
}


void AbmX10Device::CreateElectrodes()
{
	// X10 Channel mapping from ABM docs:
	// EKG[1], POz[2], Fz[3], Cz[4], C3[5], C4[6], F3[7], F4[8], P3[9], P4[10]

	mElectrodes.Clear();
	mElectrodes.Reserve(9);

	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("POz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Cz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C3"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C4"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F3"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F4"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P3"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P4"));
}


void AbmX24Device::CreateSensors()
{
	// sensors from base class
	AbmDevice::CreateSensors();

	// 3x AUX
	mAux1Sensor = new Sensor("AUX1", GetSampleRate());
	mAux1Sensor->GetChannel()->SetMinValue(-DBL_MAX);
	mAux1Sensor->GetChannel()->SetMaxValue(DBL_MAX);
	mAux1Sensor->GetChannel()->SetUnit("V");
	AddSensor(mAux1Sensor);

	mAux2Sensor = new Sensor("AUX2", GetSampleRate());
	mAux2Sensor->GetChannel()->SetMinValue(-DBL_MAX);
	mAux2Sensor->GetChannel()->SetMaxValue(DBL_MAX);
	mAux2Sensor->GetChannel()->SetUnit("V");
	AddSensor(mAux2Sensor);

	mAux3Sensor = new Sensor("AUX3", GetSampleRate());
	mAux3Sensor->GetChannel()->SetMinValue(-DBL_MAX);
	mAux3Sensor->GetChannel()->SetMaxValue(DBL_MAX);
	mAux3Sensor->GetChannel()->SetUnit("V");
	AddSensor(mAux3Sensor);
}


void AbmX24Device::CreateElectrodes()
{
	// X24 Channel mapping from ABM docs:
	// F3[1], F1[2], Fz[3], F2[4], F4[5]
	// C3[6], C1[7], Cz[8], C2[9], C4[10], CPz[11]
	// P3[12], P1[13], Pz[14], P2[15], P4[16], POz[17]
	// O1[18], Oz[19], O2[20]
	// EKG[21], AUX1[22], AUX2[23], AUX3[24]

	mElectrodes.Clear();
	mElectrodes.Reserve(20);

	// F
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F3"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F1"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F2"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F4"));

	// C
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C3"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C1"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Cz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C2"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C4"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CPz"));

	// P
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P3"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P1"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Pz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P2"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P4"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("POz"));

	// O
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O1"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Oz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O2"));
}



#endif
