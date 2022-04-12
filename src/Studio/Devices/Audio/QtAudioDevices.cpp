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

// include required headers
#include "QtAudioDevices.h"
#include "AudioDriver.h"

#ifdef INCLUDE_DEVICE_GENERIC_AUDIO

//
// QtAudioInputDevice
//

// constructor & destructor
QtAudioInputDevice::QtAudioInputDevice(QObject* parent, DeviceDriver* driver, ESampleRate sampleRate, bool fixedSamplerate) 
	: QIODevice(parent), AudioInputDevice(driver, sampleRate, fixedSamplerate)
{
	mAudioInput = NULL;

	CORE_ASSERT(driver != NULL && driver->GetType() == AudioDriver::TYPE_ID);
	mAudioDriver = static_cast<AudioDriver*>(driver);

	// configure format to 2 channel 32 bit float
	mAudioFormat.setCodec("audio/pcm");
	mAudioFormat.setChannelCount(2);
	mAudioFormat.setSampleRate(sampleRate);
	mAudioFormat.setByteOrder(QAudioFormat::LittleEndian);
	mAudioFormat.setSampleSize(32);
	mAudioFormat.setSampleType(QAudioFormat::SampleType::Float);
}


QtAudioInputDevice::~QtAudioInputDevice()
{
	Disconnect();
}


bool QtAudioInputDevice::Connect()
{
	if (mState != STATE_DISCONNECTED || mAudioInput != NULL || isOpen() == true)
	{
		//Core::LogWarning("Cannot connect to Audio Input Device, it is already opened.");
		return false;
	}

	// write only (push mode -> audioInput writes on this IO device)
	open(QIODevice::WriteOnly);

	// update sample rates
	mAudioFormat.setSampleRate(mSampleRate);
	mLeftSensor->SetSampleRate(mSampleRate);
	mRightSensor->SetSampleRate(mSampleRate);

	// check if format is supported
	QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());
	if (info.isFormatSupported(mAudioFormat) == false)
	{
		//Core::LogError("AudioInput device does not support the selected format.");
		close();
		return false;
	}

	// try to create audio input with specified format
	mAudioInput = new QAudioInput(mAudioFormat, this);
	mAudioInput->setNotifyInterval(1000.0 / mAudioDriver->GetInputUpdateRate());
	mAudioInput->start(this);

	mState = STATE_IDLE;
	return true;
}


bool QtAudioInputDevice::Disconnect()
{
	// close IO device
	close();

	if (mState == STATE_DISCONNECTED)
		return false;

	if (mAudioInput == NULL)
		return false;

	// stop audio device
	mAudioInput->stop();
	mAudioInput->disconnect(this);
	delete mAudioInput;
	mAudioInput = NULL;

	mState = STATE_DISCONNECTED;
	return true;
}


// new data arrives here
qint64 QtAudioInputDevice::writeData(const char *data, qint64 len)
{
	CORE_ASSERT(mAudioFormat.sampleSize() == 32);
	CORE_ASSERT(mAudioFormat.sampleType() == QAudioFormat::Float);
	CORE_ASSERT(mAudioFormat.channelCount() == 2);

	const int channelBytes = 4;
	const int numChannels = 2;
	const int sampleBytes = numChannels * channelBytes;
	
	CORE_ASSERT(len % sampleBytes == 0);

	const int numSamples = len / sampleBytes;
	const unsigned char *ptr = reinterpret_cast<const unsigned char *>(data);

	for (int i = 0; i < numSamples; ++i)
	{
		// TODO check order of channels
		mLeftSensor->AddQueuedSample( *reinterpret_cast<const float*>(ptr) );
		ptr += channelBytes;
		mRightSensor->AddQueuedSample( *reinterpret_cast<const float*>(ptr) );
		ptr += channelBytes;
	}

	return len;
}


//
// MicrophoneDevice
//


// constructor & destructor
QtMicrophoneDevice::QtMicrophoneDevice(QObject* parent, DeviceDriver* driver, ESampleRate sampleRate, bool fixedSamplerate) 
	: QIODevice(parent), MicrophoneDevice(driver, sampleRate, fixedSamplerate) 
{
	mAudioInput = NULL;

	CORE_ASSERT(driver != NULL && driver->GetType() == AudioDriver::TYPE_ID);
	mAudioDriver = static_cast<AudioDriver*>(driver);

	// configure format to 1 channel 32 bit float
	mAudioFormat.setCodec("audio/pcm");
	mAudioFormat.setChannelCount(1);
	mAudioFormat.setSampleRate(sampleRate);
	mAudioFormat.setByteOrder(QAudioFormat::LittleEndian);
	mAudioFormat.setSampleSize(32);
	mAudioFormat.setSampleType(QAudioFormat::SampleType::Float);
}



QtMicrophoneDevice::~QtMicrophoneDevice()
{
	Disconnect();
}



