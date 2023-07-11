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

// include required headers
#include "GraphSettings.h"
#include "../Core/LogManager.h"

using namespace Core;

void GraphSettings::Add(GraphObject* object, const char* attributeName, const char* attributeValue)
{
	mSettings.AddEmpty();
	GraphObjectSettings& settings = mSettings.GetLast();
	settings.mObjectUuid = object->GetUuid();
	settings.mObjectType = object->GetType();
	settings.mObjectName = object->GetName();
	settings.mName = attributeName;
	settings.mValue = attributeValue;
}


void GraphSettings::Save(Json::Item& item)
{
	CORE_ASSERT( item.IsArray() == true );

	const uint32 numSettings = mSettings.Size();
	for (uint32 i=0; i<numSettings; ++i)
	{
		// create item in array
		Json::Item settingsItem = item.AddObject();
	
		// object identifiers
		settingsItem.AddString( "objectName", mSettings[i].mObjectName );
		settingsItem.AddString( "objectUuid", mSettings[i].mObjectUuid );
		settingsItem.AddInt( "objectType", mSettings[i].mObjectType);
	
		// attribute name
		settingsItem.AddString( "attribute", mSettings[i].mName );

		// attribute value
		settingsItem.AddString( "value", mSettings[i].mValue);
	}
}


bool GraphSettings::Load(const Json::Item& item)
{
	mSettings.Clear();
	bool hasError = false;
	if (item.IsArray() == false)
		return false;

	// get the number of elements in the array and iterate through them
	const uint32 numSettings = item.Size();
	for (uint32 i=0; i<numSettings; ++i)
	{
		Json::Item child = item[i];
	
		Json::Item objectName	= child.Find("objectName");
		Json::Item objectUuid	= child.Find("objectUuid");
		Json::Item objectType	= child.Find("objectType");
		Json::Item nameItem		= child.Find("attribute");
		Json::Item valueItem	= child.Find("value");

		const bool hasIdentifier = objectUuid.IsString() || objectName.IsString();
		if (hasIdentifier == false  || objectType.IsInt() == false || nameItem.IsString() == false || valueItem.IsString() == false)
		{
			LogError("GraphSettings::Load(): Error reading graph object attributes.");
			hasError = true;
			continue;
		}

		// load the setting

		mSettings.AddEmpty();
		GraphObjectSettings& settings = mSettings.GetLast();
		
		if (objectUuid.IsString() == true)
			settings.mObjectUuid = objectUuid.GetString();

		if (objectName.IsString() == true)
			settings.mObjectName = objectName.GetString();

		if (objectType.IsInt() == true)
			settings.mObjectType = objectType.GetInt();

		settings.mObjectName = objectName.GetString();
		settings.mName = nameItem.GetString();
		settings.mValue = valueItem.GetString();
	}

	return hasError;
}


bool GraphSettings::LoadCloudParameter(const CloudParameters& parameters, const char* uuid, const char* parameterName)
{
	Core::Array<const CloudParameters::Parameter*> graphSettingsParameters = parameters.Find(uuid, parameterName);

	if (graphSettingsParameters.Size() == 0)
		return false;
	else if (graphSettingsParameters.Size() > 1)
		LogWarning("GraphSettings::LoadGraphSettings: found more than one graphSettings parameter for item '%s'", uuid);
	
	// Note: we load only first occurence
	const CloudParameters::Parameter* parameter = graphSettingsParameters[0];
	
	// check type
	if (parameter->mTypeId != CloudParameters::TYPE_JSON)
	{
		LogError("GraphSettings::LoadGraphSettings: parameter '%s' of item '%s' is not of type 3=JSON", parameterName, uuid);
		return false;
	}

	// get the json and try to load graph settings
	Clear();
	if (Load(parameter->GetValue<Core::Json>().GetRootItem()) == false)
		return false;

	return true;
}
