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

// include required files
#include "DeviceManager.h"
#include "Core/LogManager.h"
#include "Core/EventSource.h"
#include "EngineManager.h"
#include "Notifications.h"

using namespace Core;

// constructor
DeviceManager::DeviceManager()
{
	LogDetailedInfo("Constructing device manager ...");

	// reserve memory
	mTempOscAddressPattern.Reserve( 2048 );

	// registers itself as catch-all osc receiver, so it can automatically create osc devices
	GetOscMessageRouter()->RegisterCatchAllReceiver(this);

	// enable autoremoval by default
	mRemoveInactiveDevices = true;
}


// destructor
DeviceManager::~DeviceManager()
{
	LogDetailedInfo("Destructing device manager ...");

	// unregister itself as catch all osc receiver
	GetOscMessageRouter()->UnregisterReceiver(this);

	//
	// remove all devices
	//

	// copy list of device we have to remove (because device list will be modified during the process)
	const uint32 numDevices = mDevices.Size();
	Array<Device*> devices = mDevices;
	for (uint32 i=0; i<numDevices; ++i)
	{
		// call remove first
		RemoveDevice(devices[i]);
	}
	mDevices.Clear();

	mAddLock.Lock();
	DestructArray(mDevicesToAdd);
	mAddLock.Unlock();

	mRemoveLock.Lock();
	DestructArray(mDevicesToRemove);
	mRemoveLock.Unlock();

	//
	// remove all device drivers
	//

	DestructArray(mDeviceDrivers);

	//
	// remove registered device types
	//
	DestructArray(mRegisteredDeviceTypes);
}


// initialize device manager
bool DeviceManager::Init()
{
	return true;
}


// main update loop
void DeviceManager::Update(const Time& elapsed, const Time& delta)
{
	mFpsCounter.BeginTiming();

	// check for inactive devices and remove them (if feature is enabled)
	if (mRemoveInactiveDevices == true)
		RemoveInactiveDevices();

	// add new devices 
	mAddLock.Lock();
	const uint32 numDevicesToAdd = mDevicesToAdd.Size();
	for (uint32 i=0; i<numDevicesToAdd; ++i)
		AddDevice(mDevicesToAdd[i]);
	mDevicesToAdd.Clear();
	mAddLock.Unlock();

	// remove devices 
	mRemoveLock.Lock();
	const uint32 numDevicesToRemove = mDevicesToRemove.Size();
	for (uint32 i=0; i<numDevicesToRemove; ++i)
		RemoveDevice(mDevicesToRemove[i]);
	mDevicesToRemove.Clear();
	mRemoveLock.Unlock();

	// update device systems
	const uint32 numDrivers = mDeviceDrivers.Size();
	for (uint32 i=0; i<numDrivers; ++i)
		mDeviceDrivers[i]->Update(elapsed, delta);

	// update devices
	const uint32 numDevices = mDevices.Size();
	for (uint32 i=0; i<numDevices; ++i)
		mDevices[i]->Update(elapsed, delta);

	mFpsCounter.StopTiming();
}


// register a device using an unitialized instance
void DeviceManager::RegisterDeviceType(Device* prototype)
{
	LogDetailedInfo("Registering device prototype ...");

	// add only if there is no such device yet
	if (GetRegisteredDeviceType(prototype->GetType()) == NULL)
	{
		mRegisteredDeviceTypes.Add(prototype);
		LogInfo( "Device prototype '%s' registered", prototype->GetHardwareName() );
	}
	else
	{ 
		delete prototype;
	}
}


// unregister a device type, deletes the prototype device
void DeviceManager::UnregisterDeviceType(uint32 typeID)
{
	LogDetailedInfo("Unregistering device prototype ...");

	// iterate over registered device prototype and remove the one with the matching ID
	const uint32 numDevice = mRegisteredDeviceTypes.Size();
	for (uint32 i=0; i<numDevice; ++i)
	{
		if (mRegisteredDeviceTypes[i]->GetType() == typeID)
		{
			// destruct device
			delete mRegisteredDeviceTypes[i];
		
			// delete array object
			mRegisteredDeviceTypes.Remove(i);
					
			LogInfo( "Device prototype unregistered");

			// done
			return;
		}
	}
}


