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

#ifndef __NEUROMORE_DEVICEMANAGER_H
#define __NEUROMORE_DEVICEMANAGER_H

// include required headers
#include "Config.h"
#include "Networking/OscReceiver.h"
#include "Core/EventSource.h"
#include "Core/Mutex.h"
#include "Core/FpsCounter.h"
#include "DeviceDriver.h"
#include "Device.h"


// the DeviceManager class
class ENGINE_API DeviceManager : public Core::EventSource, OscReceiver
{
	friend Device;

	public:
		// constructors & destructor
		DeviceManager();
		virtual ~DeviceManager();

		// init, update, sync
		bool Init();
		void Update(const Core::Time& elapsed, const Core::Time& delta);
		
		void SyncDevices(double syncTime);
		void ResetDevices();
		void DetectDevices();

		//
		// Device Management
		//
		
		// device prototypes
		// TODO get rid of this, is only used by nodes to get to the sensor configuration -> will be replaced with JSON
		void RegisterDeviceType(Device* devicePrototype);
		void UnregisterDeviceType(uint32 deviceTypeID);
		const Device* GetRegisteredDeviceType(uint32 deviceTypeID);

		template<typename TDEVICE>
		inline void GetRegisteredDeviceTypeByHardwareNamePrefix(Core::String& prefix, Core::Array<const TDEVICE*>& out)
		{
			const uint32 numDevice = mRegisteredDeviceTypes.Size();
			for (uint32 i = 0; i < numDevice; ++i)
			{
				const TDEVICE* d = (const TDEVICE*)mRegisteredDeviceTypes[i];
				if (d->GetBaseType() == TDEVICE::BASE_TYPE_ID && Core::String(d->GetHardwareName()).Find(prefix) == 0)
					out.Add(d);
			}
		}

		Device* CreateDeviceObjectByType(uint32 deviceTypeID);

		// synchronous add/remove device
		void AddDevice(Device* device);
		void RemoveDevice(Device* device);

		// add/remove (async -> during next update() call)
		void AddDeviceAsync(Device* device);
		void RemoveDeviceAsync(Device* device);

		// get devices (by index)
		uint32 GetNumDevices() const									{ return mDevices.Size(); }
		Device* GetDevice(uint32 index) const							{ return mDevices[index]; }
		const Core::Array<Device*>& GetDevices() const					{ return mDevices; }

		// get devices (by type)
		uint32 FindDeviceIndex(Device* device) const;
		uint32 FindNumDevicesByType(uint32 deviceTypeID);
		Device* FindDeviceByType(uint32 deviceTypeID, uint32 deviceID);

		Core::Array<Device*> FindDevicesByUuid(const Core::String& deviceUuid);
		Core::Array<Device*> FindDevicesByType(uint32 deviceTypeID);			

		// device id management
		uint32 FindFreeDeviceId(uint32 deviceTypeID);

		// device configs (device definitions)
		bool AddDeviceConfig(const Device::DeviceConfig& config, bool replace = true);
		Core::Array<const Device::DeviceConfig*> FindDeviceConfigsByType(uint32 deviceType) const;
		bool HasDeviceConfigForDevice(uint32 deviceType, uint32 mDeviceID) const;
		const Device::DeviceConfig* FindDeviceConfigForDevice(uint32 deviceType, uint32 mDeviceID) const;
		
		// name <-> type mapping
		uint32 FindDeviceTypeByName(const Core::String& typeName);
		const char* FindDeviceNameByType(uint32 type);
		
		//
		// Device Driver management
		//

		void AddDeviceDriver(DeviceDriver* driver);
		uint32 GetNumDeviceDrivers() const									{ return mDeviceDrivers.Size(); }
		DeviceDriver* GetDeviceDriver(uint32 index) const					{ return mDeviceDrivers[index]; }
		DeviceDriver* FindDeviceDriverByDeviceType(uint32 deviceTypeID) const;
		DeviceDriver* FindDeviceDriverByType(uint32 driverTypeID) const;


		//
		// Configure Device Manager Behaviour
		//

		// enable/disable device autodetection in all device systems that support it
		void SetAutoDetectionEnabled(bool enable = true);

		// remove inactive devices after timeout is reached
		void SetRemoveInactiveDevicesEnabled(bool enable = true)		{ mRemoveInactiveDevices = enable; }
		
		// is true if at least one driver is currently searching for devices
		bool IsDetectionRunning();

		// true if at least one device is currently in test mode
		bool IsDeviceTestRunning();

		// true if at least one device is currently in test mode
		bool IsDevicePowerOk();
		

		//
		// Misc
		//

		// OscReceiver handles the autocreation of osc devices
		void ProcessMessage(OscMessageParser* message) override;

		// find the highest transmission delay accross all devices
		double FindMaximumLatency();
		
		// performance statistics
		const Core::FpsCounter& GetFpsCounter() const					{ return mFpsCounter; }

	private:
		void RemoveInactiveDevices();
		
		// active devices and drivers
		Core::Array<Device*>				mDevices;
		Core::Array<DeviceDriver*>			mDeviceDrivers;

		// prototype device instances
		Core::Array<Device*>				mRegisteredDeviceTypes;
		Core::Array<Device::DeviceConfig>	mDeviceConfigs;

		// async add/remove device lists
		Core::Array<Device*>			mDevicesToAdd;
		Core::Array<Device*>			mDevicesToRemove;

		// locks for add/remove device lists
		Core::Mutex						mAddLock;
		Core::Mutex						mRemoveLock;

		// device manager config
		bool							mRemoveInactiveDevices;

		// misc
		Core::String					mTempOscAddressPattern;
		Core::FpsCounter				mFpsCounter;

		// helpers
		int32 GetDeviceIDFromAddress(const char* address) const;
};

#endif
