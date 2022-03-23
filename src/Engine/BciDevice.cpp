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
#include "BciDevice.h"
#include "EngineManager.h"
#include "Core/LogManager.h"
#include "Core/Timer.h"

using namespace Core;

// constructor
BciDevice::BciDevice(DeviceDriver* deviceDriver) : Device(deviceDriver)
{
}


// destructor
BciDevice::~BciDevice()
{
	// note: Sensors in mNeuroSensors are deallocated by base class
}


void BciDevice::Update(const Time& elapsed, const Time& delta)
{
	Device::Update(elapsed, delta);
}


void BciDevice::Configure(const DeviceConfig& config) 
{
	Device::Configure(config);

	ConfigureElectrodes(config.mJson);
}


void BciDevice::CreateSensors()
{
	// create electrodes with default positions
	CreateElectrodes();

	// copy original electrode configuration
	mDefaultElectrodes = mElectrodes;

	// create a sensor for each electrode
	const uint32 numSensors = mElectrodes.Size();
	for (uint32 i = 0; i < numSensors; ++i)
	{
		// create sensor with the neuro headset sample rate
		Sensor* sensor = new Sensor(mElectrodes[i].GetName(), GetSampleRate());

		// set unique color for each sensor
		sensor->GetChannel()->GetColor().SetUniqueColor(i);
		
		// add sensors to our lists
		mNeuroSensors.Add(sensor);
		AddSensor(sensor);
	}
}


// UNTESTED, unused
Sensor* BciDevice::FindNeuroSensorByName(const char* electrodeName) const
{
	const uint32 numSensors = mNeuroSensors.Size();
	for (uint32 i = 0; i < numSensors; ++i)
	{
		if (Core::String::SafeCompare( mNeuroSensors[i]->GetName(), electrodeName) == 0)
			return mNeuroSensors[i];
	}

	return NULL;
}


// return the electrode position of a neuro sensor
EEGElectrodes::Electrode BciDevice::GetElectrodePosition(uint32 neuroSensorIndex) const
{
	CORE_ASSERT(neuroSensorIndex < mElectrodes.Size());

	// index is out of range
	if (neuroSensorIndex >= mElectrodes.Size())
		return EEGElectrodes::Electrode(); // we have to return something

	return mElectrodes[neuroSensorIndex];
}


// update the electrode position of a neuro sensor
void BciDevice::SetElectrodePosition(uint32 neuroSensorIndex, EEGElectrodes::Electrode electrode)
{
	// index is out of range
	CORE_ASSERT(neuroSensorIndex < mElectrodes.Size());
	if (neuroSensorIndex >= mElectrodes.Size())
		return;

	// update electrode
	mElectrodes[neuroSensorIndex] = electrode;

	// index is out of range
	CORE_ASSERT(neuroSensorIndex < GetNumNeuroSensors());
	if (neuroSensorIndex >= GetNumNeuroSensors())
		return;

	// update sensor / channel name
	GetNeuroSensor(neuroSensorIndex)->SetName(electrode.GetName());
}


// electrode configuration (subset of device definitions)
void BciDevice::ConfigureElectrodes(const Core::Json& jsonParser)
{
	Json::Item rootItem = jsonParser.GetRootItem();

	String itemName;

	// iterate over electrodes, try to find them in the config and configure them
	const uint32 numElectrodes = GetNumNeuroSensors();
	for (uint32 i = 0; i < numElectrodes; ++i)
	{
		Sensor* sensor = GetNeuroSensor(i);

		// electrode object name is like "electrode1" (its 1-indexed)
		itemName.Format("eeg%i", i+1);
		Json::Item electrodeItem = rootItem.Find(itemName.AsChar());
		if (electrodeItem.IsNull() == false)
		{
			// electrode enabled?
			Json::Item enableItem = electrodeItem.Find("enable");
			if (enableItem.IsBool() && enableItem.GetBool() == false)
				sensor->SetEnabled(false);
			else
				sensor->SetEnabled(true);

			// electrode name (is also position)
			Json::Item nameItem = electrodeItem.Find("name");
			EEGElectrodes::Electrode electrode = EEGElectrodes::Electrode();
			if (nameItem.IsString() == true)
			{
				// get electrode by name
				const String& name = nameItem.GetString();
				electrode = GetEEGElectrodes()->GetElectrodeByID(name);
				
				// TODO also allow some type of custom electrode position // TODO for this, we need to rotate (or map) the spherical coordinates in Electrode class, they are un-intuitive (rotated, Cz should be 0,0 or something)

				sensor->SetName(name);
			}
			else
			{
				sensor->SetName(mDefaultElectrodes[i].GetName());
			}
			
		}
		else // default config
		{
			sensor->SetEnabled(true);
			sensor->SetName(mDefaultElectrodes[i].GetName());
		}
	}
}


void BciDevice::WriteElectrodeConfig(Core::Json& jsonParser) const
{
	Json::Item rootItem = jsonParser.GetRootItem();

	String itemName;

	// iterate over electrodes, try to find them in the config and configure them
	const uint32 numElectrodes = GetNumNeuroSensors();
	for (uint32 i = 0; i < numElectrodes; ++i)
	{
		Sensor* sensor = GetNeuroSensor(i);

		// electrode object name is like "electrode1" (its 1-indexed)
		itemName.Format("eeg%i", i+1);

		// don't add a duplicate item
		if (rootItem.Find(itemName.AsChar()).IsNull() == false)
		{
			LogWarning("Error writing electrode configuration: electrode '%s' already exists.", itemName.AsChar());
			continue;
		}
		
		Json::Item electrodeItem = rootItem.AddObject(itemName.AsChar());
		
		// add electrode settings
		electrodeItem.AddBool("enable", sensor->IsEnabled());
		electrodeItem.AddString("name", mElectrodes[i].GetName());
	} 
}
