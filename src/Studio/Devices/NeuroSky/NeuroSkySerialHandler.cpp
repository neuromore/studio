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

// include required files
#include "NeuroSkySerialHandler.h"
#include <Devices/NeuroSky/NeuroSkyDevice.h>
#include <EngineManager.h>
#include <QCoreApplication>
#include <QTimer>
#include <System/BluetoothHelpers.h>

#ifdef INCLUDE_DEVICE_NEUROSKY_MINDWAVE


using namespace Core;

// constructor
NeuroSkySerialHandler::NeuroSkySerialHandler(NeuroSkyDevice* headset, int connectionID, QObject* parent) : QObject(parent)
{
	LogDetailedInfo("Creating  NeuroSkySerialHandler ...");
	mHeadset = headset;
	mConnectionID = connectionID;

	mTimer = new QTimer();
	QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(ReadStream()));
	mTimer->setTimerType(Qt::PreciseTimer);
	mTimer->setInterval(2 * 1000 / mHeadset->GetSampleRate());
	mTimer->start();
}


////////////////////////////////////////////////////////////////


// try to read serial data
void NeuroSkySerialHandler::ReadStream()
{
	if (mHeadset->IsEnabled() == false)
		return;

	// attempt to read a packet of data from the connection
	while (TG_ReadPackets(mConnectionID, 1) == 1)
	{
		//const double totalElapsedTime = GetSession()->GetElapsedTime();

		// signal quality
		if (TG_GetValueStatus(mConnectionID, TG_DATA_POOR_SIGNAL) != 0)
		{
			// get the value that describes how poor the signal measured by the ThinkGear is
			// range of the value is [0, 200], any non-zero value indicated that some sort of noise contamination is detected
			// the value of 200 has a special meaning and says that the ThinkGear contacts are not touching the user's skin
			const float signalQuality = TG_GetValue(mConnectionID, TG_DATA_POOR_SIGNAL);

			Sensor::EContactQuality quality;
			if (signalQuality == 0.0f)
				quality = Sensor::EContactQuality::CONTACTQUALITY_GOOD;
			else if (signalQuality < 100.0f)
				quality = Sensor::EContactQuality::CONTACTQUALITY_FAIR;
			else if (signalQuality < 200.0f)
				quality = Sensor::EContactQuality::CONTACTQUALITY_POOR;
			else
				quality = Sensor::EContactQuality::CONTACTQUALITY_NO_SIGNAL;

			mHeadset->GetRawSensor()->SetContactQuality(quality);
		}

		// battery status level
		/*if (TG_GetValueStatus(mConnectionID, TG_DATA_BATTERY) != 0)
		{
			const float batteryLevel = TG_GetValue(mConnectionID, TG_DATA_BATTERY);
			mHeadset->SetBatteryChargeLevel(batteryLevel);

			// TODO: HACK: The battery level that is returned is not valid. It always returns empty battery, no matter how charged the battery is.
			mHeadset->SetBatteryChargeLevel(1.0f);
		}*/

		// get the raw value from the electrode
		if (TG_GetValueStatus(mConnectionID, TG_DATA_RAW) != 0)
		{
			const float rawData = TG_GetValue(mConnectionID, TG_DATA_RAW);
			mHeadset->GetRawSensor()->AddQueuedSample(rawData);
		}

		// get the eSense attention value
		if (TG_GetValueStatus(mConnectionID, TG_DATA_ATTENTION) != 0)
		{
			const float attention = TG_GetValue(mConnectionID, TG_DATA_ATTENTION) * 0.01f;
			mHeadset->GetAttentionSensor()->AddQueuedSample(attention);
		}

		// get the eSense meditation value
		if (TG_GetValueStatus(mConnectionID, TG_DATA_MEDITATION) != 0)
		{
			const float meditation = TG_GetValue(mConnectionID, TG_DATA_MEDITATION) * 0.01f;
			mHeadset->GetMeditationSensor()->AddQueuedSample(meditation);
		}

		// get the delta frequency band value (0.5 - 2.75 Hz)
		if (TG_GetValueStatus(mConnectionID, TG_DATA_DELTA) != 0)
		{
			const float delta = TG_GetValue(mConnectionID, TG_DATA_DELTA);
			mHeadset->GetDeltaSensor()->AddQueuedSample(delta);
		}

		// get the theta frequency band value (3.5 - 6.75 Hz)
		if (TG_GetValueStatus(mConnectionID, TG_DATA_THETA) != 0)
		{
			const float theta = TG_GetValue(mConnectionID, TG_DATA_THETA);
			mHeadset->GetThetaSensor()->AddQueuedSample(theta);
		}

		// get the alpha 1 frequency band value (7.5 - 9.25 Hz)
		if (TG_GetValueStatus(mConnectionID, TG_DATA_ALPHA1) != 0)
		{
			const float alpha1 = TG_GetValue(mConnectionID, TG_DATA_ALPHA1);
			mHeadset->GetAlpha1Sensor()->AddQueuedSample(alpha1);
		}

		// get the alpha 2 frequency band value (10 - 11.75 Hz)
		if (TG_GetValueStatus(mConnectionID, TG_DATA_ALPHA2) != 0)
		{
			const float alpha2 = TG_GetValue(mConnectionID, TG_DATA_ALPHA2);
			mHeadset->GetAlpha2Sensor()->AddQueuedSample(alpha2);
		}

		// get the beta 1 frequency band value (13 - 16.75 Hz)
		if (TG_GetValueStatus(mConnectionID, TG_DATA_BETA1) != 0)
		{
			const float beta1 = TG_GetValue(mConnectionID, TG_DATA_BETA1);
			mHeadset->GetBeta1Sensor()->AddQueuedSample(beta1);
		}

		// get the beta 2 frequency band value (18 - 29.75 Hz)
		if (TG_GetValueStatus(mConnectionID, TG_DATA_BETA2) != 0)
		{
			const float beta2 = TG_GetValue(mConnectionID, TG_DATA_BETA2);
			mHeadset->GetBeta2Sensor()->AddQueuedSample(beta2);
		}

		// get the gamma 1 frequency band value (31 - 39.75 Hz)
		if (TG_GetValueStatus(mConnectionID, TG_DATA_GAMMA1) != 0)
		{
			const float gamma1 = TG_GetValue(mConnectionID, TG_DATA_GAMMA1);
			mHeadset->GetGamma1Sensor()->AddQueuedSample(gamma1);
		}

		// get the gamma 2 frequency band value (41 - 49.75 Hz)
		if (TG_GetValueStatus(mConnectionID, TG_DATA_GAMMA2) != 0)
		{
			const float gamma2 = TG_GetValue(mConnectionID, TG_DATA_GAMMA2);
			mHeadset->GetGamma2Sensor()->AddQueuedSample(gamma2);
		}

		// get the blink strength value (range [1, 255])
		/*if (TG_GetValueStatus(mConnectionID, TG_DATA_BLINK_STRENGTH) != 0)
		{
			float blinkStrength = TG_GetValue(mConnectionID, TG_DATA_BLINK_STRENGTH);

			// is between 0..255 (also looks like it only sends integer numbers)
			blinkStrength /= 255.0;

			// this value does not have a constant sample rate! blinks happen whenever they are detected (event signal)
			// TODO change this as soon event channels are implemented
			mHeadset->GetEyeBlinkSensor()->AddQueuedSample(blinkStrength);
		}*/
	}
}

#endif
