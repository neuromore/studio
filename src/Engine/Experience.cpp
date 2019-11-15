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
#include "Experience.h"
#include "EngineManager.h"
#include "Core/LogManager.h"
#include "Core/AttributeString.h"
#include "Core/AttributeBool.h"
#include "Graph/GraphImporter.h"
#include "Graph/GraphExporter.h"


using namespace Core;

// constructor
Experience::Experience()
{
	mName = "Experience";
	mRevision = 0;
	mAttachedClassifier = NULL;
	mAttachedStateMachine = NULL;
	mIsDirty = false;

	// register classifier UUID attribute
	AttributeSettings* classifierUuidAttribute = RegisterAttribute("Classifier UUID", "classifierUuid", "The UUID of the classifier used in the design.", ATTRIBUTE_INTERFACETYPE_STRING);
	classifierUuidAttribute->SetDefaultValue( AttributeString::Create("") );
	classifierUuidAttribute->SetVisible( false );

	// register state machine UUID attribute
	AttributeSettings* stateMachineUuidAttribute = RegisterAttribute("State Machine UUID", "stateMachineUuid", "The UUID of the state machine used in the design.", ATTRIBUTE_INTERFACETYPE_STRING);
	stateMachineUuidAttribute->SetDefaultValue( AttributeString::Create("") );
	stateMachineUuidAttribute->SetVisible( false );

	// register layout UUID attribute
	AttributeSettings* layoutUuidAttribute = RegisterAttribute("Layout UUID", "layoutUuid", "The UUID of the layout definition used in the design.", ATTRIBUTE_INTERFACETYPE_STRING);
	layoutUuidAttribute->SetDefaultValue( AttributeString::Create("") );
	layoutUuidAttribute->SetVisible( false );

	CreateDefaultAttributeValues();
}


// destructor
Experience::~Experience()
{
}

// set classifier uuid
void Experience::SetClassifierUuid(const char* uuid)
{
	if (String(GetStringAttribute(ATTRIB_CLASSIFIERUUID)).IsEqual(uuid))
		return;

	mIsDirty = true;
	SetStringAttributeByIndex(ATTRIB_CLASSIFIERUUID, uuid);
}


// set classifier (done during loading)
void Experience::AttachClassifier(Classifier* classifier)
{ 
	// set classifier
	mAttachedClassifier = classifier; 
	SetClassifierUuid( classifier != NULL ? classifier->GetUuid() : "" ); 
}


// state machine uuid
void Experience::SetStateMachineUuid(const char* uuid)
{ 
	if (String(GetStringAttribute(ATTRIB_STATEMACHINEUUID)).IsEqual(uuid))
		return;

	mIsDirty = true;
	SetStringAttributeByIndex(ATTRIB_STATEMACHINEUUID, uuid);
}


// set state machine (done during loading)
void Experience::AttachStateMachine(StateMachine* stateMachine)
{ 
	mAttachedStateMachine = stateMachine;
	SetStateMachineUuid( stateMachine != NULL ? stateMachine->GetUuid() : "" );
}


// remove all attached settings
void Experience::RemoveSettings()
{
	mClassifierSettings.Clear();
	mStateMachineSettings.Clear();
	mDeviceSettings.Clear();
}


// if there are any settings attached
bool Experience::HasSettings()
{
	if (mClassifierSettings.Size() > 0)
		return true;

	if (mStateMachineSettings.Size() > 0)
		return true;

	return false;
}


void Experience::Log()
{
	LogInfo( " - Experience '%s'%s", GetName(), (mIsDirty == true ? "(modified)" : "") );
	LogInfo( "    + UUID: %s", GetUuid() );
	LogInfo( "    + Revision: %i", mRevision );
	LogInfo( "    + Classifier UUID: %s", GetClassifierUuid() );
	LogInfo( "    + Classifier attached: %s", mAttachedClassifier != NULL ? "yes" : "no" );
	LogInfo( "    + State Machine UUID: %s", GetStateMachineUuid() );
	LogInfo( "    + State Machine attached: %s", mAttachedStateMachine != NULL ? "yes" : "no" );
	//LogInfo( "    + Layout UUID: %s", GetLayoutUuid() );
	//LogInfo( "    + Layout attached: %s", GetLayout() != NULL ? "yes" : "no" );
}


