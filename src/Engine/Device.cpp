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
#include "Device.h"
#include "Core/LogManager.h"
#include "DeviceDriver.h"
#include "EngineManager.h"
#include "Notifications.h"

using namespace Core;


// device constructor
Device::Device(DeviceDriver* deviceDriver) : OscReceiver()
{
	LogDetailedInfo("Constructing device ...");

	mDeviceDriver					= deviceDriver;
	mDeviceID						= CORE_INVALIDINDEX32;
	mName							= "";
	mOscPathPattern					= "";
	mState							= STATE_DISCONNECTED;
	mLockOwner						= NULL;
	mInactivityDuration				= 0;

	mPowerSupplyType				= POWERSUPPLY_UNKNOWN;
	mBatteryChargeLevel				= 0.0;
	mReceivedBatteryChargeLevel		= false;

	mWirelessSignalQuality			= 0.0;
	mReceivedWirelessSignalQuality  = false;
}


// destructor
Device::~Device()
{
	LogDetailedInfo("Destructing device ...");

	//if (mBatteryCritical == true)
		//EMIT_EVENT( OnClearWarning (Notifications::WARNING_DEVICE_POWERSTATECRITICAL) );

	// deallocate all sensors
	DestructArray(mSensors);
	DestructArray(mOutputReaders);

	// clear only (sensors were owned by the mSensors array)
	mInputSensors.Clear();
	mOutputSensors.Clear();
}


// device config constructor
Device::DeviceConfig::DeviceConfig(const Json& jsonParser)
{
	mJson = jsonParser;
	Init();
}


bool Device::DeviceConfig::Init()
{
	mIsValid = false;
	mIsEnabled = false;
	mDeviceType = DeviceTypeIDs::INVALID_DEVICE_TYPEID;
	mDeviceID = CORE_INVALIDINDEX32;

	// get root item
	Json::Item rootItem = mJson.GetRootItem();
	if (rootItem.IsNull() == true)
		return false;

	// get device type name
	Json::Item typeItem = rootItem.Find("deviceType");
	if (typeItem.IsString() == false)
	{
		LogError("Error loading device definition: JSON member 'deviceType' not found.");
		return false;
	}
	else
	{
		const char* typeName = typeItem.GetString();

		// find integer type by name
		mDeviceType = GetDeviceManager()->FindDeviceTypeByName(typeName);
		if (mDeviceType == DeviceTypeIDs::INVALID_DEVICE_TYPEID)
		{
			LogError("Error loading device definition: Device Type '%s' not found.", typeName);
			return false;
		}

		// enable flag
		Json::Item enableItem = rootItem.Find("enable");
		if (enableItem.IsBool() == true)
			mIsEnabled = enableItem.GetBool();
		else
			mIsEnabled = true;  // enable device by default

		// device ID
		Json::Item deviceNumberItem = rootItem.Find("deviceNumber");
		if (deviceNumberItem.IsInt() == true)
		{
			// convert device number into device ID (1-indexed to 0-indexed)
			mDeviceID = deviceNumberItem.GetInt();
			if ( mDeviceID > 0 ) 
				mDeviceID -= 1;
		}
		else
		{
			mDeviceID = 0;
		}

		// name
		Json::Item nameItem = rootItem.Find("name");
		if (nameItem.IsString() == true)
			mName = nameItem.GetString();
	}

	mIsValid = true;

	return true;
}


void Device::Configure(const DeviceConfig& config)
{
	if (config.mIsValid == false || config.mIsEnabled == false)
		return;

	mConfig = config;

	// set device parameters from configuration
	mName = mConfig.mName;
	mDeviceID = mConfig.mDeviceID;
}


// set device ID and prepare osc prefix
void Device::SetDeviceId(uint32 deviceId)
{
	mDeviceID = deviceId;

	// update osc address
	mOscAddress.Format("/%s/%i/*", GetTypeName(), mDeviceID);
}


// reset device
void Device::Reset()
{
	// reset all sensors
	const uint32 numSensors = mSensors.Size();
	for (uint32 i = 0; i<numSensors; ++i)
		mSensors[i]->Reset();

	// reset all output readers
	const uint32 numReaders = mOutputReaders.Size();
	for (uint32 i = 0; i<numReaders; ++i)
		mOutputReaders[i]->Reset();

	mReceivedBatteryChargeLevel = false;
	mReceivedWirelessSignalQuality = false;
	mBatteryChargeLevel = 0.0;
	mWirelessSignalQuality = 0.0;
}


