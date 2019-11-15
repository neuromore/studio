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

#ifndef __NEUROMORE_AUDIODEVICES_H
#define __NEUROMORE_AUDIODEVICES_H

// include required headers
#include "../../Config.h"
#include "../../Device.h"
#include "../../DeviceDriver.h"

// Baseclass for all Audio Devices
class ENGINE_API AudioDevice : public Device
{
	public:
		
		// typical samplerates
		enum ESampleRate
		{
			SAMPLERATE_8k  =  8000,
			SAMPLERATE_11k = 11025,
			SAMPLERATE_16k = 16000,
			SAMPLERATE_22k = 22050,
			SAMPLERATE_32k = 32000,
			SAMPLERATE_44k = 44100,
		};

		// enum list for attributes
		enum ESampleRateID
		{
			SAMPLERATE_ID_8k = 0,
			SAMPLERATE_ID_11k,
			SAMPLERATE_ID_16k,
			SAMPLERATE_ID_22k,
			SAMPLERATE_ID_32k,
			SAMPLERATE_ID_44k,
			NUM_SAMPLERATES
		};

		static const char* GetSampleRateString(ESampleRateID sampleRate);
		static ESampleRate GetSampleRateFromID(ESampleRateID sampleRate);

		// constructor & destructor
		AudioDevice(DeviceDriver* driver = NULL, ESampleRate sampleRate = SAMPLERATE_8k, bool fixedSamplerate = false);
		virtual ~AudioDevice();

		virtual double GetTimeoutLimit() const override			{ return DBL_MAX; }

		bool HasFixedSampleRate() const							{ return mHasFixedSampleRate; }
		double GetSampleRate() const							{ return mSampleRate; }
		bool SetSampleRate(ESampleRate sampleRate);

	protected:
		ESampleRate			mSampleRate;
		bool				mHasFixedSampleRate;

};



// Generic Stereo Audio Input
class AudioInputDevice : public AudioDevice
{
	public:	
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_GENERIC_AUDIO_IN };

		// constructor & destructor
		AudioInputDevice(DeviceDriver* driver = NULL, ESampleRate sampleRate = SAMPLERATE_8k, bool fixedSamplerate = false);
		virtual ~AudioInputDevice();

		Device* Clone() override												{ return new AudioInputDevice(mDeviceDriver, mSampleRate, mHasFixedSampleRate); }

		// overloaded
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetHardwareName() const override							{ return "Audio Input"; }
		const char* GetUuid() const override									{ return "0f31ae50-a412-11e5-bf7f-feff819cdc9f"; }
		const char* GetTypeName() const override								{ return "audio_in"; }
		static const char* GetRuleName()										{ return "DEVICE_Generic"; }	// TODO use unique rules? or just one for all?

		void CreateSensors() override;
		
		inline Sensor* GetLeftAudioChannel() const								{ return mLeftSensor; }
		inline Sensor* GetRightAudioChannel() const								{ return mRightSensor; }

	protected:
		Sensor*					mLeftSensor;
		Sensor*					mRightSensor;
};


// Mono Audio Output for microphones
class MicrophoneDevice : public AudioDevice
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_GENERIC_MICROPHONE };


		// constructor & destructor
		MicrophoneDevice(DeviceDriver* driver = NULL, ESampleRate sampleRate = SAMPLERATE_8k, bool fixedSamplerate = false);
		virtual ~MicrophoneDevice();

		Device* Clone() override												{ return new MicrophoneDevice(mDeviceDriver, mSampleRate, mHasFixedSampleRate); }

		// overloaded
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetHardwareName() const override							{ return "Microphone"; }
		const char* GetUuid() const override									{ return "fb12adba-a412-11e5-bf7f-feff819cdc9f"; }
		const char* GetTypeName() const override								{ return "mic"; }

		static const char* GetRuleName()										{ return "DEVICE_Generic"; }	// TODO use unique rules? or just one for all?

		void CreateSensors() override;

		inline Sensor* GetMicrophoneSensor() const								{ return mSensor; }

	protected:
		Sensor*					mSensor;
};


// Generic Stereo Audio Output
class AudioOutputDevice : public AudioDevice
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_GENERIC_AUDIO_OUT };

		// constructor & destructor
		AudioOutputDevice(DeviceDriver* driver = NULL, ESampleRate sampleRate = SAMPLERATE_8k, bool fixedSamplerate = false);
		virtual ~AudioOutputDevice();

		Device* Clone() override												{ return new AudioOutputDevice(mDeviceDriver, mSampleRate, mHasFixedSampleRate); }

		// overloaded
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetHardwareName() const override							{ return "Audio Output"; }
		const char* GetUuid() const override									{ return "67a8217c-a412-11e5-bf7f-feff819cdc9f"; }
		const char* GetTypeName() const override								{ return "audio_out"; }

		static const char* GetRuleName()										{ return "DEVICE_Generic"; }	// TODO use unique rules? or just one for all?

		void CreateSensors() override;

		inline Sensor* GetLeftAudioChannel() const								{ return mLeftSensor; }
		inline Sensor* GetRightAudioChannel() const								{ return mRightSensor; }

	protected:
		Sensor*					mLeftSensor;
		Sensor*					mRightSensor;
};



#endif
