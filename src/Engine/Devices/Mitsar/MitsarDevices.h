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

#ifndef __NEUROMORE_MITSARDEVICES_H
#define __NEUROMORE_MITSARDEVICES_H

// include required headers
#include "../../Config.h"
#include "../../BciDevice.h"

#if defined(NEUROMORE_PLATFORM_WINDOWS)

// Mitsar base class
class ENGINE_API MitsarDevice : public BciDevice
{
	public:
		// constructor & destructor
		MitsarDevice(DeviceDriver* driver = NULL);
		virtual ~MitsarDevice();

		// overloaded
		double GetExpectedJitter() const override			{ return 0.2; }
		const char* GetHardwareName() const override		{ return mHardwareName; }
		bool IsWireless() const override					{ return true; }
		bool HasTestMode() const override					{ return true; }
		static const char* GetRuleName()					{ return "DEVICE_Mitsar"; }
		void StartTest() override							{ mDeviceDriver->StartTest(this); }
		void StopTest() override							{ mDeviceDriver->StopTest(this); }
		bool IsTestRunning() override						{ return mDeviceDriver->IsTestRunning(this); }

		// impedance test provides contact quality
		bool HasEegContactQualityIndicator() override       { return IsTestRunning();  }

		void CreateSensors() override;
		void CreateImpedanceBuffers();

		double GetImpedance(uint32 neuroSensorIndex) override;

		// process mitsar EEG messages
		virtual void ProcessMessage(OscMessageParser* message) override;

	protected:
		// receiver sample counter for detecting lost osc packets
		uint32				mSampleCounter;

		Core::String		mHardwareName;

		// buffers for impedance values, one for each EEG sensor
		Core::Array<Channel<double>>	mImpedanceBuffers;
		Core::Array<double>				mImpedanceValues;
};


// The Mitsar 202-21
class ENGINE_API Mitsar201Device : public MitsarDevice
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_MITSAR_EEG201M};

		// constructor & destructor
		Mitsar201Device(DeviceDriver* driver = NULL);
		~Mitsar201Device();

		Device* Clone() override							{ return new Mitsar201Device(); }

		// overloaded
		uint32 GetType() const override						{ return TYPE_ID; }
		double GetSampleRate() const override				{ return 500; }
		const char* GetUuid() const override				{ return "98ac1f56-5013-11e5-885d-feff819cdc9f"; }
		const char* GetTypeName() const override			{ return "mitsar201"; }
		
		void CreateElectrodes() override;
		void CreateSensors() override;

		// process other mitsar OSC messages
		void ProcessMessage(OscMessageParser* message) override;

	private:
		Core::Array<Sensor*>	mBioSensors;


};

#endif

#endif
