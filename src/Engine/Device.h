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

#ifndef __NEUROMORE_DEVICE_H
#define __NEUROMORE_DEVICE_H

// include required headers
#include "Config.h"
#include "Core/StandardHeaders.h"
#include "Core/EventSource.h"
#include "Core/String.h"
#include "Core/Json.h"
#include "Core/AttributeSet.h"
#include "Networking/OscReceiver.h"
#include "Sensor.h"
#include "DeviceDriver.h"
#include "Devices/DeviceTypeIDs.h"



// the device base class
class ENGINE_API Device : public OscReceiver, public Core::EventSource
{
	friend DeviceDriver;
	
	public:
		enum { BASE_TYPE_ID = 0x01 };

		// constructor & destructor
		Device(DeviceDriver* deviceDriver = NULL);
		virtual ~Device();

		virtual Device* Clone() = 0;
		
		//
		// Device Properties / Configuration
		//
		
		// JSON based device config 
		class ENGINE_API DeviceConfig
		{
			public:
				DeviceConfig() : mIsValid(false), mIsEnabled(false), mDeviceType(DeviceTypeIDs::INVALID_DEVICE_TYPEID), mDeviceID(CORE_INVALIDINDEX32) {}
				DeviceConfig(const Core::Json& jsonParser);
				DeviceConfig(const DeviceConfig& config) : DeviceConfig(config.mJson) {}
				~DeviceConfig()	{}

				DeviceConfig& operator= (const DeviceConfig &source) { mJson = source.mJson; Init(); return *this; }

				Core::Json mJson;
				bool mIsValid;
				bool mIsEnabled;
		
				// preparsed values for faster searching of definitions
				Core::String mName;
				
				// type and name
				uint32 mDeviceType;
				uint32 mDeviceID;

			protected:
				virtual bool Init();
		};

		virtual void Configure(const DeviceConfig& config);
		bool IsConfigured() const												{ return mConfig.mIsValid == true; }
		const DeviceConfig& GetConfig() const									{ return mConfig; }

		// get device system that controls this device (if any)
		bool HasDeviceDriver() const											{ return mDeviceDriver != NULL; }
		DeviceDriver* GetDeviceDriver() const									{ return mDeviceDriver; }

		// base type
		virtual uint32 GetBaseType() const										{ return BASE_TYPE_ID; }

		virtual const char* GetUuid() const = 0;														// hardware property
		virtual uint32 GetType() const = 0;																// hardware property
																										   
		// extensive hardware name (but without vendor name)											   
		virtual const char* GetHardwareName() const = 0;												// hardware property
																										   
		// short device type name (a single lowercase word that describes the hardware type)			   
		virtual const char* GetTypeName() const = 0;													// hardware property
																										   
		// device timing stuff																			   
		virtual double GetLatency() const										{ return 0.0; }			// hardware property
		virtual double GetExpectedJitter() const								{ return 0.0; }			// hardware property
		virtual double GetTimeoutLimit() const									{ return 5.0; }			// hardware property
			
		// device id (continous index within devices of the same type)
		uint32 GetDeviceID() const												{ return mDeviceID; }
		void SetDeviceId(uint32 deviceId);

		// device name
		const Core::String& GetName() const										{ return mName; }
		void SetName(Core::String name)											{ mName = name; }

		// if device is input or output (maybe both)
		bool IsInputDevice() const												{ return mInputSensors.IsEmpty() == false; }
		bool IsOutputDevice() const												{ return mOutputSensors.IsEmpty() == false; }

		//
		// Device Control
		//

		// init/reset / update
		virtual void Init()														{}
		virtual void Reset();
		virtual void Update(const Core::Time& elapsed, const Core::Time& delta);

		// if device is enabled/disabled
		bool IsEnabled() const;

		bool IsLocked()	const													{ return mLockOwner == NULL; }
		bool AcquireLock(void* owner);
		bool ReleaseLock(void* owner);

		// sync device channels to engine
		virtual void Sync(const Core::Time& time, bool usePadding = true);

		//
		// Device State
		//

		enum EState
		{
			STATE_DISCONNECTED,		// startup state
			STATE_IDLE,				// connected, but doing nothing
			STATE_STREAMING,		// streaming data (main mode)
			STATE_TEST,				// test mode (depends on the device, for BCIs this it impedance test)
			STATE_TIMEOUT,			// device didn't send any data for too long
			STATE_ERROR,			// some known error happened // TODO maybe implement getter for error message
		};

		EState GetState() const													{ return mState; }
		virtual bool Connect()													{ if (mState != STATE_DISCONNECTED) return false; mState = STATE_IDLE; return true; }
		virtual bool Disconnect()												{ mState = STATE_DISCONNECTED; return true; }
		bool IsConnected() const												{ return mState == STATE_IDLE || mState == STATE_STREAMING || mState == STATE_TEST; }
		bool IsStreaming() const												{ return mState == STATE_STREAMING; }
		bool IsTimeoutReached() const											{ return mState == STATE_TIMEOUT; }
		void KeepAlive()														{ mInactivityDuration = 0; }

