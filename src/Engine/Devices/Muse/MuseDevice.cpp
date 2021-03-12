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
#include "MuseDevice.h"
#include "../../EngineManager.h"
#include "../../Core/LogManager.h"

#ifdef INCLUDE_DEVICE_INTERAXON_MUSE

using namespace Core;

// constructor
MuseDevice::MuseDevice(DeviceDriver* driver) : BciDevice(driver)
{
	LogDetailedInfo("Constructing Muse headset ...");

	// muse device doesn't exist before connecting -> begins in IDLE state
	mState = STATE_IDLE;
	mPowerSupplyType = POWERSUPPLY_BATTERY;
	mIsTouchingForehead = 0;
	mOscPathPattern = "/" + Core::String(this->GetTypeName()) + "/*/*";

	// create all sensors
	CreateSensors();

	for (uint32 i=0; i<4; ++i)
		mRawEEGMultiplier[i] = 1.0;
}


// destructor
MuseDevice::~MuseDevice()
{
	LogDetailedInfo("Destructing Muse headset ...");
}


// get the available electrodes of the neuro headset
void MuseDevice::CreateElectrodes()
{
	mElectrodes.Clear();

	mElectrodes.Reserve( 4 );
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("TP9"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp1"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp2"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("TP10"));
}

void MuseDevice::CreateSensors()
{
	// create EEG sensors first
	BciDevice::CreateSensors();
	
	// TODO clean this up using AddSensor(...) ??
	
	mConcentrationSensor = new Sensor("Concentration", 10, 0.0);
	mConcentrationSensor->GetChannel()->SetMinValue(0.0);
	mConcentrationSensor->GetChannel()->SetMaxValue(1.0);
	mConcentrationSensor->GetChannel()->SetUnit("");
	AddSensor(mConcentrationSensor);

	mMellowSensor = new Sensor( "Mellow", 10, 0.0);
	mMellowSensor->GetChannel()->SetMinValue(0.0);
	mMellowSensor->GetChannel()->SetMaxValue(1.0);
	mMellowSensor->GetChannel()->SetUnit("");
	AddSensor(mMellowSensor);

	mDRLSensor = new Sensor( "DRL", 10);
	mDRLSensor->GetChannel()->SetMinValue(0.0);
	mDRLSensor->GetChannel()->SetMaxValue(3300000.0);
	mDRLSensor->GetChannel()->SetUnit("uV");
	AddSensor(mDRLSensor);

	mRefSensor = new Sensor( "Ref", 10);
	mRefSensor->GetChannel()->SetMinValue(0.0);
	mRefSensor->GetChannel()->SetMaxValue(3300000.0);
	mRefSensor->GetChannel()->SetUnit("uV");
	AddSensor(mRefSensor);

	mAccForwardSensor = new Sensor( "Acc (Forward)", 50);
	mAccForwardSensor->GetChannel()->SetMinValue(-2000.0);
	mAccForwardSensor->GetChannel()->SetMaxValue(1996.1);
	mAccForwardSensor->GetChannel()->SetUnit("mm/s^2");
	AddSensor(mAccForwardSensor);

	mAccUpSensor = new Sensor( "Acc (Up)", 50);
	mAccUpSensor->GetChannel()->SetMinValue(-2000.0);
	mAccUpSensor->GetChannel()->SetMaxValue(1996.1);
	mAccUpSensor->GetChannel()->SetUnit("mm/s^2");
	AddSensor(mAccUpSensor);

	mAccLeftSensor = new Sensor( "Acc (Left)", 50);
	mAccLeftSensor->GetChannel()->SetMinValue(-2000.0);
	mAccLeftSensor->GetChannel()->SetMaxValue(1996.1);
	mAccLeftSensor->GetChannel()->SetUnit("mm/s^2");
	AddSensor(mAccLeftSensor);

	mEyeBlinkSensor = new Sensor("Eye Blink", 10);
	mEyeBlinkSensor->GetChannel()->SetMinValue(0.0);
	mEyeBlinkSensor->GetChannel()->SetMaxValue(1.0);
	mEyeBlinkSensor->GetChannel()->SetUnit("");
	AddSensor(mEyeBlinkSensor);

	mJawClenchSensor = new Sensor("Jaw Clench", 10);
	mJawClenchSensor->GetChannel()->SetMinValue(0.0);
	mJawClenchSensor->GetChannel()->SetMaxValue(1.0);
	mJawClenchSensor->GetChannel()->SetUnit("");
	AddSensor(mJawClenchSensor);

	// contact quality (from "Horse Shoe")
	mTP9ContactQualitySensor = new Sensor("TP9 OK?", 10, 0.0);
	mTP9ContactQualitySensor->GetChannel()->SetMinValue(0.0);
	mTP9ContactQualitySensor->GetChannel()->SetMaxValue(1.0);
	mTP9ContactQualitySensor->GetChannel()->SetUnit("");
	AddSensor(mTP9ContactQualitySensor);
	mContactQualitySensors.Add(mTP9ContactQualitySensor);

	mFp1ContactQualitySensor = new Sensor("Fp1 OK?", 10, 0.0);
	mFp1ContactQualitySensor->GetChannel()->SetMinValue(0.0);
	mFp1ContactQualitySensor->GetChannel()->SetMaxValue(1.0);
	mFp1ContactQualitySensor->GetChannel()->SetUnit("");
	AddSensor(mFp1ContactQualitySensor);
	mContactQualitySensors.Add(mFp1ContactQualitySensor);

	mFp2ContactQualitySensor = new Sensor("Fp2 OK?", 10, 0.0);
	mFp2ContactQualitySensor->GetChannel()->SetMinValue(0.0);
	mFp2ContactQualitySensor->GetChannel()->SetMaxValue(1.0);
	mFp2ContactQualitySensor->GetChannel()->SetUnit("");
	AddSensor(mFp2ContactQualitySensor);
	mContactQualitySensors.Add(mFp2ContactQualitySensor);

	mTP10ContactQualitySensor = new Sensor("TP10 OK?", 10, 0.0);
	mTP10ContactQualitySensor->GetChannel()->SetMinValue(0.0);
	mTP10ContactQualitySensor->GetChannel()->SetMaxValue(1.0);
	mTP10ContactQualitySensor->GetChannel()->SetUnit("");
	AddSensor(mTP10ContactQualitySensor);
	mContactQualitySensors.Add(mTP10ContactQualitySensor);

	mTouchingForeheadSensor = new Sensor("Put On?", 10, 0.0);
	mTouchingForeheadSensor->GetChannel()->SetMinValue(0.0);
	mTouchingForeheadSensor->GetChannel()->SetMaxValue(1.0);
	mTouchingForeheadSensor->GetChannel()->SetUnit("");
	AddSensor(mTouchingForeheadSensor);
}

