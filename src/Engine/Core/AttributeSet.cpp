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

// include the required headers
#include "StandardHeaders.h"
#include "AttributeSet.h"
#include "Attribute.h"
#include "AttributeSettings.h"
#include <rapidjson/stream.h>
#include "LogManager.h"
#include "String.h"
#include "AttributeString.h"
#include "AttributeBool.h"
#include "AttributeInt32.h"
#include "AttributeFloat.h"
#include "AttributeString.h"
#include "AttributeStringArray.h"
#include "AttributeColor.h"


namespace Core
{

// a Set....Attribute by name implementation macro
#define CORE_ATTRIBUTESET_DECLARE_SETBYNAME(AttributeType) \
	const uint32 index = FindAttributeIndexByInternalName( internalName );\
	if (index == CORE_INVALIDINDEX32)\
	{\
		if (createIfNotExists == true)\
		{\
			Attribute* attribute = AttributeType::Create( value );\
			AttributeSettings* settings = new AttributeSettings(internalName);\
			settings->SetDefaultValue( attribute->Clone() );\
			settings->SetName(internalName);\
			AddAttribute( settings, attribute );\
			return true;\
		}\
		else\
			return false;\
	}\
	\
	Attribute* attribute = mAttributes[index].mValue;\
	if (attribute->GetType() == AttributeType::TYPE_ID)\
	{\
		AttributeType* castAttribute = static_cast<AttributeType*>(attribute);\
		castAttribute->SetValue( value );\
	}\
	else\
	{\
		LogWarning("Core::AttributeSet::Set....Attribute() - The existing attribute '%s' is not of the right type.", internalName);\
		return false;\
	}\
	return true;


	
// a Set....Attribute by index implementation macro
#define CORE_ATTRIBUTESET_DECLARE_SETBYINDEX(AttributeType) \
	Attribute* attribute = mAttributes[index].mValue;\
	if (attribute->GetType() == AttributeType::TYPE_ID)\
	{\
		AttributeType* castAttribute = static_cast<AttributeType*>(attribute);\
		castAttribute->SetValue( value );\
	}\
	else\
	{\
		LogWarning("Core::AttributeSet::Set....Attribute() - The existing attribute '%s' is not of the right type.", mAttributes[index].mSettings->GetInternalNameString().AsChar() );\
		return false;\
	}\
	return true;



// a quick Get....Attribute implementation
#define CORE_ATTRIBUTESET_DECLARE_GETBYNAME(AttributeType) \
	const uint32 index = FindAttributeIndexByInternalName( internalName );\
	if (index == CORE_INVALIDINDEX32)\
		return defaultValue;\
	\
	Attribute* attribute = mAttributes[index].mValue;\
	if (attribute->GetType() != AttributeType::TYPE_ID)\
		return defaultValue;\
	\
	AttributeType* castAttribute = static_cast<AttributeType*>(attribute);\
	return castAttribute->GetValue();


// a quick Get....Attribute implementation
#define CORE_ATTRIBUTESET_DECLARE_GETBYINDEX(AttributeType) \
	Attribute* attribute = mAttributes[index].mValue;\
	if (attribute->GetType() != AttributeType::TYPE_ID)\
		return defaultValue;\
	\
	AttributeType* castAttribute = static_cast<AttributeType*>(attribute);\
	return castAttribute->GetValue();

//--------------------------------------------------------

// add an attribute
void AttributeSet::AddAttribute(AttributeSettings* settings, Attribute* attributeValue)
{ 
	CORE_ASSERT( HasAttributeWithInternalName(settings->GetInternalName()) == false );
	mAttributes.AddEmpty();
	mAttributes.GetLast().mSettings = settings;
	mAttributes.GetLast().mValue	= attributeValue;
}


// add an attribute
void AttributeSet::AddAttribute(AttributeSettings* settings)
{
	CORE_ASSERT( HasAttributeWithInternalName(settings->GetInternalName()) == false );
	mAttributes.AddEmpty();
	mAttributes.GetLast().mSettings = settings;
	mAttributes.GetLast().mValue	= NULL;
}


// remove all attributes
void AttributeSet::RemoveAllAttributes(bool delFromMem)
{
	if (delFromMem == true)
	{
		const uint32 numAttributes = mAttributes.Size();
		for (uint32 i=0; i<numAttributes; ++i)
		{
			delete mAttributes[i].mSettings;
			delete mAttributes[i].mValue;
		}
	}

	mAttributes.Clear(true);
}


// search for attribute by pointer
bool AttributeSet::HasAttribute(Attribute* attribute) const
{
	const uint32 numAttributes = mAttributes.Size();
	for (uint32 i=0; i<numAttributes; ++i)
		if (mAttributes[i].mValue == attribute)
			return true;

	return false;
}


// find an attribute by its name
uint32 AttributeSet::FindAttributeIndexByInternalName(const char* name) const
{
	const uint32 numAttributes = mAttributes.Size();
	for (uint32 i=0; i<numAttributes; ++i)
		if (mAttributes[i].mSettings->GetInternalNameString() == name)
			return i;

	return CORE_INVALIDINDEX32;
}


// find an attribute by its name
uint32 AttributeSet::FindAttributeIndexByName(const char* name) const
{
	const uint32 numAttributes = mAttributes.Size();
	for (uint32 i=0; i<numAttributes; ++i)
		if (mAttributes[i].mSettings->GetNameString() == name)
			return i;

	return CORE_INVALIDINDEX32;
}


// find the attribute settings based on the internal name
AttributeSettings* AttributeSet::FindAttributeSettingsByInternalName(const char* internalName) const
{
	const uint32 numAttributes = mAttributes.Size();
	for (uint32 i=0; i<numAttributes; ++i)
		if (mAttributes[i].mSettings->GetInternalNameString().IsEqual(internalName) == true)
			return mAttributes[i].mSettings;

	return NULL;
}


// copy over attributes
void AttributeSet::CopyFrom(const AttributeSet& other)
{
	// remove all attributes
	RemoveAllAttributes();

	// copy over all attributes
	const uint32 numAttribs = other.GetNumAttributes();
	mAttributes.Reserve( numAttribs );
	for (uint32 i=0; i<numAttribs; ++i)
	{
		const AttributeData& data = other.mAttributes[i];
		mAttributes.AddEmpty();
		mAttributes.GetLast().mSettings	= data.mSettings->Clone();
		mAttributes.GetLast().mValue	= data.mValue->Clone();
	}
}


// log the data inside the set
void AttributeSet::Log()
{
	String valueString;
	const uint32 numAttribs = mAttributes.Size();
	for (uint32 i=0; i<numAttribs; ++i)
	{
		// convert the value to a string
		if (mAttributes[i].mValue->ConvertToString( valueString ) == false)
			valueString = "<Failed to convert attribute to string>";

		LogDetailedInfo("#%d - internalName='%s' - value=%s (type=%s)", i, GetAttributeSettings(i)->GetInternalName(), valueString.AsChar(), mAttributes[i].mValue->GetTypeString());
	}
}


// write the attributes to json
void AttributeSet::Write(Json& json, Json::Item& item, bool valuesOnly) const
{
	// make sure the given parent item is valid
	if (item.IsNull() == true)
		return;

	// get the number of attributes in the set
	uint32 numAttributes = mAttributes.Size();
	if (numAttributes == 0)
		return;

	// write all attributes
	Json::Item attributesItem = item.AddArray("attributes");
	for (uint32 i=0; i<numAttributes; ++i)
	{
		Json::Item attributeItem = attributesItem.AddObject();

		if (valuesOnly == true)
		{
			// write the attribute internal name
			attributeItem.AddString( "internalName", mAttributes[i].mSettings->GetInternalName() );
		}
		else
		{
			// write the attribute settings
			mAttributes[i].mSettings->Write(json, attributeItem);
		}

		// write the attribute value
		mAttributes[i].mValue->Write( json, attributeItem, "value" );
	}
}


// write the attributes as json to a string
void AttributeSet::Write(Core::String* outString, bool valuesOnly, bool niceFormatting) const
{
	Json json;

	// construct json object
	Json::Item rootItem = json.GetRootItem();
	Write( json, rootItem, valuesOnly );

	// write to string
	json.WriteToString( *outString, niceFormatting );
}


// read the attributes from json
bool AttributeSet::Read(const Json& json, const Json::Item& parentItem, bool valuesOnly)
{
	bool result = true;

	// make sure the given parent item is valid
	if (parentItem.IsNull() == true)
		return false;

	// get the attributes item (in case there are no attributes it is all fine, there are e.g. some nodes that do not contain any attributes)
	Json::Item attributesItem = parentItem.Find("attributes");
	if (attributesItem.IsNull() == true)
		return true;

	uint32 numWarnings = 0;
	String warningText = "AttributeSettings::Read(): ";

	// get the number of attributes and iterate through them
	const uint32 numAttributes = attributesItem.Size();
	for (uint32 i=0; i<numAttributes; ++i)
	{
		Json::Item attributeItem = attributesItem[i];

		// get the internal name
		Json::Item internalNameItem = attributeItem.Find("internalName");
		if (internalNameItem.IsString() == false)
		{
			numWarnings++;
			warningText.FormatAdd( "%i. No internal name found for attribute.", numWarnings );
			result = false;
			continue;
		}

		// get the attribute index based on the internal name
		const uint32 attributeIndex = FindAttributeIndexByInternalName( internalNameItem.GetString() );
		if (attributeIndex == CORE_INVALIDINDEX32)
		{
			numWarnings++;
			warningText.FormatAdd( "%i. Cannot find index for attribute '%s'. Attribute will be skipped.", numWarnings, internalNameItem.GetString() );
			continue;
		}

		// get the attribute
		Attribute* attribute = GetAttributeValue( attributeIndex );

		if (valuesOnly == false)
		{
			// get the attribute settings
			AttributeSettings* attributeSettings = GetAttributeSettings( attributeIndex );

			// read the attribute settings
			if (attributeSettings->Read(json, attributeItem) == false)
				result = false;
		}

		// read the attribute value
		Json::Item valueItem = attributeItem.Find("value");
		if (valueItem.IsNull() == false)
			attribute->Read( json, valueItem );
	}

	if (numWarnings > 0)
		LogWarning( warningText.AsChar() );

	return result;
}


// create default attribute values
void AttributeSet::CreateDefaultAttributeValues()
{
	// get the number of attributes and iterate through them
	const uint32 numAttributes = GetNumAttributes();
	for (uint32 i=0; i<numAttributes; ++i)
	{
		AttributeSettings* attributeSettings = mAttributes[i].mSettings;

		// create a new clone of the default attribute and set it as attribute value
		if (mAttributes[i].mValue == NULL)
			mAttributes[i].mValue = attributeSettings->GetDefaultValue()->Clone();
	}
}


// register an attribute
AttributeSettings* AttributeSet::RegisterAttribute(const char* name, const char* internalName, const char* description, uint32 interfaceType)
{
	// create the attribute settings
	AttributeSettings* attributeSettings = new AttributeSettings();
	attributeSettings->SetName( name );
	attributeSettings->SetInternalName( internalName );
	attributeSettings->SetDescription( description );
	attributeSettings->SetInterfaceType( interfaceType );

	// add the attribute settings to the attribute set and return it
	AddAttribute( attributeSettings );
	return attributeSettings;
}


// enable all attributes
void AttributeSet::EnableAllAttributes()
{
	// get the number of attributes and iterate through them
	const uint32 numAttributes = GetNumAttributes();
	for (uint32 i=0; i<numAttributes; ++i)
	{
		AttributeSettings* attributeSettings = mAttributes[i].mSettings;
		attributeSettings->Enable();
	}
}


// calculate the number of visible attributes
uint32 AttributeSet::GetNumVisibleAttributes() const
{
	uint32 result = 0;

	// get the number of attributes and iterate through them
	const uint32 numAttributes = GetNumAttributes();
	for (uint32 i=0; i<numAttributes; ++i)
	{
		AttributeSettings* attributeSettings = mAttributes[i].mSettings;
		if (attributeSettings->IsVisible() == true)
			result++;
	}

	return result;
}

// FIXME: the setters/getters below have opposite naming conventions (Setters used mainly "by name", for whatever reason.. by-index should be the default). Rename setters similar to getters

// set helpers
bool AttributeSet::SetFloatAttribute(const char* internalName, double value, bool createIfNotExists)								{ CORE_ATTRIBUTESET_DECLARE_SETBYNAME( AttributeFloat ) }
bool AttributeSet::SetInt32Attribute(const char* internalName, int32 value, bool createIfNotExists)									{ CORE_ATTRIBUTESET_DECLARE_SETBYNAME( AttributeInt32 ) }
bool AttributeSet::SetBoolAttribute(const char* internalName, bool value, bool createIfNotExists)									{ CORE_ATTRIBUTESET_DECLARE_SETBYNAME( AttributeBool ) }
bool AttributeSet::SetColorAttribute(const char* internalName, const Color& value, bool createIfNotExists)							{ CORE_ATTRIBUTESET_DECLARE_SETBYNAME( AttributeColor ) }
bool AttributeSet::SetStringAttribute(const char* internalName, const char* value, bool createIfNotExists)							{ CORE_ATTRIBUTESET_DECLARE_SETBYNAME( AttributeString ) }
bool AttributeSet::SetTextAttribute(const char* internalName, const char* value, bool createIfNotExists)							{ CORE_ATTRIBUTESET_DECLARE_SETBYNAME( AttributeText ) }

bool AttributeSet::SetFloatAttributeByIndex(uint32 index, double value, bool createIfNotExists)										{ CORE_ATTRIBUTESET_DECLARE_SETBYINDEX( AttributeFloat ) }
bool AttributeSet::SetInt32AttributeByIndex(uint32 index, int32 value, bool createIfNotExists)										{ CORE_ATTRIBUTESET_DECLARE_SETBYINDEX( AttributeInt32 ) }
bool AttributeSet::SetBoolAttributeByIndex(uint32 index, bool value, bool createIfNotExists)										{ CORE_ATTRIBUTESET_DECLARE_SETBYINDEX( AttributeBool ) }
bool AttributeSet::SetColorAttributeByIndex(uint32 index, const Color& value, bool createIfNotExists)								{ CORE_ATTRIBUTESET_DECLARE_SETBYINDEX( AttributeColor ) }
bool AttributeSet::SetStringAttributeByIndex(uint32 index, const char* value, bool createIfNotExists)								{ CORE_ATTRIBUTESET_DECLARE_SETBYINDEX( AttributeString ) }
bool AttributeSet::SetStringArrayAttributeByIndex(uint32 index, const Array<String>&  value, bool createIfNotExists)				{ CORE_ATTRIBUTESET_DECLARE_SETBYINDEX( AttributeStringArray ) }
bool AttributeSet::SetTextAttributeByIndex(uint32 index, const char* value, bool createIfNotExists)								{ CORE_ATTRIBUTESET_DECLARE_SETBYINDEX( AttributeText ) }


// get helpers
bool AttributeSet::GetBoolAttributeByName(const char* internalName, bool defaultValue) const
{
	const uint32 index = FindAttributeIndexByInternalName( internalName );
	if (index == CORE_INVALIDINDEX32)
		return defaultValue;
	
	Attribute* attribute = mAttributes[index].mValue;
	if (attribute->GetType() != AttributeBool::TYPE_ID)
		return defaultValue;
	
	AttributeBool* castAttribute = static_cast<AttributeBool*>(attribute);
	return castAttribute->GetValue();
}



int32 AttributeSet::GetInt32AttributeByName(const char* internalName, int32 defaultValue) const										{ CORE_ATTRIBUTESET_DECLARE_GETBYNAME( AttributeInt32 ) }
double AttributeSet::GetFloatAttributeByName(const char* internalName, double defaultValue)	const									{ CORE_ATTRIBUTESET_DECLARE_GETBYNAME( AttributeFloat ) }
const char* AttributeSet::GetStringAttributeByName(const char* internalName, const char* defaultValue) const						{ CORE_ATTRIBUTESET_DECLARE_GETBYNAME( AttributeString ) }
const Array<String>& AttributeSet::GetStringArrayAttributeByName(const char* internalName, const Array<String>& defaultValue) const	{ CORE_ATTRIBUTESET_DECLARE_GETBYNAME( AttributeStringArray ) }
Color AttributeSet::GetColorAttributeByName(const char* internalName, const Color& defaultValue) const								{ CORE_ATTRIBUTESET_DECLARE_GETBYNAME( AttributeColor ) }

bool AttributeSet::GetBoolAttribute(uint32 index, bool defaultValue) const															{ CORE_ATTRIBUTESET_DECLARE_GETBYINDEX( AttributeBool ) }
int32 AttributeSet::GetInt32Attribute(uint32 index, int32 defaultValue) const														{ CORE_ATTRIBUTESET_DECLARE_GETBYINDEX( AttributeInt32 ) }
double AttributeSet::GetFloatAttribute(uint32 index, double defaultValue) const														{ CORE_ATTRIBUTESET_DECLARE_GETBYINDEX( AttributeFloat ) }
const char* AttributeSet::GetStringAttribute(uint32 index, const char* defaultValue) const											{ CORE_ATTRIBUTESET_DECLARE_GETBYINDEX( AttributeString ) }
const Array<String>& AttributeSet::GetStringArrayAttribute(uint32 index, const Array<String>& defaultValue) const					{ CORE_ATTRIBUTESET_DECLARE_GETBYINDEX( AttributeStringArray ) }
Color AttributeSet::GetColorAttribute(uint32 index, const Color& defaultValue) const												{ CORE_ATTRIBUTESET_DECLARE_GETBYINDEX( AttributeColor ) }
const char* AttributeSet::GetTextAttribute(uint32 index, const char* defaultValue) const											{ CORE_ATTRIBUTESET_DECLARE_GETBYINDEX( AttributeText ) }

} // namespace Core
