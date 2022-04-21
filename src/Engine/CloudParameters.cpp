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
#include "CloudParameters.h"

#include "Experience.h"
#include "Core/LogManager.h"
#include "Graph/Classifier.h"
#include "User.h"
#include "Core/String.h"
#include "Core/Json.h"


// set value function using specialised GetType (thus it can't be placed in the header)
template <class T> void CloudParameters::Parameter::SetValue(const T& value)
{ 
	delete mValue;
	mValue = new Value<T>(value); 
	mTypeId = GetType<T>(); 
}


Core::Array<const CloudParameters::Parameter*> CloudParameters::Find(const char* name, const char* itemId, bool requireItemId) const
{
	Core::Array<const CloudParameters::Parameter*> parameters;

	const uint32 numParams = mParameters.Size();
	for (uint32 i = 0; i < numParams; ++i)
		if (mParameters[i].mName.IsEqual(name) == true && (requireItemId == false || mParameters[i].mItemId.IsEqual(itemId) == true) )
			parameters.Add(&mParameters[i]);

	return parameters;
}


Core::Array<const CloudParameters::Parameter*> CloudParameters::Find(const char* itemId) const
{
	Core::Array<const CloudParameters::Parameter*> parameters;

	const uint32 numParams = mParameters.Size();
	for (uint32 i = 0; i < numParams; ++i)
		if (mParameters[i].mItemId.IsEqual(itemId) == true)
			parameters.Add(&mParameters[i]);

	return parameters;
}


// Input: init from json array, e.g. from find parameters reply
bool CloudParameters::Load(const Core::Json::Item& arrayItem)
{
	bool success = true;

	// iterate over child objects and forward the values to the corresponding cloud input nodes
	const uint32 numParams = arrayItem.Size();
	for (uint32 i = 0; i < numParams; ++i)
	{
		Core::Json::Item paramItem = arrayItem[i];

		Core::Json::Item userIdItem = paramItem.Find("userId");
		Core::Json::Item itemIdItem = paramItem.Find("itemId");
		Core::Json::Item timeStampItem = paramItem.Find("timestamp");
		Core::Json::Item nameItem =  paramItem.Find("name");
		Core::Json::Item typeItem =  paramItem.Find("type");
		Core::Json::Item parameterItem =  paramItem.Find("parameter");

		// filter parameters that don't belong to the user
		if (userIdItem.IsString() == true && mUser != NULL && mUser->GetIdString().IsEqual(userIdItem.GetString()) == false)
		{
			Core::LogError("CloudParameters::Load: parameter's userd id '%s' does not match the user '%s'", userIdItem.GetString(), mUser->GetId());
			CORE_ASSERT(false);
			success = false;
			continue;
		}

		// all fields have to be present
		if (typeItem.IsInt() == false || nameItem.IsString() == false || parameterItem.IsNull() == true )
		{
			Core::LogError("CloudParameters::Load: Mandatory item is missing.");
			success = false;
			continue;
		}

		// itemid and timestamp is optional
		if (itemIdItem.IsNull() == false && itemIdItem.IsString() == false)
		{
			Core::LogError("CloudParameters::Load: ItemId is not a string.");
			success = false;
			continue;
		}

		// create parameter value
		const int32 typeId = typeItem.GetInt();
		if (typeId < 0 || typeId >= NUM_TYPES)
		{
			Core::LogError("CloudParameters::Load: parameter '%s' is of wrong or unknown type", nameItem.GetString());
			success = false;
			continue;
		}

		// create parameter
		Parameter& parameter = mParameters.AddEmpty();
		
		// optional item id
		parameter.mItemId = (itemIdItem.IsString() ? itemIdItem.GetString() : "");

		// required fields
		parameter.mUserId = userIdItem.GetString();
		parameter.mName = nameItem.GetString();

		// parameter value
		switch (typeId)
		{
			case TYPE_INT:		parameter.SetValue(parameterItem.GetInt()); break;
			default:
			case TYPE_FLOAT:	parameter.SetValue(parameterItem.GetDouble()); break;
			case TYPE_STRING:	parameter.SetValue(Core::String(parameterItem.GetString())); break;
			case TYPE_JSON:		parameter.SetValue(Core::Json(parameterItem)); break;
		}

		// optional timestamp
		if (timeStampItem.IsString() == true)
			parameter.mTimeStamp = Core::Time::FromUtcString(timeStampItem.GetString());
		else
			parameter.mTimeStamp = 0;

	}

	return success;
}