// muse-io OSC streaming format
void MuseDevice::ProcessMessage(OscMessageParser* message)
{
	// TODO call default baseclass ProcessMessage as soon as its implemented

	// do nothing if device was disabled
	if (IsEnabled() == false)
		return;

	// raw 4 channel eeg
	if (message->MatchAddress("/*/*/eeg") == true)
	{
		if (message->GetNumArguments() != 4)
			return;

		// add one sample to each channel

		// sensor order is: tp9, fp1, fp2, tp10
		for (uint32 i = 0; i<4; ++i)
		{
			//float compressedValue; (*message) >> compressedValue;
			float rawValue; (*message) >> rawValue;

			// from muse doc: To get microvolts: uV=(x/1023*quantizationMultiplier)*3.3V*(1/A)*1000000 where A = 1961 (gain)
			//const float rawValue = compressedValue / 1023.0 *  (float)mRawEEGMultiplier[i] * 3.3 * 1000000.0 / 1961.0;
			GetSensor(i)->AddQueuedSample(rawValue);
		}
	}

	// eeg quantization (the multiplier we have to use to "uncompressed" the eeg values)
	else if (message->MatchAddress("/*/*/eeg/quant") == true)
	{
		for (uint32 i = 0; i<4; ++i)
			(*message) >> mRawEEGMultiplier[i];
	}

	// handle dropped EEG samples
	else if (message->MatchAddress("/*/*/eeg/dropped_samples") == true)
	{
		// get number of dropped samples
		uint32 numDropped;
		(*message) >> numDropped;

		// add zero samples to all channels 
		for (uint32 i = 0; i<4; ++i)
			GetSensor(i)->HandleLostSamples(numDropped);
	}

	// sensor status 1 : user is wearing muse correctly
	else if (message->MatchAddress("/*/*/dsp/touching_forehead") == true)
	{
		(*message) >> mIsTouchingForehead;
		mTouchingForeheadSensor->AddQueuedSample(mIsTouchingForehead);
	}

	// contact quality, one value per sensor
	else if (message->MatchAddress("/*/*/dsp/horseshoe") == true)
	{
		// values: 1=good, 2=ok, >2 = bad

		// no contact -> set bad signal quality
		if (mIsTouchingForehead == 0)
		{
			for (uint32 i = 0; i<4; ++i)
				GetSensor(i)->SetContactQuality(Sensor::CONTACTQUALITY_NO_SIGNAL);
		}
		else
		{
			for (uint32 i = 0; i<4; ++i)
			{
				float contact; (*message) >> contact;
				if (contact <= 1.0f)		// good
				{
					GetSensor(i)->SetContactQuality(Sensor::CONTACTQUALITY_GOOD);
					mContactQualitySensors[i]->AddQueuedSample(1.0);
				}
				else if (contact <= 2.0f)	// ok
				{
					GetSensor(i)->SetContactQuality(Sensor::CONTACTQUALITY_FAIR);
					mContactQualitySensors[i]->AddQueuedSample(0.5);
				}
				else if (contact <= 3.0f)	// bad
				{
					GetSensor(i)->SetContactQuality(Sensor::CONTACTQUALITY_POOR);
					mContactQualitySensors[i]->AddQueuedSample(0.0);
				}
				else if (contact > 3.0f)	// bad
				{
					GetSensor(i)->SetContactQuality(Sensor::CONTACTQUALITY_VERY_BAD);
					mContactQualitySensors[i]->AddQueuedSample(0.0);
				}
			}
		}
	}

	// concentration // EXPERIMENTAL
	else if (message->MatchAddress("/*/*/dsp/experimental/concentration") == true)
	{
		float concentration; (*message) >> concentration;
		GetSensor(SENSOR_CONCENTRATION)->AddQueuedSample(concentration);
	}

	// mellow // EXPERIMENTAL
	else if (message->MatchAddress("/*/*/dsp/experimental/mellow") == true)
	{
		float mellow; (*message) >> mellow;

		GetSensor(SENSOR_MELLOW)->AddQueuedSample(mellow);
	}

	// eye blink
	else if (message->MatchAddress("/*/*/dsp/blink") == true)
	{
		int32 eyeBlink; (*message) >> eyeBlink;

		GetSensor(SENSOR_EYEBLINK)->AddQueuedSample(eyeBlink);
	}

	// jaw clench
	else if (message->MatchAddress("/*/*/dsp/jaw_clench") == true)
	{
		int32 jawClench; (*message) >> jawClench;

		GetSensor(SENSOR_JAWCLENCH)->AddQueuedSample(jawClench);
	}

	// DRL/Ref
	else if (message->MatchAddress("/*/*/drl") == true)
	{

		float drl; (*message) >> drl;
		float ref; (*message) >> ref;

		GetSensor(SENSOR_DRL)->AddQueuedSample(drl);
		GetSensor(SENSOR_REF)->AddQueuedSample(ref);
	}

	// accelerometer
	else if (message->MatchAddress("/*/*/acc") == true)
	{
		float forward; (*message) >> forward;
		float up;	   (*message) >> up;
		float left;    (*message) >> left;

		GetSensor(SENSOR_ACC_FORWARD)->AddQueuedSample(forward);
		GetSensor(SENSOR_ACC_UP)->AddQueuedSample(up);
		GetSensor(SENSOR_ACC_LEFT)->AddQueuedSample(left);
	}

	// handle dropped Accerlerometer samples
	else if (message->MatchAddress("/*/*/acc/dropped_samples") == true)
	{
		// get number of dropped samples
		uint32 numDropped;
		(*message) >> numDropped;

		// handle samples
		GetSensor(SENSOR_ACC_FORWARD)->HandleLostSamples(numDropped);
		GetSensor(SENSOR_ACC_UP)->HandleLostSamples(numDropped);
		GetSensor(SENSOR_ACC_LEFT)->HandleLostSamples(numDropped);
	}

	// battery
	else if (message->MatchAddress("/*/*/batt") == true)
	{
		// state of Charge, Divide this by 100 to get percentage of charge remaining, (e.g. 5367 is 53.67%) Range: 16 bit, 0-10000
		int32 charge = 0;
		(*message) >> charge;

		// Millivolts measured by Fuel Gauge, Range: 16bit, 3000-4200 mV.
		int32 fuelGaugeRange = 0;
		(*message) >> fuelGaugeRange;

		// Millivolts measured by ADC, Range: 16bits, 3200-4200 mV. Values below 3350 are not reliable(they will flat line and stop falling) and you can consider the battery close to dead at that point(about 5 mins left).
		int32 adcRange = 0;
		(*message) >> adcRange;

		// Temperature in degrees Celcius, signed integer, 1C Resolution, range is -40 to +125 C.
		int32 temperature = 0;
		(*message) >> temperature;

		SetBatteryChargeLevel ((double)charge / 10000.0 );
	}

}


#endif
