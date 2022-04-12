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
#include "EmotivDriver.h"
#include <Core/LogManager.h>
#include <QMessageBox>

#ifdef INCLUDE_DEVICE_EMOTIV

// link to the EDK libraries
//#pragma comment(lib, "edk.lib")

// NOTE:
// The raw EEG starts life as an unsigned integer and is rescaled, where (approximately) 4000 points represents the DC level, and 1 point is 1uV. 

using namespace Core;

// constructor
EmotivDriver::EmotivDriver() : DeviceDriver(false)
{
	LogDetailedInfo("Constructing Emotiv driver ...");

	AddSupportedDevice(EmotivEPOCDevice::TYPE_ID);
	AddSupportedDevice(EmotivInsightDevice::TYPE_ID);

	// initialize member variables
	mDataSizeInSeconds	= 1.0f;
	mTimeFromStart		= 0.0f;
	mDetectOnce			= false;
	mIsSearching		= false;
}


// destructor
EmotivDriver::~EmotivDriver()
{
	LogDetailedInfo("Destructing Emotiv driver ...");

	// remove event handler
	CORE_EVENTMANAGER.RemoveEventHandler(this);

	// shutdown Emotiv engine
	EE_EngineDisconnect();

	// get rid of helper handles
	EE_EmoStateFree(mStateHandle);
	EE_EmoEngineEventFree(mEventHandle);
	EE_DataFree(mDataHandle);
}


// initialization
bool EmotivDriver::Init()
{
	LogInfo("Initializing Emotiv driver (%s)...", GetSoftwareVersionString().AsChar() );

	// initialize Emotiv engine
	int error = 0;
	if ( (error = EE_EngineConnect()) != EDK_OK)
	{
		LogError("Failed to connect to Emotiv engine (0x%x)", error);
		mInitialized = false;
		return false;
	}
	else
	{
		LogInfo( "Emotiv EPOC engine initialized." );
		mInitialized = true;
	}

	// create helper handles
	mEventHandle = EE_EmoEngineEventCreate();
	mStateHandle = EE_EmoStateCreate();
	mDataHandle  = EE_DataCreate();

	// initialize the state handle
	ES_Init( mStateHandle );

	// sets the size of the data buffer
	// the size of the buffer affects how frequent EE_DataUpdateHandle() needs to be called to prevent data loss
	EE_DataSetBufferSizeInSec( mDataSizeInSeconds );

	// register event handler for reacting to device removal
	CORE_EVENTMANAGER.AddEventHandler(this);

	LogDetailedInfo("Emotiv EPOC driver initialized ...");
	return true;
}


