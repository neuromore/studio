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

#ifndef __CORE_ATTRIBUTESTRING_H
#define __CORE_ATTRIBUTESTRING_H

// include the required headers
#include "StandardHeaders.h"
#include "Attribute.h"
#include "String.h"

namespace Core
{

/**
 * The string attribute class.
 * This attribute represents one string.
 */
class ENGINE_API AttributeString : public Attribute
{
	public:
		enum { TYPE_ID = 0x00000003 };

		// constructor and destructor
		AttributeString()                                                           : Attribute()  {}
		AttributeString(const Core::String& value)                                  : Attribute(), mValue(value) {}
		AttributeString(const char* value)                                          : Attribute(), mValue(value) {}
		~AttributeString() {}

		// adjust values
		const char* GetValue() const                                                { return mValue.AsChar(); }
		void SetValue(const char* value)                                            { mValue = value; }

		// overloaded from the attribute base class
		static AttributeString* Create(const String& value)							{ return new AttributeString(value); }
		static AttributeString* Create(const char* value="")						{ return new AttributeString(value); }
		Attribute* Clone() const override                                           { return Create(mValue); }
		const char* GetTypeString() const override                                  { return "String"; }
		uint32 GetType() const override                                             { return TYPE_ID; }
		bool InitFrom(const Attribute* other) override                              { if (other->GetType() != TYPE_ID) return false; mValue = static_cast<const AttributeString*>(other)->GetValue(); return true; }
		bool InitFromString(const String& valueString) override                     { mValue=valueString; return true; }
		bool ConvertToString(String& outString) const override                      { outString = mValue; return true; }

		// json serialization
		void Write(Json& json, Json::Item& item, const char* name) const override	{ item.AddString( name, mValue.AsChar() ); }
		void Read(const Json& json, const Json::Item& item) override                { CORE_ASSERT(item.IsString()==true); mValue = item.GetString(); }

	private:
		Core::String	mValue;		/**< The string value. */
};

} // namespace Core

#endif
