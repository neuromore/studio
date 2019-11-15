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

// include the required headers
#include "AudioNodes.h"


using namespace Core;

// destructor : disconnect from audio device
AudioInputNodeBase::~AudioInputNodeBase()
{
}


// init nodes
void AudioInputNodeBase::Init()
{
	// base class init
	DeviceInputNode::Init();
	
	// ATTRIBUTES

	// sample rate
	AttributeSettings* sampleRateAttrib = RegisterAttribute("Sample Rate", "SampleRate", "The sample rate of the audio device.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	sampleRateAttrib->ResizeComboValues(AudioDevice::NUM_SAMPLERATES);
	for (uint32 i = 0; i < AudioDevice::NUM_SAMPLERATES; ++i)
		sampleRateAttrib->SetComboValue(i, AudioDevice::GetSampleRateString((AudioDevice::ESampleRateID)i));
	sampleRateAttrib->SetDefaultValue(AttributeInt32::Create(AudioDevice::SAMPLERATE_ID_8k));

	// hide unused attributes
	GetAttributeSettings(ATTRIB_RAWOUTPUT)->SetVisible(false);
	GetAttributeSettings(ATTRIB_UPLOAD)->SetVisible(false);
}


// override and reimplement ReInit of DeviceInputNode (we have to change the samplerate during connection)
// NOTE this can be removed if we use device settings in all devices and nodes, then DeviceInputNode could configure the device after connection
void AudioInputNodeBase::ReInit(const Time& elapsed, const Time& delta)
{
	// Note: Skip InputNode::ReInit()
	SPNode::ReInit(elapsed, delta);

	AudioDevice::ESampleRate selectedRate = AudioDevice::GetSampleRateFromID((AudioDevice::ESampleRateID)GetInt32Attribute(ATTRIB_SAMPLERATE));

	// get device and stop node if there is not a running one
	mCurrentDevice = NULL;
	Device* device = FindDevice();
	if (device != NULL && device->IsEnabled())
	{
		AudioInputDevice* audioDevice = static_cast<AudioInputDevice*>(device);

		// make sure we're the only one who has the lock
		if (audioDevice->AcquireLock(this) == true)
		{
			// try to configure sample rate
			if (audioDevice->SetSampleRate(selectedRate) == false)
			{
				SetError(ERROR_CANNOT_CHANGE_SAMPLERATE, "Cannot change sample rate.");
				mIsInitialized = false;
			}
		
			// connect if not already
			if (audioDevice->IsConnected() == false)
				audioDevice->Connect();

			// check if we are connected now
			if (device->IsConnected() == true)
				mCurrentDevice = device;
			else
				SetError(ERROR_DEVICE_NOT_FOUND, "Device not connected.");
		}
		else
		{
			SetError(ERROR_DEVICE_LOCKED, "Device is already in use.");
		}
	}
	if (mCurrentDevice == NULL)
		mIsInitialized = false;

	PostReInit(elapsed, delta);
}


void AudioInputNodeBase::OnAttributesChanged()
{
	DeviceInputNode::OnAttributesChanged();

	// check if samplerate has changed
	if (mCurrentDevice != NULL)
	{
		AudioDevice* audioDevice = static_cast<AudioDevice*>(mCurrentDevice);

		// reset node if samplerate was changed
		AudioDevice::ESampleRate selectedRate = AudioDevice::GetSampleRateFromID((AudioDevice::ESampleRateID)GetInt32Attribute(ATTRIB_SAMPLERATE));
		if (audioDevice->GetSampleRate() != selectedRate)
			ResetAsync();
	}
}




//
// Audio Output Node Base
//




void AudioOutputNodeBase::Init()
{
	// base class init
	DeviceOutputNode::Init();

	// ATTRIBUTES

	// sample rate
	AttributeSettings* sampleRateAttrib = RegisterAttribute("Sample Rate", "SampleRate", "The sample rate of the audio device.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	sampleRateAttrib->ResizeComboValues(AudioDevice::NUM_SAMPLERATES);
	for (uint32 i = 0; i < AudioDevice::NUM_SAMPLERATES; ++i)
		sampleRateAttrib->SetComboValue(i, AudioDevice::GetSampleRateString((AudioDevice::ESampleRateID)i));
	sampleRateAttrib->SetDefaultValue(AttributeInt32::Create(AudioDevice::SAMPLERATE_ID_8k));
}





// override and reimplement ReInit of DeviceInputNode (we have to change the samplerate during connection)
// NOTE this can be removed if we use device settings in all devices and nodes, then DeviceInputNode could configure the device after connection
void AudioOutputNodeBase::ReInit(const Time& elapsed, const Time& delta)
{
	// shared base reinit helper
	if (BaseReInit(elapsed, delta) == false)
		return;

	SPNode::ReInit(elapsed, delta);

	AudioDevice::ESampleRate selectedRate = AudioDevice::GetSampleRateFromID((AudioDevice::ESampleRateID)GetInt32Attribute(ATTRIB_SAMPLERATE));

	// skip if node base cannot init
	if (mIsInitialized == true)
	{
		const uint32 numPorts = GetNumInputPorts();
		for (uint32 i = 0; i < numPorts && mIsInitialized == true; ++i)
		{
			// check input connection
			if (GetInputPort(i).HasConnection() == false ||
				(GetInputPort(i).HasConnection() == true && GetInputPort(i).GetChannels()->GetNumChannels() != 1))
			{
				SetWarning(WARNING_NO_INPUT, "Input missing.");
				mIsInitialized = false;
			}

			// check samplerate of input
			else if (GetInputPort(i).HasConnection() == true && GetInputPort(i).GetChannels()->GetSampleRate() != (double)selectedRate)
			{
				SetError(ERROR_WRONG_SAMPLERATE, "Input has wrong sample rate.");
				mIsInitialized = false;
			}
		}
	}

	// skip device search if node cannot init
	if (mIsInitialized == true)
	{
		// get device and stop node if there is not a running one
		Device* device = FindDevice();
		if (device != NULL && device->IsEnabled())
		{
			AudioOutputDevice* audioDevice = static_cast<AudioOutputDevice*>(device);

			// make sure we're the only one who has the lock
			if (audioDevice->AcquireLock(this) == true)
			{
				// try to configure sample rate
				if (audioDevice->SetSampleRate(selectedRate) == false)
				{
					SetError(ERROR_CANNOT_CHANGE_SAMPLERATE, "Cannot change sample rate.");
					mIsInitialized = false;
				}
				
				// connect if not already
				if (audioDevice->IsConnected() == false)
					audioDevice->Connect();

				// check if we are connected now
				if (device->IsConnected() == true)
					mCurrentDevice = device;
				else
					SetError(ERROR_DEVICE_NOT_FOUND, "Device not connected.");
			}
			else
			{
				SetError(ERROR_DEVICE_LOCKED, "Device is already in use.");
			}
		}
	}

	// have no device
	if (mCurrentDevice == NULL)
		mIsInitialized = false;

	PostReInit(elapsed, delta);
}


void AudioOutputNodeBase::OnAttributesChanged()
{
	DeviceOutputNode::OnAttributesChanged();

	// check if samplerate has changed
	if (mCurrentDevice != NULL)
	{
		AudioDevice* audioDevice = static_cast<AudioDevice*>(mCurrentDevice);

		// reset node if samplerate was changed
		AudioDevice::ESampleRate selectedRate = AudioDevice::GetSampleRateFromID((AudioDevice::ESampleRateID)GetInt32Attribute(ATTRIB_SAMPLERATE));
		if (audioDevice->GetSampleRate() != selectedRate)
			ResetAsync();
	}
}
