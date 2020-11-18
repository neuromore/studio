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
#include "MitsarDevices.h"
#include "../../EngineManager.h"
#include "../../Core/LogManager.h"

#ifdef INCLUDE_DEVICE_MITSAR

using namespace Core;


MitsarDevice::MitsarDevice(DeviceDriver* driver) : BciDevice(driver)
{
	// start in connected state
	mState = STATE_IDLE;
}


MitsarDevice::~MitsarDevice()
{
}


void MitsarDevice::CreateSensors()
{
	// create EEG sensors first
	BciDevice::CreateSensors();

	// create impedance buffers for EEG sensors
	CreateImpedanceBuffers();
}


void MitsarDevice::CreateImpedanceBuffers()
{
	// store 50 samples of impedance values (assuming 125 Hz sample rate, which is the mitsar SDK default, the interval is 0.4 seconds)
	const uint32 impedanceBufferSize = 50;

	const uint32 numEEGSensors = GetNumNeuroSensors();
	const uint32 numImpedanceBuffers = numEEGSensors + 1;  // +1 for neutral impedance
	mImpedanceBuffers.Resize(numImpedanceBuffers);
	mImpedanceValues.Resize(numEEGSensors);	   // doesnt contain neutral impedance

	for (uint32 i = 0; i < numImpedanceBuffers; ++i)
	{
		// init buffer
		mImpedanceBuffers[i].SetBufferSize(impedanceBufferSize);
		mImpedanceBuffers[i].Clear();
	}

	// reset final value buffers
	for (uint32 i = 0; i < numEEGSensors; ++i)
		mImpedanceValues[i] = 1000.0;
}


double MitsarDevice::GetImpedance(uint32 neuroSensorIndex)
{
	if (neuroSensorIndex < mImpedanceValues.Size())
		return mImpedanceValues[neuroSensorIndex];

	return 0.0;
}


void MitsarDevice::ProcessMessage(OscMessageParser* message)
{

// do nothing if device was disabled
	if (IsEnabled() == false)
		return;

	const uint32 numArgs = message->GetNumArguments();

	// Raw EEG channels
	if (message->MatchAddress("/*/*/eeg") == true)
	{
		// verify the number of arguments: must be a multiple of numChannels (and +1)
		const uint32 numChannels = GetNumNeuroSensors();
		if (numArgs != numChannels + 1)
		{
			// wrong number of arguments
			LogDebug("MitsarDevice: %s received wrong number of arguments for /eeg address (got %i for %i channels)", message->GetAddress(), numArgs, numChannels+1);
			return;
		}

		// first value is the sample counter (number of samples sent so far, including the ones that are contained in this message)
		int32 sampleCounter = 0;
		(*message) >> sampleCounter;

		// iterate over channels, and break early if max index is reached (in case argument count is wrong)
		float value;
		for (uint32 c = 0; c < numChannels; ++c)
		{
			// read argument and push sample into sensor queue
			(*message) >> value;
			GetNeuroSensor(c)->AddQueuedSample(value);
		}

		// increase local sample counter
		mSampleCounter++;

		//// verify that sample counters match
		//if (mSampleCounter != sampleCounter)
		//{
		//	// counter of transmitter lags behind our own -> something has gone wrong, or the uint32 limit was reached (unlikely)
		//	// -> reset counter
		//	if (sampleCounter < mSampleCounter)
		//		mSampleCounter = sampleCounter;
		//	else
		//	{
		//		// packets were lost or the order got switched.. for now we assume they were dropped
		//		// TODO handle out of order packets!
		//		if (sampleCounter > mSampleCounter)
		//		{
		//			const uint32 numLostSamples = sampleCounter - mSampleCounter;
		//			
		//			// handle lost samples
		//			const uint32 numSensors = GetNumSensors();
		//			for (uint32 c = 0; c < numSensors; ++c)
		//				GetSensor(c)->HandleLostSamples(numLostSamples);
		//		}
		//	}
		//}
	}
	// Impedance values during impedance mode
	else if (message->MatchAddress("/*/*/imp") == true)
	{
		// verify the number of arguments: must be a multiple of numChannels (and +1)
		const uint32 numChannels = GetNumNeuroSensors();
		if (numArgs != numChannels + 2)  // (+1 for sample counter, +1 for impedance of N channel
		{
			// wrong number of arguments
			LogDebug("MitsarDevice: %s received wrong number of arguments for /eeg address (got %i for %i channels)", message->GetAddress(), numArgs, numChannels + 1);
			return;
		}

		// first value is the sample counter (number of samples sent so far, including the ones that are contained in this message)
		int32 sampleCounter = 0;
		(*message) >> sampleCounter;

		// iterate over channels, and break early if max index is reached (in case argument count is wrong)
		float value = 0.0f;
		for (uint32 c = 0; c < numChannels; ++c)
		{
			// read impedance samples
			(*message) >> value;
			mImpedanceBuffers[c].AddSample(Math::Abs(value));
		}

		// impedance of neutral channel
		(*message) >> value;
		mImpedanceBuffers[numChannels].AddSample(Math::Abs(value));

		// call keepalive because we dont add sample to the sensors
		KeepAlive();

		// postprocessing: subtract neutral value
		const double maxImpedance = 250;

		// calculate neutral impedance first
		double neutralImpedance = maxImpedance;
		mImpedanceBuffers[numChannels].CalculateAverage(&neutralImpedance);

		// update impedance values
		for (uint32 c = 0; c < numChannels; ++c)
		{
			double electrodeImpedance = maxImpedance;
			mImpedanceBuffers[c].CalculateAverage(&electrodeImpedance);

			// set to 1Mohm impedance in case electrode or neutral dont look like they are connected
			if (electrodeImpedance >= maxImpedance || neutralImpedance  >= maxImpedance)
				mImpedanceValues[c] = 1000;
			
			// actual electrode impedance is electrode impedance - neutral impedance
			else 
				mImpedanceValues[c] = Max(0.0, electrodeImpedance - neutralImpedance);
		}


		//// iterate over channels, and break early if max index is reached (in case argument count is wrong)
		//float value = 0.0f;
		//for (uint32 c = 0; c < numChannels; ++c)
		//{
		//	// read impedance samples
		//	(*message) >> value;
		//	mImpedanceValues[c] = value;
		//}

	}
	// device model version
	else if (message->MatchAddress("/*/*/version") == true)
	{
		if (numArgs == 0)
			return;

		int versionNumber = 0;
		(*message) >> versionNumber;

		// update hardware name
		mHardwareName.Format("Mitsar-EEG 201 v%i", versionNumber);
	}
	else
	{
		// process all other messages in baseclass
		Device::ProcessMessage(message);
	}
	

}


