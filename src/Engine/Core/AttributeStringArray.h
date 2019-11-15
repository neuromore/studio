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

#ifndef __CORE_ATTRIBUTESTRINGARRAY_H
#define __CORE_ATTRIBUTESTRINGARRAY_H

// include the required headers
#include "StandardHeaders.h"
#include "Attribute.h"
#include "String.h"

namespace Core
{

/**
 * The string array attribute class.
 * This attribute represents a list of strings and serializes it as a standard string using pipe '|' to separate the (escaped) strings
 */
class ENGINE_API AttributeStringArray : public Attribute
{
	public:
		enum { TYPE_ID = 0x00000005 };

		// constructor and destructor
		AttributeStringArray()											: Attribute()  {}
		AttributeStringArray(const Core::Array<Core::String>& values)	: Attribute(), mValues(values) {}
		AttributeStringArray(const char* csValues)						: Attribute()  { InitFromString(String(csValues)); }
		~AttributeStringArray() {}

		// get and adjust values
		const Core::Array<Core::String>& GetValue() const                           { return mValues; }
		String AsString()                                                           { return Concatenated(); } // TODO remove when final widget is implemented
		void SetValue(const Core::Array<Core::String>& values)                      { mValues = values; }
		void AddString(const Core::String& value)                                   { mValues.Add(value); }
		uint32 GetNumStrings()                                                      { return mValues.Size(); }
		Core::String& GetString(uint32 index)                                       { return mValues[index]; }
		void Clear()                                                                { mValues.Clear(); }

		// overloaded from the attribute base class
		static AttributeStringArray* Create(const Core::Array<Core::String>& values){ return new AttributeStringArray(values); }
		static AttributeStringArray* Create(const char* values)						{ return new AttributeStringArray(values); }
		Attribute* Clone() const override                                           { return Create(mValues); }
		const char* GetTypeString() const override                                  { return "StringArray"; }
		uint32 GetType() const override                                             { return TYPE_ID; }
		bool InitFrom(const Attribute* other) override                              { if (other->GetType() != TYPE_ID) return false; mValues.Clear(); mValues.Add(static_cast<const AttributeStringArray*>(other)->GetValue()); return true; }
		bool InitFromString(const String& valueString) override                     { Split(valueString); return true; }
		bool ConvertToString(String& outString) const override                      { outString = Concatenated(); return true; }

		// json serialization
		void Write(Json& json, Json::Item& item, const char* name) const override   { item.AddString( name, Concatenated().AsChar() ); }
		void Read(const Json& json, const Json::Item& item) override                { CORE_ASSERT(item.IsString()==true); Split(item.GetString()); }

	private:
		Core::Array<Core::String>	mValues;				// the unescaped string values

		// create string from mValues
		Core::String Concatenated() const
		{ 
			String result; 
			const uint32 num = mValues.Size();
			for (uint32 i=0; i<num; ++i) 
			{
				result += Escape(mValues[i]); 
				if (i < num-1) 
					result += ",";  
			}
			return result;
		}

		// extract mValues from string
		void Split(Core::String values)		
		{
			mValues.Clear();

			Core::Array<Core::String> strings = values.Split(StringCharacter::comma);

			const uint32 num = strings.Size();
			for (uint32 i=0; i<num; ++i) 
				mValues.Add(Unescape(strings[i]));
		}

	public:
		static Core::String Escape(const Core::String& unescaped)		{ String escaped = unescaped;   escaped.Replace( ",", "\\,"); return escaped; }
		static Core::String Unescape(const Core::String& escaped)		{ String unescaped = escaped; unescaped.Replace("\\,",  ","); return unescaped; }
};

} // namespace Core

#endif
