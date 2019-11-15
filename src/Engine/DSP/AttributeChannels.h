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

#ifndef __CORE_ATTRIBUTECHANNELS_H
#define __CORE_ATTRIBUTECHANNELS_H

// include the required headers
#include "../Core/Array.h"
#include "../Core/Json.h"
#include "../Core/Attribute.h"
#include "MultiChannel.h"

template <class T>
class AttributeChannels : public Core::Attribute, public MultiChannel
{
	public:
		static ENGINE_API const uint32 TYPE_ID;

		// FIXME specialized TYPE_ID cannot be used in switches?! not a constant expression, apparently
		enum
		{
			TYPE_ID_DOUBLE = 0x0001008,
			TYPE_ID_SPECTRUM = 0x0001007
		};

		AttributeChannels()																					{}
		virtual ~AttributeChannels()																		{}

		static AttributeChannels<T>* Create()																{ return new AttributeChannels<T>(); }
		
		const MultiChannel& GetValue() const																{ return static_cast<const MultiChannel&>(*this); }
		const MultiChannel& GetValue()																		{ return *this; }

		// overloaded from the attribute base class
		Core::Attribute* Clone() const override																{ return new AttributeChannels<T>(); }
		virtual uint32 GetType() const override																{ return TYPE_ID; }
		const char* GetTypeString() const override															{ return "AttributeChannels"; }
		bool InitFrom(const Core::Attribute* other) override												{ CORE_ASSERT(false); return false; /* not implemented */ }

		// not yet supported:
		bool InitFromString(const Core::String& valueString) override										{ return false; }
		bool ConvertToString(Core::String& outString) const override										{ return false; }
		void Write(Core::Json& json, Core::Json::Item& item, const char* name) const override				{}
		void Read(const Core::Json& json, const Core::Json::Item& item) override							{}
};


#endif
