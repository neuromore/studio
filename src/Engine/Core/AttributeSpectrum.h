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

#ifndef __CORE_ATTRIBUTESPECTRUM_H
#define __CORE_ATTRIBUTESPECTRUM_H

// include the required headers
#include "StandardHeaders.h"
#include "Attribute.h"
#include "../DSP/SpectrumChannel.h"


namespace Core
{

class ENGINE_API AttributeSpectrum : public Core::Attribute
{
	public:
		enum { TYPE_ID = 0x00001006 };

		AttributeSpectrum()							 : Core::Attribute()							{ mValue=NULL; }
		AttributeSpectrum(SpectrumChannel* channel)	 : Core::Attribute()							{ mValue = channel; }
		~AttributeSpectrum() {}

		static AttributeSpectrum* Create()															{ return new AttributeSpectrum(); }
		static AttributeSpectrum* Create(SpectrumChannel* channel)									{ return new AttributeSpectrum(channel); }

		void SetValue(SpectrumChannel* value)														{ mValue = value; }
		SpectrumChannel* GetValue() const															{ return mValue; }
		SpectrumChannel* GetValue()																	{ return mValue; }

		// overloaded from the attribute base class
		Core::Attribute* Clone() const override														{ return Create(mValue); }
		const char* GetTypeString() const override													{ return "SpectrumChannel"; }
		uint32 GetType() const override																{ return TYPE_ID; }
		bool InitFrom(const Core::Attribute* other) override										{ if (other->GetType() != TYPE_ID) return false; const AttributeSpectrum* attribute = static_cast<const AttributeSpectrum*>(other); mValue = attribute->GetValue(); return true; }

		// not yet supported:
		bool InitFromString(const Core::String& valueString) override								{ return false; }
		bool ConvertToString(Core::String& outString) const override								{ return false; }

		void Write(const Json& json, const Json::Item& item, const char* name) const override		{}
		void Read(const Json& json, const Json::Item& item) override								{}

	private:
		SpectrumChannel* mValue;
};

} // namespace Core

#endif
