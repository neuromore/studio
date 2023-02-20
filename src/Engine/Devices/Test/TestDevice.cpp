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

// include required files
#include "TestDevice.h"
#include "../../EngineManager.h"
#include "../../Core/LogManager.h"
#include "../../Core/Math.h"

#include <limits>

#ifdef INCLUDE_DEVICE_TEST
	
using namespace Core;

// constructor
TestDevice::TestDevice(DeviceDriver* driver, uint32 sampleRate) : BciDevice()
{
	mDeviceDriver = driver;
	mSampleRate	= sampleRate;
	mClock.SetFrequency(sampleRate);
	mState = STATE_IDLE;

	srand(1);

	// init with battery for testing
	//mPowerSupplyType = POWERSUPPLY_BATTERY;
	//SetBatteryChargeLevel(1);
	
	// create all sensors
	CreateSensors();

	// set random time offset so it looks good
	const uint32 numSensors = mSensors.Size();
	mElectrodeTimeOffsets.Resize( numSensors );
	for (uint32 i=0; i<numSensors; i++) {
		// set min and max value for channel: Possible min/max range - [short::min,short::max].
		mSensors[i]->GetChannel()->SetMinValue(std::numeric_limits<short>::min());
		mSensors[i]->GetChannel()->SetMaxValue(std::numeric_limits<short>::max());
		mElectrodeTimeOffsets[i] = Math::RandD( 0.0, 100.0 );
	}

	// Mitsar long-term Crash example code:
	/*
	Array<Array<double>> bigbuffs;
	const uint32 numarrs = 23;
	for (uint32 i=0;i<numarrs;++i)
	{
		bigbuffs.AddEmpty();
		bigbuffs.GetLast().Resize(100);
	}

	int i=0;
	while(bigbuffs[0].GetPtr() != NULL)
	{
		for (uint32 j=0;j<numarrs;++j)
		{
			const uint32 size = bigbuffs[j].Size();
			bigbuffs[j].Resize(size + size / 4);
		}
		i++;
		LogInfo("Iteration %i: Size is %i", i , bigbuffs[0].Size());
	}
	*/


	/*
	Array<Channel<double>> bigbuffs;
	const uint32 numarrs = 23;
	for (uint32 i=0;i<numarrs;++i)
	{
		bigbuffs.AddEmpty();
		bigbuffs[0].SetSampleRate(500);
		bigbuffs[0].Clear();
	}

	int i=0;
	while(bigbuffs[0].GetRawArray().GetPtr() != NULL)
	{
		for (uint32 j=0;j<numarrs;++j)
		{
			for(uint32 k=0; k<50; ++k)
				bigbuffs[j].AddSample(j);
		}
		i++;
		LogInfo("Iteration %i: Size is %i", i , bigbuffs[0].GetNumSamples());
	}*/
}


// destructor
TestDevice::~TestDevice()
{
}


// get the available electrodes of the neuro headset
void TestDevice::CreateElectrodes()
{
	mElectrodes.Clear();
	mElectrodes.Reserve(8);
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("Pz") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("Cz") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("F3") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("F4") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("AF3") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("AF4") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("O1") );
	mElectrodes.Add( GetEEGElectrodes()->GetElectrodeByID("O2") );

/*
	for (uint32 i=0; i< EEGElectrodes::NUM; ++i)
		result.Add((EEGElectrodes::EElectrode)i);
*/
}


