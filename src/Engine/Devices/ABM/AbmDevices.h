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

#ifndef __NEUROMORE_ADVANCEDBRAINMONITORING_X24_DEVICE_H
#define __NEUROMORE_ADVANCEDBRAINMONITORING_X24_DEVICE_H

// include required headers
#include "../../Config.h"
#include "../../BciDevice.h"

#ifdef INCLUDE_DEVICE_ADVANCEDBRAINMONITORING


// Advanced Brain Monitoring BCI class
class ENGINE_API AbmDevice : public BciDevice
{
	public:
		// constructor & destructor
		AbmDevice(DeviceDriver* driver = NULL);
		virtual ~AbmDevice() {}
		double GetSampleRate() const override				{ return 256.0; }
		virtual double GetTiltSampleRate() const = 0;
		
		bool IsWireless() const override					{ return true; }

		virtual void CreateSensors() override;

		//
		// Sensors
		//
		
		// other raw sensors
		Sensor* GetEKGSensor() const						{ return mEKGSensor; }

		// brain state sensor
		Sensor* GetClassificationSensor() const				{ return mClassificationSensor; }
		Sensor* GetHighEngagementSensor() const				{ return mHighEngagementSensor; }
		Sensor* GetLowEngagementSensor() const				{ return mLowEngagementSensor; }
		Sensor* GetDistractionSensor() const				{ return mDistractionSensor; }
		Sensor* GetDrowsySensor() const						{ return mDrowsySensor; }

		// workload
		Sensor* GetWorkloadAverageSensor() const			{ return mWorkloadAverageSensor; }
		Sensor* GetWorkloadBDSSensor() const				{ return mWorkloadBDSSensor; }
		Sensor* GetWorkloadFBDSSensor() const				{ return mWorkloadFBDSSensor; }

		// movement sensors
		Sensor* GetMovementValueSensor() const				{ return mMovementValueSensor; }
		Sensor* GetMovementLevelSensor() const				{ return mMovementLevelSensor; }

		// tilt sensors
		Sensor* GetTiltXSensor() const						{ return mTiltXSensor; }
		Sensor* GetTiltYSensor() const						{ return mTiltYSensor; }
		Sensor* GetTiltZSensor() const						{ return mTiltZSensor; }

		// heartrate
		Sensor* GetHeartRateSensor() const					{ return mHeartRateSensor; }

		// data mode
		enum EDataMode
		{
			DATAMODE_RAW= 0,
			DATAMODE_DECONTAMINATED,
			DATAMODE_BRAINSTATE, 
			DATAMODE_WORKLOAD
		};

		enum EFilterMode
		{
			FILTERMODE_NONE,
			FILTERMODE_ALL,
			FILTERMODE_DECON
		};

		// switch data mode
		void SetDataMode(EDataMode mode)					{ mDataMode = mode; }
		EDataMode GetDataMode() const						{ return mDataMode; }

		// switch filter mode
		void SetFilterMode(EFilterMode mode)				{ mFilterMode = mode; }
		EFilterMode GetFilterMode() const					{ return mFilterMode; }

		// play file
		bool HasPlaybackFile()								{ return !mPlaybackFilename.IsEmpty(); }
		void SetPlaybackFilename(const char* filename)		{ mPlaybackFilename = filename; }
		const char* GetPlaybackFilename() const				{ return mPlaybackFilename; }

		// destination file
		void SetDestinationFilename(const char* filename)	{ mDestinationFilename = filename; }
		const char* GetDestinationFilename() const			{ return mDestinationFilename; }

		// internal num channels for sdk
		void SetNumRawChannelsInternal(uint32 num)			{ mNumRawChannels = num; }
		uint32 GetNumRawChannelsInternal() const			{ return mNumRawChannels; }

	private:
		uint32			mNumRawChannels;

		EDataMode		mDataMode;
		EFilterMode		mFilterMode;
		Core::String	mPlaybackFilename;
		Core::String	mDestinationFilename;

		// other raw sensors
		Sensor*			mEKGSensor;

		// brain states
		Sensor*			mClassificationSensor;
		Sensor*			mHighEngagementSensor;
		Sensor*			mLowEngagementSensor;
		Sensor*			mDistractionSensor;
		Sensor*			mDrowsySensor;

		// Workload
		Sensor*			mWorkloadAverageSensor;
		Sensor*			mWorkloadBDSSensor;
		Sensor*			mWorkloadFBDSSensor;

		// movement
		Sensor*			mMovementValueSensor;
		Sensor*			mMovementLevelSensor;

		// tilt angles
		Sensor*			mTiltXSensor;
		Sensor*			mTiltYSensor;
		Sensor*			mTiltZSensor;
		
		// heart rate
		Sensor*			mHeartRateSensor;

		// decon (ABM's proprietary artifact algorithm detects, extracts and compensates 5 types of artifacts)
		/*Sensor*		mEyeBlinkSensor;
		Sensor*		mSaturationSensor;
		Sensor*		mExcursionSensor;
		Sensor*		mSpikeSensor;
		Sensor*		mEMGSensor;*/
};


//
// X10 Device
//

class ENGINE_API AbmX10Device : public AbmDevice
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_ABM_BALERT_X10 };

		// constructor & destructor
		AbmX10Device(DeviceDriver* driver = NULL);
		~AbmX10Device();

		// overloaded
		uint32 GetType() const override					{ return TYPE_ID; }
		const char* GetUuid() const override			{ return "e600797e-86e5-11e5-af63-feff819cdc9f"; }
		Device* Clone() override						{ return new AbmX10Device(); }
		const char* GetHardwareName() const override	{ return "B-Alert X10"; }
		static const char* GetRuleName()				{ return "DEVICE_ABMX10"; }
		const char* GetTypeName() const override		{ return "abmx10"; }
		double GetTiltSampleRate() const override		{ return 128.0; }

		void CreateElectrodes() override;
		

};


//
// X24 Device
//
 
class ENGINE_API AbmX24Device : public AbmDevice
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_ABM_BALERT_X24 };

		// constructor & destructor
		AbmX24Device(DeviceDriver* driver = NULL);
		~AbmX24Device();

		// overloaded
		uint32 GetType() const override					{ return TYPE_ID; }
		const char* GetUuid() const override			{ return "6a94a128-dd00-11e4-b9d6-1681e6b88ec1"; }
		Device* Clone() override						{ return new AbmX24Device(); }
		const char* GetHardwareName() const override	{ return "B-Alert X24"; }
		static const char* GetRuleName()				{ return "DEVICE_ABMX24"; }
		const char* GetTypeName() const override		{ return "abmx24"; }
		double GetTiltSampleRate() const override		{ return 256.0; }

		void CreateSensors() override;
		void CreateElectrodes() override;

		Sensor* GetAux1Sensor() const					{ return mAux1Sensor; }
		Sensor* GetAux2Sensor() const					{ return mAux2Sensor; }
		Sensor* GetAux3Sensor() const					{ return mAux3Sensor; }

	private:
		Sensor*		mAux1Sensor;
		Sensor*		mAux2Sensor;
		Sensor*		mAux3Sensor;
};



#endif

#endif
