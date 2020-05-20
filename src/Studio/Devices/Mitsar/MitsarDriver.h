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

#ifndef __NEUROMORE_MITSARDRIVER_H
#define __NEUROMORE_MITSARDRIVER_H

// include required headers
#include <DeviceDriver.h>
#include <EngineManager.h>
#include <Devices/Mitsar/MitsarDevices.h>

#ifdef INCLUDE_DEVICE_MITSAR

// forward decl. of classes at bottom of header
class MitsarConnectorHandler;

// the Mitsar system class
class MitsarDriver : public DeviceDriver, Core::EventHandler
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DRIVER_TYPEID_MITSAR };

		// constructor & destructor
		MitsarDriver();
		virtual ~MitsarDriver();

		const char* GetName() const override							{ return "Mitsar Devices"; }

		uint32 GetType() const override final							{ return TYPE_ID; }

		// initialization
		bool Init() override;

		// update process
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		bool HasAutoDetectionSupport() const override					{ return true; }
		void SetAutoDetectionEnabled(bool enable = true) override;
		bool IsDetectionRunning() const override;

		void DetectDevices() override;
		Device* CreateDevice(uint32 deviceTypeID) override;

		// event handler (stop thread if device is removed)
		void OnRemoveDevice(Device* device) override;
		void OnDeviceAdded(Device* device) override;

		// impedance test
		void StartTest(Device* device) override;
		void StopTest(Device* device) override;
		bool IsTestRunning(Device* device) override;

   protected:
      virtual void StartAutoDetection() override;
      virtual void StopAutoDetection() override;

	private:
		
		Device*							mDevice;

		// thread for device detection
		Core::Thread*					mThread;
		MitsarConnectorHandler*			mThreadHandler;
};


class MitsarConnectorHandler : public Core::ThreadHandler
{
	friend MitsarDriver;

	public:
		// constructor & destructor
		MitsarConnectorHandler(MitsarDriver* driver);
		virtual ~MitsarConnectorHandler()					{}

		void Execute();
		void Terminate();

		void SetAutoDetectEnabled(bool enable = true);
		void DetectOnce();
		void StartImpedanceTest();
		void StopImpedanceTest();

		bool IsSearching() const							{ return mState == STATE_SEARCHING; }

	private:
		MitsarDriver*			mDriver;

		enum EMode
		{
			MODE_STREAM,			// normal EEG data stream
			MODE_STREAM_RAW,		// raw stream (uncalibrated frontend values)
			MODE_IMPEDANCE			// impedance test 
		};
		EMode					mMode;
		Core::Mutex				mModeLock;

		EMode GetDefaultStreamMode() const		{ return MODE_STREAM; /*Note: switch to Raw stream here*/}

		bool					mUseAutoDetection;

		// break flag for terminating thread loop
		bool					mBreak;
		bool					mForceStart;


		enum EState
		{
			STATE_IDLE,
			STATE_SEARCHING,
			STATE_STREAMING,
		};
		EState					mState;

		// holds the handle to the Mitsar Connector Process
		PROCESS_INFORMATION		mProcessInformation;

		bool StartMitsarConnector(EMode mode);
		void StopMitsarConnector();
		
};

#endif

#endif
