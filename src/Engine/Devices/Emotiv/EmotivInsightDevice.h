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

#ifndef __NEUROMORE_EMOTIVINSIGHTDEVICE_H
#define __NEUROMORE_EMOTIVINSIGHTDEVICE_H

// include required headers
#include "../../Config.h"
#include "../../BciDevice.h"

#ifdef INCLUDE_DEVICE_EMOTIV


// the Emotiv neuro headset class
class ENGINE_API EmotivInsightDevice : public BciDevice
{
	friend class EmotivDriver;

	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_EMOTIV_INSIGHT };

		// constructor & destructor
		EmotivInsightDevice(DeviceDriver* driver = NULL);
		~EmotivInsightDevice();

		Device* Clone() override								{ return new EmotivInsightDevice(); }

		// overloaded
		uint32 GetType() const override							{ return TYPE_ID; }
		double GetSampleRate() const override					{ return 128; }
		const char* GetHardwareName() const override			{ return "Emotiv Insight"; }
		static const char* GetRuleName()						{ return "DEVICE_EmotivInsight"; }
		const char* GetUuid() const override					{ return "ae051577-ed68-4726-90fb-e829dbb40e2d"; }
		const char* GetTypeName() const override				{ return "insight"; }
		double GetTimeoutLimit() const override					{ return 3.0; }
		double GetLatency() const override						{ return 0.1; }
		double GetExpectedJitter() const override				{ return 0.1; }
		bool IsWireless() const override						{ return true; }
		bool HasWirelessIndicator() const override				{ return true; }
		bool HasEegContactQualityIndicator() override			{ return true; }

		void CreateElectrodes() override;
		void CreateSensors() override;

	protected:
		Sensor*					mGyroXSensor;
		Sensor*					mGyroYSensor;
		double					mGyroX;
		double					mGyroY;

		// facial expressions
		Sensor*					mBlinkSensor;
		Sensor*					mWinkLeftSensor;
		Sensor*					mWinkRightSensor;
		Sensor*					mEyeLeftClosedSensor;
		Sensor*					mEyeRightClosedSensor;
		Sensor*					mEyesPitchSensor;
		Sensor*					mEyesYawSensor;
		Sensor*					mSmileSensor;
		Sensor*					mClenchSensor;
		Sensor*					mSurprise;
		Sensor*					mFrown;

		// performance metrics
		Sensor*					mExcitement;
		Sensor*					mLongTermExcitement;
		Sensor*					mRelaxation;
		Sensor*					mStress;
		Sensor*					mEngagementBoredom;
		Sensor*					mInterest;
};

#endif

#endif