// search for the device prototype with the matching id
const Device* DeviceManager::GetRegisteredDeviceType(uint32 typeID)
{
	const uint32 numDevice = mRegisteredDeviceTypes.Size();
	for (uint32 i=0; i<numDevice; ++i)
	{
		if (mRegisteredDeviceTypes[i]->GetType() == typeID)
			return mRegisteredDeviceTypes[i];
	}

	return NULL;
}


// run device detection once
void DeviceManager::DetectDevices()
{
	const uint32 numDrivers = mDeviceDrivers.Size();
	for (uint32 i = 0; i<numDrivers; ++i)
	{
		DeviceDriver* driver = mDeviceDrivers[i];
		driver->DetectDevices();
	}
}

// enable/disable device autodetection in all device drivers that support it
void DeviceManager::SetAutoDetectionEnabled(bool enable)
{
	if (enable == true)
		LogDetailedInfo("Enabling device auto detection ...");
	else
		LogDetailedInfo("Disabling device auto detection ...");

	const uint32 numDrivers = mDeviceDrivers.Size();
	for (uint32 i=0; i<numDrivers; ++i)
	{
		DeviceDriver* driver = mDeviceDrivers[i];
		if (driver->HasAutoDetectionSupport())
			driver->SetAutoDetectionEnabled(enable);
	}
}


// check if a driver is currently searching for devices
bool DeviceManager::IsDetectionRunning()
{
	const uint32 numDrivers = mDeviceDrivers.Size();
	for (uint32 i = 0; i<numDrivers; ++i)
	{
		DeviceDriver* driver = mDeviceDrivers[i];
		if (driver->HasAutoDetectionSupport())
			if (driver->IsDetectionRunning())
				return true;
	}

	return false;
}


// check if any of the devices is in test mode
bool DeviceManager::IsDeviceTestRunning()
{
	const uint32 numDevices = mDevices.Size();
	for (uint32 i=0; i<numDevices; ++i)
		if (mDevices[i]->IsTestRunning() == true)
			return true;

	return false;
}


// check if any of the devices is in test mode
bool DeviceManager::IsDevicePowerOk()
{
	const uint32 numDevices = mDevices.Size();
	for (uint32 i=0; i<numDevices; ++i)
		if (mDevices[i]->IsBatteryCritical() == true)
			return false;

	return true;
}



// add a new device
void DeviceManager::AddDevice(Device* device)
{
	LogDetailedInfo("Adding device ...");

	// get unique device id, if needed
	if (device->GetDeviceID() == CORE_INVALIDINDEX32)
	{
		uint32 freeId = FindFreeDeviceId(device->GetType());
		device->SetDeviceId(freeId);
	}
	else 
	{
		// check if device id is unique, can't add another device of this type and id
		if (FindDeviceByType(device->GetType(), device->GetDeviceID()) != NULL)
		{
			uint32 freeId = FindFreeDeviceId(device->GetType());
			LogError("Can't add device '%s': a device of type '%s' with ID %i already exists - changing ID to %i to avoid clash.", device->GetName().AsChar(), device->GetTypeName(), device->GetDeviceID(), freeId);
			device->SetDeviceId(freeId);
		}
	}

	// configure device if not already (and if possible)
 	if (device->IsConfigured() == false && HasDeviceConfigForDevice(device->GetType(), device->GetDeviceID()))
	{
		const Device::DeviceConfig* config = FindDeviceConfigForDevice(device->GetType(), device->GetDeviceID());
		device->Configure(*config);
	}

	// set device name, if not already
	if (device->GetName().IsEmpty() == true)
	{
		// format as "HardwareName (ID)", if ID>0
		if (device->GetDeviceID() == 0)
			device->SetName(device->GetHardwareName());
		else
		{
			String tmpStr; tmpStr.Format("%s (%i)", device->GetHardwareName(), device->GetDeviceID());
			device->SetName(tmpStr.AsChar());
		}
	}


	// add device
	mDevices.Add(device);

	// fire events
	EMIT_EVENT( OnDeviceAdded(device) );

	//String message;		message.Format("%s connected", device->GetName().AsChar());
	//String description; description.Format("The device '%s' connected sucessfully.", device->GetName().AsChar());
	//EMIT_EVENT( OnInfo(Notifications::INFO_DEVICE_CONNECTED, message.AsChar(), description.AsChar()) );

	// if device is a headset.. set it as the active one
	if (device->GetBaseType() == BciDevice::BASE_TYPE_ID)
		GetEngine()->SetActiveBci(static_cast<BciDevice*>(device));

	// Auto Sync: schedule an engine sync (doesn't disturb a running session)
	if (GetEngine()->GetAutoSyncSetting() == true)
		GetEngine()->SyncAsync();

	LogInfo( "Device '%s' added", device->GetHardwareName() );
}


