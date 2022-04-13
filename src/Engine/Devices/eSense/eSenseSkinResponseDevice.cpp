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
#include "eSenseSkinResponseDevice.h"
#include "../../EngineManager.h"
#include "../../Core/LogManager.h"

#ifdef INCLUDE_DEVICE_ESENSESKINRESPONSE

using namespace Core;


// constructor
eSenseSkinResponseDevice::eSenseSkinResponseDevice(DeviceDriver* driver) : Device(driver)
{
	// create all sensors
	CreateSensors();

	// start in connected state
	mState = STATE_IDLE;
}


// destructor
eSenseSkinResponseDevice::~eSenseSkinResponseDevice()
{
}


void eSenseSkinResponseDevice::CreateSensors()
{
	// for inputs with variable sample rate
	const double sampleRate = 10;

	mGsrSensor = AddSensor(SENSOR_INPUT, "GSR", sampleRate, true, 0.0, CORE_FLOAT_MAX, "uSiemens" );
}

#endif
