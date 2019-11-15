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

#ifndef __NEUROMORE_GAMEPADDEVICE_H
#define __NEUROMORE_GAMEPADDEVICE_H

// include required headers
#include "../../Config.h"
#include "../../Device.h"

#ifdef INCLUDE_DEVICE_GAMEPAD

// gamepad class
class ENGINE_API GamepadDevice : public Device
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_GENERIC_GAMEPAD };

		// constructor & destructor
		GamepadDevice(DeviceDriver* driver = NULL);
		virtual ~GamepadDevice();

		Device* Clone() override							{ return new GamepadDevice(); }

		// overloaded
		uint32 GetType() const override						{ return TYPE_ID; }
		const char* GetHardwareName() const override		{ return "Gamepad"; }
		const char* GetUuid() const override				{ return "effcc89c-7b40-11e6-8b77-86f30ca893d3"; }
		const char* GetTypeName() const override			{ return "gamepad"; }

		static const char* GetRuleName()					{ return "DEVICE_Gamepad"; }

		void CreateSensors() override;

	protected:
		// left joystick
		Sensor*			mLeftAxisXSensor;
		Sensor*			mLeftAxisYSensor;

		// right joystick
		Sensor*			mRightAxisXSensor;
		Sensor*			mRightAxisYSensor;

		// abxy buttons
		Sensor*			mButtonASensor;
		Sensor*			mButtonBSensor;
		Sensor*			mButtonXSensor;
		Sensor*			mButtonYSensor;

		// left & right fire buttons
		Sensor*			mButtonL1Sensor;
		Sensor*			mButtonL2Sensor;
		Sensor*			mButtonL3Sensor;
		Sensor*			mButtonR1Sensor;
		Sensor*			mButtonR2Sensor;
		Sensor*			mButtonR3Sensor;

		// misc buttons
		Sensor*			mButtonSelectSensor;
		Sensor*			mButtonStartSensor;
		Sensor*			mButtonGuideSensor;

		// joystick buttons
		Sensor*			mButtonUpSensor;
		Sensor*			mButtonDownSensor;
		Sensor*			mButtonLeftSensor;
		Sensor*			mButtonRightSensor;
		Sensor*			mButtonCenterSensor;
};


#endif

#endif
