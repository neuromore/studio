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
#include "GamepadDevice.h"
#include "../../EngineManager.h"
#include "../../Core/LogManager.h"


#ifdef INCLUDE_DEVICE_GAMEPAD

using namespace Core;

// constructor
GamepadDevice::GamepadDevice(DeviceDriver* driver) : Device(driver)
{
	LogDetailedInfo("Constructing gamepad device ...");

	// start in connected state
	mState = STATE_IDLE;

	// create all sensors
	CreateSensors();
}


// destructor
GamepadDevice::~GamepadDevice()
{
	LogDetailedInfo("Destructing gamepad device ...");
}


void GamepadDevice::CreateSensors()
{
	// left joystick
	mLeftAxisXSensor = new Sensor("Left X", 0);			AddSensor(mLeftAxisXSensor);
	mLeftAxisYSensor = new Sensor("Left Y", 0);			AddSensor(mLeftAxisYSensor);

	// right joystick
	mRightAxisXSensor = new Sensor("Right X", 0);		AddSensor(mRightAxisXSensor);
	mRightAxisYSensor = new Sensor("Right Y", 0);		AddSensor(mRightAxisYSensor);

	// abxy buttons
	mButtonASensor = new Sensor("A", 0);				AddSensor(mButtonASensor);
	mButtonBSensor = new Sensor("B", 0);				AddSensor(mButtonBSensor);
	mButtonXSensor = new Sensor("X", 0);				AddSensor(mButtonXSensor);
	mButtonYSensor = new Sensor("Y", 0);				AddSensor(mButtonYSensor);

	// left & right fire buttons
	mButtonL1Sensor = new Sensor("L1", 0);				AddSensor(mButtonL1Sensor);
	mButtonL2Sensor = new Sensor("L2", 0);				AddSensor(mButtonL2Sensor);
	mButtonL3Sensor = new Sensor("L3", 0);				AddSensor(mButtonL3Sensor);
	mButtonR1Sensor = new Sensor("R1", 0);				AddSensor(mButtonR1Sensor);
	mButtonR2Sensor = new Sensor("R2", 0);				AddSensor(mButtonR2Sensor);
	mButtonR3Sensor = new Sensor("R3", 0);				AddSensor(mButtonR3Sensor);

	// misc buttons
	mButtonSelectSensor = new Sensor("Select", 0);		AddSensor(mButtonSelectSensor);
	mButtonStartSensor = new Sensor("Start", 0);		AddSensor(mButtonStartSensor);
	mButtonGuideSensor = new Sensor("Guide", 0);		AddSensor(mButtonGuideSensor);

	// joystick buttons
	mButtonUpSensor = new Sensor("Up", 0);				AddSensor(mButtonUpSensor);
	mButtonDownSensor = new Sensor("Down", 0);			AddSensor(mButtonDownSensor);
	mButtonLeftSensor = new Sensor("Left", 0);			AddSensor(mButtonLeftSensor);
	mButtonRightSensor = new Sensor("Right", 0);		AddSensor(mButtonRightSensor);
	mButtonCenterSensor = new Sensor("Center", 0);		AddSensor(mButtonCenterSensor);
}

#endif
