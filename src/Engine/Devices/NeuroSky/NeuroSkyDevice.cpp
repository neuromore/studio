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
#include "NeuroSkyDevice.h"
#include "../../EngineManager.h"
#include "../../Core/LogManager.h"

#ifdef INCLUDE_DEVICE_NEUROSKY_MINDWAVE

using namespace Core;

// constructor
NeuroSkyDevice::NeuroSkyDevice(DeviceDriver* driver) : BciDevice(driver)
{
	LogDetailedInfo("Constructing NeuroSky headset ...");

	mState = STATE_IDLE;
	mPowerSupplyType = POWERSUPPLY_BATTERY;

	// create all sensors
	CreateSensors();

	mRawSensor = mSensors[0];
	
	// setup raw sensors
	Channel<double>* rawChannel = mRawSensor->GetChannel();

	rawChannel->SetMinValue(-100);
	rawChannel->SetMaxValue(100);
	rawChannel->SetUnit("uV");
}


// destructor
NeuroSkyDevice::~NeuroSkyDevice()
{
	LogDetailedInfo("Destructing NeuroSky headset ...");
}


// get the available electrodes of the neuro headset
void NeuroSkyDevice::CreateElectrodes()
{
	mElectrodes.Clear();
	mElectrodes.Reserve(1);

	// note : electrode position could also be Fp1 depending on how you wear it
	mElectrodes.Add( GetEngine()->GetEEGElectrodes()->GetElectrodeByID("F3") ); 
}


// create all neurosky mindwave sensors
void NeuroSkyDevice::CreateSensors()
{
	// create EEG sensors first
	BciDevice::CreateSensors();

	//// TEMP turn off drift correction
	//const uint32 numSensors = GetNumSensors();
	//for (uint32 i = 0; i < numSensors; ++i)
	//	GetSensor(i)->SetDriftCorrectionEnabled(false);
	
	mMeditationSensor	 = new Sensor( "Meditation", 1 );
	mAttentionSensor	 = new Sensor( "Attention", 1 );
	mDeltaSensor		 = new Sensor( "Delta", 1 );
	mThetaSensor		 = new Sensor( "Theta", 1 );
	mAlpha1Sensor		 = new Sensor( "Low Alpha", 1 );
	mAlpha2Sensor		 = new Sensor( "High Alpha", 1 );
	mBeta1Sensor		 = new Sensor( "Low Beta", 1 );
	mBeta2Sensor		 = new Sensor( "High Beta", 1 );
	mGamma1Sensor		 = new Sensor( "Low Gamma", 1 );
	mGamma2Sensor		 = new Sensor( "High Gamma", 1 );
	//mEyeBlinkSensor		 = new Sensor( "Eye Blink", 0 );

	//// disable drift correction for now
	//mMeditationSensor->SetDriftCorrectionEnabled(false);
	//mAttentionSensor->SetDriftCorrectionEnabled(false);
	//mDeltaSensor->SetDriftCorrectionEnabled(false);
	//mThetaSensor->SetDriftCorrectionEnabled(false);
	//mAlpha1Sensor->SetDriftCorrectionEnabled(false);
	//mAlpha2Sensor->SetDriftCorrectionEnabled(false);
	//mBeta1Sensor->SetDriftCorrectionEnabled(false);
	//mBeta2Sensor->SetDriftCorrectionEnabled(false);
	//mGamma1Sensor->SetDriftCorrectionEnabled(false);
	//mGamma2Sensor->SetDriftCorrectionEnabled(false);
	//mBlinkStrengthSensor->SetDriftCorrectionEnabled(false);

	// add the sensors
	AddSensor( mMeditationSensor );
	AddSensor( mAttentionSensor );
	AddSensor( mDeltaSensor );
	AddSensor( mThetaSensor );
	AddSensor( mAlpha1Sensor );
	AddSensor( mAlpha2Sensor );
	AddSensor( mBeta1Sensor );
	AddSensor( mBeta2Sensor );
	AddSensor( mGamma1Sensor );
	AddSensor( mGamma2Sensor );
	//AddSensor( mEyeBlinkSensor );
}

#endif