void Experience::Save(Json& json, Json::Item& item, bool attachClassifier, bool attachStateMachine, bool attachLayout)
{
	// write attributes
	Write(json, item, true);

	// add all settings (devices/classifier/statemachine)
	const bool hasClassifierSettings = (mClassifierSettings.Size() > 0);
	const bool hasStateMachineSettings = (mStateMachineSettings.Size() > 0);

	// TODO get only the device settings that are actually used. For simplicity we just add all configs of devices that are currently active, even if they are not used in the classifier
	bool hasDeviceSettings = false;
	const uint32 numDevices = GetDeviceManager()->GetNumDevices();
	for (uint32 i=0; i<numDevices; ++i)
	{
		Device* device = GetDeviceManager()->GetDevice(i);
		const Device::DeviceConfig& config = device->GetConfig();
		if (config.mIsValid == true)
		{
			hasDeviceSettings = true;
			break;
		}
	}
		

	// add experience settings item (if there are settings)
	if (hasClassifierSettings || hasStateMachineSettings || hasDeviceSettings)
	{
		Json::Item settingsItem = item.AddObject("settings");

		// classifier settings
		if (hasClassifierSettings == true)
		{
			Json::Item classifierSettingsItem = settingsItem.AddArray("classifierSettings");
			mClassifierSettings.Save( classifierSettingsItem );
		}

		// state machine settings
		if (hasStateMachineSettings == true)
		{
			Json::Item stateMachineSettingsItem = settingsItem.AddArray("statemachineSettings");
			mStateMachineSettings.Save( stateMachineSettingsItem );
		}
#ifndef PRODUCTION_BUILD
		// device settings
		if (hasDeviceSettings == true)
		{
			for (uint32 i=0; i<numDevices; ++i)
			{
				/*Device* device = GetDeviceManager()->GetDevice(i);
				const Device::DeviceConfig& config = device->GetConfig();
				if (config.mIsValid == true)
				{
					// one item per device, containing the device config json
					jsonParser->AddJSON("deviceSettings", settingsItem, &config.mJSON);
				}*/
			}
		}
#endif
	}

	// attach classifier
	if (attachClassifier && mAttachedClassifier != NULL)
	{
		Json::Item classifierItem = item.AddObject("classifier");
		mAttachedClassifier->Save( json, classifierItem );
	}

	// attach state machine
	if (attachStateMachine && mAttachedStateMachine != NULL)
	{
		Json::Item stateMachineItem = item.AddObject("statemachine");
		mAttachedStateMachine->Save( json, stateMachineItem );
	}

	// TODO save layouts

	// reset dirty flag
	mIsDirty = false;
}


bool Experience::Load(const Json& json, const Json::Item& item)
{
	// clearn reinit
	mRevision = 0;
	mAttachedClassifier = NULL;
	mAttachedStateMachine = NULL;
	//mLayout = NULL;
	SetClassifierUuid("");
	SetStateMachineUuid("");
	//SetLayoutUuid("");

	// load attributes
	Read(json, item, true);

	// load all other items
	Json::Item nameItem				= item.Find("name");
	Json::Item descriptionItem		= item.Find("description");
	Json::Item classifierItem		= item.Find("classifier");
	Json::Item stateMachineItem		= item.Find("statemachine");
	Json::Item layoutItem			= item.Find("layout");
	Json::Item settingsItem			= item.Find("settings");

	// load attributes
	if (nameItem.IsString())		mName			= nameItem.GetString();
	if (descriptionItem.IsString())	mDescription	= descriptionItem.GetString();

	// attached classifier
	if (classifierItem.IsNull() == false)
	{
		mAttachedClassifier = new Classifier();
		if (GraphImporter::LoadFromJSON(json, classifierItem, mAttachedClassifier) == false)
		{
			delete mAttachedClassifier;
			mAttachedClassifier = NULL;
		}
	}

	// attached state machine
	if (stateMachineItem.IsNull() == false)
	{
		mAttachedStateMachine = new StateMachine();
		if (GraphImporter::LoadFromJSON(json, stateMachineItem, mAttachedStateMachine) == false)
		{
			delete mAttachedStateMachine;
			mAttachedStateMachine = NULL;
		}
	}
	
	// load attached layout file (if any)
	if (layoutItem.IsNull() == false)
	{
		// TODO load attached layout file (if any)
	}

	// load designsettings
	if (settingsItem.IsNull() == false)
	{
		// classifier settings
		Json::Item classifierSettingsItem = settingsItem.Find("classifierSettings");
		if (classifierSettingsItem.IsNull() == false)
			mClassifierSettings.Load(classifierSettingsItem);

		// state machine settings
		Json::Item stateMachineSettingsItem = settingsItem.Find("statemachineSettings");
		if (stateMachineSettingsItem.IsNull() == false)
			mStateMachineSettings.Load(stateMachineSettingsItem);

#ifndef PRODUCTION_BUILD
		// load device settings
		Json::Item deviceSettingsItem = settingsItem.Find("deviceSettings");
		if (deviceSettingsItem.IsNull() == false)
		{
			const uint32 numDevices = deviceSettingsItem.Size();
			for (uint32 i=0; i<numDevices; ++i)
			{
				// extract device config json
				//Json json; 
				//json.GetRootItem().AddJson("", deviceSettingsItem[i]);
				//mDeviceSettings.Add(Device::DeviceConfig());
			}
		}
#endif
	}

	// reset dirty flag
	mIsDirty = false;

	return true;
}


// load all cloud parameter used in the experience from the parameter array
uint32 Experience::LoadCloudParameters(const CloudParameters& parameters)
{
	uint32 numLoaded = 0;

	// classifier settings
	if (mAttachedClassifier != NULL)
		if (mClassifierSettings.LoadCloudParameter(parameters, mAttachedClassifier->GetUuid(), CloudParameters::GetClassifierSettingsParameterName()))
			numLoaded++;

	// state machine settings
	if (mAttachedStateMachine != NULL)
		if (mStateMachineSettings.LoadCloudParameter(parameters,  mAttachedStateMachine->GetUuid(), CloudParameters::GetStateMachineSettingsParameterName()))
			numLoaded++;
	
	return numLoaded;
}



// save all cloud parameter used in the experience into the parameter array
uint32 Experience::SaveCloudParameters(CloudParameters& parameter) const
{

	//
	// TODO we can save the graph settings here if we want to - implement this later, as it requires some more thoughts about how to handle this in the studio UI
	//

	return false;
}
