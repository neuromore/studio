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

#ifndef __NEUROMORE_ACTICHAMPDRIVER_H
#define __NEUROMORE_ACTICHAMPDRIVER_H

// include required headers
#include <DeviceDriver.h>
#include <EngineManager.h>
#include <Devices/BrainProducts/ActiChampDevice.h>

#ifdef INCLUDE_DEVICE_ACTICHAMP

class ActiChampDriver : public DeviceDriver, Core::EventHandler
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DRIVER_TYPEID_BRAINPRODUCTS };

		// constructor & destructor
		ActiChampDriver();
		virtual ~ActiChampDriver();

		const char* GetName() const override							{ return "ActiChamp Devices"; }

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

	private:
		Device*							mDevice;

		enum EState
		{
			STATE_IDLE,
			STATE_SEARCHING,
			STATE_STREAMING,
		};
		EState					mState;
};

#endif
#endif
