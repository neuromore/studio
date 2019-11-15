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

#ifndef __NEUROMORE_ABMDRIVER_H
#define __NEUROMORE_ABMDRIVER_H

// include required headers
#include <DeviceDriver.h>
#include <EngineManager.h>
#include <Devices/ABM/AbmDevices.h>

#ifdef INCLUDE_DEVICE_ADVANCEDBRAINMONITORING

// include ABM SDK
#include <ABM/AbmSdkInclude.h>


// forward declaration
class AbmThreadHandler;

// the Advanced Brain Monitoring driver class
class AbmDriver : public DeviceDriver, Core::EventHandler
{
	friend AbmThreadHandler;

	public:
		enum { TYPE_ID = DeviceTypeIDs::DRIVER_TYPEID_ABM_BALERT };

		// constructor & destructor
		AbmDriver();
		virtual ~AbmDriver();

		const char* GetName() const override						{ return "Advanced Brain Monitoring Devices"; }
		
		uint32 GetType() const override								{ return TYPE_ID; }

		// initialization
		bool Init() override;

		// update process
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		Device* CreateDevice(uint32 deviceTypeID) override;
		bool HasAutoDetectionSupport() const override				{ return true; }
		void SetAutoDetectionEnabled(bool enable = true) override;
		void DetectDevices() override;

		bool IsDetectionRunning() const override					{ return mIsSearching; }

		// event handler
		void OnRemoveDevice(Device* device) override;

		// the ABM device (driver currently supports only one)
		AbmDevice*				mDevice;				 // note: has to be public due to global callback below

		static Core::String GetAbmErrorAsString(int errorcode);

	private:
		void LogDeviceInfo(_DEVICE_INFO* deviceInfo);

		// map SDK channel index to device sensors
		Sensor* GetRawSensorByIndex(AbmDevice* device, uint32 index);


		// thread
		Core::Thread*			mThread;
		bool					mIsSearching;
		AbmThreadHandler*		mAbmThreadHandler;
		_DEVICE_INFO			mPreviousDeviceInfo;
		bool					mHasPreviousDeviceInfo;

};


// FIXME this sometimes gets called during studio shutdown, even after driver/device have been removed
// Note: unfortunately, this has to be global for the SDK
// global callback for status info from device
static void __stdcall AbmDeviceOnStatusInfoCallback(_STATUS_INFO* sInfo)
{
	// somehow callback was called, but we have no device 
	AbmDriver* driver = static_cast<AbmDriver*>(GetDeviceManager()->FindDeviceDriverByDeviceType(AbmX24Device::TYPE_ID));
	if (driver == NULL)
		return;

	if (driver->mDevice == NULL)
		return;

	const double batteryChargeNormalized = sInfo->BatteryPercentage / 100.0;
	driver->mDevice->SetBatteryChargeLevel(batteryChargeNormalized);
}



// Note: unfortunately, this has to be global for the SDK
// global callback for status info from device
static void __stdcall AbmDeviceErrorCallback(int errorcode)
{
	LogError("ABM device reported error %i (%s)", errorcode, AbmDriver::GetAbmErrorAsString(errorcode) );

}



#endif

#endif