//
// Mitsar 201-21
//

// constructor
Mitsar201Device::Mitsar201Device(DeviceDriver* driver) : MitsarDevice(driver)
{
	LogDetailedInfo("Constructing Mitsar-EEG 201 headset ...");
	
	// init with error hardware name so the device status widget will display the mitsar error image instead of the hardware image - this notifies the user that a mitsar connector is still running (e.g. because the studio crashed). This works because the real hardware name is only transmitted and set when mitsar connector is started.
	mHardwareName = "Mitsar (Error)";
	mOscPathPattern = "/" + Core::String(this->GetTypeName()) + "/*/*";

	// create all sensors
	CreateSensors();
}


// destructor
Mitsar201Device::~Mitsar201Device()
{
	LogDetailedInfo("Destructing Mitsar-EEG 201 headset ...");
}


// get the available electrodes of the neuro headset
void Mitsar201Device::CreateElectrodes()
{
	mElectrodes.Clear();

	mElectrodes.Reserve(23);
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp1"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fpz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp2"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F7"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F3"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F4"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F8"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T3"));		// modified 10-20
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C3"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Cz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C4"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T4"));		// modified 10-20
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T5"));		// modified 10-20
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P3"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Pz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P4"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T6"));		// modified 10-20
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O1"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Oz"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O2"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("A1"));
	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("A2"));
}


void Mitsar201Device::CreateSensors()
{
	// create EEG sensors first
	MitsarDevice::CreateSensors();

	// create poly channels
	String tempString;
	const uint32 polySampleRate = GetSampleRate();
	mBioSensors.Reserve(4);
	const uint32 numBioSensor = 4;
	for (uint32 i=0; i<numBioSensor; ++i)
	{
		tempString.Format("Poly%i", i+1);
		Sensor* bioSensor = new Sensor(tempString.AsChar(), polySampleRate);
		bioSensor->GetChannel()->SetMinValue(-DBL_MAX);
		bioSensor->GetChannel()->SetMaxValue(DBL_MAX);
		bioSensor->GetChannel()->SetUnit("");

		mBioSensors.Add(bioSensor);
		AddSensor(bioSensor);
	}
}



void Mitsar201Device::ProcessMessage(OscMessageParser* message)
{
	// 4x Poly channels
	if (message->MatchAddress("/*/*/bio") == true)
	{
		// verify the number of arguments: must be a multiple of numChannels (and +1)
		const uint32 numArgs = message->GetNumArguments();
		if (numArgs != 4 + 1)
		{
			LogDebug("MitsarDevice: %s received wrong number of arguments for /bio address (got %i for %i channels)", message->GetAddress(), numArgs, 5);
			return;
		}

		// first value is the sample counter (number of samples sent so far, including the ones that are contained in this message)
		int32 sampleCounter = 0;
		(*message) >> sampleCounter;

		// iterate over channels, and break early if max index is reached (in case argument count is wrong)
		float value = 0;
		for (int32 c = 0; c < 4; ++c)
		{
			// read argument and push sample into sensor queue
			(*message) >> value;
			mBioSensors[c]->AddQueuedSample(value);
		}
	}
	else
	{
		// process EEG and other message in baseclass
		MitsarDevice::ProcessMessage(message);
	}
}

//
//
//// get the available electrodes of the neuro headset
//void Mitsar20231Device::CreateElectrodes()
//{
//	mElectrodes.Clear();
//
//	mElectrodes.Reserve(31);
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp1"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fpz"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp2"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F7"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F3"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fz"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F4"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F8"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FT7"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC3"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FCz"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC4"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FT8"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T3"));		// modified 10-20
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C3"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Cz"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C4"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T4"));		// modified 10-20
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("TP7"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP3"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CPz"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP4"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("TP8"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T5"));		// modified 10-20
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P3"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Pz"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P4"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T6"));		// modified 10-20
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O1"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Oz"));
//	mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O2"));
//
//	// TODO add stimulation input channels
//}
#endif
