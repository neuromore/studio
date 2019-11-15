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

#ifndef __CORE_ATTRIBUTEINT32_H
#define __CORE_ATTRIBUTEINT32_H

// include the required headers
#include "StandardHeaders.h"
#include "Attribute.h"


namespace Core
{

class ENGINE_API AttributeInt32 : public Attribute
{
	public:
		enum { TYPE_ID = 0x00000002 };

		// constructor & destructor
		AttributeInt32()															: Attribute(), mValue(0)	 {}
		AttributeInt32(int32 value)													: Attribute(), mValue(value) {}
		~AttributeInt32() {}

		// adjust values
		inline int32 GetValue() const												{ return mValue; }
		inline void SetValue(int32 value)											{ mValue = value; }
		
		// overloaded from the attribute base class
		static AttributeInt32* Create();
		static AttributeInt32* Create(int32 value);
		Attribute* Clone() const override											{ return Create(mValue); }
		const char* GetTypeString() const override									{ return "Int32"; }
		uint32 GetType() const override												{ return TYPE_ID; }
		bool InitFrom(const Attribute* other) override								{ if (other->GetType() != TYPE_ID) return false; mValue = static_cast<const AttributeInt32*>(other)->GetValue(); return true; }
		bool InitFromString(const String& valueString) override						{ if (valueString.IsValidInt() == false) return false; mValue = valueString.ToInt(); return true; }
		bool ConvertToString(String& outString) const override						{ outString.Format("%d", mValue); return true; }

		// json serialization
		void Write(Json& json, Json::Item& item, const char* name) const override	{ item.AddInt( name, mValue ); }
		void Read(const Json& json, const Json::Item& item) override				{ CORE_ASSERT(item.IsInt()==true); mValue = item.GetInt(); }

	private:
		int32	mValue;		/**< The signed integer value. */
};

} // namespace Core

#endif