// update the sensors
void Device::Update(const Time& elapsed, const Time& delta)
{
	mInactivityDuration += delta;

	// enter timeout state
	if (mInactivityDuration.InSeconds() > GetTimeoutLimit())
		mState = STATE_TIMEOUT;

	// do not update sensors if system is disabled
	if (IsEnabled() == false)
		return;

	// update all sensors
	bool receivedData = false;
	bool isActive = false;
	const uint32 numSensors = mSensors.Size();
	for (uint32 i = 0; i < numSensors; ++i)
	{
		Sensor* sensor = mSensors[i];

		// update the sensor
		sensor->Update(elapsed, delta);
		const double sampleRate = sensor->GetSampleRate();
		
		// set the latency of the sensor to (average_burst_overhead / 2 + transmission_latency)
		uint32 burstSize = sensor->CalculateAverageBurstSize();
		if (burstSize <= 1)
		{
			const double latency = GetLatency();
			sensor->SetLatency(latency);
		}
		else
		{
			// calculate burst duration, set to zero in case sample rate is 0
			double burstDuration = 0.0;
			if (sampleRate > Math::epsilon)
				burstDuration = (burstSize - 1) / sensor->GetSampleRate();

			const double totalLatency = burstDuration / 2.0 + GetLatency();
			sensor->SetLatency(totalLatency);
		}

		// set flag so we know if any data was received and if the sensor is active
		receivedData |= (sensor->GetInput()->GetNumNewSamples() > 0);
		isActive |= (sensor->GetInput()->IsActive());

		// apply device jitter value for all sensors
		sensor->SetExpectedJitter(GetExpectedJitter());
	}

	// update output readers
	const uint32 numReaders = mOutputReaders.Size();
	for (uint32 i = 0; i<numReaders; ++i)
		mOutputReaders[i]->Update();


	// reset inactivity timer
	if (isActive == true)
		mInactivityDuration = 0;

	// state change
	switch (mState)
	{
		case STATE_IDLE:
			// start streaming: go from IDLE to STREAMING as soon as the first data is received
			if (receivedData)
			{
				// TODO sync only the device to the engine instead of doing a full engine resync
				//GetEngine()->SyncAsync();
				Sync(elapsed, false);
			}
			break;

		case STATE_STREAMING:
			break;

		// other states not used here yet
		case STATE_TEST:
			break;
		case STATE_TIMEOUT:
			break;
		case STATE_DISCONNECTED:
			break;
		case STATE_ERROR:
			break;
	}
    
    // jump-set the state to streaming as we received data (allow this from any current state!)
    if (receivedData)
        mState = STATE_STREAMING;

	//// emit battery warning
	//if (mBatteryCritical == true && mEmitBatteryEvent == true)
	//{
	//	mEmitBatteryEvent = false;
	//
	//	String message;
	//	message.Format("%s battery low", GetName().AsChar());
	//	String description;
	//	description.Format("Device \'%s\' has reached critical battery state of %.0f%%.", GetName().AsChar(), GetBatteryChargeLevel() * 100.0);
	//	
	//	//EMIT_EVENT( OnWarning (Notifications::WARNING_DEVICE_POWERSTATECRITICAL, message.AsChar(), description.AsChar()) );
	//}

}


bool Device::IsEnabled() const
{
	if (HasDeviceDriver() == true)
		return mDeviceDriver->IsEnabled();

	return true;
}


bool Device::AcquireLock(void* owner)
{
	if (mLockOwner != NULL && mLockOwner != owner)
		return false;

	mLockOwner = owner;

	return true;
}


bool Device::ReleaseLock(void* owner)
{
	//CORE_ASSERT(mLockOwner == owner);

	if (owner != mLockOwner)
		return false;

	mLockOwner = NULL;

	return true;
}


// sync all device sensors 
void Device::Sync(const Core::Time& time, bool usePadding)
{
	// TODO sync inputs/outputs differently?
	const uint32 numSensors = mSensors.Size();
	for (uint32 s = 0; s<numSensors; ++s)
		mSensors[s]->Sync(time, usePadding);
}


void Device::AddSensor(Sensor* sensor, ESensorDirection direction)			
{ 
	if (direction == SENSOR_INPUT)
	{
		mInputSensors.Add(sensor);
	}
	else
	{
		mOutputSensors.Add(sensor);

		// add output reader 
		ChannelReader* reader = new ChannelReader(sensor->GetChannel());
		mOutputReaders.Add(reader);
	}

	mSensors.Add(sensor); 
}


