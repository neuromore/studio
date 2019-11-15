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

#ifndef __NEUROMORE_QTAUDIODRIVER_H
#define __NEUROMORE_QTAUDIODRIVER_H


#include <Config.h>
#include <DeviceDriver.h>
#include <Devices/Audio/AudioDevices.h>
#include "QtAudioDevices.h"
#include <EngineManager.h>
#include <Core/EventHandler.h>
#include <Core/Array.h>

#include <QObject>

#ifdef 	INCLUDE_DEVICE_GENERIC_AUDIO


// the Audio Driver class
class AudioDriver : public QObject, public DeviceDriver, public Core::EventHandler
{
	Q_OBJECT
	public:
		enum { TYPE_ID = DeviceTypeIDs::DRIVER_TYPEID_AUDIO };

		// constructor & destructor
		AudioDriver();
		virtual ~AudioDriver();

		const char* GetName() const override				{ return "Audio Devices"; }

		uint32 GetType() const override final				{ return TYPE_ID; }

		// initialization
		bool Init() override;

		// main update function
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		bool HasAutoDetectionSupport() const override		{ return false; }
		void DetectDevices() override;

		virtual Device* CreateDevice(uint32 deviceTypeID) override;

		// add device at given serial port
		void AddDevice(AudioDevice* device, const char* serialPort);

		//
		// settings
		//

		// update rates
		void SetOutputUpdateRate(double fps);
		double GetOutputUpdateRate() const					{ return mOutputUpdateRate; }

		void SetInputUpdateRate(double fps);
		double GetInputUpdateRate() const					{ return mInputUpdateRate; }

		// buffers
		void SetOutputBufferSize(double ms);
		double GetOutputBufferSize() const					{ return mOutputBufferSize; }
		void SetOutputDelay(double ms);
		double GetOutputDelay() const						{ return mOutputDelay; }


		////////////////////
		// event handler
		void OnRemoveDevice(Device* device) override;

	private:

		// list to keep track of connected devices
		//Core::Array<Device*>			mDevices;
		// NOTE: for now, we use one input and one output device (where AudioInput and Mic may share the same device, but one is two channel)
		QtMicrophoneDevice*				mMicrophoneDevice;
		QtAudioInputDevice*				mAudioInputDevice;
		QtAudioOutputDevice*			mAudioOutputDevice;

		double							mTimeSinceDeviceCheck;

		// settings
		double	mInputUpdateRate;
		double	mOutputUpdateRate;
		double	mOutputBufferSize;
		double	mOutputDelay;


};



#endif

#endif
