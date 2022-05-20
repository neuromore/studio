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

#ifndef __NEUROMORE_OPENBCIHEADSET_H
#define __NEUROMORE_OPENBCIHEADSET_H

// include required headers
#include "../../BciDevice.h"
#include "../../DeviceDriver.h"

// the base class for all OpenBCI devices
class ENGINE_API OpenBCIDeviceBase : public BciDevice
{
	class OpenBCISerialThread;
	friend class OpenBCISerialThread;

	public:
		// constructors & destructor
		OpenBCIDeviceBase(DeviceDriver* driver = NULL)				{ mState = STATE_IDLE; }
		virtual ~OpenBCIDeviceBase()								{}

		// information
		double GetSampleRate() const override						{ return 250; }
		double GetLatency() const override							{ return 0.1; }
		double GetExpectedJitter() const override					{ return 0.1; }
		bool IsWireless() const override							{ return true; }

		void CreateSensors() override;

        // custom sensor
        inline Sensor* GetAccForwardSensor() const					{ return mAccForwardSensor; }
		inline Sensor* GetAccUpSensor() const						{ return mAccUpSensor; }
		inline Sensor* GetAccLeftSensor() const						{ return mAccLeftSensor; }

	private:
        Sensor*					mAccForwardSensor;
		Sensor*					mAccUpSensor;
		Sensor*					mAccLeftSensor;

};



// the default OpenBCI device class
class ENGINE_API OpenBCIDevice : public OpenBCIDeviceBase
{
public:
	enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_OPENBCI };

	// constructors & destructor
	OpenBCIDevice(DeviceDriver* driver = NULL);
	virtual ~OpenBCIDevice();

	Device* Clone() override										{ return new OpenBCIDevice(); }

	// information
	uint32 GetType() const override									{ return TYPE_ID; }
	const char* GetTypeName() const override						{ return "openbci"; }
	const char* GetHardwareName() const override					{ return "OpenBCI"; }
	const char* GetUuid() const override							{ return "5108993a-fe1b-11e4-a322-1697f925ec7b"; }
	static const char* GetRuleName()								{ return "DEVICE_OpenBCI"; }
	double GetSampleRate() const override							{ return 250; }

	double GetTimeoutLimit() const override							{ return 60; } // Long timeout limit because channel config takes so long

	void CreateElectrodes() override;
};



class ENGINE_API OpenBCIDaisyDevice : public OpenBCIDeviceBase
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_OPENBCI_DAISY };
		
		// constructors & destructor
		OpenBCIDaisyDevice(DeviceDriver* driver = NULL);
		virtual ~OpenBCIDaisyDevice();

		Device* Clone() override									{ return new OpenBCIDaisyDevice(); }

		// information
		uint32 GetType() const override								{ return TYPE_ID; }
		const char* GetTypeName() const override					{ return "openbci16"; }
		const char* GetHardwareName() const override				{ return "OpenBCI + Daisy"; }
		const char* GetUuid() const override						{ return "23893c38-8ecd-11e5-8994-feff819cdc9f"; }
		static const char* GetRuleName()							{ return "DEVICE_OpenBCIDaisy"; }
		double GetSampleRate() const override						{ return 125; }
	
		void CreateElectrodes() override;
};


#endif