// add the device during the next update call
void DeviceManager::AddDeviceAsync(Device* device)
{
	LogDetailedInfo("Adding device async ...");

	mAddLock.Lock();

	mDevicesToAdd.Add(device);

	// register device as osc message receiver
	GetOscMessageRouter()->RegisterReceiver(device);

	mAddLock.Unlock();
}


// remove the given device
void DeviceManager::RemoveDevice(Device* device)
{
	String name = device->GetHardwareName();
	LogInfo( "Removing '%s' device ...", name.AsChar() );

	// get the index of the device and return if it is not in anymore
	const uint32 index = mDevices.Find(device);
	if (index == CORE_INVALIDINDEX32)
	{
		LogError( "Cannot remove device. Device is not part of the core manager." );
		return;
	}

	// 1) change active headset before removing
	// if device is the current active headset, we try to find another one we can select
	if (GetEngine()->GetActiveBci() == device)
	{
		Device* deviceToSelect = NULL;

		// find a headset in the device list that is not the one we are removing
		const uint32 numDevices = mDevices.Size();
		for (uint32 i = 0; i<numDevices; ++i)
		{
			if (mDevices[i] == device)
				continue;

			if (mDevices[i]->GetBaseType() == BciDevice::BASE_TYPE_ID)
				deviceToSelect = mDevices[i];
		}

		// found a device?
		if (deviceToSelect != NULL)
		{
			BciDevice* bciDevice = static_cast<BciDevice*>(deviceToSelect);
			GetEngine()->SetActiveBci(bciDevice);
		}
		else
		{
			// no more device we can select: select none
			GetEngine()->SetActiveBci(NULL);
		}
	}

	// 2) fire pre-event
	EMIT_EVENT(OnRemoveDevice(device));

	// 3) unregister device from the osc message router
	GetOscMessageRouter()->UnregisterReceiver(device);

	// 5) remove the device from manager
	mDevices.Remove(index);

	// 6) deallocate device
	delete device;

	// 7) fire post-events
	EMIT_EVENT(OnDeviceRemoved(device));

	//String message;		message.Format("%s disconnected", name.AsChar() );
	//String description; description.Format("The device '%s' has been disconnected.", name.AsChar() );
	//EMIT_EVENT( OnInfo(Notifications::INFO_DEVICE_CONNECTED, message.AsChar(), description.AsChar()) );

	LogDetailedInfo( "Device removed" );
}


// add the device during the next update call
void DeviceManager::RemoveDeviceAsync(Device* device)
{
	LogInfo( "Removing '%s' device ...", device->GetHardwareName() );

	mRemoveLock.Lock();

	mDevicesToRemove.Add(device);

	mRemoveLock.Unlock();

	LogDetailedInfo( "Device removal scheduled" );
}


// check for devices that have timed out, and schedule them for async removal
void DeviceManager::RemoveInactiveDevices()
{
	// iterate through devices and remov devices that are inactive for at last $seconds seconds
	const uint32 numDevices = mDevices.Size();
	for (uint32 i=0; i<numDevices; ++i)
	{
		Device* device = mDevices[i];

		if (device->IsTimeoutReached())
		{
			LogDetailedInfo( "Device '%s' timed out. Adding it to async removal queue ...", device->GetHardwareName());
			mDevicesToRemove.Add(device);			
		}
	}
}


// find the highest latency of all device sensors
double DeviceManager::FindMaximumLatency()
{
	// iterate through devices and find the maximum transmission delay
	const uint32 numDevices = mDevices.Size();

	double maxLatency = 0;
	for (uint32 i=0; i<numDevices; ++i)
	{
		maxLatency = Max(maxLatency, mDevices[i]->FindMaxLatency());
	}

	return maxLatency;
}


// synchronize all sensors of all devices so the next sample that is received falls on the given relative time
void DeviceManager::SyncDevices(double syncTime)
{
	const uint32 numDevices = mDevices.Size();

	for (uint32 i=0; i<numDevices; ++i)
	{
		Device* device = mDevices[i];
		device->Sync(syncTime);
	}
}


