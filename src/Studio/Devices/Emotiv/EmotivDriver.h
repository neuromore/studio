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

#ifndef __NEUROMORE_EMOTIVSYSTEM_H
#define __NEUROMORE_EMOTIVSYSTEM_H

// include required headers
#include <DeviceDriver.h>
#include <EngineManager.h>
#include <Devices/Emotiv/EmotivEPOCDevice.h>
#include <Devices/Emotiv/EmotivInsightDevice.h>

#ifdef INCLUDE_DEVICE_EMOTIV

// include Emotiv EPOC SDK
#include <emotiv/EmoStateDLL.h>
#include <emotiv/EmoStateAffectiv.h>
#include <emotiv/edk.h>
#include <emotiv/edkErrorCode.h>
#include <emotiv/EegData.h>

// include Emotiv Insight SDK
#include <emotiv/IEmoStateDLL.h>
#include <emotiv/IEmoStatePerformanceMetric.h>
#include <emotiv/Iedk.h>
#include <emotiv/IedkErrorCode.h>
#include <emotiv/IEegData.h>


// the Emotiv system class
class EmotivDriver : public DeviceDriver, Core::EventHandler
{
	public:

		enum { TYPE_ID = DeviceTypeIDs::DRIVER_TYPEID_EMOTIV };

		// constructor & destructor
		EmotivDriver();
		virtual ~EmotivDriver();

		const char* GetName() const override							{ return "Emotiv Devices"; }

		uint32 GetType() const override									{ return TYPE_ID; }

		// initialization
		bool Init() override;

		// update process
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		bool HasAutoDetectionSupport() const override final				{ return true; }
		void SetAutoDetectionEnabled(bool enable = true) override final;
		bool IsDetectionRunning() const override final					{ return mIsSearching; }

		virtual void DetectDevices() override;
		Device* CreateDevice(uint32 deviceTypeID) override				{ return NULL; /* not available for emotiv device */}

		// helper information retrieval
		//inline float GetTimeFromStart() const							{ return mTimeFromStart; }
		Core::String GetSoftwareVersionString() const;

		// access to Emotiv data handles
		inline DataHandle GetDataHandle()								{ return mDataHandle; }
		inline EmoStateHandle GetStateHandle()							{ return mStateHandle; }

		// find Emotiv EPOC by Emotiv user id
		Device* FindDeviceByEmotivUserId(uint32 userId);
		Device* FindAndCreateDeviceByEmotivUserId(uint32 userId);
		EmotivEPOCDevice* CreateEPOCForUserId(uint32 userId);
		EmotivInsightDevice* CreateInsightForUserId(uint32 userId);

		// state update handlers
		void DoEmoStateUpdateEPOC(EmotivEPOCDevice* epocDevice);
		void DoEmoStateUpdateInsight(EmotivInsightDevice* insightDevice);

		// data update handlers
		void DoDataUpdateEPOC(EmotivEPOCDevice* epocDevice, uint32 userId);
		void DoDataUpdateInsight(EmotivInsightDevice* insightDevice, uint32 userId);

		// conversion helpers
		static Sensor::EContactQuality FromEmotivContactQuality(IEE_EEG_ContactQuality_t contactQuality);
		static float FromEmotivWirelessSignalStatus(IEE_SignalStrength_t signalStrength);

		EE_DataChannel_t ToEPOCChannelID(const char* electrode);
		const char* FromEPOCChannelID(EE_DataChannel_t emotivChannel);
		
		IEE_DataChannel_t ToInsightChannelID(const char* electrode);
		const char* FromInsightChannelID(IEE_DataChannel_t emotivChannel);

		// event handler
		void OnRemoveDevice(Device* device) override;

	private:
		// all emotiv devices handled by the driver
		Core::Array<Device*>			mDevices;
		bool							mDetectOnce;
		bool							mIsSearching;

		// emotiv user ID for every device
		Core::Array<uint32>				mUserIDs;

		Core::String					mTempSensorName;
		Core::Array<double>				mTempSamplesBuffer;
		bool							mInitialized;
		EmoEngineEventHandle			mEventHandle;
		EmoStateHandle					mStateHandle;
		DataHandle						mDataHandle;
		float							mDataSizeInSeconds;
		float							mTimeFromStart;

		//////////////////////////////
		// Device hardware interfacing

		union UHwVersion
		{
			// original value from SDK
			unsigned long mValue;

			// for reading out both values without bitfiddling
			struct
			{
				uint16 mDongleVersion;
				uint16 mHeadsetVersion;
			} mVersions;
		};

		// hardware version of the devices
		enum EHardwareVersion
		{
			HWVERSION_EPOC_PREMIUM = 0x0500,
			HWVERSION_EPOC_CONSUMER = 0x1000,
			HWVERSION_EPOCPLUS_PREMIUM = 0x0600,
			HWVERSION_EPOCPLUS_CONSUMER = 0x3000,
			HWVERSION_INSIGHT_PREMIUM = 0x0800,
			HWVERSION_INSIGHT_CONSUMER = 0x5000,
		};

		// readable name
		static const char* GetReadableHardwareName(EHardwareVersion version)
		{
			switch (version)
			{
			case HWVERSION_EPOC_PREMIUM:		return "EPOC (Premium)";
			case HWVERSION_EPOC_CONSUMER:		return "EPOC (Consumer)";
			case HWVERSION_EPOCPLUS_PREMIUM:	return "EPOC+ (Premium)";
			case HWVERSION_EPOCPLUS_CONSUMER:	return "EPOC+ (Consumer)";
			case HWVERSION_INSIGHT_PREMIUM:		return "Insight (Premium)";
			case HWVERSION_INSIGHT_CONSUMER:	return "Insight (consumer)";
			default: return "Unknown";
			}
		}

		// all compatible EPOC versions
		static bool IsEpocDevice(uint16 version)
		{
			const uint16 hwVersion = version & 0xFF00;
			
			if (hwVersion == HWVERSION_EPOC_PREMIUM ||
				hwVersion == HWVERSION_EPOC_CONSUMER ||
				hwVersion == HWVERSION_EPOCPLUS_PREMIUM ||
				hwVersion == HWVERSION_EPOCPLUS_CONSUMER)
			{
				return true;
			}
			return false;
		}

		// all compatible Insight versions
		static bool IsInsightDevice(uint16 version)
		{
			const uint16 hwVersion = version & 0xFF00;
			if (hwVersion == HWVERSION_INSIGHT_PREMIUM ||
				hwVersion == HWVERSION_INSIGHT_CONSUMER)
			{
				return true;
			}
			return false;
		}
};

#endif

#endif
