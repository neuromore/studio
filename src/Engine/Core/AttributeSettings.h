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

#ifndef __CORE_ATTRIBUTESETTINGS_H
#define __CORE_ATTRIBUTESETTINGS_H

// include the required headers
#include "StandardHeaders.h"
#include "Array.h"
#include "String.h"
#include "Json.h"


namespace Core
{

// forward declarations
class Attribute;

// the attribute interface types
enum
{
	ATTRIBUTE_INTERFACETYPE_FLOATSPINNER	= 0,	// Core::AttributeFloat
	ATTRIBUTE_INTERFACETYPE_FLOATSLIDER		= 1,	// Core::AttributeFloat
	ATTRIBUTE_INTERFACETYPE_INTSPINNER		= 2,	// Core::AttributeInt32
	ATTRIBUTE_INTERFACETYPE_INTSLIDER		= 3,	// Core::AttributeInt32
	ATTRIBUTE_INTERFACETYPE_COMBOBOX		= 4,	// Core::AttributeInt32
	ATTRIBUTE_INTERFACETYPE_CHECKBOX		= 5,	// Core::AttributeBool
	ATTRIBUTE_INTERFACETYPE_COLOR			= 10,	// Core::AttributeColor
	ATTRIBUTE_INTERFACETYPE_STRING			= 11,	// Core::AttributeString
	ATTRIBUTE_INTERFACETYPE_FLOATSLIDERLABEL= 12,	// Core::AttributeFloat
	ATTRIBUTE_INTERFACETYPE_COLORMAPPING	= 13,	// Core::AttributeInt32
	ATTRIBUTE_INTERFACETYPE_WINDOWFUNCTION	= 14,	// Core::AttributeInt32
	ATTRIBUTE_INTERFACETYPE_STRINGARRAY		= 15,	// Core::AttributeStringArray
	ATTRIBUTE_INTERFACETYPE_BUTTON			= 16,	// Core::AttributeButton
	ATTRIBUTE_INTERFACETYPE_TEXT			= 17,	// Core::AttributeText
	ATTRIBUTE_INTERFACETYPE_UNKNOWN			= 0xFFFFFFFF
};


class ENGINE_API AttributeSettings
{
	public:
		AttributeSettings();
		AttributeSettings(const char* internalName);
		~AttributeSettings();

		// mutators
		void SetInternalName(const char* internalName);
		void SetName(const char* name);
		void SetDescription(const char* description)					{ mDescription = description; }
		void SetInterfaceType(uint32 interfaceTypeID)					{ mInterfaceType = interfaceTypeID; }

		// accessors
		const char* GetInternalName() const;
		const char* GetName() const;
		const char* GetDescription() const								{ return mDescription.AsChar(); }
		uint32 GetInterfaceType() const									{ return mInterfaceType; }
		const String& GetInternalNameString() const;
		const String& GetNameString() const;
		const String& GetDescriptionString() const						{ return mDescription; }

		// enabled state
		bool IsEnabled() const											{ return mIsEnabled; }
		void SetIsEnabled(bool isEnabled)								{ mIsEnabled = isEnabled; }
		void Enable()													{ mIsEnabled = true; }
		void Disable()													{ mIsEnabled = false; }
		bool IsVisible() const											{ return mIsVisible; }
		void SetVisible(bool isVisible)									{ mIsVisible = isVisible; }

		// combo values
		const char* GetComboValue(uint32 index) const;
		const String& GetComboValueString(uint32 index) const;
		uint32 GetNumComboValues() const								{ return mComboValues.Size(); }
		void ReserveComboValues(uint32 numToReserve)					{ mComboValues.Reserve( numToReserve ); }
		void ResizeComboValues(uint32 numToResize)						{ mComboValues.Resize( numToResize ); }
		void AddComboValue(const char* value);
		void SetComboValue(uint32 index, const char* value);
		
		// default, min and max values
		Attribute* GetDefaultValue() const								{ return mDefaultValue; }
		Attribute* GetMinValue() const									{ return mMinValue; }
		Attribute* GetMaxValue() const									{ return mMaxValue; }
		void SetDefaultValue(Attribute* value)							{ mDefaultValue = value; }
		void SetMinValue(Attribute* value)								{ mMinValue = value; }
		void SetMaxValue(Attribute* value)								{ mMaxValue = value; }

		AttributeSettings* Clone() const;		
		AttributeSettings& operator=(const AttributeSettings& other);

		// json serialization
		void Write(Json& json, Json::Item& attributeItem) const;
		bool Read(const Json& json, const Json::Item& attributeItem);

	private:
		Attribute*		mMinValue;		
		Attribute*		mMaxValue;		
		Attribute*		mDefaultValue;	
		Array<Core::String>	mComboValues;	
		String			mDescription;	
		Core::String	mName;			
		Core::String	mInternalName;	
		uint32			mInterfaceType;	
		bool			mIsEnabled;
		bool			mIsVisible;
};

} // namespace Core


#endif
