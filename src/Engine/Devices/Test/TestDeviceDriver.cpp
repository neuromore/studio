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

// include precompiled header
#include <Engine/Precompiled.h>

// include required files
#include "TestDeviceDriver.h"
#include "../../EngineManager.h"
#include "../../Core/LogManager.h"

using namespace Core;

// constructor
TestDeviceDriver::TestDeviceDriver() : DeviceDriver((brandingName == AntBrandingName || brandingName == StarrbaseBrandingName))
{
	AddSupportedDevice(TestDevice::TYPE_ID);

	mTimeSinceDeviceCheck = 0.0;
}


// destructor
TestDeviceDriver::~TestDeviceDriver()
{
}


bool TestDeviceDriver::Init()
{
	// just add one test headset
	//TestDevice* headset = new TestDevice(this);
	//GetDeviceManager()->AddDeviceAsync(headset);

	//
	//// just add one test headset
	//TestDevice* headset2 = new TestDevice(this);
	//GetDeviceManager()->AddDeviceAsync(headset2);

	return true;
}


// update the system
void TestDeviceDriver::Update(const Time& elapsed, const Time& delta)
{
	if (IsEnabled() == false)
		return;

	mTimeSinceDeviceCheck += delta.InSeconds();
	if (mTimeSinceDeviceCheck > 1.0)
	{
		// add one test headset in case no other device is connected
		const uint32 numTestHeadsets = GetDeviceManager()->FindNumDevicesByType(TestDevice::TYPE_ID);
		if (numTestHeadsets == 0)
		{
			if (GetDeviceManager()->GetNumDevices() == 0)
			{
				Device* testHeadset = new TestDevice(this);
				GetDeviceManager()->AddDeviceAsync(testHeadset);
			}
		}
		else
		{
			// remove the test headset as soon as a real device gets connected
			if (GetDeviceManager()->GetNumDevices() > numTestHeadsets)
			{
				Device* testHeadset = GetDeviceManager()->FindDeviceByType(TestDevice::TYPE_ID, 0);
				GetDeviceManager()->RemoveDeviceAsync(testHeadset);
			}
		}

		mTimeSinceDeviceCheck = 0.0;
	}
}