bool QtMicrophoneDevice::Connect()
{
	if (mState != STATE_DISCONNECTED || mAudioInput != NULL || isOpen() == true)
	{
		//Core::LogWarning("Cannot connect to Audio Input Device, it is already opened.");
		return false;
	}

	// write only (push mode -> audioInput writes on this IO device)
	open(QIODevice::WriteOnly);

	// update sample rates
	mAudioFormat.setSampleRate(mSampleRate);
	mSensor->SetSampleRate(mSampleRate);
	
	// check if format is supported
	QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());
	if (info.isFormatSupported(mAudioFormat) == false) 
	{
		//Core::LogError("AudioInput device does not support the selected format.");
		close();
		return false;
	}

	// try to create audio input with specified format
	mAudioInput = new QAudioInput(mAudioFormat, this);
	mAudioInput->start(this);

	mState = STATE_IDLE;
	return true;
}


bool QtMicrophoneDevice::Disconnect()
{
	// close IO device
	close();

	if (mState == STATE_DISCONNECTED)
		return false;

	if (mAudioInput == NULL)
		return false;

	// stop audio device
	mAudioInput->stop();
	mAudioInput->disconnect(this);
	delete mAudioInput;
	mAudioInput = NULL;

	mState = STATE_DISCONNECTED;
	return true;
}


// new data arrives here
qint64 QtMicrophoneDevice::writeData(const char *data, qint64 len)
{
	CORE_ASSERT(mAudioFormat.sampleSize() == 32);
	CORE_ASSERT(mAudioFormat.sampleType() == QAudioFormat::Float);
	CORE_ASSERT(mAudioFormat.channelCount() == 1);

	const int channelBytes = 4;
	const int numChannels = 1;
	const int sampleBytes = numChannels * channelBytes;

	CORE_ASSERT(len % sampleBytes == 0);

	const int numSamples = len / sampleBytes;
	const unsigned char *ptr = reinterpret_cast<const unsigned char *>(data);

	for (int i = 0; i < numSamples; ++i)
	{
		mSensor->AddQueuedSample(*reinterpret_cast<const float*>(ptr));
		ptr += channelBytes;
	}

	return len;
}



//
// AudioOutputDevice
//


QtAudioOutputDevice::QtAudioOutputDevice(QObject* parent, DeviceDriver* driver, ESampleRate sampleRate, bool fixedSamplerate) 
	: QObject(parent), AudioOutputDevice(driver, sampleRate, fixedSamplerate)//, QIODevice(parent)
{

	CORE_ASSERT(driver != NULL && driver->GetType() == AudioDriver::TYPE_ID);
	mAudioDriver = static_cast<AudioDriver*>(driver);

	mAudioOutput = NULL;
	mAudioIODevice = NULL;
	mPos = 0;

	// configure format to 2 channel 32 bit float
	mAudioFormat.setCodec("audio/pcm");
	mAudioFormat.setChannelCount(2);
	mAudioFormat.setSampleRate(sampleRate);
	mAudioFormat.setByteOrder(QAudioFormat::LittleEndian);
	mAudioFormat.setSampleSize(32);
	mAudioFormat.setSampleType(QAudioFormat::SampleType::Float);
}

QtAudioOutputDevice::~QtAudioOutputDevice()
{
	Disconnect();
}


void QtAudioOutputDevice::Reset()
{
   Disconnect();
   AudioOutputDevice::Reset();
}


bool QtAudioOutputDevice::Connect()
{
	if (mState != STATE_DISCONNECTED || mAudioOutput != NULL || mAudioIODevice != NULL)
	{
		Core::LogWarning("Cannot connect to Audio Input Device, it is already opened.");
		return false;
	}

	// update sample rate in format
	mAudioFormat.setSampleRate(mSampleRate);

	// check if format is supported
	QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
	if (info.isFormatSupported(mAudioFormat) == false)
	{
		Core::LogError("AudioInput device does not support the selected format.");
		return false;
	}
	//////////////////
	const int channelBytes = 4;
	const int numChannels = 2;
	const int sampleBytes = numChannels * channelBytes;
	const int bytesPerSecond = mSampleRate * sampleBytes;
	mBuffer.resize(bytesPerSecond * mAudioDriver->GetOutputBufferSize() / 1000.0);
	//////////////////

	// try to create audio input with specified format
	mAudioOutput = new QAudioOutput(mAudioFormat);
	mAudioOutput->setBufferSize(bytesPerSecond * mAudioDriver->GetOutputBufferSize() / 1000.0);
	mAudioOutput->setNotifyInterval(1000.0 / mAudioDriver->GetOutputUpdateRate());
//	// NOTE: don't start immediately, but as soon as the first input sample arrives!
	mAudioIODevice = mAudioOutput->start();
	connect(mAudioOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(OnAudioDeviceStateChanged(QAudio::State)));

	// zero buffer
	char *ptr = mBuffer.data();
	const int buffLen = mBuffer.length();
	for (int i = 0; i < buffLen; ++i)
		*ptr++ = 0.0;

	const int delayBytes = bytesPerSecond * mAudioDriver->GetOutputDelay() / 1000.0;
	const int maxBytes = Core::Min(delayBytes, mBuffer.length());
	mAudioIODevice->write(mBuffer.data(), maxBytes);

	//////////////////

	mState = STATE_IDLE;
	return true;
}