// reset all device sensors and queues (dont reset connection)
void DeviceManager::ResetDevices()
{
	const uint32 numDevices = mDevices.Size();

	for (uint32 i = 0; i<numDevices; ++i)
	{
		Device* device = mDevices[i];
		device->Reset();
	}
}


// get number of devices of this type
uint32 DeviceManager::FindNumDevicesByType(uint32 deviceTypeID)
{
	uint32 numFound = 0;

	// get the number of devices and iterate through them
	const uint32 numDevices = mDevices.Size();
	for (uint32 i=0; i<numDevices; ++i)
	{
		if (mDevices[i]->GetType() == deviceTypeID)
			numFound++;
	}

	return numFound;
}


// get the device of this type with this device index
Device* DeviceManager::FindDeviceByType(uint32 deviceTypeID, uint32 deviceID)
{
	Device* foundDevice = NULL;

	// get the number of devices and iterate through them
	const uint32 numDevices = mDevices.Size();
	for (uint32 i=0; i<numDevices; ++i)
	{
		Device* device = mDevices[i];
		if (device->GetType() == deviceTypeID && device->GetDeviceID() == deviceID)
			foundDevice = device;
	}

	return foundDevice;
}


// find the index for the given device
uint32 DeviceManager::FindDeviceIndex(Device* device) const
{
	const uint32 numDevices = mDevices.Size();
	for (uint32 i=0; i<numDevices; ++i)
	{
		if (mDevices[i] == device)
			return i;
	}

	return CORE_INVALIDINDEX32;
}


// find devices of a certain UUID
Array<Device*> DeviceManager::FindDevicesByUuid(const Core::String& deviceUuid)
{
	Array<Device*> foundDevices;

	// get the number of devices and iterate through them
	const uint32 numDevices = mDevices.Size();
	for (uint32 i=0; i<numDevices; ++i)
	{
		// check uuid
		if (deviceUuid.IsEqual(mDevices[i]->GetUuid()) == true)
			foundDevices.Add(mDevices[i]);
	}

	return foundDevices;
}


// find devices of a certain type or UUID
Array<Device*> DeviceManager::FindDevicesByType(uint32 deviceTypeID)
{
	Array<Device*> foundDevices;

	// get the number of devices and iterate through them
	const uint32 numDevices = mDevices.Size();
	for (uint32 i=0; i<numDevices; ++i)
	{
		// check device type
		if (mDevices[i]->GetType() == deviceTypeID)
			foundDevices.Add(mDevices[i]);
	}

	return foundDevices;
}


// find driver that is responsible for a certain device
DeviceDriver* DeviceManager::FindDeviceDriverByType(uint32 driverTypeID) const
{
	const uint32 numDrivers = mDeviceDrivers.Size();
	for (uint32 i = 0; i < numDrivers; ++i)
	{
		DeviceDriver* driver = mDeviceDrivers[i];
		if (driver->GetType() == driverTypeID)
			return driver;
	}

	return NULL;
}


// find driver that is responsible for a certain device
DeviceDriver* DeviceManager::FindDeviceDriverByDeviceType(uint32 deviceTypeID) const
{
	const uint32 numDrivers = mDeviceDrivers.Size();
	for (uint32 i = 0; i < numDrivers; ++i)
	{
		DeviceDriver* driver = mDeviceDrivers[i];
		if (driver->IsDeviceSupported(deviceTypeID) == true)
			return driver;
	}

	return NULL;
}


