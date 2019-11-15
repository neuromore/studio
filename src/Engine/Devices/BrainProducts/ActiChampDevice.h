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

#ifndef __NEUROMORE_ACTICHAMPDEVICE_H
#define __NEUROMORE_ACTICHAMPDEVICE_H

// include required headers
#include "../../Config.h"
#include "../../BciDevice.h"

#ifdef INCLUDE_DEVICE_ACTICHAMP

class ENGINE_API ActiChampDevice : public BciDevice
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_BRAINPRODUCTS_ACTICHAMP};

		// constructor and destructor
		ActiChampDevice(DeviceDriver* driver = NULL);
		virtual ~ActiChampDevice();

		// overloaded methods
		double GetExpectedJitter() const override			{ return 0.2; }
		const char* GetHardwareName() const override		{ return mHardwareName; }
		bool IsWireless() const override					{ return true; }
		bool HasTestMode() const override					{ return true; }
		// TODO: Fix this name to the correct name
		static const char* GetRuleName()					{ return "NODE_Development"; }
		void StartTest() override							{ mDeviceDriver->StartTest(this); }
		void StopTest() override							{ mDeviceDriver->StopTest(this); }
		bool IsTestRunning() override						{ return mDeviceDriver->IsTestRunning(this); }

		Device* Clone() override							{ return new ActiChampDevice(); }

		// overloaded
		uint32 GetType() const override						{ return TYPE_ID; }
		double GetSampleRate() const override				{ return 10000; }
		const char* GetUuid() const override				{ return "66f0cc86-7086-4cfa-9ec5-71fb37b7922a"; }
		const char* GetTypeName() const override			{ return "actichamp"; }
		
		void CreateElectrodes() override;
		void CreateSensors() override;

	protected:
		uint32			mSampleCounter;
		Core::String	mHardwareName;

	private:
		Core::Array<Sensor*>	mBioSensors;

};

#endif
#endif
