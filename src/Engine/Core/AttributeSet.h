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

#ifndef __CORE_ATTRIBUTESET_H
#define __CORE_ATTRIBUTESET_H

// include the required headers
#include "StandardHeaders.h"
#include "String.h"
#include "Vector.h"
#include "Color.h"
#include "Json.h"
#include "AttributeSettings.h"


namespace Core
{

// forward declarations
class Attribute;
class AttributeSettings;
class Stream;


class ENGINE_API AttributeSet
{
	public:
		AttributeSet()																{}
		virtual ~AttributeSet()														{ RemoveAllAttributes(); }

		inline uint32 GetNumAttributes() const										{ return mAttributes.Size(); }
		inline AttributeSettings* GetAttribute(uint32 index) const					{ return mAttributes[index].mSettings; }
		inline AttributeSettings* GetAttributeSettings(uint32 index) const			{ return mAttributes[index].mSettings; }
		inline Attribute* GetAttributeValue(uint32 index) const						{ return mAttributes[index].mValue; }
		inline void SetAttributeSettings(uint32 index, AttributeSettings* settings)	{ mAttributes[index].mSettings = settings; }
		inline void SetAttributeValue(uint32 index, Attribute* value)				{ mAttributes[index].mValue = value; }

		void AddAttribute(AttributeSettings* settings, Attribute* attributeValue);
		void AddAttribute(AttributeSettings* settings);
		void RemoveAllAttributes(bool delFromMem=true);
		void Resize(uint32 numAttributes)											{ mAttributes.Resize( numAttributes ); }

		uint32 FindAttributeIndexByInternalName(const char* name) const;
		uint32 FindAttributeIndexByName(const char* name) const;

		bool HasAttribute(Attribute* attribute) const;
		bool HasAttributeWithInternalName(const char* name) const					{ return (FindAttributeIndexByInternalName(name) != CORE_INVALIDINDEX32); }
		bool HasAttributeWithName(const char* name) const							{ return (FindAttributeIndexByName(name) != CORE_INVALIDINDEX32); }

		AttributeSettings* FindAttributeSettingsByInternalName(const char* internalName) const;

		// register attribute
		AttributeSettings* RegisterAttribute(const char* name, const char* internalName, const char* description, uint32 interfaceType);

		// set helpers
		bool SetBoolAttribute(const char* internalName, bool value, bool createIfNotExists=true);
		bool SetInt32Attribute(const char* internalName, int32 value, bool createIfNotExists=true);
		bool SetFloatAttribute(const char* internalName, double value, bool createIfNotExists=true);
		bool SetStringAttribute(const char* internalName, const char* value, bool createIfNotExists=true);
		bool SetColorAttribute(const char* internalName, const Color& value, bool createIfNotExists=true);

		bool SetStringAttributeByIndex(uint32 index, const char* value, bool createIfNotExists=true);
		bool SetFloatAttributeByIndex(uint32 index, double value, bool createIfNotExists=true);
		bool SetInt32AttributeByIndex(uint32 index, int32 value, bool createIfNotExists=true);
		bool SetBoolAttributeByIndex(uint32 index, bool value, bool createIfNotExists=true);
		bool SetColorAttributeByIndex(uint32 index, const Color& value, bool createIfNotExists=true);
		bool SetStringArrayAttributeByIndex(uint32 index, const Array<String>& value, bool createIfNotExists=true);
		

		// get helpers
		bool GetBoolAttributeByName(const char* internalName, bool defaultValue=false) const;
		int32 GetInt32AttributeByName(const char* internalName, int32 defaultValue=0) const;
		double GetFloatAttributeByName(const char* internalName, double defaultValue=0.0) const;
        const char* GetStringAttributeByName(const char* internalName, const char* defaultValue="") const;
		const Array<String>& GetStringArrayAttributeByName(const char* internalName, const Array<String>& defaultValue) const;
		Color GetColorAttributeByName(const char* internalName, const Color& defaultValue=Color()) const;
		
		bool GetBoolAttribute(uint32 index, bool defaultValue=false) const;
		int32 GetInt32Attribute(uint32 index, int32 defaultValue=0) const;
		double GetFloatAttribute(uint32 index, double defaultValue=0.0) const;
		const char* GetStringAttribute(uint32 index, const char* defaultValue="") const;
		const Array<String>& GetStringArrayAttribute(uint32 index, const Array<String>& defaultValue) const;
		Color GetColorAttribute(uint32 index, const Color& defaultValue=Color()) const;

		// enable or disable attributes
		void EnableAllAttributes();
		inline bool IsAttributeEnabled(uint32 index) const						{ return mAttributes[index].mSettings->IsEnabled(); }
		inline void SetIsAttributeEnabled(uint32 index, bool isEnabled)			{ mAttributes[index].mSettings->SetIsEnabled(isEnabled); }
		inline void EnableAttribute(uint32 index)								{ mAttributes[index].mSettings->Enable(); }
		inline void DisableAttribute(uint32 index)								{ mAttributes[index].mSettings->Disable(); }

		uint32 GetNumVisibleAttributes() const;
		bool HasVisibleAttributes() const										{ return GetNumVisibleAttributes() > 0; }

		void CreateDefaultAttributeValues();

		void CopyFrom(const AttributeSet& other);
		void Log();

		// json serialization
		void Write(Json& json, Json::Item& item, bool valuesOnly = true) const;
		void Write(Core::String* outString, bool valuesOnly = true, bool niceFormatting = true) const;
		bool Read(const Json& json, const Json::Item& parentItem, bool valuesOnly=false);
		
	private:
		struct AttributeData
		{
			AttributeSettings*	mSettings;
			Attribute*			mValue;

			AttributeData() : mSettings(NULL), mValue(NULL) {}
			~AttributeData() {}
		};

		Array<AttributeData>	mAttributes;
};

} // namespace Core


#endif