bool QtAudioOutputDevice::Disconnect()
{
    if (mAudioOutput == NULL)
		return false;
		
	// close IO device
	//if (mAudioIODevice != NULL)
		//mAudioIODevice->close();

	//if (mState == STATE_DISCONNECTED)
		//return false;
		
	// stop audio device
	mAudioOutput->stop();
	//mAudioIODevice->disconnect(this);
	delete mAudioOutput;
	mAudioOutput = NULL;
	mAudioIODevice = NULL;

	mState = STATE_DISCONNECTED;
	return true;
}

void QtAudioOutputDevice::OnAudioDeviceStateChanged(QAudio::State newState)
{
    switch (newState) {
        case QAudio::IdleState:
            Core::LogInfo("Idle State");// Finished playing (no more data)
            Disconnect();
            break;

        case QAudio::StoppedState:
            // Stopped for other reasons
            if (mAudioOutput->error() != QAudio::NoError) 
            {
                // Error handling
                Core::LogInfo("QAudio Error: %s", mAudioOutput->error());// Finished playing (no more data)
                Disconnect();
            }
            break;

        default:
            // ... other cases as appropriate
            break;
    }
}


// copy the new samples to the IODevice so QAudioOutput can read from it
void QtAudioOutputDevice::Update(const Core::Time& elapsed, const Core::Time& delta)
{
	// update baseclass first so the queued sensor data is forwarded
	AudioOutputDevice::Update(elapsed, delta);
	
	if (mAudioOutput == NULL || mAudioIODevice == NULL)
		return;
		
    if (mAudioOutput->state() != QAudio::ActiveState)
        return;
	
	CORE_ASSERT(mAudioFormat.sampleSize() == 32);
	CORE_ASSERT(mAudioFormat.sampleType() == QAudioFormat::Float);
	CORE_ASSERT(mAudioFormat.channelCount() == 2);

	const int channelBytes = 4;
	const int numChannels = 2;
	const int sampleBytes = numChannels * channelBytes;
	//const int bytesPerSecond = mSampleRate * sampleBytes;

	const uint32 numNewSamples = Core::Min(mOutputReaders[0]->GetNumNewSamples(), mOutputReaders[1]->GetNumNewSamples());
	const int numBytes = sampleBytes * numNewSamples;

	// start with first sample
	//if (mAudioIODevice == NULL)
	//{
	//	mAudioIODevice = mAudioOutput->start();

		// push second zero samples into the buffer so its a little ahead
	//	if (mAudioDriver->GetOutputDelay() > 0.0)
	//	{
	//		const int delayBytes = bytesPerSecond * mAudioDriver->GetOutputDelay() / 1000.0;
	//		const int maxBytes = Core::Min(delayBytes, mBuffer.length());
	//		mAudioIODevice->write(mBuffer.data(), maxBytes);
	//	}
	//}
	//Core::LogInfo("%f", mAudioOutput->periodSize());

	// provide at least a full period of data (whatever that means, thank QT doc!)
	if (numBytes < mAudioOutput->periodSize())
		return;

	// dont overflow buffer
	if (numBytes > mAudioOutput->bufferSize())
	{
		Core::LogWarning("Audio Output: Audio Buffer is too small (%i bytes to push > %i bytes buffer size)", numBytes, mAudioOutput->bufferSize());
		mOutputReaders[0]->Flush();
		mOutputReaders[1]->Flush();
		return;
	}
	if (numBytes > mBuffer.length() )
	{
		Core::LogWarning("Audio Output: Buffer is too small (%i bytes to push > %i bytes buffer size)", numBytes, mBuffer.length());
		mOutputReaders[0]->Flush();
		mOutputReaders[1]->Flush(); 
		return;
	}
	
	float *ptr = reinterpret_cast<float*>(mBuffer.data());

	for (uint32 i = 0; i < numNewSamples; ++i)
	{
		const float leftSample = mOutputReaders[0]->PopOldestSample<double>();
		const float rightSample = mOutputReaders[1]->PopOldestSample<double>();
		*ptr++ = leftSample;
		*ptr++ = rightSample;
	}
	Core::LogDebug("writing %i byte", numBytes);
	
	mAudioIODevice->write(mBuffer.data(), numBytes);
}

#endif