		//
		// Device Sensors
		//

		enum ESensorDirection
		{
			SENSOR_INPUT,
			SENSOR_OUTPUT
		};

		void AddSensor(Sensor* sensor, ESensorDirection direction = SENSOR_INPUT);
		Sensor* AddSensor(ESensorDirection direction, const char* name, double sampleRate = 0, bool isIrregularInput = false, double minValue = 0.0, double maxValue = 1.0, const char* unit = "", const Core::Color& color = Core::Color(1, 1, 1));
	
		uint32 GetNumSensors() const											{ return mSensors.Size(); }
		Sensor* GetSensor(uint32 index) const									{ return mSensors[index]; }
		Sensor* FindSensorByName(const char* name) const;
	
		// input sensors: data from device to engine (input device)
		uint32 GetNumInputSensors() const										{ return mInputSensors.Size(); }
		Sensor* GetInputSensor(uint32 index) const								{ return mInputSensors[index]; }
	
		// output sensors: data from engine to device (output device)
		uint32 GetNumOutputSensors() const										{ return mOutputSensors.Size(); }
		Sensor* GetOutputSensor(uint32 index) const								{ return mOutputSensors[index]; }

		//
		// Power mode, Battery state and Wireless state
		//

		enum EPowerSupplyType
		{
			POWERSUPPLY_BATTERY,		// powered by battery
			POWERSUPPLY_LINE,			// powered by static powersupply (usb or wallplug etc)
			POWERSUPPLY_UNKNOWN			// not really sure if we need this, it's the startup state
		};

		EPowerSupplyType GetPowerSupplyType() const								{ return mPowerSupplyType; }
		void SetPowerSupplyType(EPowerSupplyType type)							{ mPowerSupplyType = type; }

		bool HasBatteryIndicator() const										{ return mReceivedBatteryChargeLevel; }
		void SetBatteryChargeLevel(double normalizedBatteryLevel);
		double GetBatteryChargeLevel() const;

		virtual double GetCriticalBatteryLevel() const							{ return 0.25; }		// hardware property; less than one session remaining or sth.
		bool IsBatteryCritical() const;
		
		// wireless status
		virtual bool IsWireless() const											{ return false; }		// hardware property
		virtual bool HasWirelessIndicator() const								{ return mReceivedWirelessSignalQuality; }		
		void SetWirelessSignalQuality(double normalizedQuality);
		double GetWirelessSignalQuality() const;

		//
		// Device test mode (e.g. impedance test)
		//
		virtual bool HasTestMode() const										{ return false; }
		virtual void StartTest()												{ }
		virtual void StopTest() 												{ }
		virtual bool IsTestRunning() 											{ return false; }


		//
		// Device Statistics
		//
		// get highest latency accross all sensors
		double FindMaxLatency();

		// total number of bytes allocated for the sensors
		uint32 CalculateSensorMemoryUsage();

		// disable drift correction for all sensors
		void SetDriftCorrectionEnabled(bool enable = true);

		// default osc receive method
		virtual void ProcessMessage(OscMessageParser* message) override;

	protected:

		Core::Array<Sensor*>		mSensors;						// main sensor array: all the sensors the device provides							// TODO use Array<Sensor> here
		Core::Array<Sensor*>		mInputSensors;					// references to all input sensors (writable to driver, readable from engine)		// keep using poitner here (or use reference)
		Core::Array<Sensor*>		mOutputSensors;					// references to all readable sensors (readable from driver, writable from engine)	// keep using poitner here (or use reference)
		Core::Array<ChannelReader*>	mOutputReaders;					// output reader for reading the output sensor streams in the device or driver implementation	// TODO use Array<ChannelReader> here

		DeviceDriver*				mDeviceDriver;					// pointer to the driver responsible for this device / may be NULL, i.e. osc-only devices like Muse 
		uint32						mDeviceID;						// device id for multi device support
		DeviceConfig				mConfig;						// the device configuration (may be empty)
		Core::String				mName;							// device instance name
		
		void*						mLockOwner;						// exclusive access to device (for output devices)

		EState						mState;							// device state
		Core::Time					mInactivityDuration;			// timeout timer during streaming

		EPowerSupplyType			mPowerSupplyType;				// current supply (can change during runtime)
		double						mBatteryChargeLevel;			// normalized charge state of battery (0..1)
		bool						mReceivedBatteryChargeLevel;	// false until SetBatteryChargeLevel was called with valid level
		//bool						mEmitBatteryEvent;
		double						mWirelessSignalQuality;			// normalized wireless signal quality (0..1)
		bool						mReceivedWirelessSignalQuality;	// false until SetWirelessSignalQuality was called with valid level


	public:
	
		virtual void CreateSensors() = 0;
		void CollectSensors();
};



#endif