void Device::SetBatteryChargeLevel(double normalizedBatteryLevel)
{
	// set flag to true after we received a valid non-zero charge level
	if (normalizedBatteryLevel > 0.0)
		mReceivedBatteryChargeLevel = true;

	// use hysteresis so it doesn't jitter if there is noise in the signal
	if (Math::AbsD( mBatteryChargeLevel - normalizedBatteryLevel) > 0.011 ) // >1%
		mBatteryChargeLevel = normalizedBatteryLevel; 
}


double Device::GetBatteryChargeLevel() const
{
	if (mPowerSupplyType == POWERSUPPLY_BATTERY && HasBatteryIndicator() == true && mReceivedBatteryChargeLevel == true)
		return mBatteryChargeLevel;
	else if (mPowerSupplyType == POWERSUPPLY_LINE)
		return 1.0;
	else
		return 0.0;	// default value
}


bool Device::IsBatteryCritical() const
{
	if (mPowerSupplyType == POWERSUPPLY_BATTERY && HasBatteryIndicator() == true && mReceivedBatteryChargeLevel == true)
		return (mBatteryChargeLevel <= GetCriticalBatteryLevel());

	return false;
}


double Device::GetWirelessSignalQuality() const
{
	if (IsWireless() && HasWirelessIndicator()) 
		return mWirelessSignalQuality; 
	else 
		return 0.0; 
}


void Device::SetWirelessSignalQuality(double normalizedQuality)
{
	// set flag to true after we received a valid non-zero charge level
	if (normalizedQuality > 0.0)
		mReceivedWirelessSignalQuality = true;

	mWirelessSignalQuality = normalizedQuality; 
}


// enable/disable drift correction of all sensors
void Device::SetDriftCorrectionEnabled(bool enable)
{
	const uint32 numSensors = mSensors.Size();
	for (uint32 s = 0; s < numSensors; ++s)
		mSensors[s]->SetDriftCorrectionEnabled(enable);
}



// default processor for osc messages
void Device::ProcessMessage(OscMessageParser* message) 
{
	if (IsEnabled() == false)
		return;

	// battery message
	if (message->MatchAddress("/*/*/batt") == true)
	{
		float level = 0.0;
		(*message) >> level;
		mBatteryChargeLevel = level;
	}
}


// get highest latency accross all sensors
double Device::FindMaxLatency()
{
	const uint32 numSensors = mSensors.Size();

	double maxLatency = 0.0;
	for (uint32 i=0; i<numSensors; ++i)
		maxLatency = Max(maxLatency, mSensors[i]->GetLatency());

	return maxLatency;
}


// total number of bytes allocated for the sensors
uint32 Device::CalculateSensorMemoryUsage()
{
	uint32 numBytes = 0;

	const uint32 numSensors = mSensors.Size();
	for (uint32 i=0; i<numSensors; ++i)
	{
		ChannelBase* channel = mSensors[i]->GetChannel();
		numBytes += channel->CalculateMemoryAllocated(false);
	}

	return numBytes;
}


// find sensor by name
Sensor* Device::FindSensorByName(const char* name) const
{
	const uint32 numSensors = mSensors.Size();
	for (uint32 i=0; i<numSensors; ++i)
	{
		if (mSensors[i]->GetNameString().IsEqual(name) == true)
			return mSensors[i];
	}

	return NULL;
}


Sensor* Device::AddSensor(ESensorDirection direction, const char* name, double sampleRate, bool isIrregularInput, double minValue, double maxValue, const char* unit, const Color& color)
{
	Sensor* sensor;

	// input does not have a stable sample rate
	if (isIrregularInput == true)
		sensor = new Sensor( name, sampleRate, 0.0);
	else
		sensor = new Sensor( name, sampleRate );
	
	sensor->GetChannel()->SetMinValue( minValue );
	sensor->GetChannel()->SetMaxValue( maxValue );
	sensor->GetChannel()->SetUnit( unit );
	sensor->GetChannel()->SetColor(color);

	AddSensor(sensor, direction);

	return sensor;
}

int32 Device::GetOscPathDeviceId(const Core::String& address) const
{
	int32 deviceId = -1;

	// get the number between the second pair of slashes
	// e.g. if the address has the form "/muse/13/foobar" the result should be 13 as a decimal number

	// no performance required here, so we just use strings and split them by '/'
	Array<String> elements = address.Split(StringCharacter::forwardSlash);

	// try to convert the second element of the address into a decimal number
	if (elements.Size() > 2)
	{
		int32 id = elements[2].ToInt();
		if (id >= 0 && elements[2].IsValidInt())
			deviceId = id;
	}

	return deviceId;
}
