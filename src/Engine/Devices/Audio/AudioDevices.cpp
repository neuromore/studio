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

// include required headers
#include "../../Config.h"
#include "AudioDevices.h"
#include "../../Device.h"
#include "../../DeviceDriver.h"


//
// AudioDevice base class
//

// constructor & destructor
AudioDevice::AudioDevice(DeviceDriver* driver, ESampleRate sampleRate, bool fixedSamplerate)
{
	mSampleRate = sampleRate;
	mHasFixedSampleRate = fixedSamplerate;
}


AudioDevice::~AudioDevice()	
{
}


const char* AudioDevice::GetSampleRateString(ESampleRateID sampleRate)
{
	switch (sampleRate)
	{
		default:
		case SAMPLERATE_ID_8k:  return "8000 Hz";
		case SAMPLERATE_ID_11k: return "11025 Hz";
		case SAMPLERATE_ID_16k: return "16000 Hz";
		case SAMPLERATE_ID_22k: return "22050 Hz";
		case SAMPLERATE_ID_32k: return "32000 Hz";
		case SAMPLERATE_ID_44k: return "44100 Hz";
	}
}


AudioDevice::ESampleRate AudioDevice::GetSampleRateFromID(ESampleRateID sampleRate)
{
	switch (sampleRate)
	{
		default:
		case SAMPLERATE_ID_8k:  return SAMPLERATE_8k;
		case SAMPLERATE_ID_11k: return SAMPLERATE_11k;
		case SAMPLERATE_ID_16k: return SAMPLERATE_16k;
		case SAMPLERATE_ID_22k: return SAMPLERATE_22k;
		case SAMPLERATE_ID_32k: return SAMPLERATE_32k;
		case SAMPLERATE_ID_44k: return SAMPLERATE_44k;
	}
}


bool AudioDevice::SetSampleRate(ESampleRate sampleRate)
{ 
	// can't reconfigure samplerate
	if (mHasFixedSampleRate == true)
		return false; 

	if (mSampleRate == sampleRate)
		return true;
			
	mSampleRate = sampleRate; 

	// set samplerate of all sensors
	const uint32 numSensors = mSensors.Size();
	for (uint32 i = 0; i < numSensors; ++i)
		mSensors[i]->SetSampleRate(mSampleRate);

	return true;
}

//
// AudioInputDevice
//

// constructor & destructor
AudioInputDevice::AudioInputDevice(DeviceDriver* driver , ESampleRate sampleRate, bool fixedSamplerate) 
	: AudioDevice(driver, sampleRate, fixedSamplerate)	
{
	CreateSensors(); 
}


AudioInputDevice::~AudioInputDevice()
{
}


void AudioInputDevice::CreateSensors()
{
	mLeftSensor = new Sensor("Left", mSampleRate);	
	mLeftSensor->GetChannel()->SetBufferSize(5 * mSampleRate);
	mLeftSensor->SetDriftCorrectionEnabled(false);
	AddSensor(mLeftSensor, SENSOR_INPUT);

	mRightSensor = new Sensor("Right", mSampleRate);	
	mRightSensor->GetChannel()->SetBufferSize(5 * mSampleRate);
	mRightSensor->SetDriftCorrectionEnabled(false);
	AddSensor(mRightSensor, SENSOR_INPUT);
}


//
// MicrophoneDevice
//


// constructor & destructor
MicrophoneDevice::MicrophoneDevice(DeviceDriver* driver, ESampleRate sampleRate, bool fixedSamplerate) : AudioDevice(driver, sampleRate, fixedSamplerate)
{
	CreateSensors();
}


MicrophoneDevice::~MicrophoneDevice()
{
}


void MicrophoneDevice::CreateSensors()
{
	mSensor = new Sensor("Out", mSampleRate);
	mSensor->GetChannel()->SetBufferSize(5 * mSampleRate);
	mSensor->SetDriftCorrectionEnabled(false);
	AddSensor(mSensor, SENSOR_INPUT);

}


//
// AudioOutputDevice
//


AudioOutputDevice::AudioOutputDevice(DeviceDriver* driver , ESampleRate sampleRate, bool fixedSamplerate) : AudioDevice(driver, sampleRate, fixedSamplerate)		
{
	CreateSensors(); 
}


AudioOutputDevice::~AudioOutputDevice()													
{
}


void AudioOutputDevice::CreateSensors()
{
	mLeftSensor = new Sensor("Left", mSampleRate);
	mLeftSensor->GetChannel()->SetBufferSize(5 * mSampleRate);
	mLeftSensor->SetDriftCorrectionEnabled(false);
	AddSensor(mLeftSensor, SENSOR_OUTPUT);

	mRightSensor = new Sensor("Right", mSampleRate);
	mRightSensor->GetChannel()->SetBufferSize(5 * mSampleRate);
	mRightSensor->SetDriftCorrectionEnabled(false);
	AddSensor(mRightSensor, SENSOR_OUTPUT);
}

