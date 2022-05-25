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
#include "AttributeSettings.h"
#include "Attribute.h"
#include "LogManager.h"
#include "../EngineManager.h"


namespace Core
{

AttributeSettings::AttributeSettings()
{
	mInterfaceType	= CORE_INVALIDINDEX32;
	mMinValue		= NULL;
	mMaxValue		= NULL;
	mDefaultValue	= NULL;
	mIsEnabled		= true;
	mIsVisible		= true;
}


AttributeSettings::AttributeSettings(const char* internalName)
{
	mInterfaceType	= CORE_INVALIDINDEX32;
	mMinValue		= NULL;
	mMaxValue		= NULL;
	mDefaultValue	= NULL;
	mInternalName	= internalName;
	mName			= mInternalName;
}


AttributeSettings::~AttributeSettings()
{
	delete mMinValue;
	delete mMaxValue;
	delete mDefaultValue;
}


void AttributeSettings::SetInternalName(const char* internalName)					{ mInternalName = internalName; }
void AttributeSettings::SetName(const char* name)									{ mName = name; }
const char* AttributeSettings::GetInternalName() const								{ return mInternalName.AsChar(); }
const char* AttributeSettings::GetName() const										{ return mName.AsChar(); }
const char* AttributeSettings::GetComboValue(uint32 index) const					{ return mComboValues[index].AsChar(); }
const String& AttributeSettings::GetComboValueString(uint32 index) const			{ return mComboValues[index]; }
const String& AttributeSettings::GetInternalNameString() const						{ return mInternalName; }
const String& AttributeSettings::GetNameString() const								{ return mName; }
void AttributeSettings::AddComboValue(const char* value)							{ mComboValues.Add( value ); }
void AttributeSettings::SetComboValue(uint32 index, const char* value)				{ CORE_ASSERT(index < mComboValues.Size()); mComboValues[index] = value; }


// the equal/copy operator
AttributeSettings& AttributeSettings::operator=(const AttributeSettings& other)
{
	// clone the attributes
	if (other.mMinValue != NULL)		mMinValue		= other.mMinValue->Clone();
	if (other.mMaxValue != NULL)		mMaxValue		= other.mMaxValue->Clone();
	if (other.mDefaultValue != NULL)	mDefaultValue	= other.mDefaultValue->Clone();

	// copy the rest			
	mName			= other.mName;
	mInternalName	= other.mInternalName;
	mDescription	= other.mDescription;
	mInterfaceType	= other.mInterfaceType;
	mComboValues	= other.mComboValues;
	mIsVisible		= other.mIsVisible;
	mIsEnabled		= other.mIsEnabled;

	return *this;									
}													


// clone the attribute settings
AttributeSettings* AttributeSettings::Clone() const
{
	AttributeSettings* newSettings = new AttributeSettings();
	*newSettings = *this;
	return newSettings;
}


// write to json
void AttributeSettings::Write(Json& json, Json::Item& attributeItem) const
{
	attributeItem.AddString( "name", GetName() );
	attributeItem.AddString( "internalName", GetInternalName() );
	attributeItem.AddString( "description", GetDescription() );
	attributeItem.AddInt( "interfaceType", mInterfaceType );
	attributeItem.AddBool( "isEnabled", mIsEnabled );
	attributeItem.AddBool( "isVisible", mIsVisible );

	// combo strings
	const uint32 numComboValues = GetNumComboValues();
	if (numComboValues > 0)
	{
		Json::Item comboItem = attributeItem.AddArray("comboValues");
		for (uint32 i=0; i<numComboValues; ++i)
			comboItem.AddString( GetComboValue(i) );
	}

	// save the default, min and max values
	if (mDefaultValue != NULL)	mDefaultValue->Write( json, attributeItem, "default" );
	if (mMinValue != NULL)		mMinValue->Write( json, attributeItem, "min" );
	if (mMaxValue != NULL)		mMaxValue->Write( json, attributeItem, "max" );
}


// read from json
bool AttributeSettings::Read(const Json& json, const Json::Item& attributeItem)
{
	bool result = true;

	// set the name
	Json::Item nameItem = attributeItem.Find("name");
	if (nameItem.IsString() == true)
	{
		SetName( nameItem.GetString() );
	}
	else
	{
		LogWarning( "AttributeSettings::Read(): 'name' not found or is no valid string." );
		result = false;
	}

	// set the internal name
	Json::Item internalNameItem = attributeItem.Find("internalName");
	if (internalNameItem.IsString() == true)
	{
		SetInternalName( internalNameItem.GetString() );
	}
	else
	{
		LogWarning( "AttributeSettings::Read(): 'internalName' not found or is no valid string." );
		result = false;
	}

	// set the description
	Json::Item descriptionItem = attributeItem.Find("description");
	if (descriptionItem.IsString() == true)
	{
		SetDescription( descriptionItem.GetString() );
	}
	else
	{
		LogWarning( "AttributeSettings::Read(): 'description' not found or is no valid string." );
		result = false;
	}

	// set the interface type
	Json::Item interfaceTypeItem = attributeItem.Find("interfaceType");
	if (interfaceTypeItem.IsInt() == true)
	{
		SetInterfaceType( interfaceTypeItem.GetInt() );
	}
	else
	{
		LogWarning( "AttributeSettings::Read(): 'interfaceType' not found or is no valid integer." );
		result = false;
	}

	// set the enabled state
	Json::Item isEnabledItem = attributeItem.Find("isEnabled");
	if (isEnabledItem.IsBool() == true)
		SetIsEnabled( isEnabledItem.GetBool() );

	// set the visibility state
	Json::Item isVisibleItem = attributeItem.Find("isVisible");
	if (isVisibleItem.IsBool() == true)
		SetVisible( isVisibleItem.GetBool() );

	// set the combo values (in case there are some)
	Json::Item comboValuesItem = attributeItem.Find("comboValues");
	if (comboValuesItem.IsArray() == true)
	{
		// get the number of combo value strings and iterate through them
		const uint32 numComboValues = comboValuesItem.Size();
		for (uint32 i=0; i<numComboValues; ++i)
		{
			Json::Item comboValueItem = comboValuesItem[i];
			SetComboValue(i, comboValueItem.GetString() );
		}
	}

	// set the default value
	if (mDefaultValue != NULL)
	{
		Json::Item valueItem = attributeItem.Find("default");
		if (valueItem.IsNull() == false)
			mDefaultValue->Read( json, valueItem );
	}

	// set the min value
	if (mMinValue != NULL)
	{
		Json::Item valueItem = attributeItem.Find("min");
		if (valueItem.IsNull() == false)
			mMinValue->Read( json, valueItem );
	}

	// set the max value
	if (mMaxValue != NULL)
	{
		Json::Item valueItem = attributeItem.Find("max");
		if (valueItem.IsNull() == false)
			mMaxValue->Read( json, valueItem );
	}

	return result;
}

} // namespace Core
