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
#include <Studio/Precompiled.h>

// include required files
#include <Config.h>
#include <Devices/Audio/AudioDevices.h>
#include "AudioDriver.h"
#include <EngineManager.h>
#include <QApplication>

#ifdef INCLUDE_DEVICE_GENERIC_AUDIO

using namespace Core;


// constructor
AudioDriver::AudioDriver() : DeviceDriver(false), EventHandler()
{
	LogInfo("Constructing Audio device driver ...");

	AddSupportedDevice(AudioInputDevice::TYPE_ID);
	AddSupportedDevice(MicrophoneDevice::TYPE_ID);
	//AddSupportedDevice(AudioOutputDevice::TYPE_ID);

	mTimeSinceDeviceCheck = 0;
	mMicrophoneDevice = NULL;
	mAudioInputDevice = NULL;
	mAudioOutputDevice = NULL;

	// settings
	mOutputUpdateRate	= 100.0;
	mInputUpdateRate	= 100.0;
	mOutputBufferSize	= 1000.0;
	mOutputDelay		= 0.0;

	LogDetailedInfo("Audio device driver constructed");
}


// destructor
AudioDriver::~AudioDriver()
{
	LogInfo("Destructing Audio device driver ...");

	// remove event handler
	CORE_EVENTMANAGER.RemoveEventHandler(this);

	LogDetailedInfo("Audio device driver destructed");
}


// system initialization
bool AudioDriver::Init()
{
	LogInfo("Initializing Audio device driver");

	// register event handler for reacting to device removal
	CORE_EVENTMANAGER.AddEventHandler(this);

	return true;
}


void AudioDriver::Update(const Time& elapsed, const Time& delta)
{
	mTimeSinceDeviceCheck += delta.InSeconds();
	if (mTimeSinceDeviceCheck > 1.0)
	{
		DetectDevices();
		mTimeSinceDeviceCheck = 0.0;
	}
}


// detect devices once
void AudioDriver::DetectDevices()
{
	if (mIsEnabled == false)
		return;

	const QAudioDeviceInfo &defaultInputDeviceInfo = QAudioDeviceInfo::defaultInputDevice();

	// check for microphone device (if not already)
	if (mMicrophoneDevice == NULL)
	{
		if (defaultInputDeviceInfo.isNull() == false)
		{
			QtMicrophoneDevice* micDevice = new QtMicrophoneDevice(NULL, this);
			if (micDevice->Connect() == true)
			{
				GetDeviceManager()->AddDeviceAsync(micDevice);
				mMicrophoneDevice = micDevice;

				Core::LogInfo("AudioDriver: Added Microphone device");
			}
		}
	}


	// check for audio input device (can be same as microphone, but acquires two channels from QT)
	if (mAudioInputDevice == NULL)
	{
		if (defaultInputDeviceInfo.isNull() == false)
		{
			QtAudioInputDevice* inputDevice = new QtAudioInputDevice(NULL, this);
			if (inputDevice->Connect() == true)
			{
				GetDeviceManager()->AddDeviceAsync(inputDevice);
				mAudioInputDevice = inputDevice;

				Core::LogInfo("AudioDriver: Added Audio Input device");
			}
		}
	}

	// check for audio input device (can be same as microphone, but acquires two channels from QT)
	if (mAudioOutputDevice == NULL)
	{
		const QAudioDeviceInfo &defaultOuputDeviceInfo = QAudioDeviceInfo::defaultOutputDevice();
		if (defaultOuputDeviceInfo.isNull() == false)
		{
			QtAudioOutputDevice* outputDevice = new QtAudioOutputDevice(NULL, this);
			if (outputDevice->Connect() == true)
			{
				GetDeviceManager()->AddDeviceAsync(outputDevice);
				mAudioOutputDevice = outputDevice;

				Core::LogInfo("AudioDriver: Added Audio Output device");
			}
		}
	}
}


Device* AudioDriver::CreateDevice(uint32 deviceTypeID)
{
	// check if its the correct device
	CORE_ASSERT(IsDeviceSupported(deviceTypeID));

	switch (deviceTypeID)
	{
		case AudioInputDevice::TYPE_ID: 	return new QtAudioInputDevice(this);
		case MicrophoneDevice::TYPE_ID: 	return new QtMicrophoneDevice(this);
		case AudioOutputDevice::TYPE_ID: 	return new QtAudioOutputDevice(this);
		default:  /* does not happen*/	return NULL;	
	}
}


void AudioDriver::SetInputUpdateRate(double fps)
{
	mInputUpdateRate = fps;
	if (mAudioInputDevice != NULL)
		mAudioInputDevice->SetUpdateRate(fps);
}


void AudioDriver::SetOutputUpdateRate(double fps)
{
	mOutputUpdateRate = fps;
	if (mAudioOutputDevice != NULL)
		mAudioOutputDevice->SetUpdateRate(fps);
}


void AudioDriver::SetOutputBufferSize(double ms)
{
	mOutputBufferSize = ms;
	// note: can't set buffer size while running
}


void AudioDriver::SetOutputDelay(double ms)
{
	mOutputDelay = ms;
	// note: can't set buffer size while running
}



void AudioDriver::AddDevice(AudioDevice* device, const char* serialPort)
{
}


void AudioDriver::OnRemoveDevice(Device* device)
{

	if (mMicrophoneDevice == device)
		mMicrophoneDevice = NULL;

	if (mAudioInputDevice == device)
		mAudioInputDevice = NULL;

	if (mAudioOutputDevice == device)
		mAudioOutputDevice = NULL;
}

#endif