// Input: helper to create json for requesting input parameters for an experience (including classifier)
uint32 CloudParameters::CreateFindParametersJson(const Experience& experience, Core::Json::Item& arrayItem)
{
	uint32 numItems = 0;

	// classifier settings
	if (Core::String(experience.GetClassifierUuid()).GetLength() > 0)
	{
		Core::Json::Item settingsItem = arrayItem.AddObject();
		settingsItem.AddString("itemId", experience.GetUuid());
		settingsItem.AddString("name", GetClassifierSettingsParameterName());
		settingsItem.AddInt("type", TYPE_JSON);
		settingsItem.AddInt("maxResults", 1);
		numItems++;
	}

	// statemachine settings
	if (Core::String(experience.GetStateMachineUuid()).GetLength() > 0)
	{
		Core::Json::Item settingsItem = arrayItem.AddObject();
		settingsItem.AddString("itemId", experience.GetUuid());
		settingsItem.AddString("name", GetStateMachineSettingsParameterName());
		settingsItem.AddInt("type", TYPE_JSON);
		settingsItem.AddInt("maxResults", 1);
		numItems++;
	}

	return numItems;
}


// Input: helper to create json for requesting input parameters for a classifier
uint32 CloudParameters::CreateFindParametersJson(const Classifier& classifier, Core::Json::Item& arrayItem)
{
	uint32 numItems = 0;

	const uint32 numParameters = classifier.GetNumCloudInputNodes();

	// iterate through parameters and create json
	for (uint32 i = 0; i < numParameters; ++i)
	{
		CloudInputNode* node = classifier.GetCloudInputNode(i);
		
		// add array item
		Core::Json::Item parameterObject = arrayItem.AddObject();

		// name/type
		parameterObject.AddString("name", node->GetName());
		parameterObject.AddInt("type", TYPE_FLOAT);

		// classifier id
		if (node->GetStorageType() == CloudInputNode::STORAGE_CLASSIFIERPARAMETER)
			parameterObject.AddString("itemId", classifier.GetUuid());

		// add timeSpan / maxResults depending on mode
		if (node->GetRequestMode() == CloudInputNode::REQUEST_TIMERANGE)
		{
			parameterObject.AddInt("maxResults", CORE_INT32_MAX);

			const uint32 seconds = node->GetTimeRange();
			const Core::Time startTime = Core::Time::Now() - Core::Time(seconds);
			//parameterObject.AddInt("timeSpan", seconds);
			parameterObject.AddString("startDateTime", startTime.AsUtcString());
		}
		else if (node->GetRequestMode() == CloudInputNode::REQUEST_ALL)
		{
			parameterObject.AddString("startDateTime", Core::Time(0).AsUtcString());	// must add a timestamp to get historic values
			parameterObject.AddInt("maxResults", CORE_INT32_MAX);
		}
		else if (node->GetRequestMode() == CloudInputNode::REQUEST_COUNT)
		{
			parameterObject.AddString("startDateTime", Core::Time(0).AsUtcString()); // must add a timestamp to get historic values
			parameterObject.AddInt("maxResults", node->GetSampleRange());
		}
		else // default to CloudInputNode::REQUEST_CURRENT
		{
			parameterObject.AddInt("maxResults", 1);
		}

		numItems++;
	}

	return numItems;
}


