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
#include "AbmThreadHandler.h"
#include "AbmDriver.h"

#ifdef INCLUDE_DEVICE_ADVANCEDBRAINMONITORING


using namespace Core;


// constructor
AbmThreadHandler::AbmThreadHandler(AbmDriver* driver)
{
	mDriver	= driver;
	mMode = MODE_DETECTDEVICES;
	mDetectOnce = false;
	mBreak = false;

	// register error callback
	if (RegisterCallbackOnError(AbmDeviceErrorCallback) == false)
		LogError("ABM driver failed to register device error callback");
	
	// register status callback for battery state
	if (RegisterCallbackOnStatusInfo(AbmDeviceOnStatusInfoCallback) == false)
		LogError("ABM driver failed to register device status callback");
	
}


// destructor
AbmThreadHandler::~AbmThreadHandler()
{
}


// main function
void AbmThreadHandler::Execute()
{
	mIsFinished = false;
	while (mBreak == false)
	{
		switch (mMode)
		{
			case AbmThreadHandler::MODE_DETECTDEVICES:
			{
				if (mDriver->IsAutoDetectionEnabled() == true || mDetectOnce == true)
				{
					mDriver->mIsSearching = true;
					DetectDevices();
				}

				mDetectOnce = false;
				mDriver->mIsSearching = false;

				// sleep if no device was detected
				if (mMode == AbmThreadHandler::MODE_DETECTDEVICES)
					Sleep(1000);

				break;
			}

			case AbmThreadHandler::MODE_DATAACQUISITION:
			{
				if (mDriver->mDevice != NULL)
					DataAcquisition(mDriver->mDevice);

				// run with 500 Hz
				Sleep(2);

				break;
			}

			case AbmThreadHandler::MODE_IMPEDANCECHECK:		 /* not implemented */ Sleep(100); break;
			case AbmThreadHandler::MODE_TECHNICALMONITORING: /* not implemented */ Sleep(100); break;

			default:
			{
				break;
			}
		}
	}

	switch (mMode)
	{
		case MODE_DETECTDEVICES: break;
		case MODE_DATAACQUISITION: 
			StopAcquisition();
			break;
		case MODE_IMPEDANCECHECK: 
			StopImpedance();
			break;
		case MODE_TECHNICALMONITORING:
			StopTechnicalMonitoring();
			break;
	}
	
	
	mMode = MODE_DETECTDEVICES;
	mIsFinished = true;
	mBreak = false;
}


// terminate
void AbmThreadHandler::Terminate()
{
	mBreak = true;
}