// update the emotiv manager
void EmotivDriver::Update(const Time& elapsed, const Time& delta)
{
	// do not detect devices if system is disabled
	if (IsEnabled() == false)
		return;

	// process all new events from the emotiv engine
	int state;
	while ( (state = IEE_EngineGetNextEvent(mEventHandle)) == EDK_OK)
	{
		// get the event type and the user id from which the event got fired
		unsigned int userId = 0;
		IEE_Event_t eventType = IEE_EmoEngineEventGetType(mEventHandle);
		if (IEE_EmoEngineEventGetUserId(mEventHandle, &userId) != EDK_OK)
		{
			LogError("Emotiv Driver failed reading user ID!");
			continue;
		}

		switch (eventType)
		{
			// user was added (dongle plugged in, or headset turned on)
			case IEE_UserAdded:
			{
				LogInfo( "Emotiv user added. Connection established (UserId=%i)", userId );

				if (IEE_DataAcquisitionEnable(userId, true) == EDK_OK)
					LogInfo( "Data aquisition enabled for Emotiv user %i.", userId );
				else
					LogError("Data aquisition failed for Emotiv user %i.", userId);

				break;
			}

			// check if a neuro headset disconnected
			case IEE_UserRemoved:
			{
				LogInfo( "Emotiv USB dongle pulled out." );
				Device* headset = FindDeviceByEmotivUserId(userId);
				if (headset == NULL)
					LogWarning("Cannot add remove Emotiv EPOC headset. Headset with user id %i does not exists.", userId);
				else
					GetDeviceManager()->RemoveDeviceAsync( headset );

				break;
			}

			// check if the engine state got updated
			case IEE_EmoStateUpdated:
			{
				// update the state
				if (IEE_EmoEngineEventGetEmoState(mEventHandle, mStateHandle) != EDK_OK)
				{
					LogError("Emotiv Driver failed reading Emo State!");
					continue;
				}
				
				//
				// get the device and check if its an insight or epoc
				
				Device* emotivDevice = FindAndCreateDeviceByEmotivUserId(userId);
				
				// device was not created yet
				if (emotivDevice == NULL)
					continue;

				// device is an epoc : use epoc SDKs for reading emo events
				if (emotivDevice->GetType() == EmotivEPOCDevice::TYPE_ID)
				{
					EmotivEPOCDevice* epocDevice = static_cast<EmotivEPOCDevice*>(emotivDevice);
					DoEmoStateUpdateEPOC(epocDevice);
				} 

				// this is an insight device : use insight sdk for reading emo events
				else if (emotivDevice->GetType() == EmotivInsightDevice::TYPE_ID)
				{
					EmotivInsightDevice* insightDevice = static_cast<EmotivInsightDevice*>(emotivDevice);
					DoEmoStateUpdateInsight(insightDevice);
				}
				
				break;
			}

			// profile event
			case IEE_ProfileEvent:
			{
				LogInfo( "EE_ProfileEvent" );
				break;
			}

			
			// cognitive event
			case EE_CognitivEvent:		// Note: does not exist in Insight SDK
			{
				LogInfo( "EmotivDriver: received EE_CognitivEvent" );
				break;
			}

			// expressive event
			case EE_ExpressivEvent:		// Note: does not exist in Insight SDK
			{
				LogInfo( "EmotivDriver: received EE_ExpressivEvent" );
				break;
			}

			// error state
			case IEE_EmulatorError:		
			{
				LogError( "Emotiv EmoEngine internal error (IEE_EmulatorError)." );
				break;
			}
		}
	}

	// stream device data update
	const uint32 numDevices = mDevices.Size();
	for (uint32 i = 0; i < numDevices; ++i)
	{
		Device* emotivDevice = mDevices[i]; 
		if (emotivDevice->IsEnabled() == false)
			continue;

		uint32 userId = mUserIDs[i];

		if (emotivDevice->GetType() == EmotivEPOCDevice::TYPE_ID)
		{
			EmotivEPOCDevice* epocDevice = static_cast<EmotivEPOCDevice*>(emotivDevice);
			DoDataUpdateEPOC(epocDevice, userId);
		}

		// this is an insight device : use insight sdk for reading emo events
		else if (emotivDevice->GetType() == EmotivInsightDevice::TYPE_ID)
		{
			EmotivInsightDevice* insightDevice = static_cast<EmotivInsightDevice*>(emotivDevice);
			DoDataUpdateInsight(insightDevice, userId);
		}
	}

	// get the time since Emo Engine has been successfully connected to the headset
	mTimeFromStart = ES_GetTimeFromStart( mStateHandle );
}


