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

#ifndef __NEUROMORE_TESTHEADSET_H
#define __NEUROMORE_TESTHEADSET_H

// include required headers
#include "../../Config.h"
#include "../../DSP/ClockGenerator.h"
#include "../../BciDevice.h"

// the test neuro headset class
class ENGINE_API TestDevice : public BciDevice
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_TEST };

		// constructor & destructor
		TestDevice(DeviceDriver* driver = NULL, uint32 sampleRate = 128);
		virtual ~TestDevice();

		Device* Clone() override							{ return new TestDevice(); }

		// information
		uint32 GetType() const override						{ return TYPE_ID; }
		double GetSampleRate() const override				{ return mSampleRate; }
		const char* GetHardwareName() const override		{ return "Test Device"; }
		static const char* GetRuleName()					{ return "DEVICE_TestSystem"; }
		bool IsWireless() const override					{ return false; }
		bool HasEegContactQualityIndicator() override		{ return false; }
		const char* GetUuid() const override				{ return "542c5710-303f-486d-bc57-0ce7e90952ca"; }
		const char* GetTypeName() const override			{ return "testdevice"; }
		double GetTimeoutLimit() const override				{ return 5; }
		void CreateElectrodes() override;

		// device is self-driving and need update/reset
		void Reset() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void Sync(const Core::Time& time, bool usePadding = true) override;


	private:
		ClockGenerator			mClock;						// clock for generating samples
		Core::Array<double>		mElectrodeTimeOffsets;		// random offset for each sensor
		double					mSampleRate;				// output sample rate
};

#endif
