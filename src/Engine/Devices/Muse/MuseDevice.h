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

#ifndef __NEUROMORE_MUSEHEADSET_H
#define __NEUROMORE_MUSEHEADSET_H

// include required headers
#include "../../BciDevice.h"

#ifdef INCLUDE_DEVICE_INTERAXON_MUSE

// forward declaration
class MuseSystem;

// the Muse neuro headset class
class ENGINE_API MuseDevice : public BciDevice
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_INTERAXON_MUSE };

		enum { 
			SENSOR_CONCENTRATION = 4,
			SENSOR_MELLOW		 = 5,
			SENSOR_DRL			 = 6,
			SENSOR_REF			 = 7,
			SENSOR_ACC_FORWARD	 = 8,
			SENSOR_ACC_UP		 = 9,
			SENSOR_ACC_LEFT		 = 10,
			SENSOR_EYEBLINK		 = 11,
			SENSOR_JAWCLENCH	 = 12,
		};

		// constructor & destructor
		MuseDevice(DeviceDriver* driver = NULL);
		~MuseDevice();

		Device* Clone() override							{ return new MuseDevice(); }

		// overloaded
		uint32 GetType() const override						{ return TYPE_ID; }
		double GetSampleRate() const override				{ return 220; }
		const char* GetHardwareName() const override		{ return "InteraXon Muse"; }
		const char* GetUuid() const override				{ return "ce0c3b9e-7945-11e4-82f8-0800200c9a66"; }
		const char* GetTypeName() const override			{ return "muse"; }
		double GetLatency() const override					{ return 0.1; }
		double GetExpectedJitter() const override			{ return 0.2; }
		bool IsWireless() const override					{ return false; /* we do not know the connection quality of the muse; this removes the connection quality icon in the device manager */ }
		bool HasEegContactQualityIndicator() override		{ return true; }
		static const char* GetRuleName()					{ return "DEVICE_InteraxonMuse"; }

		void CreateElectrodes() override;
		void CreateSensors() override;

		// parse muse-io OSC streaming format
		void ProcessMessage(OscMessageParser* message) override;

	private:
		int32					mIsTouchingForehead;

		Sensor*					mConcentrationSensor;
		Sensor*					mMellowSensor;
		Sensor*					mEyeBlinkSensor;
		Sensor*					mJawClenchSensor;

		Sensor*					mDRLSensor;
		Sensor*					mRefSensor;

		Sensor*					mAccForwardSensor;
		Sensor*					mAccUpSensor;
		Sensor*					mAccLeftSensor;

		Core::Array<Sensor*>	mContactQualitySensors;
		Sensor*					mTP9ContactQualitySensor;
		Sensor*					mFp1ContactQualitySensor;
		Sensor*					mFp2ContactQualitySensor;
		Sensor*					mTP10ContactQualitySensor;
		Sensor*					mTouchingForeheadSensor;
		

		// for parsing muse-io osc packets
		int32					mRawEEGMultiplier[4];
};

#endif

#endif