// handle state update of epoc devices
void EmotivDriver::DoEmoStateUpdateEPOC(EmotivEPOCDevice* epocDevice)
{
	// set the signal strength
	IEE_SignalStrength_t signalStrength = IS_GetWirelessSignalStatus(mStateHandle);
	epocDevice->SetWirelessSignalQuality( FromEmotivWirelessSignalStatus(signalStrength) );

	// set if the headset has been put on correctly or not
	//int headsetOn = ES_GetHeadsetOn(mStateHandle);
	//epocDevice->SetHeadsetOnCorrectly(headsetOn == 1);

	// set the battery charge level
	int chargeLevel, maxChargeLevel;
	IS_GetBatteryChargeLevel(mStateHandle, &chargeLevel, &maxChargeLevel);
	epocDevice->SetBatteryChargeLevel( (double)chargeLevel / (double)maxChargeLevel ) ;

	const int numContactQualityChannels = ES_GetNumContactQualityChannels(mStateHandle);

	// get the contact quality for the input sensors
	for (int i = 0; i < numContactQualityChannels; ++i)
	{
		// get the contact quality for the given EEG eletrode
		IEE_EEG_ContactQuality_t contactQuality = IS_GetContactQuality(mStateHandle, (IEE_InputChannels_t)i);

		// retrieve input sensor information
		InputSensorDescriptor_t sensorData;
		if (EE_HeadsetGetSensorDetails((EE_InputChannels_t)i, &sensorData) != EDK_OK)
		{
			LogError("Failed to get input sensor descriptor for sensor %i.", i);
			continue;
		}

		// make sure the channel id conversion we did is valid
		CORE_ASSERT((EE_InputChannels_t)i == sensorData.channelId);

		// skip the sensors that do not exist on our neuro headset
		int exists = sensorData.fExists;
		if (exists == false)
			continue;

		// get the sensor name
		mTempSensorName = sensorData.pszLabel;

		// find the corresponding sensor
		Sensor* sensor = epocDevice->FindSensorByName(mTempSensorName.AsChar());
		if (sensor != NULL)
		{
			// set the sensor information
			sensor->SetContactQuality(FromEmotivContactQuality(contactQuality));
			//sensor->SetPosition( Vector3(sensorData.yLoc, sensorData.zLoc, -sensorData.xLoc) );
		}
	}

	// EXPRESSIVE

	// eye blink
	double isBlink = 0.0;
	//if (ES_ExpressivIsActive(mStateHandle, EXP_BLINK) == 1)
	isBlink = ES_ExpressivIsBlink(mStateHandle);
	epocDevice->mBlinkSensor->AddQueuedSample(isBlink);

	// wink left
	double winkLeft = 0.0;
	//if (ES_ExpressivIsActive(mStateHandle, EXP_WINK_LEFT) == 1)
	winkLeft = ES_ExpressivIsLeftWink(mStateHandle);
	epocDevice->mWinkLeftSensor->AddQueuedSample(winkLeft);

	// wink right
	double winkRight = 0.0;
	//if (ES_ExpressivIsActive(mStateHandle, EXP_WINK_RIGHT) == 1)
	winkRight = ES_ExpressivIsRightWink(mStateHandle);
	epocDevice->mWinkRightSensor->AddQueuedSample(winkRight);

	// eyes open
	float eyeLeftClosed = 0.0; // use float here, as Emotiv SDK uses floats
	float eyeRightClosed = 0.0;
	ES_ExpressivGetEyelidState(mStateHandle, &eyeLeftClosed, &eyeRightClosed);
	epocDevice->mEyeLeftClosedSensor->AddQueuedSample(eyeLeftClosed);
	epocDevice->mEyeRightClosedSensor->AddQueuedSample(eyeRightClosed);

	// eye location
	float eyesPitch = 0.0; // use float here, as Emotiv SDK uses floats
	float eyesYaw = 0.0;
	ES_ExpressivGetEyeLocation(mStateHandle, &eyesPitch, &eyesYaw);
	epocDevice->mEyesPitchSensor->AddQueuedSample(eyesPitch);
	epocDevice->mEyesYawSensor->AddQueuedSample(eyesYaw);

	// FACE ACTIONS

	EE_ExpressivAlgo_t upperFaceType = ES_ExpressivGetUpperFaceAction(mStateHandle);
	EE_ExpressivAlgo_t lowerFaceType = ES_ExpressivGetLowerFaceAction(mStateHandle);

	float upperFaceAmp = ES_ExpressivGetUpperFaceActionPower(mStateHandle);
	float lowerFaceAmp = ES_ExpressivGetLowerFaceActionPower(mStateHandle);

	double eyebrowExtent = 0.0;
	double furrow = 0.0;
	double smile = 0.0;
	double clench = 0.0;
	double laugh = 0.0;
	double smirkLeft = 0.0;
	double smirkRight = 0.0;

	if (upperFaceAmp > 0.0)
	{
		switch (upperFaceType)
		{
		case EXP_EYEBROW:	{ eyebrowExtent = upperFaceAmp; break; }
		case EXP_FURROW:    { furrow = upperFaceAmp; break; }
		default:			break;
		}
	}

	if (lowerFaceAmp > 0.0)
	{
		switch (lowerFaceType)
		{
		case EXP_CLENCH:		{ clench = lowerFaceAmp; break; }
		case EXP_SMILE:			{ smile = lowerFaceAmp; break; }
		case EXP_LAUGH:			{ laugh = lowerFaceAmp; break; }
		case EXP_SMIRK_LEFT:	{ smirkLeft = lowerFaceAmp; break; }
		case EXP_SMIRK_RIGHT:	{ smirkRight = lowerFaceAmp; break; }
		default:				{ break; }
		}
	}

	epocDevice->mEyebrowExtentSensor->AddQueuedSample(eyebrowExtent);
	epocDevice->mFurrowSensor->AddQueuedSample(furrow);
	epocDevice->mSmileSensor->AddQueuedSample(smile);
	epocDevice->mClenchSensor->AddQueuedSample(clench);
	epocDevice->mLaughSensor->AddQueuedSample(laugh);
	epocDevice->mSmirkLeftSensor->AddQueuedSample(smirkLeft);
	epocDevice->mSmirkRightSensor->AddQueuedSample(smirkRight);

	// AFFECTIVE EMOTIONAL

	// excitement
	double excitement = 0.0;
	//if (ES_AffectivIsActive(mStateHandle, AFF_EXCITEMENT) == 1)
	excitement = ES_AffectivGetExcitementShortTermScore(mStateHandle);
	epocDevice->mExcitement->AddQueuedSample(excitement);

	// excitement long-term
	double excitementLongTerm = 0.0;
	//if (ES_AffectivIsActive(mStateHandle, AFF_EXCITEMENT) == 1)
	excitementLongTerm = ES_AffectivGetExcitementLongTermScore(mStateHandle);
	epocDevice->mLongTermExcitement->AddQueuedSample(excitementLongTerm);

	// engagement-boredom
	double engagementBoredom = 0.0;
	//if (ES_AffectivIsActive(mStateHandle, AFF_ENGAGEMENT_BOREDOM) == 1)
	engagementBoredom = ES_AffectivGetEngagementBoredomScore(mStateHandle);
	epocDevice->mEngagementBoredom->AddQueuedSample(engagementBoredom);

	// meditation
	double meditation = 0.0;
	//if (ES_AffectivIsActive(mStateHandle, AFF_MEDITATION) == 1)
	meditation = ES_AffectivGetMeditationScore(mStateHandle);
	epocDevice->mMeditation->AddQueuedSample(meditation);

	// frustration
	double frustration = 0.0;
	//if (ES_AffectivIsActive(mStateHandle, AFF_FRUSTRATION) == 1)
	frustration = ES_AffectivGetFrustrationScore(mStateHandle);
	epocDevice->mFrustration->AddQueuedSample(frustration);

	// valence score
	double valenceScore = 0.0;
	//if (ES_AffectivIsActive(mStateHandle, AFF_VALENCE) == 1)
	valenceScore = ES_AffectivGetValenceScore(mStateHandle);
	epocDevice->mValenceScore->AddQueuedSample(valenceScore);

}


