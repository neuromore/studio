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

#ifndef __NEUROMORE_QTAUDIODEVICES_H
#define __NEUROMORE_QTAUDIODEVICES_H

// include required headers
#include "../../Config.h"
#include <Device.h>
#include <DeviceDriver.h>
#include <Devices/Audio/AudioDevices.h>

#include <QObject>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioOutput>

#ifdef INCLUDE_DEVICE_GENERIC_AUDIO

class AudioDriver;

// Qt Stereo Audio Input
class QtAudioInputDevice : public QIODevice, public AudioInputDevice
{
	Q_OBJECT
	public:	
		// constructor & destructor
		QtAudioInputDevice(QObject* parent = NULL, DeviceDriver* driver = NULL, ESampleRate sampleRate = SAMPLERATE_8k, bool fixedSamplerate = false);
		~QtAudioInputDevice();

		Device* Clone() override						{ return new QtAudioInputDevice(NULL, mDeviceDriver, mSampleRate, mHasFixedSampleRate); }

		bool Connect() override;
		bool Disconnect() override;

		void SetUpdateRate(double fps)					{ if (mAudioInput != NULL) mAudioInput->setNotifyInterval(1000.0 / fps); }

	private:
		qint64 readData(char *data, qint64 len) override				{ return 0; }
		qint64 writeData(const char *data, qint64 len) override;

		AudioDriver*		mAudioDriver;

		QAudioInput*		mAudioInput;
		QAudioDeviceInfo	mAudioInfo;
		QAudioFormat		mAudioFormat;
};


// Mono Audio Output for microphones
class QtMicrophoneDevice : QIODevice, public MicrophoneDevice
{
	Q_OBJECT
	public:
		// constructor & destructor
		QtMicrophoneDevice(QObject* parent = NULL, DeviceDriver* driver = NULL, ESampleRate sampleRate = SAMPLERATE_8k, bool fixedSamplerate = false);
		~QtMicrophoneDevice();

		Device* Clone() override						{ return new QtMicrophoneDevice(NULL, mDeviceDriver, mSampleRate, mHasFixedSampleRate); }

		bool Connect() override;
		bool Disconnect() override;

		void SetUpdateRate(double fps)					{ if (mAudioInput != NULL) mAudioInput->setNotifyInterval(1000.0 / fps); }

	private:	
		qint64 readData(char *data, qint64 maxlen) override				{ return 0; }
		qint64 writeData(const char *data, qint64 len) override;

		AudioDriver*		mAudioDriver;

		QAudioInput*		mAudioInput;
		QAudioDeviceInfo	mAudioInfo;
		QAudioFormat		mAudioFormat;
};


// Qt Stereo Audio Input
class QtAudioOutputDevice : public QObject, public AudioOutputDevice
{
    Q_OBJECT
	public:
		// constructor & destructor
		QtAudioOutputDevice(QObject* parent = NULL, DeviceDriver* driver = NULL, ESampleRate sampleRate = SAMPLERATE_8k, bool fixedSamplerate = false);
		~QtAudioOutputDevice();

		Device* Clone() override						{ return new QtAudioOutputDevice(NULL, mDeviceDriver, mSampleRate, mHasFixedSampleRate); }

		void Reset() override;

		bool Connect() override;
		bool Disconnect() override;

        

		void Update(const Core::Time& elapsed, const Core::Time& delta) override final;

		void SetUpdateRate(double fps)					{ if (mAudioOutput != NULL) mAudioOutput->setNotifyInterval(1000.0 / fps); }

    public slots:
        void OnAudioDeviceStateChanged(QAudio::State newState);
        
	private:

		AudioDriver*		mAudioDriver;

		QAudioOutput*		mAudioOutput;
		QIODevice*			mAudioIODevice;
		QAudioDeviceInfo	mAudioInfo;
		QAudioFormat		mAudioFormat;

		QByteArray			mBuffer;
		qint64				mPos;
};



#endif

#endif
