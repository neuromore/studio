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

#ifndef __NEUROMORE_NEUROSKYDEVICE_H
#define __NEUROMORE_NEUROSKYDEVICE_H

// include required headers
#include "../../Config.h"
#include "../../BciDevice.h"
#include "../../Core/Thread.h"
#include "../../Core/AttributeString.h"

#ifdef INCLUDE_DEVICE_NEUROSKY_MINDWAVE

// the NeuroSky neuro headset class
class ENGINE_API NeuroSkyDevice : public BciDevice
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_NEUROSKY_MINDWAVE };

		// constructor & destructor
		NeuroSkyDevice(DeviceDriver* driver = NULL);
		~NeuroSkyDevice();

		Device* Clone() override								{ return new NeuroSkyDevice(); }

		// overloaded
		uint32 GetType() const override							{ return TYPE_ID; }
		double GetSampleRate() const override					{ return 512; }
		const char* GetHardwareName() const override			{ return "NeuroSky MindWave"; }
		const char* GetUuid() const override					{ return "56b3cc30-bb2f-4d8a-b1f1-f5e0d7ab5344"; }
		const char* GetTypeName() const override				{ return "mindwave"; }
		bool IsWireless() const override						{ return true; }
		bool HasEegContactQualityIndicator() override			{ return true; }

		double GetLatency() const override						{ return 0.1; }
		double GetExpectedJitter() const override				{ return 0.2; }

		static const char* GetRuleName()						{ return "DEVICE_NeuroSkyThinkGear"; }

		void CreateElectrodes() override;
		void CreateSensors() override;

		Sensor* GetRawSensor() const							{ return mRawSensor; }
		Sensor* GetMeditationSensor() const						{ return mMeditationSensor; }
		Sensor* GetAttentionSensor() const						{ return mAttentionSensor; }
		Sensor* GetDeltaSensor() const							{ return mDeltaSensor; }
		Sensor* GetThetaSensor() const							{ return mThetaSensor; }
		Sensor* GetAlpha1Sensor() const							{ return mAlpha1Sensor; }
		Sensor* GetAlpha2Sensor() const							{ return mAlpha2Sensor; }
		Sensor* GetBeta1Sensor() const							{ return mBeta1Sensor; }
		Sensor* GetBeta2Sensor() const							{ return mBeta2Sensor; }
		Sensor* GetGamma1Sensor() const							{ return mGamma1Sensor; }
		Sensor* GetGamma2Sensor() const							{ return mGamma2Sensor; }
		//Sensor* GetEyeBlinkSensor() const						{ return mEyeBlinkSensor; }

	private:
		Sensor*					mRawSensor;
		Sensor*					mMeditationSensor;
		Sensor*					mAttentionSensor;
		Sensor*					mDeltaSensor;
		Sensor*					mThetaSensor;
		Sensor*					mAlpha1Sensor;
		Sensor*					mAlpha2Sensor;
		Sensor*					mBeta1Sensor;
		Sensor*					mBeta2Sensor;
		Sensor*					mGamma1Sensor;
		Sensor*					mGamma2Sensor;
		//Sensor*				mEyeBlinkSensor;
};

#endif

#endif