// add a device system (type ID has to be unique!)
void DeviceManager::AddDeviceDriver(DeviceDriver* driver)
{
	LogDetailedInfo("Adding device Driver ...");

	// make sure this driver does not support devices that are supported by another driver (every type of device can have only a single driver)
	const uint32 numDrivers = mDeviceDrivers.Size();
	const uint32 numDevicesToSupport = driver->GetNumSupportedDevices();
	for (uint32 i = 0; i < numDevicesToSupport; ++i)
	{
		// the devices supported by the driver to be added
		uint32 deviceTypeToSupport = driver->GetSupportedDeviceType(i);

		// check against all other drivers that were added before
		for (uint32 d = 0; d < numDrivers; ++d)
		{
			DeviceDriver* otherDriver = mDeviceDrivers[d];
		
			// check for overlap
			const uint32 numSupportedDevices = otherDriver->GetNumSupportedDevices();
			for (uint32 s = 0; s < numSupportedDevices; ++s)
			{
				uint32 deviceTypeSupportByOther = otherDriver->GetSupportedDeviceType(s);
				CORE_ASSERT(deviceTypeToSupport != deviceTypeSupportByOther);
				if (deviceTypeToSupport == deviceTypeSupportByOther)
				{
					// we have overlap! The driver cannot be added.
					LogWarning("Multiple drivers were added for devices of type %x, driver %s will be removed.", deviceTypeToSupport, driver->GetName());
					delete driver;
					return;
				}
			}
		}
	}

	// configure driver autodetection
	if (driver->HasAutoDetectionSupport() == true)
		driver->SetAutoDetectionEnabled(GetEngine()->GetAutoDetectionSetting());
	
	// add driver
	mDeviceDrivers.Add( driver );

	LogInfo("Device driver '%s' added", driver->GetName());
}

// search for a free device id
uint32 DeviceManager::FindFreeDeviceId(uint32 deviceType)
{
	uint32 foundIndex = CORE_INVALIDINDEX32;

	// for all ids: iterate over devices (of the given type) and check if the id is free
	const uint32 numDevices = mDevices.Size();
	for (uint32 id=0;; ++id)
	{
		// check if this id is unique
		bool unique = true;
		for (uint32 i=0; i<numDevices && unique; ++i)
		{
			Device* device = mDevices[i];
			if (device->GetType() != deviceType)
				continue;

			// check if id is free
			if (device->GetDeviceID() == id)
				unique = false;
		}
		
		// found a free id 
		if (unique == true)
		{
			foundIndex = id;
			break;
		}
	}
	
	return foundIndex;
}


// load from json (does not delete jsonParser on failure, but in case of success the DeviceManager takes ownership
bool DeviceManager::AddDeviceConfig(const Device::DeviceConfig& config, bool replace)
{
	if (config.mIsValid == false)
		return false;

	if (config.mIsEnabled == false)
	{
		LogInfo("Skipping disabled device definition");
		return true;
	}

	// check if we already have a definition with this exact name
	String name = config.mName;
	name.ToLower();

	const uint32 numDefs = mDeviceConfigs.Size();
	for (uint32 i = 0; i < numDefs; ++i)
	{
		Device::DeviceConfig* def = &mDeviceConfigs[i];

		// name matches and replace is disabled
		if (def->mName.Compare(name) == 0)
		{
			if (replace == false)
			{
				LogWarning("Error loading device definition: There already is a definition with the name '%s'.", name.AsChar());
				return false;
			}
			else // replace existing definition
			{
				mDeviceConfigs[i] = config;
				break;
			}
		}
	}

	mDeviceConfigs.Add(config);
	
	// we're done
	const char* typeName = FindDeviceNameByType(config.mDeviceType);
	LogInfo("Loaded device definition '%s' for device of type '%s'", name.AsChar(), typeName);

	return true;
}


Core::Array<const Device::DeviceConfig*> DeviceManager::FindDeviceConfigsByType(uint32 deviceType) const
{
	Core::Array<const Device::DeviceConfig*> configs;
	
	const uint32 numConfigs = mDeviceConfigs.Size();
	for (uint32 i = 0; i < numConfigs; ++i)
	{
		const Device::DeviceConfig& config = mDeviceConfigs[i];

		if (config.mDeviceType == deviceType)
			configs.Add(&config);
	}

	return configs;
}

bool DeviceManager::HasDeviceConfigForDevice(uint32 deviceType, uint32 mDeviceID) const
{
	const uint32 numDefs = mDeviceConfigs.Size();
	for (uint32 i = 0; i < numDefs; ++i)
	{
		const Device::DeviceConfig& definition = mDeviceConfigs[i];

		if (definition.mDeviceType != deviceType)
			continue;

		if (definition.mDeviceID != mDeviceID)
			continue;

		// found matching device type with matching index
		return true;
	}

	return false;
}


