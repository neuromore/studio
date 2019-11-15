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

#ifndef __CORE_ATTRIBUTEBOOL_H
#define __CORE_ATTRIBUTEBOOL_H

// include the required headers
#include "StandardHeaders.h"
#include "Attribute.h"


namespace Core
{

class ENGINE_API AttributeBool : public Attribute
{
	public:
		enum { TYPE_ID = 0x00000004 };

		// constructor & destructor
		AttributeBool() : Attribute()																	{ mValue=false; }
		AttributeBool(bool value) : Attribute()															{ mValue=value; }
		~AttributeBool()																				{}

		// adjust values
		bool GetValue() const																			{ return mValue; }
		void SetValue(bool value)																		{ mValue = value; }

		// overloaded from the attribute base class
		static AttributeBool* Create()																	{ return new AttributeBool(); }
		static AttributeBool* Create(bool value)														{ return new AttributeBool(value); }
		Attribute* Clone() const override																{ return Create(mValue); }
		const char* GetTypeString() const override														{ return "Bool"; }
		uint32 GetType() const override																	{ return TYPE_ID; }
		bool InitFrom(const Attribute* other) override													{ if (other->GetType() != TYPE_ID) return false; mValue = static_cast<const AttributeBool*>(other)->GetValue(); return true; }
		bool InitFromString(const String& valueString) override											{ if (valueString.IsValidBool() == false) return false; mValue = valueString.ToBool(); return true; }
		bool ConvertToString(String& outString) const override											{ outString.Format("%d", (mValue==true) ? 1 : 0); return true; }

		// json serialization
		void Write(Json& json, Json::Item& item, const char* name) const override						{ item.AddBool( name, mValue ); }
		void Read(const Json& json, const Json::Item& item) override									{ CORE_ASSERT(item.IsBool()==true); mValue = item.GetBool(); }

	private:
		bool mValue;
};

} // namespace Core


#endif
