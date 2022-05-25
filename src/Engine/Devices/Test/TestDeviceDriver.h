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

#ifndef __NEUROMORE_TESTDEVICEDRIVER_H
#define __NEUROMORE_TESTDEVICEDRIVER_H

// include required headers
#include "../../DeviceDriver.h"
#include "TestDevice.h"
#include "../../EngineManager.h"

// the driver for test devices
class ENGINE_API TestDeviceDriver : public DeviceDriver
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DRIVER_TYPEID_TEST };

		// constructor & destructor
		TestDeviceDriver();
		virtual ~TestDeviceDriver();

		const char* GetName() const override							{ return "Test Devices"; }

		uint32 GetType() const override									{ return TYPE_ID; }

		// initialization
		bool Init() override;

		// update process
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		Device* CreateDevice(uint32 deviceTypeID) override				{ return new TestDevice(this); }

		bool HasAutoDetectionSupport() const override					{ return false; }

	private:
		double		mTimeSinceDeviceCheck;
};

#endif