// update neuro headset
void TestDevice::Update(const Time& elapsed, const Time& delta)
{
	mClock.Update(elapsed, delta);

	// TEST low battery event
	//if (elapsed > 4)
	//	SetBatteryChargeLevel(0.1);

	// dont generate data if driver is disabled
	if (mDeviceDriver->IsEnabled() == false)
	{
		// device will be removed after timeout:
		BciDevice::Update(elapsed, delta);
		return;
	}

	// get number of new ticks from clock
	const uint32 numSamplesToAdd = mClock.GetNumNewTicks();

	// TEST drift correction
	//const uint32 numSamplesToAdd = mClock.GetNumNewTicks() * Random::RandD();

	// get the number of sensors, iterate through them and output the samples
	const uint32 numSensors = mSensors.Size();
	for (uint32 s = 0; s<numSensors; ++s)
	{
		// get the sensor
		Sensor* sensor = mSensors[s];

		// iterate through the samples, calculate the time and value and add them to the sensors channel
		for (uint32 i = 0; i<numSamplesToAdd; ++i)
		{
			// timestamp of the oldest
			const double sampleTime = mClock.GetTickTime(mClock.GetTick(i)).InSeconds();
			const double offsetSampleTime = mElectrodeTimeOffsets[s] + sampleTime;
			const double a = 100; // amplitude +-300uv
			const double dc = 0; // DC voltage offset

			// select test signal type here
			const uint32 signalType = 4;

			// value of the sample
			double value;
			switch (signalType)
			{		
				case 1: // variable amplitude, frequency and dc offset
					value	= (numSensors-s) * sin( 2.0 * Math::pi * (s+1) * offsetSampleTime ) + s * dc;
					break;

				case 2:	// variable frequency (dc = 0V)
					value = a * sin(2.0 * Math::pi * (s + 1) * offsetSampleTime) + dc;
					break;

				// equal distributed ac noise
				case 3:
					value = a * ((double)rand() / RAND_MAX -0.5) * 2.0 + dc;
					break;

				// poorly simulated brainwave (delta/alpha/SMR)
				case 4:
				{
					double var, offset, amplitude;
					value = 0;

					// offset sinuses 0..3
					offset = RemapRange( s, 0.0, numSensors, 0.4, 0.6 );
					amplitude = a + RemapRange( s, 0.0, numSensors, 0.0, a*0.25 );
					var = 1.0 + (sin((double)s + 2.0 * Math::pi * offsetSampleTime / 23.0) * sin(1.0 - 2.0 * Math::pi * offsetSampleTime / 13.0 ) +  sin((double)s - 2.0 * Math::pi * offsetSampleTime / 13.0 ) ) / 2.0;
					for (double j = 0.0; j <= 3.5; j+=offset)
						value += 0.1 * var * sin(fmod(j,0.11+(s+1)) + 2.0 * Math::pi * j * offsetSampleTime) * amplitude;

					// theta sinuses 3..8 Hz
					offset = RemapRange( s, 0.0, numSensors, 0.5, 1.0 );
					amplitude = a - RemapRange( s, 0.0, numSensors, 0.0, a*0.25 );
					var = 1.0 + (sin((double)s + 0.001743952 * offsetSampleTime * s) * sin(0.053309 * offsetSampleTime) + sin(0.1+0.04349 * offsetSampleTime) * sin((double)s - 2.0 * Math::pi * (0.0175543) * offsetSampleTime)) / 3.0;
					for (double j = 3.0; j <= 8.0; j+=offset)
						value +=  0.1 * var * sin(fmod(j,0.42+(s+1)) + 2.0 * Math::pi * j * offsetSampleTime) * amplitude;

					// alpha sinuses 8..12
					offset = RemapRange( s, 0.0, numSensors, 0.2, 0.4 );
					amplitude = a + RemapRange( s, 0.0, numSensors, 0.0, a*0.5 );
					var = 1.0 + (sin((double)s + 0.001284952 * offsetSampleTime * s) * sin(0.034309 * offsetSampleTime) + sin(0.1+0.01549 * offsetSampleTime) * sin((double)s - 2.0 * Math::pi * (0.0133543) * offsetSampleTime)) / 2.0;
					for (double j = 8.0; j <= 12.0; j+=offset)
						value +=  0.2 * var * sin(fmod(j,0.42+(s+1)) + 2.0 * Math::pi * j * offsetSampleTime) * amplitude;

					// SMR 13..16
					offset = RemapRange( s, 0.0, numSensors, 0.05, 0.2 );
					amplitude = a + RemapRange( s, 0.0, numSensors, 0.0, a*0.25 );
					var = 1.0 + (sin((double)s + 0.0284952 * offsetSampleTime) * sin(1.0+ 0.014309 * offsetSampleTime) + sin(0.01549 * offsetSampleTime) * sin((double)s - 2.0 + 2.0 * Math::pi * (0.033654) * offsetSampleTime) * sin(2.0 * Math::pi * (0.00765465) * offsetSampleTime))/2.0;
					for (double j = 13.0; j <= 16.0; j+=offset)
						value += 0.05 * var * sin(fmod(j,0.23+(s+1)) + 2.0 * Math::pi * j * offsetSampleTime) * amplitude;

					// dc noise
					//value += 0.1 * a * (double)rand() / RAND_MAX;

					// ac noise
					value += 0.3 * a * ((double)rand() / RAND_MAX -0.5) * 2.0;

					value += Math::RandD( -a * 0.05, a * 0.05 );
				}break;

				default:
					value = 0;
			}

			// add the sample
			sensor->AddQueuedSample(value);
		}
	}

	// mark clock ticks as processed
	mClock.ClearNewTicks();

	// update the neuro headset
	BciDevice::Update(elapsed, delta);
}


void TestDevice::Reset()
{
	// clear sensors
	Device::Reset();

	mClock.Reset();
}


void TestDevice::Sync(const Core::Time& time, bool usePadding)
{
	Device::Sync(time, usePadding);

	mClock.Reset();
	mClock.SetStartTime(time);
}

#endif
