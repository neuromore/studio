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

#ifndef __NEUROMORE_DEVICEDRIVER_H
#define __NEUROMORE_DEVICEDRIVER_H

// include required headers
#include "Config.h"
#include "Core/StandardHeaders.h"
#include "Core/String.h"
#include "Core/Array.h"
#include "Core/Thread.h"
#include "Core/Time.h"


// forward declaration
class Device;

// the device driver class
class ENGINE_API DeviceDriver
{
	public:
		// constructor & destructor
		DeviceDriver();
		virtual ~DeviceDriver();

		// type
		virtual uint32 GetType() const = 0;

		// name of the driver (e.g. "<vendorname> Driver")
		virtual const char* GetName() const = 0;

		// list of devices supported by the driver and the creator method
		uint32 GetNumSupportedDevices() const									{ return mSupportedDeviceTypes.Size(); }
		uint32 GetSupportedDeviceType(uint32 index) const						{ return mSupportedDeviceTypes[index]; }
		bool   IsDeviceSupported(uint32 deviceTypeID) const						{ return mSupportedDeviceTypes.Contains(deviceTypeID); }
		virtual Device* CreateDevice(uint32 deviceTypeID) = 0;

		// initialization
		virtual bool Init() = 0;

		// main update function
		virtual void Update(const Core::Time& delta, const Core::Time& elapsed) = 0;

		// enable/disable device driver
		bool IsEnabled() const													{ return mIsEnabled; }
		void SetEnabled(bool enabled = true);

		// autodetection of local devices
		virtual bool HasAutoDetectionSupport() const = 0;
		virtual void SetAutoDetectionEnabled(bool enable = true)				{ mUseAutoDetection = enable; }
		virtual bool IsAutoDetectionEnabled() const								{ return mUseAutoDetection; }
		virtual void DetectDevices()											{}
		virtual bool IsDetectionRunning() const									{ return false; }

		// test (impedance test and others)
		virtual void StartTest(Device* device)  								{ }
		virtual void StopTest(Device* device)  									{ }
		virtual bool IsTestRunning(Device* device)  							{ return false; }



	protected:
		bool mIsEnabled;
		bool mUseAutoDetection;

		void AddSupportedDevice(uint32 deviceTypeID)							{ mSupportedDeviceTypes.Add(deviceTypeID); }
		Core::Array<uint32> mSupportedDeviceTypes;

};

#endif
