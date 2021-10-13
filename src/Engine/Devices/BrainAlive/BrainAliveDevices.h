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

#ifndef __NEUROMORE_BRAINALIVE_DEVICES_H
#define __NEUROMORE_BRAINALIVE_DEVICES_H


// include required headers
#include "../../BciDevice.h"
#include "../../DeviceDriver.h"
#include "../../../Engine/DSP/MultiChannel.h"

#ifdef INCLUDE_DEVICE_BRAINALIVE

// the base class for all BrainAlive devices

class ENGINE_API BrainAliveDeviceBase : public BciDevice
{
	class BrainAliveSerialThread;
	friend class BrainAliveSerialThread;

public:
	// constructors & destructor
	BrainAliveDeviceBase(DeviceDriver* driver = NULL) { mState = STATE_IDLE; }
	virtual ~BrainAliveDeviceBase() {}

	// information 

	double GetSampleRate() const override { return 250; }
	double GetLatency() const override { return 0.1; }
	double GetExpectedJitter() const override { return 0.1; }
	bool IsWireless() const override { return true; }

	void CreateSensors() override;
	// custom sensor
	inline Sensor* GetAccForwardSensor() const { return mAccForwardSensor; }
	inline Sensor* GetAccUpSensor() const { return mAccUpSensor; }
	inline Sensor* GetAccLeftSensor() const { return mAccLeftSensor; }
	inline Sensor* GetPpgRedSensor() const { return mRed; }
	inline Sensor* GetPpgIRSensor() const { return mIR; }
	inline Sensor* GetPpgGreenSensor() const { return mGreen; }
	inline Sensor* GetAXLSensor() const { return Axl; }
	inline Sensor* GetPIDSensor() const { return mPID; }
	

private:

	Sensor* Axl;
	Sensor* mAccForwardSensor;
	Sensor* mAccUpSensor;
	Sensor* mAccLeftSensor;
	Sensor* mRed;
	Sensor* mIR;
	Sensor* mGreen;
	Sensor* mPID;

};

class ENGINE_API BrainAliveDevice : public BrainAliveDeviceBase
{
public:
	enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_BRAINALIVE };

	// constructors & destructor
	BrainAliveDevice(DeviceDriver* driver = NULL);
	virtual ~BrainAliveDevice();

	Device* Clone() override { return new BrainAliveDevice(); }

	// information
	uint32 GetType() const override { return TYPE_ID; }
	const char* GetTypeName() const override { return "BrainAlive"; }
	const char* GetHardwareName() const override { return "BrainAlive"; }
	const char* GetUuid() const override { return "5108993a-fe1b-11e4-a322-1697f925ec7b"; }
	static const char* GetRuleName() { return "DEVICE_OpenBCI"; }
	double GetSampleRate() const override { return 250; }
	 
	double GetTimeoutLimit() const override { return 120; } // Long timeout limit because channel config takes so long

	void CreateElectrodes() override;
};


#endif

#endif