// handle state updates of insight devices
void EmotivDriver::DoEmoStateUpdateInsight(EmotivInsightDevice* insightDevice)
{
	// set the signal strength
	IEE_SignalStrength_t insightSignalStrength = IS_GetWirelessSignalStatus(mStateHandle);
	insightDevice->SetWirelessSignalQuality( FromEmotivWirelessSignalStatus(insightSignalStrength) );

	// set if the headset has been put on correctly or not
	int headsetOn = IS_GetHeadsetOn(mStateHandle);
	//insightDevice->SetHeadsetOnCorrectly(headsetOn == 1);

	// set the battery charge level
	int chargeLevel, maxChargeLevel;
	IS_GetBatteryChargeLevel(mStateHandle, &chargeLevel, &maxChargeLevel);
	insightDevice->SetBatteryChargeLevel ( (double)chargeLevel / (double)maxChargeLevel );

	const int numContactQualityChannels = IS_GetNumContactQualityChannels(mStateHandle);

	// get the contact quality for the input sensors
	for (int i = 0; i<numContactQualityChannels; ++i)
	{
		// get the contact quality for the given EEG eletrode
		IEE_EEG_ContactQuality_t contactQuality = IS_GetContactQuality(mStateHandle, (IEE_InputChannels_t)i);

		// retrieve input sensor information
		IInputSensorDescriptor_t sensorData;
		if (IEE_HeadsetGetSensorDetails((IEE_InputChannels_t)i, &sensorData) != EDK_OK)
		{
			LogError("Failed to get input sensor descriptor for sensor %i.", i);
			continue;
		}

		// make sure the channel id conversion we did is valid
		CORE_ASSERT((IEE_InputChannels_t)i == sensorData.channelId);

		// skip the sensors that do not exist on our neuro headset
		int exists = sensorData.fExists;
		if (exists == false)
			continue;

		// get the sensor name
		mTempSensorName = sensorData.pszLabel;

		// find the corresponding sensor
		Sensor* sensor = insightDevice->FindSensorByName(mTempSensorName.AsChar());
		if (sensor != NULL)
		{
			// set the sensor information
			sensor->SetContactQuality(FromEmotivContactQuality(contactQuality));
			//sensor->SetPosition( Vector3(sensorData.yLoc, sensorData.zLoc, -sensorData.xLoc) );
		}
	}

	// FACIAL EXPRESSION

	// eye blink
	double isBlink = 0.0;
	//if (ES_ExpressivIsActive(mStateHandle, EXP_BLINK) == 1)
	isBlink = IS_FacialExpressionIsBlink(mStateHandle);
	insightDevice->mBlinkSensor->AddQueuedSample(isBlink);

	// wink left
	double winkLeft = 0.0;
	//if (ES_ExpressivIsActive(mStateHandle, EXP_WINK_LEFT) == 1)
	winkLeft = IS_FacialExpressionIsLeftWink(mStateHandle);
	insightDevice->mWinkLeftSensor->AddQueuedSample(winkLeft);

	// wink right
	double winkRight = 0.0;
	//if (ES_ExpressivIsActive(mStateHandle, EXP_WINK_RIGHT) == 1)
	winkRight = IS_FacialExpressionIsRightWink(mStateHandle);
	insightDevice->mWinkRightSensor->AddQueuedSample(winkRight);

	// eyes open
	float eyeLeftClosed = 0.0; // use float here, as Emotiv SDK uses floats
	float eyeRightClosed = 0.0;
	IS_FacialExpressionGetEyelidState(mStateHandle, &eyeLeftClosed, &eyeRightClosed);
	insightDevice->mEyeLeftClosedSensor->AddQueuedSample(eyeLeftClosed);
	insightDevice->mEyeRightClosedSensor->AddQueuedSample(eyeRightClosed);

	// eye location
	float eyesPitch = 0.0; // use float here, as Emotiv SDK uses floats
	float eyesYaw = 0.0;
	IS_FacialExpressionGetEyeLocation(mStateHandle, &eyesPitch, &eyesYaw);
	insightDevice->mEyesPitchSensor->AddQueuedSample(eyesPitch);
	insightDevice->mEyesYawSensor->AddQueuedSample(eyesYaw);

	// FACE ACTIONS

	IEE_FacialExpressionAlgo_t upperFaceType = IS_FacialExpressionGetUpperFaceAction(mStateHandle);
	IEE_FacialExpressionAlgo_t lowerFaceType = IS_FacialExpressionGetLowerFaceAction(mStateHandle);

	float upperFaceAmp = IS_FacialExpressionGetUpperFaceActionPower(mStateHandle);
	float lowerFaceAmp = IS_FacialExpressionGetLowerFaceActionPower(mStateHandle);

	double smile = 0.0;
	double clench = 0.0;
	double surprise = 0.0;
	double frown = 0.0;

	if (lowerFaceAmp > 0.0)
	{
		switch (lowerFaceType)
		{
		case FE_CLENCH:			{ clench = lowerFaceAmp; break; }
		case FE_SMILE:			{ smile = lowerFaceAmp; break; }
		case FE_SURPRISE:		{ smile = lowerFaceAmp; break; }
		case FE_FROWN:			{ smile = lowerFaceAmp; break; }
		default:				{ break; }
		}
	}

	insightDevice->mSmileSensor->AddQueuedSample(smile);
	insightDevice->mClenchSensor->AddQueuedSample(clench);
	insightDevice->mSurprise->AddQueuedSample(surprise);
	insightDevice->mFrown->AddQueuedSample(frown);

	// PERFORMANCE METRIC

	// excitement
	double excitement = 0.0;
	//if (ES_AffectivIsActive(mStateHandle, AFF_EXCITEMENT) == 1)
	excitement = IS_PerformanceMetricGetInstantaneousExcitementScore(mStateHandle);
	insightDevice->mExcitement->AddQueuedSample(excitement);

	// excitement long-term
	double excitementLongTerm = 0.0;
	//if (ES_AffectivIsActive(mStateHandle, AFF_EXCITEMENT) == 1)
	excitementLongTerm = IS_PerformanceMetricGetExcitementLongTermScore(mStateHandle);
	insightDevice->mLongTermExcitement->AddQueuedSample(excitementLongTerm);

	// engagement-boredom
	double engagementBoredom = 0.0;
	//if (ES_AffectivIsActive(mStateHandle, AFF_ENGAGEMENT_BOREDOM) == 1)
	engagementBoredom = IS_PerformanceMetricGetEngagementBoredomScore(mStateHandle);
	insightDevice->mEngagementBoredom->AddQueuedSample(engagementBoredom);

	// relaxation
	double meditation = 0.0;
	//if (ES_AffectivIsActive(mStateHandle, AFF_MEDITATION) == 1)
	meditation = IS_PerformanceMetricGetRelaxationScore(mStateHandle);
	insightDevice->mRelaxation->AddQueuedSample(meditation);

	// stress
	double frustration = 0.0;
	//if (ES_AffectivIsActive(mStateHandle, AFF_FRUSTRATION) == 1)
	frustration = IS_PerformanceMetricGetStressScore(mStateHandle);
	insightDevice->mStress->AddQueuedSample(frustration);

	// interest
	double interest = 0.0;
	//if (ES_AffectivIsActive(mStateHandle, AFF_INTEREST) == 1)
	interest = IS_PerformanceMetricGetInterestScore(mStateHandle);
	insightDevice->mInterest->AddQueuedSample(interest);
}


