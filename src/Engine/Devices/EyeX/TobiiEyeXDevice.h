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

#ifndef __NEUROMORE_TOBIIEYEXTRACKER_H
#define __NEUROMORE_TOBIIEYEXTRACKER_H

#include "../../Config.h"
#include "../../Device.h"
#include "../../DeviceDriver.h"
#include "../../BciDevice.h"

#ifdef INCLUDE_DEVICE_TOBIIEYEX

class ENGINE_API TobiiEyeXDevice : public Device
{
	friend class TobiiEyeXDriver;

	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_TOBII_EYEX };

		// constructor and destructor
		TobiiEyeXDevice(DeviceDriver* driver = NULL);
		~TobiiEyeXDevice();

		Device* Clone() override									{ return new TobiiEyeXDevice(); }

		// overloaded stuff
		uint32 GetType() const override								{ return TYPE_ID; }
		const char* GetHardwareName() const override				{ return "Tobii EyeX"; }
		const char* GetUuid() const override						{ return "2173d807-cf13-440e-bc46-504628e742f4"; }
		static const char* GetRuleName()							{ return "DEVICE_TobiiEyeX"; }
		const char* GetTypeName() const override					{ return "eyex"; }
		double GetTimeoutLimit() const override						{ return 3.0; }
		double GetLatency() const override							{ return 0.1; }
		double GetExpectedJitter() const override					{ return 0.1; }
		bool IsWireless() const override							{ return true; }
		void CreateSensors() override;
	protected:
		Sensor* mLeftEyeXSensor;
		Sensor* mLeftEyeYSensor;
		Sensor* mLeftEyeZSensor;
		Sensor* mLeftEyeXNormalizedSensor;
		Sensor* mLeftEyeYNormalizedSensor;
		Sensor* mLeftEyeZNormalizedSensor;

		Sensor* mRightEyeXSensor;
		Sensor* mRightEyeYSensor;
		Sensor* mRightEyeZSensor;
		Sensor* mRightEyeXNormalizedSensor;
		Sensor* mRightEyeYNormalizedSensor;
		Sensor* mRightEyeZNormalizedSensor;

		Sensor* mGazeXSensor;
		Sensor* mGazeYSensor;
	private:
		int mSampleRate;
};

#endif

#endif
