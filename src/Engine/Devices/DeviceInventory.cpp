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
#include "DeviceInventory.h"
#include "../Config.h"
#include "../DeviceManager.h"


#include "Test/TestDevice.h"
#include "Test/TestDeviceNode.h"

#include "Muse/MuseDevice.h"
#include "Muse/MuseNode.h"

#include "Versus/VersusDevice.h"
#include "Versus/VersusNode.h"

#ifdef INCLUDE_DEVICE_NEUROSKY_MINDWAVE
  #include "NeuroSky/NeuroSkyDevice.h"
  #include "NeuroSky/NeuroSkyNode.h"
#endif

#ifdef INCLUDE_DEVICE_EMOTIV
	#include "Emotiv/EmotivEPOCDevice.h"
	#include "Emotiv/EmotivInsightDevice.h"
	#include "Emotiv/EmotivEPOCNode.h"
	#include "Emotiv/EmotivInsightNode.h"
#endif

#ifdef INCLUDE_DEVICE_ADVANCEDBRAINMONITORING
	#include "ABM/AbmDevices.h"
	#include "ABM/AbmNodes.h"
#endif

#include "OpenBCI/OpenBCIDevices.h"
#include "OpenBCI/OpenBCINodes.h"

#if defined(NEUROMORE_PLATFORM_WINDOWS)
	#include "Mitsar/MitsarDevices.h"
	#include "Mitsar/MitsarNodes.h"
#endif

#if !defined(NEUROMORE_PLATFORM_OSX)
	#include "eemagine/eemagineDevices.h"
	#include "eemagine/eemagineNodes.h"
#endif

#ifdef INCLUDE_DEVICE_BRAINQUIRY
	#include "Brainquiry/BrainquiryDevice.h"
	#include "Brainquiry/BrainquiryNode.h"
#endif

#include "BrainMaster/BrainMasterDevices.h"
#include "BrainMaster/BrainMasterNodes.h"

#include "eSense/eSenseSkinResponseDevice.h"
#include "eSense/eSenseSkinResponseNode.h"

#ifdef INCLUDE_DEVICE_ACTICHAMP
	#include "BrainProducts/ActiChampDevice.h"
	#include "BrainProducts/ActiChampNode.h"
#endif

#include "BrainFlow/BrainFlowDevices.h"
#include "BrainFlow/BrainFlowNodes.h"

#include "Generic/GenericDevices.h"
#include "Generic/GenericDeviceNodes.h"

#ifdef INCLUDE_DEVICE_GAMEPAD
	#include "Gamepad/GamepadDevice.h"
	#include "Gamepad/GamepadNode.h"
#endif

#ifdef INCLUDE_DEVICE_GENERIC_AUDIO
	#include "Audio/AudioDevices.h"
	#include "Audio/AudioNodes.h"
#endif

#ifdef INCLUDE_DEVICE_TOBIIEYEX
	#include "EyeX/TobiiEyeXDevice.h"
	#include "EyeX/TobiiEyeXNode.h"
#endif

#include "Neurosity/NotionDevices.h"
#include "Neurosity/NotionNode.h"
#include "Neurosity/CrownDevice.h"
#include "Neurosity/CrownNode.h"



#include "../EngineManager.h"
#include "../Core/LogManager.h"


using namespace Core;

//--------------------------------------------------