// handle raw data stream of epoc
void EmotivDriver::DoDataUpdateEPOC(EmotivEPOCDevice* epocDevice, uint32 userId)
{
	EE_DataUpdateHandle(userId, mDataHandle);

	// get the number of samples taken since the last update call
	uint32 numSamples;
	EE_DataGetNumberOfSample(mDataHandle, &numSamples);

	// if there are no new samples, we can directly interrupt
	if (numSamples > 0)
	{
		// make sure our temporary buffers to store the samples are large enough
		//if (numSamples > mCounterBuffer.Size())		mCounterBuffer.Resize( numSamples );
		//if (numSamples > mTimesBuffer.Size())		mTimesBuffer.Resize( numSamples );
		if (numSamples > mTempSamplesBuffer.Size())	mTempSamplesBuffer.Resize(numSamples);

		// get counter values
		//EE_DataGet(dataHandle, ED_COUNTER, mCounterBuffer.GetPtr(), numSamples);

		// get time values
		//EE_DataGet(dataHandle, ED_TIMESTAMP, mTimesBuffer.GetPtr(), numSamples);

		// get the number of sensors and iterate through them
		const uint32 numSensors = epocDevice->GetNumNeuroSensors();
		for (uint32 s = 0; s<numSensors; ++s)
		{
			// get the current sensor and skip it in case it is no valid neuro sensor
			Sensor* sensor = epocDevice->GetNeuroSensor(s);
			if (sensor == NULL)
				continue;

			EE_DataChannel_t channelID = ToEPOCChannelID(sensor->GetName());

			// check if the sensor is actually outputting keyframes or if they are just dummys or reference sensors, skip them if yes
			if (channelID == ED_MARKER)
				continue;

			// get samples for the given sensors
			EE_DataGet(mDataHandle, channelID, mTempSamplesBuffer.GetPtr(), numSamples);

			// iterate through the samples
			for (uint32 i = 0; i<numSamples; ++i)
				sensor->AddQueuedSample(mTempSamplesBuffer[i]);
		}

		// get gyro values
		int gyroX, gyroY;
		EE_HeadsetGetGyroDelta(userId, &gyroX, &gyroY);

		epocDevice->mGyroX += gyroX * 0.001;
		epocDevice->mGyroY += gyroY * 0.001;

		// add the gyro samples
		for (uint32 i = 0; i<numSamples; ++i)
		{
			epocDevice->mGyroXSensor->AddQueuedSample(epocDevice->mGyroX);
			epocDevice->mGyroYSensor->AddQueuedSample(epocDevice->mGyroY);
		}
	}

}


