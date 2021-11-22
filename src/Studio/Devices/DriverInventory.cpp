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
#include "DriverInventory.h"
#include <Config.h>


#ifdef INCLUDE_DEVICE_NEUROSKY_MINDWAVE
  #include "NeuroSky/NeuroSkyDriver.h"
#endif
  
#ifdef INCLUDE_DEVICE_EMOTIV
	#include "Emotiv/EmotivDriver.h"
#endif

#ifdef INCLUDE_DEVICE_EEMAGINE
	#include "eemagine/eemagineDriver.h"
#endif

#ifdef INCLUDE_DEVICE_ADVANCEDBRAINMONITORING
	#include "ABM/ABMDriver.h"
#endif

#ifdef INCLUDE_DEVICE_OPENBCI
    #include "OpenBCI/OpenBCIDriver.h"
#endif

#ifdef INCLUDE_DEVICE_SENSELABS_VERSUS
	#include "Versus/VersusDriver.h"
#endif

#ifdef INCLUDE_DEVICE_MITSAR
  #include "Mitsar/MitsarDriver.h"
#endif

#ifdef INCLUDE_DEVICE_BRAINQUIRY
  #include "Brainquiry/BrainquiryDriver.h"
#endif

#ifdef INCLUDE_DEVICE_TOBIIEYEX
	#include "EyeX\TobiiEyeXDriver.h"
#endif

#ifdef INCLUDE_DEVICE_BRAINFLOW
#include "BrainFlow/BrainFlowDriver.h"
#endif

#ifdef INCLUDE_DEVICE_BRAINALIVE
#include "./BrainAlive/BrainAliveDriver.h"
#endif

#include "Bluetooth/BluetoothDriver.h"
#include "Audio/AudioDriver.h"


#include <EngineManager.h>
#include <Core/LogManager.h>

using namespace Core;

//--------------------------------------------------