// Output: create set requests json
void CloudParameters::CreateSetRequestJson(Core::Json::Item& rootItem) const
{
	Core::Json::Item arrayItem = rootItem.AddArray("parameters");

	// collect parameters by iterating over all cloud output nodes
	const uint32 numParams = mParameters.Size();
	for (uint32 i = 0; i < numParams; ++i)
	{
		const Parameter& parameter = mParameters[i];

		// add unnamed object
		Core::Json::Item parameterObject = arrayItem.AddObject();

		// skip invalid parmeters
		if (parameter.mName == "" || parameter.HasValue() == false)
			continue;

		// add optional items
		if (parameter.mItemId.IsEmpty() == false)	parameterObject.AddString("itemId", parameter.mItemId);
		if (parameter.mTimeStamp != 0)				parameterObject.AddString("timestamp", parameter.mTimeStamp.AsUtcString());
		
		// add mandatory items
		parameterObject.AddInt("type", parameter.mTypeId);
		parameterObject.AddString("name", parameter.mName);
		AddParameterValueItem("parameter", parameter, parameterObject);
	}

}


// add parameter value to json
bool CloudParameters::AddParameterValueItem(const char* name, const CloudParameters::Parameter& parameter, Core::Json::Item& rootItem)
{
	// use correct type
	switch (parameter.mTypeId)
	{
		case TYPE_INT:			rootItem.AddInt(name, parameter.GetValue<int32>()); break;
		case TYPE_FLOAT:		rootItem.AddDouble(name, parameter.GetValue<double>()); break;
		case TYPE_STRING:		rootItem.AddString(name, parameter.GetValue<Core::String>()); break;
		case TYPE_JSON:			rootItem.AddJson(name, parameter.GetValue<Core::Json>()); break;
		//case TYPE_JSONARRAY:	rootItem.AddInt(name, parameter.GetValue<Core::Array<Json>>()); break;
	
		default:				return false;
	}

	return true;
}


void CloudParameters::Log() const
{
	const uint32 numParams = mParameters.Size();
	
	Core::LogDebug("CloudParameters object ( userId='%s' ) contains %i elements: ", (mUser == NULL ? "" : mUser->GetId()), numParams);

	for (uint32 i = 0; i < numParams; ++i)
	{
		const Parameter& parameter = mParameters[i];

		if (parameter.HasValue() == false)
			continue;

		Core::String value;
		switch (parameter.mTypeId)
		{
			case TYPE_INT:		value.Format("%i",	parameter.GetValue<int32>()); break;
			case TYPE_FLOAT:	value.Format("%.6f", parameter.GetValue<double>()); break;
			case TYPE_STRING:	value.Format("'%s'", parameter.GetValue<Core::String>().AsChar() ); break;
			case TYPE_JSON:		value.Format("'%s'", parameter.GetValue<Core::Json>().ToString(false).AsChar()); break;
			//case TYPE_JSONARRAY:		value.Format("'%s'", parameter.GetValue<Core::Array<Core::Json>>().ToString(false)); break;
		}

		Core::LogDebug("  %i:   name='%s'  itemId='%s'  time='%s'  type=%i  value=%s ", i, parameter.mName.AsChar(), parameter.mItemId.AsChar(), (parameter.mTimeStamp == 0 ? "0" : parameter.mTimeStamp.AsUtcString().AsChar()), parameter.mTypeId, value.AsChar());
	}
}


// explicit instantiations of everything that will be needed
//template CloudParameters::Parameter::Value<int32>;
//template CloudParameters::Parameter::Value<double>;
//template CloudParameters::Parameter::Value<Core::String>;
//template CloudParameters::Parameter::Value<Core::Json>;
//template CloudParameters::Parameter::Value<Core::Array<Core::Json>>;

template void CloudParameters::Parameter::SetValue<int32>(const int32& value);
template void CloudParameters::Parameter::SetValue<double>(const double& value);
template void CloudParameters::Parameter::SetValue<Core::String>(const Core::String& value);
template void CloudParameters::Parameter::SetValue<Core::Json>(const Core::Json& value);
//template void CloudParameters::Parameter::SetValue<Core::Array<Core::Json>>(const Core::Array<Core::Json>& value);

template const int32& CloudParameters::Parameter::GetValue<int32>() const;
template const double& CloudParameters::Parameter::GetValue<double>() const;
template const Core::String& CloudParameters::Parameter::GetValue<Core::String>() const;
template const Core::Json& CloudParameters::Parameter::GetValue<Core::Json>() const;
//template const Core::Array<Core::Json>& CloudParameters::Parameter::GetValue<Core::Array<Core::Json>>() const;