// device detection
void AbmThreadHandler::DetectDevices()
{
	// can only support one device right now
	if (mDriver->mDevice != NULL)
	{
		LogInfo("Already have an ABM device, can't add more. Detection terminated.");
		return;
	}

	// check if there is a device
	// Note: we have to remember the device info becaues we get it only once.. its a really stupid SDK
	_DEVICE_INFO* deviceInfo = NULL;
	if (mDriver->mHasPreviousDeviceInfo == true)
	{
		deviceInfo = &mDriver->mPreviousDeviceInfo;
	}
	else
	{
		deviceInfo = GetDeviceInfo();
		if (deviceInfo != NULL && deviceInfo->nNumberOfChannel != -1)
		{
			mDriver->mPreviousDeviceInfo = *deviceInfo;
			mDriver->mHasPreviousDeviceInfo = true;
		}
	}

	// invalid device info
	if (deviceInfo == NULL || deviceInfo->nNumberOfChannel == -1)
	{
		LogError("ABM: Device info is invalid, cannot start.");
		return;	
	}


	// log device information
	mDriver->LogDeviceInfo(deviceInfo);

	// firmware version
	_BALERT_FIRMWARE_VERSION firmwareVersion;
	if (GetFirmwareVer(firmwareVersion) == ABM_RESULT_OK)
	{
		LogInfo("Advanced Brain Monitoring BCI:");
		LogInfo("   Headset Firmware Version: %s", firmwareVersion.ucFirmwareVersion);
		LogInfo("   Dongle Firmware Version: %s", firmwareVersion.ucDONGLEFirmwareVersion);
		LogInfo("   MCESU Firmware Version: %s", firmwareVersion.ucMCESUFirmwareVersion);
	}


	// determine device type from the number of channels
	uint32 coreDeviceType = AbmX24Device::TYPE_ID;
	int	abmDeviceType = ABM_DEVICE_X24Standard;
	
	
	// upload the definition file
	String definitionFile;

	switch (deviceInfo->nNumberOfChannel)
	{
		case 4:
			LogError("Cannot add ABM B-Alert X4 device (not implemented).");
			return;
			//	coreDeviceType = AbmX4Device::TYPE_ID;
			//	abmDeviceType = ABM_DEVICE_X4BAlert;
			//	break;
		case 10:
			coreDeviceType = AbmX10Device::TYPE_ID;
			abmDeviceType = ABM_DEVICE_X10Standard;
			definitionFile.Format("%sABM/X10_definition.def", FromQtString(GetQtBaseManager()->GetAppDir()).AsChar());
			break;
		case 24:
			coreDeviceType = AbmX24Device::TYPE_ID;
			abmDeviceType = ABM_DEVICE_X24Standard;
			definitionFile.Format("%sABM/X24_definition.def", FromQtString(GetQtBaseManager()->GetAppDir()).AsChar());
			break;

		default:
			LogError("Unknown ABM device \"%s\" (has %i channels).", deviceInfo->chDeviceName, deviceInfo->nNumberOfChannel);
			return;
	}

	if (SetDefinitionFile((char*)definitionFile.AsChar()) == false)
	{
		LogError("ABM definition file not set.");
	}

	// create and add device
	AbmDevice* device = static_cast<AbmDevice*>(mDriver->CreateDevice(coreDeviceType));
	CORE_ASSERT(device != NULL);
	LogInfo("Creating ABM device \"%s\".", device->GetName());

	// preliminary: load config from app folder
	String configFilename = FromQtString( GetQtBaseManager()->GetAppDir() ) + "ABM/device.config";
	Json jsonParser;
	if (jsonParser.ParseFile(configFilename.AsChar()) == true)
	{
		LogDebug("  ABM device config");
		jsonParser.Log();

		// raw/filtered/decon
		Json::Item eegModeItem = jsonParser.Find("filter");
		if (eegModeItem.IsString() == true)
		{
			String mode = eegModeItem.GetString();
			if (mode.CompareNoCase("none") == 0)
				device->SetFilterMode(AbmDevice::FILTERMODE_NONE);
			else if (mode.CompareNoCase("all") == 0)
				device->SetFilterMode(AbmDevice::FILTERMODE_ALL);
			else if (mode.CompareNoCase("decon") == 0)
				device->SetFilterMode(AbmDevice::FILTERMODE_DECON);
		}


		// brainstate classifier
		device->SetDataMode(AbmDevice::DATAMODE_RAW);
		Json::Item brainstateItem = jsonParser.Find("brainstate");
		if (brainstateItem.IsBool() == true && brainstateItem.GetBool() == true)
			device->SetDataMode(AbmDevice::DATAMODE_BRAINSTATE);

		// workload classifier
		Json::Item workloadItem = jsonParser.Find("workload");
		if (workloadItem.IsBool() == true && workloadItem.GetBool() == true)
			device->SetDataMode(AbmDevice::DATAMODE_WORKLOAD);
	}


	// initialize the ABM session
	int	abmSessionType = ABM_SESSION_RAW;
	
	// set session mode
	switch (device->GetDataMode())
	{
		default:
		case AbmDevice::DATAMODE_RAW:				abmSessionType = ABM_SESSION_RAW; break;
		case AbmDevice::DATAMODE_DECONTAMINATED:	abmSessionType = ABM_SESSION_DECON; break;
		case AbmDevice::DATAMODE_BRAINSTATE:		abmSessionType = ABM_SESSION_BSTATE; break;
		case AbmDevice::DATAMODE_WORKLOAD:			abmSessionType = ABM_SESSION_WORKLOAD; break;
	}

	if (InitSession(abmDeviceType, abmSessionType, -1, device->HasPlaybackFile()) != INIT_SESSION_OK)
	{
		LogError("ABM session initialization failed.");
		
		delete device;
		return;
	}


	// get the channel number information
	int numRawChannels, numDeconChannels, numPsdChannels, numRawPsdChannels, numQualityChannels;
	GetPacketChannelNmbInfo(numRawChannels, numDeconChannels, numPsdChannels, numRawPsdChannels, numQualityChannels);
	device->SetNumRawChannelsInternal(numRawChannels);

	LogInfo("Settings num raw channels to %i", numRawChannels);

	// start data acquisition
	if (StartAcquisition() != ACQ_STARTED_OK)
	{
		LogError("ABM acquisition start failed.");

		delete device;
		return;
	}

	// load playback file, if set
	if (device->HasPlaybackFile() == true)
	{
		_SESSION_INFO sessionInfo = { 0 };
		sessionInfo.bApply65HzFilter = true;
		sessionInfo.bBrainState = 0x3f;
		sessionInfo.bPlayEbsMode = 1;
		int deviceType = ABM_DEVICE_X24Standard;

		if (OpenCustom((char*)device->GetPlaybackFilename(), &sessionInfo, deviceType) == false)
		{
			LogError("Cannot load .ebs file.");
			
			delete device;
			return;
		}

		double pauseInMS = 1.0 / device->GetSampleRate();
		if (PlayFile() == false)
		{
			LogError("Cannot play .ebs file ");
			
			delete device;
			return;
		}
	}

	// add device to engine
	mDriver->mDevice = device;
	GetEngine()->GetDeviceManager()->AddDeviceAsync(device);

	// switch into aquisition / streaming mode
	mMode = MODE_DATAACQUISITION;
}