// handle raw data stream of insight
// NOTE: this is simly a copy of the epoc versions with the function names replaced EE_xxx() -> IEE_xxx()
void EmotivDriver::DoDataUpdateInsight(EmotivInsightDevice* insightDevice, uint32 userId)
{
	IEE_DataUpdateHandle(userId, mDataHandle);

	// get the number of samples taken since the last update call
	uint32 numSamples;
	IEE_DataGetNumberOfSample(mDataHandle, &numSamples);

	// if there are no new samples, we can directly interrupt
	if (numSamples > 0)
	{
		// make sure our temporary buffers to store the samples are large enough
		//if (numSamples > mCounterBuffer.Size())		mCounterBuffer.Resize( numSamples );
		//if (numSamples > mTimesBuffer.Size())		mTimesBuffer.Resize( numSamples );
		if (numSamples > mTempSamplesBuffer.Size())	mTempSamplesBuffer.Resize(numSamples);

		// get counter values
		//IEE_DataGet(dataHandle, ED_COUNTER, mCounterBuffer.GetPtr(), numSamples);

		// get time values
		//IEE_DataGet(dataHandle, ED_TIMESTAMP, mTimesBuffer.GetPtr(), numSamples);

		// get the number of sensors and iterate through them
		const uint32 numSensors = insightDevice->GetNumNeuroSensors();
		for (uint32 s = 0; s<numSensors; ++s)
		{
			// get the current sensor and skip it in case it is no valid neuro sensor
			Sensor* sensor = insightDevice->GetNeuroSensor(s);
			if (sensor == NULL)
				continue;

			IEE_DataChannel_t channelID = ToInsightChannelID(sensor->GetName());

			// check if the sensor is actually outputting keyframes or if they are just dummys or reference sensors, skip them if yes
			if (channelID == ED_MARKER)
				continue;

			// get samples for the given sensors
			IEE_DataGet(mDataHandle, channelID, mTempSamplesBuffer.GetPtr(), numSamples);

			// iterate through the samples
			for (uint32 i = 0; i<numSamples; ++i)
				sensor->AddQueuedSample(mTempSamplesBuffer[i]);
		}

		// get gyro values
		int gyroX, gyroY;
		EE_HeadsetGetGyroDelta(userId, &gyroX, &gyroY);

		insightDevice->mGyroX += gyroX * 0.001;
		insightDevice->mGyroY += gyroY * 0.001;

		// add the gyro samples
		for (uint32 i = 0; i<numSamples; ++i)
		{
			insightDevice->mGyroXSensor->AddQueuedSample(insightDevice->mGyroX);
			insightDevice->mGyroYSensor->AddQueuedSample(insightDevice->mGyroY);
		}
	}

}

