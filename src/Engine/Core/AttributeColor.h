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

#ifndef __CORE_ATTRIBUTECOLOR_H
#define __CORE_ATTRIBUTECOLOR_H

// include the required headers
#include "StandardHeaders.h"
#include "Attribute.h"
#include "Vector.h"
#include "Color.h"


namespace Core
{

class ENGINE_API AttributeColor : public Attribute
{
	public:
		enum { TYPE_ID = 0x0000000a };

		// constructor & destructor
		AttributeColor() : Attribute()								{ mValue.Set(0.0, 0.0, 0.0, 1.0); }
		AttributeColor(const Color& value) : Attribute()			{ mValue=value; }
		~AttributeColor() {}

		// adjust values
		inline const Color& GetValue() const						{ return mValue; }
		inline void SetValue(const Color& value)					{ mValue = value; }

		// overloaded from the attribute base class
		static AttributeColor* Create()								{ return new AttributeColor(); }
		static AttributeColor* Create(const Color& value)			{ return new AttributeColor(value); }
		Attribute* Clone() const override							{ return Create(mValue); }
		const char* GetTypeString() const override					{ return "Color"; }
		uint32 GetType() const override								{ return TYPE_ID; }
		bool InitFrom(const Attribute* other) override				{ if (other->GetType() != TYPE_ID) return false; mValue = static_cast<const AttributeColor*>(other)->GetValue(); return true; }
		bool InitFromString(const String& valueString) override		{ if (valueString.IsValidVector4() == false) return false; Vector4 vec=valueString.ToVector4(); mValue.Set(vec.x, vec.y, vec.z, vec.w); return true; }
		bool ConvertToString(String& outString) const override		{ outString.FromVector4( Vector4(mValue.r, mValue.g, mValue.b, mValue.a) ); return true; }

		// json serialization
		void Write(Json& json, Json::Item& item, const char* name) const override
		{
			Json::Item colorItem = item.AddObject( name );
			colorItem.AddDouble( "r", mValue.r );
			colorItem.AddDouble( "g", mValue.g );
			colorItem.AddDouble( "b", mValue.b );
			colorItem.AddDouble( "a", mValue.a );
		}

		void Read(const Json& json, const Json::Item& item) override
		{
			Json::Item rItem = item.Find("r");
			Json::Item gItem = item.Find("g");
			Json::Item bItem = item.Find("b");
			Json::Item aItem = item.Find("a");

			CORE_ASSERT( rItem.IsNumber() && gItem.IsNumber() && bItem.IsNumber() && aItem.IsNumber() );

			mValue = Color( rItem.GetDouble(), gItem.GetDouble(), bItem.GetDouble(), aItem.GetDouble() );
		}

	private:
		Color	mValue;		/**< The color value. */
};

} // namespace Core

#endif
