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

#ifndef __NEUROMORE_GENERICDEVICES_H
#define __NEUROMORE_GENERICDEVICES_H

// include required headers
#include "../../Config.h"
#include "../../Device.h"


// heart rate monitor
class ENGINE_API HeartRateDevice : public Device
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_GENERIC_HEARTRATE };

		// constructor & destructor
		HeartRateDevice(DeviceDriver* driver = NULL)	{ mState = STATE_IDLE;  CreateSensors(); }
		~HeartRateDevice()								{}

		Device* Clone() override						{ return new HeartRateDevice(); }

		// overloaded
		uint32 GetType() const override					{ return TYPE_ID; }
		const char* GetHardwareName() const override	{ return "HeartRate"; }
		const char* GetUuid() const override			{ return "db98a54e-5d71-11e5-885d-feff819cdc9f"; }
		const char* GetTypeName() const override		{ return "hr"; }

		static const char* GetRuleName()				{ return "DEVICE_HRM"; }

		void CreateSensors() override
		{
			mHeartRateSensor = new Sensor("BPM", 0);				mHeartRateSensor->GetChannel()->SetUnit("bpm");		AddSensor(mHeartRateSensor);
			mRRIntervalSensor = new Sensor("RR-Interval", 0);		mRRIntervalSensor->GetChannel()->SetUnit("ms");		AddSensor(mRRIntervalSensor);  
		}
		
		inline Sensor* GetHeartRateSensor() const		{ return mHeartRateSensor; }
		inline Sensor* GetRRIntervalSensor() const		{ return mRRIntervalSensor; }

	private:
		//double				mSampleRate; // unused variable
		Sensor*					mHeartRateSensor;
		Sensor*					mRRIntervalSensor;
};

////////////////////////////////////////////////////////////////////////////////////

// Accelerometer
class AccelerometerDevice: public Device
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_GENERIC_ACCELEROMETER };

		// constructor & destructor
		AccelerometerDevice(DeviceDriver* driver = NULL){ mState = STATE_IDLE; CreateSensors(); }
		~AccelerometerDevice()							{}

		Device* Clone() override						{ return new AccelerometerDevice(); }

		// overloaded
		uint32 GetType() const override					{ return TYPE_ID; }
		const char* GetHardwareName() const override	{ return "Accelerometer"; }
		const char* GetUuid() const override			{ return "db98a7e2-5d71-11e5-885d-feff819cdc9f"; }
		const char* GetTypeName() const override		{ return "acc"; }

		static const char* GetRuleName()				{ return "DEVICE_Accelerometer"; }

		void CreateSensors() override
		{
			// TODO find sensible samplerate
			mSampleRate = 50.0;
			mXAxisSensor = new Sensor("X", mSampleRate, 0.0);	mXAxisSensor->GetChannel()->SetUnit("m/s^2");	AddSensor(mXAxisSensor);
			mYAxisSensor = new Sensor("Y", mSampleRate, 0.0);	mYAxisSensor->GetChannel()->SetUnit("m/s^2");	AddSensor(mYAxisSensor);
			mZAxisSensor = new Sensor("Z", mSampleRate, 0.0);	mZAxisSensor->GetChannel()->SetUnit("m/s^2");	AddSensor(mZAxisSensor);
		}

		inline Sensor* GetXAxisSensor() const			{ return mXAxisSensor; }
		inline Sensor* GetYAxisSensor() const			{ return mYAxisSensor; }
		inline Sensor* GetZAxisSensor() const			{ return mZAxisSensor; }

	private:
		double		mSampleRate;
		Sensor*		mXAxisSensor;
		Sensor*		mYAxisSensor;
		Sensor*		mZAxisSensor;
};

////////////////////////////////////////////////////////////////////////////////////

// Gyroscope
class GyroscopeDevice : public Device
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_GENERIC_GYROSCOPE};

		// constructor & destructor
		GyroscopeDevice(DeviceDriver* driver = NULL)	{ mState = STATE_IDLE; CreateSensors(); }
		~GyroscopeDevice()								{}

		Device* Clone() override						{ return new GyroscopeDevice(); }

		// overloaded
		uint32 GetType() const override					{ return TYPE_ID; }
		const char* GetHardwareName() const override	{ return "Gyroscope"; }
		const char* GetUuid() const override			{ return "db98ac92-5d71-11e5-885d-feff819cdc9f"; }
		const char* GetTypeName() const override		{ return "gyro"; }

		static const char* GetRuleName()				{ return "DEVICE_Gyroscope"; }

		void CreateSensors() override
		{
			// TODO find reasonable samplerate
			mSampleRate = 10.0;
			mXAxisSensor = new Sensor("X", mSampleRate, 0.0);	mXAxisSensor->GetChannel()->SetUnit("rad/s^2");	AddSensor(mXAxisSensor);
			mYAxisSensor = new Sensor("Y", mSampleRate, 0.0);	mYAxisSensor->GetChannel()->SetUnit("rad/s^2");	AddSensor(mYAxisSensor);
			mZAxisSensor = new Sensor("Z", mSampleRate, 0.0);	mZAxisSensor->GetChannel()->SetUnit("rad/s^2");	AddSensor(mZAxisSensor);
		}

		inline Sensor* GetXAxisSensor() const			{ return mXAxisSensor; }
		inline Sensor* GetYAxisSensor() const			{ return mYAxisSensor; }
		inline Sensor* GetZAxisSensor() const			{ return mZAxisSensor; }

	private:
		double		mSampleRate;
		Sensor*		mXAxisSensor;
		Sensor*		mYAxisSensor;
		Sensor*		mZAxisSensor;
};


#endif
