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
#include "EmotivEPOCDevice.h"
#include "../../EngineManager.h"
#include "../../Core/LogManager.h"

#ifdef INCLUDE_DEVICE_EMOTIV

using namespace Core;


// constructor
EmotivEPOCDevice::EmotivEPOCDevice(DeviceDriver* driver) : BciDevice(driver)
{
	mGyroX			= 0.0;
	mGyroY			= 0.0;

	// create all sensors
	CreateSensors();

	// start in connected state
	mState = STATE_IDLE;
	
	// TODO: my epoc doesn't send a valid battery state (only -1), so i disabled it
	mPowerSupplyType = POWERSUPPLY_LINE;
}


// destructor
EmotivEPOCDevice::~EmotivEPOCDevice()
{
}

// get the available electrodes of the neuro headset
void EmotivEPOCDevice::CreateElectrodes()
{
	mElectrodes.Clear();

	mElectrodes.Reserve( 14 );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("AF3") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("F7") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("F3") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("FC5") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("T7") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("P7") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("O1") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("O2") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("P8") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("T8") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("FC6") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("F4") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("F8") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("AF4") );
}


void EmotivEPOCDevice::CreateSensors()
{
	// create EEG sensors first
	BciDevice::CreateSensors();

	// for inputs with variable sample rate
	const double sampleRate = 10;

	// gyro sensor
	mGyroXSensor			= AddSensor(SENSOR_INPUT, "Gyro (Yaw)",			GetSampleRate(),	false,	-100.0,		100.0,	"" );
	mGyroYSensor			= AddSensor(SENSOR_INPUT, "Gyro (Pitch)",		GetSampleRate(),	false,	-100.0,		100.0,	"" );
	
	// expressive
	mBlinkSensor			= AddSensor(SENSOR_INPUT, "Blink",				sampleRate,			true,	 0.0,		1.0,	"" );
	mWinkLeftSensor			= AddSensor(SENSOR_INPUT, "Wink Left",			sampleRate,			true,	 0.0,		1.0,	"" );
	mWinkRightSensor		= AddSensor(SENSOR_INPUT, "Wink Right",			sampleRate,			true,	 0.0,		1.0,	"" );
	mEyeLeftClosedSensor	= AddSensor(SENSOR_INPUT, "Eye Left Closed",	sampleRate,			true,	 0.0,		1.0,	"" );
	mEyeRightClosedSensor	= AddSensor(SENSOR_INPUT, "Eye Right Closed",	sampleRate,			true,	 0.0,		1.0,	"" );
	mEyesPitchSensor		= AddSensor(SENSOR_INPUT, "Eyes Pitch",			sampleRate,			true,	-1.0,		1.0,	"" );
	mEyesYawSensor			= AddSensor(SENSOR_INPUT, "Eyes Yaw",			sampleRate,			true,	-1.0,		1.0,	"" );

	// facial actions
	mEyebrowExtentSensor	= AddSensor(SENSOR_INPUT, "Eyebrow extent",		sampleRate,			true,	 0.0,		100.0,	"" );
	mFurrowSensor			= AddSensor(SENSOR_INPUT, "Furrow",				sampleRate,			true,	 0.0,		100.0,	"" );
	mSmileSensor			= AddSensor(SENSOR_INPUT, "Smile",				sampleRate,			true,	 0.0,		100.0,	"" );
	mClenchSensor			= AddSensor(SENSOR_INPUT, "Clench",				sampleRate,			true,	 0.0,		100.0,	"" );
	mLaughSensor			= AddSensor(SENSOR_INPUT, "Laugh",				sampleRate,			true,	 0.0,		100.0,	"" );
	mSmirkLeftSensor		= AddSensor(SENSOR_INPUT, "Smirk Left",			sampleRate,			true,	 0.0,		100.0,	"" );
	mSmirkRightSensor		= AddSensor(SENSOR_INPUT, "Smirk Right",		sampleRate,			true,	 0.0,		100.0,	"" );

	// affective emotional
	mExcitement				= AddSensor(SENSOR_INPUT, "Excitement",			sampleRate,			true,	 0.0,		1.0,	"" );
	mLongTermExcitement		= AddSensor(SENSOR_INPUT, "Long-term Excitement",sampleRate,		true,	 0.0,		1.0,	"" );
	mMeditation				= AddSensor(SENSOR_INPUT, "Meditation",			sampleRate,			true,	 0.0,		1.0,	"" );
	mFrustration			= AddSensor(SENSOR_INPUT, "Frustration",		sampleRate,			true,	 0.0,		1.0,	"" );
	mEngagementBoredom		= AddSensor(SENSOR_INPUT, "Engagement-Boredom",	sampleRate,			true,	 0.0,		1.0,	"" );
	mValenceScore			= AddSensor(SENSOR_INPUT, "Valence Score",		sampleRate,			true,	 0.0,		1.0,	"" );
}

#endif