// construct the Emotiv software version string
String EmotivDriver::GetSoftwareVersionString() const
{
	char versionOut[4096];
	unsigned long outBuildNumber;
	if (EE_SoftwareGetVersion(versionOut, 4096, &outBuildNumber) != EDK_OK)
		return "";

	Core::String result;
	result.Format( "%s", versionOut);
	return result;
}


// convert from the contact quality enumeration from the Emotiv SDK to ours
Sensor::EContactQuality EmotivDriver::FromEmotivContactQuality(IEE_EEG_ContactQuality_t contactQuality)
{
	switch (contactQuality)
	{
		case EEG_CQ_NO_SIGNAL:	return Sensor::CONTACTQUALITY_NO_SIGNAL;
		case EEG_CQ_VERY_BAD:	return Sensor::CONTACTQUALITY_VERY_BAD;
		case EEG_CQ_POOR:		return Sensor::CONTACTQUALITY_POOR;
		case EEG_CQ_FAIR:		return Sensor::CONTACTQUALITY_FAIR;
		case EEG_CQ_GOOD:		return Sensor::CONTACTQUALITY_GOOD;
	}

	return Sensor::CONTACTQUALITY_NO_SIGNAL;
}


// convert from the wireless signal strength enumeration from the Emotiv SDK to ours
float EmotivDriver::FromEmotivWirelessSignalStatus(IEE_SignalStrength_t signalStrength)
{
	switch (signalStrength)
	{
		case NO_SIGNAL:		return 0.0f;
		case BAD_SIGNAL:	return 0.5f;
		case GOOD_SIGNAL:	return 1.0f;
	}

	return 0.0f;
}


void EmotivDriver::SetAutoDetectionEnabled(bool enable)
{
	DeviceDriver::SetAutoDetectionEnabled(enable);

	if (enable == true)
		mDetectOnce = false;
}


// forward access to detection routine to outside
void EmotivDriver::DetectDevices()
{
	if (mIsEnabled == false)
		return;

	mDetectOnce = true;
}



Device* EmotivDriver::FindDeviceByEmotivUserId(uint32 userId)
{
	// find user ID in array
	uint32 deviceIndex = mUserIDs.Find(userId);
	
	// not found
	if (deviceIndex == CORE_INVALIDINDEX32)
		return NULL;

	return mDevices[deviceIndex];
}


// find the corresponding Emotiv EPOC headset based on the Emotiv user id
Device* EmotivDriver::FindAndCreateDeviceByEmotivUserId(uint32 userId)
{
	// 1) check if the device already exist

	Device* device = FindDeviceByEmotivUserId(userId);
	if (device != NULL)
		return device;

	// 2) no device found, we need to create it 

	// skip this if we are not in detection mode
	if (mUseAutoDetection == false && mDetectOnce == false)
		return NULL;
	
	// read HW version so we know what device to create
	UHwVersion hwVersions;
	hwVersions.mValue = 0;
	IEE_HardwareGetVersion(userId, &hwVersions.mValue);
	
	LogInfo("Found Emotiv device version %x / %x (UserId=%i)", hwVersions.mVersions.mHeadsetVersion, hwVersions.mVersions.mDongleVersion, userId);

	// its an epoc/epoc+
	if (IsEpocDevice(hwVersions.mVersions.mHeadsetVersion))
	{
		LogInfo("Emotiv EPOC detected (UserId=%i)", userId);
		device = CreateEPOCForUserId(userId);

	}
	// its an insight
	else if (IsInsightDevice(hwVersions.mVersions.mHeadsetVersion))
	{
		LogInfo("Emotiv Insight detected (UserId=%i)", userId);
		device = CreateInsightForUserId(userId);
	}

	// return found/created device
	if (device != NULL)
		return device;

	// sad times :(
	LogInfo("Unknown Emotiv device (UserId=%i): deviceVersion=%x dongleVersion=%x", userId, hwVersions.mVersions.mHeadsetVersion, hwVersions.mVersions.mDongleVersion);
	return NULL;
}


// find and create Emotiv headset
EmotivEPOCDevice* EmotivDriver::CreateEPOCForUserId(uint32 userId)
{
	EmotivEPOCDevice* epocDevice  = new EmotivEPOCDevice(this);
	epocDevice->Init();

	GetDeviceManager()->AddDeviceAsync(epocDevice);

	// add to lists
	mDevices.Add(epocDevice);
	mUserIDs.Add(userId);

	// reset the gyro
	EE_HeadsetGyroRezero(userId);

	LogInfo( "Adding Emotiv EPOC device asyncly for user id %i.", userId );

	return epocDevice;
}



