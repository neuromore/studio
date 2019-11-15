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

#ifndef __NEUROMORE_VERSUSSYSTEM_H
#define __NEUROMORE_VERSUSSYSTEM_H


#include <Config.h>
#include <DeviceDriver.h>
#include <Devices/Versus/VersusDevice.h>
#include <EngineManager.h>
#include <Core/EventHandler.h>
#include <Core/Array.h>

#include <QObject>
#include <QThread>
#include <QTimer>

#ifdef INCLUDE_DEVICE_SENSELABS_VERSUS


// forward decl. of classes at bottom of header
class VersusAutoDetection;
class VersusSerialThread;

// the SenseLabs Versus system class
class VersusDriver : public QObject, public DeviceDriver, public Core::EventHandler
{
	Q_OBJECT
	public:

		enum { TYPE_ID = DeviceTypeIDs::DRIVER_TYPEID_SENSELABS };

		// constructor & destructor
		VersusDriver();
		virtual ~VersusDriver();

		const char* GetName() const override							{ return "Senselabs Devices"; }

		uint32 GetType() const override									{ return TYPE_ID; }

		// initialization
		bool Init() override;

		// main update function
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		bool HasAutoDetectionSupport() const override		{ return true; }
		void SetAutoDetectionEnabled(bool enable = true) override;
		bool IsDetectionRunning() const override;
		void DetectDevices() override;

		virtual Device* CreateDevice(uint32 deviceTypeID) override;

		// add device at given serial port
		void AddDevice(VersusDevice* device, const char* serialPort);

		// event handler (removes serial threads)
		void OnRemoveDevice(Device* device) override;

	private:

		// for autodetection
		QThread*							mAutoDetectionThread;
		QTimer*								mAutoDetectionTimer;
		VersusAutoDetection*				mAutoDetection;

		// list to keep track of connected devices (index-matched with theh serial thread array below)
		Core::Array<VersusDevice*>			mDevices;

		// one thread per device
		// TODO use one thread for all serial handlers
		Core::Array<VersusSerialThread*>	mSerialHandlerThreads;
};


class VersusAutoDetection : public QObject
{
	Q_OBJECT
	public:
		// constructor & destructor
		VersusAutoDetection(VersusDriver* driver) : QObject()		{ mDriver = driver; mBreak = false; mIsRunning = false; mDetectOnce = false; mIsSearching = false; }
		virtual ~VersusAutoDetection()								{}

		void Break()												{ mBreak = true; }
		void Start()												{ mIsRunning = true; }
		void Stop()													{ mIsRunning = false; }

		void DetectDevicesOnce()									{ mDetectOnce = true; }
		bool IsSearching() const									{ return mIsSearching; }

		public slots :
			void DetectDevices();

	private:
		VersusDriver*	mDriver;
		bool			mIsRunning;
		bool			mDetectOnce;
		bool			mIsSearching;
		bool			mBreak;
};


class VersusSerialThread : public QThread
{
	Q_OBJECT
	public:
		VersusSerialThread(VersusDevice* device, const char* serialPortName) : mDevice(device), mPortName(serialPortName)	{}
		virtual ~VersusSerialThread() {};

		void run() override;

	private:
		VersusDevice*	mDevice;
		Core::String	mPortName;
};

#endif

#endif