// add prototypes for all available devices
void DeviceInventory::RegisterDevices(bool disablePermissionCheck)
{
	LogDetailedInfo("Device Inventory: registering alle devices and device nodes...");

	// backend plugin security check
	User* user = GetEngine()->GetUser();

	if (includeDeviceTest) {
		if (disablePermissionCheck || user->ReadAllowed(TestDevice::GetRuleName()))
		{
			GetDeviceManager()->RegisterDeviceType(new TestDevice());
			GetGraphObjectFactory()->RegisterObjectType(new TestDeviceNode(NULL));
		}
	}

	if (includeDeviceInteraxonMuse) {
		if (disablePermissionCheck || user->ReadAllowed(MuseDevice::GetRuleName()))
		{
			GetDeviceManager()->RegisterDeviceType(new MuseDevice());
			GetGraphObjectFactory()->RegisterObjectType(new MuseNode(NULL));
		}
	}


	if (includeDeviceSenselabsVersus) {
		if (disablePermissionCheck || user->ReadAllowed(VersusDevice::GetRuleName()))
		{
			GetDeviceManager()->RegisterDeviceType(new VersusDevice());
			GetGraphObjectFactory()->RegisterObjectType(new VersusNode(NULL));
		}
	}

#ifdef INCLUDE_DEVICE_ADVANCEDBRAINMONITORING
	// X10
	if (disablePermissionCheck || user->ReadAllowed(AbmX10Device::GetRuleName()))
	{
		GetDeviceManager()->RegisterDeviceType(new AbmX10Device());
		GetGraphObjectFactory()->RegisterObjectType(new AbmX10Node(NULL));
	}
	// X24
	if (disablePermissionCheck || user->ReadAllowed(AbmX24Device::GetRuleName()))
	{
		GetDeviceManager()->RegisterDeviceType(new AbmX24Device());
		GetGraphObjectFactory()->RegisterObjectType(new AbmX24Node(NULL));
	}
#endif

#ifdef INCLUDE_DEVICE_NEUROSKY_MINDWAVE
	if (disablePermissionCheck || user->ReadAllowed(NeuroSkyDevice::GetRuleName()))
	{
		GetDeviceManager()->RegisterDeviceType(new NeuroSkyDevice());
		GetGraphObjectFactory()->RegisterObjectType(new NeuroSkyNode(NULL));
	}
#endif

#ifdef INCLUDE_DEVICE_ZEPHYR_BIOHARNESS
	if (disablePermissionCheck || user->ReadAllowed(BioHarnessDevice::GetRuleName()))
	{
		GetDeviceManager()->RegisterDeviceType(new BioHarnessDevice());
		GetGraphObjectFactory()->RegisterObjectType(new BioHarnessNode(NULL));
	}
#endif

#ifdef INCLUDE_DEVICE_EMOTIV
	if (disablePermissionCheck || user->ReadAllowed(EmotivEPOCDevice::GetRuleName()))
	{
		GetDeviceManager()->RegisterDeviceType(new EmotivEPOCDevice());
		GetGraphObjectFactory()->RegisterObjectType(new EmotivEPOCNode(NULL));
	}

	if (disablePermissionCheck || user->ReadAllowed(EmotivInsightDevice::GetRuleName()))
	{
		GetDeviceManager()->RegisterDeviceType(new EmotivInsightDevice());
		GetGraphObjectFactory()->RegisterObjectType(new EmotivInsightNode(NULL));
	}
#endif

#if !defined(NEUROMORE_PLATFORM_OSX)
	if (includeDeviceEEMagine) {
		if (disablePermissionCheck || user->ReadAllowed(eemagineDevice::GetRuleName()))
		{
			GetDeviceManager()->RegisterDeviceType(new eemagine8Device());
			GetGraphObjectFactory()->RegisterObjectType(new eemagine8Node(NULL));

			GetDeviceManager()->RegisterDeviceType(new eemagine32Device());
			GetGraphObjectFactory()->RegisterObjectType(new eemagine32Node(NULL));

			GetDeviceManager()->RegisterDeviceType(new eemagine64Device());
			GetGraphObjectFactory()->RegisterObjectType(new eemagine64Node(NULL));
		}
	}
#endif

	if (includeDeviceOpenBCI) {
		if (disablePermissionCheck || user->ReadAllowed(OpenBCIDevice::GetRuleName()))
		{
			GetDeviceManager()->RegisterDeviceType(new OpenBCIDevice());
			GetGraphObjectFactory()->RegisterObjectType(new OpenBCINode(NULL));
		}

		if (disablePermissionCheck || user->ReadAllowed(OpenBCIDaisyDevice::GetRuleName()))
		{
			GetDeviceManager()->RegisterDeviceType(new OpenBCIDaisyDevice());
			GetGraphObjectFactory()->RegisterObjectType(new OpenBCIDaisyNode(NULL));
		}
	}

#if defined(NEUROMORE_PLATFORM_WINDOWS)
	if (includeDeviceMitsar) {
		if (disablePermissionCheck || user->ReadAllowed(MitsarDevice::GetRuleName()))
		{
			GetDeviceManager()->RegisterDeviceType(new Mitsar201Device());
			GetGraphObjectFactory()->RegisterObjectType(new Mitsar201Node(NULL));
		}
	}
#endif

#ifdef INCLUDE_DEVICE_BRAINQUIRY
	if (disablePermissionCheck || user->ReadAllowed(BrainquiryDevice::GetRuleName()))
	{
		GetDeviceManager()->RegisterDeviceType(new BrainquiryDevice());
		GetGraphObjectFactory()->RegisterObjectType(new BrainquiryNode(NULL));
	}
#endif

	if (includeDeviceBrainmaster) {
		if (disablePermissionCheck || user->ReadAllowed(DiscoveryDevice::GetRuleName()))
		{
			GetDeviceManager()->RegisterDeviceType(new Discovery20Device());
			GetGraphObjectFactory()->RegisterObjectType(new Discovery20Node(NULL));
		}
	}

	if (includeDeviceEsenseskinResponse) {
		if (disablePermissionCheck || user->ReadAllowed(eSenseSkinResponseDevice::GetRuleName()))
		{
			GetDeviceManager()->RegisterDeviceType(new eSenseSkinResponseDevice());
			GetGraphObjectFactory()->RegisterObjectType(new eSenseSkinResponseNode(NULL));
		}
	}


#ifdef INCLUDE_DEVICE_ACTICHAMP
	if (disablePermissionCheck || user->ReadAllowed(ActiChampDevice::GetRuleName()))
	{
		GetDeviceManager()->RegisterDeviceType(new ActiChampDevice());
		GetGraphObjectFactory()->RegisterObjectType(new ActiChampNode(NULL));
	}
#endif


	if (includeDeviceNeurosityNotion) {
		if (disablePermissionCheck || user->ReadAllowed(NotionDevice::GetRuleName()))
		{
			GetDeviceManager()->RegisterDeviceType(new NotionDevice());
			GetDeviceManager()->RegisterDeviceType(new CrownDevice());
			GetGraphObjectFactory()->RegisterObjectType(new NotionNode(NULL));
			GetGraphObjectFactory()->RegisterObjectType(new CrownNode(NULL));
		}
	}

	if (includeDeviceBrainflow) {
		//if (disablePermissionCheck || user->ReadAllowed(BrainFlowDeviceCyton::GetRuleName()))
		{
			GetDeviceManager()->RegisterDeviceType(new BrainFlowDevice());
			GetGraphObjectFactory()->RegisterObjectType(new BrainFlowNode(NULL));
			GetGraphObjectFactory()->RegisterObjectType(new BrainFlowCytonNode(NULL));
		}
	}

	/////////////////////////
	// Generic Devices
	
#ifdef INCLUDE_DEVICE_GENERIC_HEARTRATE
	// HeartRate
	if (disablePermissionCheck || user->ReadAllowed(HeartRateDevice::GetRuleName()))
	{
		GetDeviceManager()->RegisterDeviceType(new HeartRateDevice());
		GetGraphObjectFactory()->RegisterObjectType(new HeartRateNode(NULL));
	}
#endif

#ifdef INCLUDE_DEVICE_GENERIC_ACCELEROMETER
	// Accelerometer
	if (disablePermissionCheck || user->ReadAllowed(AccelerometerDevice::GetRuleName()))
	{
		GetDeviceManager()->RegisterDeviceType(new AccelerometerDevice());
		GetGraphObjectFactory()->RegisterObjectType(new AccelerometerNode(NULL));
	}
#endif

#ifdef INCLUDE_DEVICE_GENERIC_GYROSCOPE
	// Gyroscope
	if (disablePermissionCheck || user->ReadAllowed(GyroscopeDevice::GetRuleName()))
	{
		GetDeviceManager()->RegisterDeviceType(new GyroscopeDevice());
		GetGraphObjectFactory()->RegisterObjectType(new GyroscopeNode(NULL));
	}
#endif

#ifdef INCLUDE_DEVICE_GAMEPAD
	if (disablePermissionCheck || user->ReadAllowed(GamepadDevice::GetRuleName()))
	{
		GetDeviceManager()->RegisterDeviceType(new GamepadDevice());
		GetGraphObjectFactory()->RegisterObjectType(new GamepadNode(NULL));
	}
#endif

#ifdef INCLUDE_DEVICE_TOBIIEYEX
	// TobiiEyeX eyetracker
	GetDeviceManager()->RegisterDeviceType(new TobiiEyeXDevice());
	GetGraphObjectFactory()->RegisterObjectType(new TobiiEyeXNode(NULL));
#endif


#ifdef INCLUDE_DEVICE_GENERIC_AUDIO
	// Audio
	GetDeviceManager()->RegisterDeviceType(new AudioInputDevice());
	GetGraphObjectFactory()->RegisterObjectType(new AudioInputNode(NULL));

	GetDeviceManager()->RegisterDeviceType(new MicrophoneDevice());
	GetGraphObjectFactory()->RegisterObjectType(new MicrophoneNode(NULL));

	GetDeviceManager()->RegisterDeviceType(new AudioOutputDevice());
	GetGraphObjectFactory()->RegisterObjectType(new AudioOutputNode(NULL));

#endif

	LogDetailedInfo("Device Inventory finished registering devices.");
}