// find and create Emotiv headset
EmotivInsightDevice* EmotivDriver::CreateInsightForUserId(uint32 userId)
{
	EmotivInsightDevice* insightDevice = new EmotivInsightDevice(this);
	insightDevice->Init();

	GetDeviceManager()->AddDeviceAsync(insightDevice);

	// add to lists
	mDevices.Add(insightDevice);
	mUserIDs.Add(userId);

	// reset the gyro
	IEE_HeadsetGyroRezero(userId);

	LogInfo("Adding Emotiv Insight device asyncly for user id %i.", userId);

	return insightDevice;
}

// event handler
void EmotivDriver::OnRemoveDevice(Device* device) 
{
	// not an emotiv device
	if (device->GetType() != EmotivEPOCDevice::TYPE_ID && device->GetType() != EmotivInsightDevice::TYPE_ID)
		return;

	// find device in drivers list
	const uint32 deviceIndex = mDevices.Find(device);

	// device not handled by driver
	if (deviceIndex == CORE_INVALIDINDEX32)
		return;

	// remove device and user IDs from list
	mDevices.Remove(deviceIndex);
	mUserIDs.Remove(deviceIndex);
}


// convert from our representation to the the data channel enumeration from the Emotiv SDK
EE_DataChannel_t EmotivDriver::ToEPOCChannelID(const char* electrode)
{
	if (String::SafeCompare(electrode, "AF3") == 0) 	return ED_AF3;
	if (String::SafeCompare(electrode, "F7") == 0)		return ED_F7;
	if (String::SafeCompare(electrode, "F3") == 0)		return ED_F3;
	if (String::SafeCompare(electrode, "FC5") == 0) 	return ED_FC5;
	if (String::SafeCompare(electrode, "T7") == 0)		return ED_T7;
	if (String::SafeCompare(electrode, "P7") == 0)		return ED_P7;
	if (String::SafeCompare(electrode, "O1") == 0)		return ED_O1;
	if (String::SafeCompare(electrode, "O2") == 0)		return ED_O2;
	if (String::SafeCompare(electrode, "P8") == 0)		return ED_P8;
	if (String::SafeCompare(electrode, "T8") == 0)		return ED_T8;
	if (String::SafeCompare(electrode, "FC6") == 0)		return ED_FC6;
	if (String::SafeCompare(electrode, "F4") == 0)		return ED_F4;
	if (String::SafeCompare(electrode, "F8") == 0)		return ED_F8;
	if (String::SafeCompare(electrode, "AF4") == 0) 	return ED_AF4;

	// failure
	return ED_MARKER;
}


// convert from the data channel enumeration from the Emotiv SDK to ours
const char* EmotivDriver::FromEPOCChannelID(EE_DataChannel_t emotivChannel)
{
	switch (emotivChannel)
	{
		case ED_AF3:	return "AF3";
		case ED_F7:		return "F7";
		case ED_F3:		return "F3";
		case ED_FC5:	return "FC5";
		case ED_T7:		return "T7";
		case ED_P7:		return "P7";
		case ED_O1:		return "O1";
		case ED_O2:		return "O2";
		case ED_P8:		return "P8";
		case ED_T8:		return "T8";
		case ED_FC6:	return "FC6";
		case ED_F4:		return "F4";
		case ED_F8:		return "F8";
		case ED_AF4:	return "AF4";
	}

	// failure
	CORE_ASSERT(1 == 0);
	return "";
}


// convert from our representation to the the data channel enumeration from the Emotiv SDK 
IEE_DataChannel_t EmotivDriver::ToInsightChannelID(const char* electrode)
{
	if (String::SafeCompare(electrode, "AF3") == 0) 	return IED_AF3;
	if (String::SafeCompare(electrode, "T7") == 0)		return IED_T7;
	if (String::SafeCompare(electrode, "T8") == 0)		return IED_T8;
	if (String::SafeCompare(electrode, "AF4") == 0) 	return IED_AF4;
	if (String::SafeCompare(electrode, "Pz") == 0) 		return IED_Pz;

	// failure
	return IED_MARKER;
}


// convert from the data channel enumeration from the Emotiv SDK to ours
const char* EmotivDriver::FromInsightChannelID(IEE_DataChannel_t emotivChannel)
{
	switch (emotivChannel)
	{
		case IED_AF3:	return "AF3";
		case IED_T7:	return "T7";
		case IED_T8:	return "T8";
		case IED_AF4:	return "AF4";
		case IED_Pz:	return "Pz";
	}

	// failure
	CORE_ASSERT(1 == 0);
	return "";
}


#endif