// initialize the supported systems
void DriverInventory::RegisterDrivers()
{
	LogDetailedInfo("Registering drivers ...");

	// backend plugin security check
	User* user = GetEngine()->GetUser();
	DeviceManager* deviceManager = GetDeviceManager();

#ifdef INCLUDE_DEVICE_SENSELABS_VERSUS
    // NOTE: somehow destroys the Bluetooth LE driver when activated ... why???
	if (user->ReadAllowed(VersusDevice::GetRuleName()) == true)
	{
		// create and add the Versus system
        VersusDriver* versusDriver = new VersusDriver();
		versusDriver->Init();
		deviceManager->AddDeviceDriver(versusDriver);
	}
#endif

#ifdef INCLUDE_DEVICE_ADVANCEDBRAINMONITORING
	if (user->ReadAllowed(AbmX24Device::GetRuleName()) == true || user->ReadAllowed(AbmX10Device::GetRuleName()) == true)
	{
		AbmDriver* abmDriver = new AbmDriver();
		abmDriver->Init();
		deviceManager->AddDeviceDriver(abmDriver);
	}
#endif

#ifdef INCLUDE_DEVICE_NEUROSKY_MINDWAVE
	if (user->ReadAllowed(NeuroSkyDevice::GetRuleName()) == true)
	{
		// create and add the neurosky system
		NeuroSkyDriver* neuroSkyDriver = new NeuroSkyDriver();
		neuroSkyDriver->Init();
		deviceManager->AddDeviceDriver(neuroSkyDriver);
	}
#endif
//
//#ifdef INCLUDE_DEVICE_ZEPHYR_BIOHARNESS
//	if (user->ReadAllowed(BioHarnessDriver::GetRuleName()) == true)
//	{
//		// create and add the Zephyr BioHarness system
//		BioHarnessDriver* bioHarnessDriver = new BioHarnessDriver();
//		bioHarnessDriver->Init();
//		deviceManager->AddDeviceDriver(bioHarnessDriver);
//	}
//#endif
//
#ifdef INCLUDE_DEVICE_EMOTIV
	if (user->ReadAllowed(EmotivEPOCDevice::GetRuleName()) == true || user->ReadAllowed(EmotivInsightDevice::GetRuleName()) == true)
	{
		// create and add the emotiv driver
		EmotivDriver* emotivDriver = new EmotivDriver();
		emotivDriver->Init();
		deviceManager->AddDeviceDriver(emotivDriver);
	}
#endif

#ifdef INCLUDE_DEVICE_EEMAGINE
	if (user->ReadAllowed(eemagineDevice::GetRuleName()) == true)
	{
		// create and add the eemagine system
		eemagineDriver* _eemagineDriver = new eemagineDriver();
		_eemagineDriver->Init();
		deviceManager->AddDeviceDriver(_eemagineDriver);
	}
#endif

#ifdef INCLUDE_DEVICE_OPENBCI
	if (user->ReadAllowed(OpenBCIDevice::GetRuleName()) == true || user->ReadAllowed(OpenBCIDaisyDevice::GetRuleName()) == true)
	{
		// create and add the openbci system
		OpenBCIDriver* openBCIDriver = new OpenBCIDriver();
		openBCIDriver->Init();
		deviceManager->AddDeviceDriver(openBCIDriver);
	}
#endif


#ifdef INCLUDE_DEVICE_MITSAR
	if (user->ReadAllowed(MitsarDevice::GetRuleName()) == true)
	{
		// create and add the mitsar system
		MitsarDriver* mitsarDriver = new MitsarDriver();
		mitsarDriver->Init();
		deviceManager->AddDeviceDriver(mitsarDriver);
	}
#endif

#ifdef INCLUDE_DEVICE_BRAINQUIRY
	if (user->ReadAllowed(BrainquiryDevice::GetRuleName()) == true)
	{
		BrainquiryDriver* brainquiryDriver = new BrainquiryDriver();
		brainquiryDriver->Init();
		deviceManager->AddDeviceDriver(brainquiryDriver);
	}
#endif

#ifdef INCLUDE_DEVICE_TOBIIEYEX
		TobiiEyeXDriver* tobiiEyeXDriver = new TobiiEyeXDriver();
		tobiiEyeXDriver->Init();
		deviceManager->AddDeviceDriver(tobiiEyeXDriver);
#endif

#ifdef INCLUDE_DEVICE_GENERIC_HEARTRATE
    // NOTE: DOES NOT WORK ANYMORE AS SOON AS THE VERSUS DRIVER GETS ACTIVATED
	if (user->ReadAllowed(HeartRateDevice::GetRuleName()) == true)
	{
		// create and add the Bluetooth system
		BluetoothDriver* bluetoothDriver = new BluetoothDriver();
		bluetoothDriver->Init();
		deviceManager->AddDeviceDriver(bluetoothDriver);
	}
#endif


#ifdef INCLUDE_DEVICE_GENERIC_AUDIO
	//if (user->ReadAllowed(HeartRateDevice::GetRuleName()) == true)
	{
		// create and add the Bluetooth system
		AudioDriver* audioDriver = new AudioDriver();
		audioDriver->Init();
		deviceManager->AddDeviceDriver(audioDriver);
	}
#endif

#ifdef INCLUDE_DEVICE_BRAINFLOW
	//if (user->ReadAllowed(BrainFlowDevice::GetRuleName()) == true)
	{
		auto* driver = new BrainFlowDriver();
		driver->Init();
		deviceManager->AddDeviceDriver(driver);
	}
#endif

#ifdef INCLUDE_DEVICE_BRAINALIVE
	if (user->ReadAllowed(BrainAliveDevice::GetRuleName()) == true)
	{
		// create and add the BrainAlive system
		BrainAliveDriver* brainAliveDriver = new BrainAliveDriver();
		brainAliveDriver->Init();
		deviceManager->AddDeviceDriver(brainAliveDriver);
	}
#endif
}
