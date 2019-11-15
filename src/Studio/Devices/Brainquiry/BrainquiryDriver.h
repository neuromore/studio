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

#ifndef __NEUROMORE_BRAINQUIRYDRIVER_H
#define __NEUROMORE_BRAINQUIRYDRIVER_H

#include <Config.h>
#include <DeviceDriver.h>
#include <Devices/Brainquiry/BrainquiryDevice.h>
#include "BrainquirySerialHandler.h"
#include <EngineManager.h>
#include <Core/EventHandler.h>
#include <Core/Array.h>

#include <QObject>
#include <QThread>
#include <QTimer>

#ifdef INCLUDE_DEVICE_BRAINQUIRY

#include <brainquiry/BQPetDLL.h>


class BrainquiryAutoDetection;
class BrainquirySerialThread;

// the SenseLabs OpenBCI system class
class BrainquiryDriver : public QObject, public DeviceDriver, public Core::EventHandler
{
	Q_OBJECT
	public:
		enum { TYPE_ID = DeviceTypeIDs::DRIVER_TYPEID_BRAINQUIRY };

		// constructor & destructor
		BrainquiryDriver();
		virtual ~BrainquiryDriver();

		const char* GetName() const override										{ return "Brainquiry Devices"; }

		uint32 GetType() const override												{ return TYPE_ID; }

		// initialization
		bool Init() override;

		// main update function
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		bool HasAutoDetectionSupport() const override								{ return true; }
		void SetAutoDetectionEnabled(bool enable = true) override;
		bool IsDetectionRunning() const override;
		void DetectDevices() override ;
	
		Device* CreateDevice(uint32 deviceTypeID) override;

		// add device at given serial port
		void AddDevice(BrainquiryDevice* device, int handle, Core::Array<int> channelNumbers, const char* serialPortName);

		// event handler (removes serial threads)
		void OnRemoveDevice(Device* device) override;

	private:

		// for autodetection
		QThread*							mAutoDetectionThread;
		QTimer*								mAutoDetectionTimer;
		BrainquiryAutoDetection*			mAutoDetection;

		// list to keep track of connected devices (index-matched with theh serial thread array below)
		Core::Array<BrainquiryDevice*>		mDevices;

		// connection IDs of each device
		Core::Array<int>					mHandles;
		
			// one thread per device
		// TODO use one thread for all serial handlers
		Core::Array<BrainquirySerialThread*>mSerialHandlerThreads;
};



class BrainquiryAutoDetection : public QObject
{
	Q_OBJECT
	public:
		// constructor & destructor
		BrainquiryAutoDetection(BrainquiryDriver* driver) : QObject()	{ mDriver = driver; mBreak = false; mIsRunning = false; mDetectOnce = false; mIsSearching = false; }
		virtual ~BrainquiryAutoDetection()								{}

		void Break()													{ mBreak = true; }
		void Start()													{ mIsRunning = true; }
		void Stop()														{ mIsRunning = false; }

		void DetectDevicesOnce()										{ mDetectOnce = true; }
		bool IsSearching() const										{ return mIsSearching; }

		public slots :
			void DetectDevices();

	private:
		BrainquiryDriver*	mDriver;
		bool				mDetectOnce;
		bool				mIsRunning;
		bool				mIsSearching;
		bool				mBreak;

		Core::String		mTempString;
};


class BrainquirySerialThread : public QThread
{
	Q_OBJECT
	public:
		BrainquirySerialThread(BrainquiryDevice* device, int handle,  Core::Array<int> channelNumbers, const char* serialPortName) : mDevice(device), mHandle(handle), mChannelNumbers(channelNumbers), mSerialPortName(serialPortName)  { }
		virtual ~BrainquirySerialThread()  {};

		void run() override;

	private:
		BrainquiryDevice*	mDevice;
		int					mHandle;
		Core::Array<int>	mChannelNumbers;
		Core::String		mSerialPortName;
};


#endif

#endif