void AbmThreadHandler::DetectDevicesOnce()
{
	mDetectOnce = true;
	mMode = MODE_DETECTDEVICES;
	mDriver->mIsSearching = true;
}


void AbmThreadHandler::DataAcquisition(AbmDevice* device)
{
	const uint32 numRawChannels = device->GetNumRawChannelsInternal();
	const bool playMode = device->HasPlaybackFile();

	//
	// Raw data
	//

	// NOTE: pointer to array of float values containing raw data samples.
	// the size of the return array = (nChannel+6)*numRawSamples, where, nChannel is the number of channels in the ABM device (see GetPacketChannelNmbInfo), numRawSamples is the number of samples acquired.
	// the number of samples will vary based on the delay between successive calls to the function.
	int		numRawSamples = 0;
	float*	rawData = NULL;

	if (playMode == true)
	{
		switch (device->GetFilterMode())
		{
			case AbmDevice::FILTERMODE_NONE:	rawData = GetRawDataPE(numRawSamples); 		break;
			case AbmDevice::FILTERMODE_ALL:		rawData = GetFilteredDataPE(numRawSamples); break;
			case AbmDevice::FILTERMODE_DECON:	rawData = GetDeconDataPE(numRawSamples);	break;
		}
	}
	else
	{
		// normal aquisition
		switch (device->GetFilterMode())
		{
			case AbmDevice::FILTERMODE_NONE:	rawData = GetRawData(numRawSamples);		break;
			case AbmDevice::FILTERMODE_ALL:		rawData = GetFilteredData(numRawSamples);	break;
			case AbmDevice::FILTERMODE_DECON:	rawData = GetDeconData(numRawSamples);	break;
		}
	}

	// do we have samples at all?
	if (numRawSamples > 0 && rawData != NULL)
	{
		// ; data to sensors
		float* dataPtr = rawData;
		for (int i = 0; i<numRawSamples; ++i)
		{
			// read header
			const float epoch		= *dataPtr; dataPtr++;
			const float pffset		= *dataPtr; dataPtr++;
			const float hour		= *dataPtr; dataPtr++;
			const float min			= *dataPtr; dataPtr++;
			const float sec			= *dataPtr; dataPtr++;
			const float milliSec	= *dataPtr; dataPtr++;

			// read samples
			for (uint32 j = 0; j<numRawChannels; ++j)
			{
				Sensor* sensor = mDriver->GetRawSensorByIndex(device, j);
				if (sensor != NULL)
					sensor->AddQueuedSample(*dataPtr);
				dataPtr++;
			}
		}
	}


	//
	// Brain states
	//

	int numBrainStateEpochs = 0;
	_BRAIN_STATE* brainState= NULL;

	if (playMode == true)	{ brainState = GetBrainStatePE(numBrainStateEpochs); }
	else					{ brainState = GetBrainState(numBrainStateEpochs); }

	if (brainState != NULL)
	{
		// FIXME we only get on value.. does this have more than one in the epoch? probably not because the samplerate is so low
		for (int i = 0; i < numBrainStateEpochs; i++)
		{


			//fEpoch;
			//fABMSDKTimeStampHour;
			//fABMSDKTimeStampMinute;
			//fABMSDKTimeStampSecond;
			//fABMSDKTimeStampMilsecond;

			if (brainState->fClassificationEstimate >= 0.0f)
				device->GetClassificationSensor()->AddQueuedSample(brainState->fClassificationEstimate);
			if (brainState->fHighEngagementEstimate >= 0.0f)
				device->GetHighEngagementSensor()->AddQueuedSample(brainState->fHighEngagementEstimate);
			if (brainState->fLowEngagementEstimate >= 0.0f)
				device->GetLowEngagementSensor()->AddQueuedSample(brainState->fLowEngagementEstimate);
			if (brainState->fDistractionEstimate >= 0.0f)
				device->GetDistractionSensor()->AddQueuedSample(brainState->fDistractionEstimate);
			if (brainState->fDrowsyEstimate >= 0.0f)
				device->GetDrowsySensor()->AddQueuedSample(brainState->fDrowsyEstimate);
		}

		//
		// Workload (contained within brainstate)
		//
		
		// FIXME we only get on value.. does this have more than one in the epoch? probably not because the samplerate is so low
		for (int i = 0; i < numBrainStateEpochs; i++)
		{
			//fEpoch;
			//fABMSDKTimeStampHour;
			//fABMSDKTimeStampMinute;
			//fABMSDKTimeStampSecond;
			//fABMSDKTimeStampMilsecond;

			if (brainState->fWorkloadAverage >= 0.0f)
				device->GetWorkloadAverageSensor()->AddQueuedSample(brainState->fWorkloadAverage);
			if (brainState->fWorkloadBDS >= 0.0f)
				device->GetWorkloadBDSSensor()->AddQueuedSample(brainState->fWorkloadBDS);
			if (brainState->fWorkloadFBDS >= 0.0f)
				device->GetWorkloadFBDSSensor()->AddQueuedSample(brainState->fWorkloadFBDS);
		}
	}

	//
	// Movement and Tilt
	//

	int		numMovementEpochs	= 0;
	float*	movementData		= NULL;

	if (playMode == true)	{ movementData = GetMovementDataPE(numMovementEpochs); }
	else					{ movementData = GetMovementData(numMovementEpochs); }

	if (movementData != NULL)
	{
		for (int i = 0; i < numMovementEpochs; i++)
		{
			const double movementValue = movementData[i*(2 + 5) + 5];
			const double movementLevel = movementData[i*(2 + 5) + 6];

			device->GetMovementValueSensor()->AddQueuedSample(movementValue);
			device->GetMovementLevelSensor()->AddQueuedSample(movementLevel);
		}
	}

	// tilt
	int		numTiltSamples	= 0;
	float*	tiltData		= NULL;

	if (playMode == true)	{ tiltData = GetAnglesDataPE(numTiltSamples); }
	else					{ tiltData = GetTiltAnglesData(numTiltSamples); }

	if (tiltData != NULL)
	{
		for (int i = 0; i < numTiltSamples; i++)
		{
			const double tiltX = tiltData[i*(6 + 3) + 6];
			const double tiltY = tiltData[i*(6 + 3) + 7];
			const double tiltZ = tiltData[i*(6 + 3) + 8];

			device->GetTiltXSensor()->AddQueuedSample(tiltX);
			device->GetTiltYSensor()->AddQueuedSample(tiltY);
			device->GetTiltZSensor()->AddQueuedSample(tiltZ);
		}
	}

	//
	// Signal Quality Data
	//

	// NOTE: pointer to array of float values containing overall data quality and the % of EMG and artifacts in each individual channel.
	// the size of the return array = (2*nChannel+6+1)*nEpoch, where, nChannel is the number of EEG channels in the device (see GetPacketChannelNmbInfo), nEpoch is the number of epochs.
	// the number of epochs may vary based on the delay between two calls to the function
	int		numChannelQualities	= 0;
	float*	channelQualityData	= NULL;

	if (playMode == true)	{ channelQualityData = GetQualityChannelDataPE(numChannelQualities); }
	else					{ channelQualityData = GetQualityChannelData(numChannelQualities); }

	// Note: read only the first value received, discard the rest
	if (channelQualityData != NULL && numChannelQualities > 0)
	{
		float* dataPtr = channelQualityData;

		// read header
		const float epoch = *dataPtr; dataPtr++;
		const float pffset = *dataPtr; dataPtr++;
		const float hour = *dataPtr; dataPtr++;
		const float min = *dataPtr; dataPtr++;
		const float sec = *dataPtr; dataPtr++;
		const float milliSec = *dataPtr; dataPtr++;

		// read channel values
		const uint32 numNeuroChannels = device->GetNumNeuroSensors();
		for (uint32 j = 0; j < numNeuroChannels; ++j)
		{
			const float percentEMG = *dataPtr; dataPtr++;
			const float percentArtifacts = *dataPtr; dataPtr++;

			// use average emg + artefact content
			const double totalArtifacts = percentEMG + percentArtifacts;
			Sensor* sensor = device->GetNeuroSensor(j);
			if (sensor != NULL)
			{
				if (totalArtifacts >= 50.0)				sensor->SetContactQuality(Sensor::CONTACTQUALITY_VERY_BAD);
				else if (totalArtifacts >= 20.0)		sensor->SetContactQuality(Sensor::CONTACTQUALITY_POOR);
				else if (totalArtifacts >= 5.0)			sensor->SetContactQuality(Sensor::CONTACTQUALITY_FAIR);
				else									sensor->SetContactQuality(Sensor::CONTACTQUALITY_GOOD);
			}
		}
	}
}


#endif
