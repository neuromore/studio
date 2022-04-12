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
#include "BrainquirySerialHandler.h"
#include <Devices/Brainquiry/BrainquiryDevice.h>
#include <EngineManager.h>
#include <QCoreApplication>
#include <QTimer>
#include <System/BluetoothHelpers.h>

#ifdef INCLUDE_DEVICE_BRAINQUIRY

// link to the EDK libraries
#pragma comment(lib, "BQPetDLL.lib")

using namespace Core;

// constructor
BrainquirySerialHandler::BrainquirySerialHandler(BrainquiryDevice* device, int handle, Core::Array<int> channelNumbers, QObject* parent) : QObject(parent)
{
	LogDetailedInfo("Creating  BrainquirySerialHandler ...");
	mDevice = device;
	mHandle = handle;
	mChannelNumbers = channelNumbers;

	mTimer = new QTimer();
	QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(ReadStream()));
	mTimer->setTimerType(Qt::PreciseTimer);

	// query library for new samples every two samples 
	// NOTE: we somehow lose samples if we query it with a frequency of twice the samplerate (we lose every other sample, to be exact)
	mTimer->setInterval(0.5 * 1000 / mDevice->GetSampleRate());
	mTimer->start();
}


////////////////////////////////////////////////////////////////


// try to read serial data
void BrainquirySerialHandler::ReadStream()
{
	if (mDevice->IsEnabled() == false)
		return;
	
	const uint32 numChannels = mChannelNumbers.Size();

	double sample = 0;
	int retval = 0;

	// attempt to read samples of data from the connection
	while ( (retval = BQGetEEGdata(mHandle, &sample, 0)) > 0)
	{

		// check which channel this sample belongs to and push it into queue
		for (uint32 i = 0; i<numChannels; ++i)
		{
			if (retval == (int)mChannelNumbers[i])
			{
				mDevice->GetSensor(i)->AddQueuedSample(sample);
				continue;
			}
		}
	}
}

#endif