// find device definition for unique device (type + ID makes is unique identification)
const Device::DeviceConfig* DeviceManager::FindDeviceConfigForDevice(uint32 deviceType, uint32 mDeviceID) const
{
	const uint32 numDefs = mDeviceConfigs.Size();
	for (uint32 i = 0; i < numDefs; ++i)
	{
		const Device::DeviceConfig& definition = mDeviceConfigs[i];

		if (definition.mDeviceType != deviceType)
			continue;

		if (definition.mDeviceID != mDeviceID)
			continue;

		// found matching device type with matching index
		return &definition;
	}

	CORE_ASSERT(false);
	return NULL;
}


// mapping typeName -> tupeId via registered devices
uint32 DeviceManager::FindDeviceTypeByName(const String& typeName)
{
	// lowercase device type name, just to be general
	String lcTypeName = typeName; 
	lcTypeName.ToLower();

	const uint32 numRegDevices = mRegisteredDeviceTypes.Size();
	for (uint32 i = 0; i < numRegDevices; ++i)
	{
		Device* device = mRegisteredDeviceTypes[i];

		// found matching device type
		if (lcTypeName.Compare(device->GetTypeName()) == 0)
			return device->GetType();
	}

	return DeviceTypeIDs::INVALID_DEVICE_TYPEID;
}


// mapping typeID -> typeName via registered devices
const char* DeviceManager::FindDeviceNameByType(uint32 type)
{
	const uint32 numRegDevices = mRegisteredDeviceTypes.Size();
	for (uint32 i = 0; i < numRegDevices; ++i)
	{
		Device* device = mRegisteredDeviceTypes[i];

		// found matching device type
		if (device->GetType() == type)
			return device->GetTypeName();
	}

	return "UNKNOWNDEVICE";
}
		

// process the osc message that could not be routed anywhere else and try to autocreate devices if the osc address prefix is known
void DeviceManager::ProcessMessage(OscMessageParser* message) 
{
	// check if the address of the message fits any of the known device types

	// NOTE: Reverted back temporarly
	// we don't want to loose messages during device creation: 
	// Multiple messages come in at the same time and the first message creates the devices, then all other messages are in the wrong queue.
	// -> We simply route the messages again (just like the OscMessageRouter does)
	// -> feels weird at first but makes sense: "The Device manager handle the routing during device startup, because we can't reroute messages)
	//OscMessageQueue* queue = GetOscMessageRouter()->FindMessageQueueByOscAddress(message->GetAddress());
	//if (queue != NULL)
	//{
	//	// clone message and push it into the other queue
	//	queue->Push(new OscMessageParser(*message));
	//	message->mIsReady = true;
	//	return;
	//}

	const uint32 numRegistered = mRegisteredDeviceTypes.Size();
	for (uint32 i=0; i<numRegistered; ++i)
	{
		Device* prototype = mRegisteredDeviceTypes[i];
		const uint32 deviceType = prototype->GetType();

		// check if address matches the address pattern of the device
		if (message->MatchAddress(prototype->GetOscPathPattern().AsChar()))
		{
			// try to extract device ID (mandatory) and the device String (optional) from the message address
			const int32 deviceId = prototype->GetOscPathDeviceId(message->GetAddress());
			const Core::String deviceString = prototype->GetOscPathDeviceString(message->GetAddress());

			// looks like we have a match! Create the device!
			if (deviceId != -1)
			{
				// find driver for that device
				DeviceDriver* driver = FindDeviceDriverByDeviceType(deviceType);
				
				Device* device = NULL;

				// let device driver create the device, clone it if there isn't one
				if (driver != NULL)
					device = driver->CreateDevice(deviceType);
				else
					device = prototype->Clone();
			
				// configure the device id and string
				device->SetDeviceId(deviceId);
				device->SetDeviceString(deviceString);
			
				// add device to engine asynchronously
				// Note: this also registers the receiver immediately!
				AddDeviceAsync(device);

				// process the first message directly so we don't lose it
				device->ProcessMessage(message);

				// NOTE: Reverted back temporarly
				// the other (old) solution: we have to clear the message queue
				GetOscMessageRouter()->GetMessageQueue(this)->ClearQueue();

				break;
			}
		}

		// mark message as processed
		message->mIsReady = true;
	}
}


Device* DeviceManager::CreateDeviceObjectByType(uint32 deviceTypeID)
{
	DeviceDriver* driver = FindDeviceDriverByDeviceType(deviceTypeID);
	if (driver == NULL)
		return NULL;

	return driver->CreateDevice(deviceTypeID);
}
