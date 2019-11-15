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

#ifndef __CORE_ATTRIBUTESPECTRUMCHANNELS_H
#define __CORE_ATTRIBUTESPECTRUMCHANNELS_H

// include the required headers
#include "../Core/Array.h"
#include "../Core/Attribute.h"
#include "Channel.h"
#include "Spectrum.h"

#include "AttributeChannels.h"

typedef  AttributeChannels<Spectrum> AttributeSpectrumChannels;
#if FALSE

// helper typedef for an array containing spectrum channel pointers
typedef Core::Array<Channel<Spectrum>*> SpectrumChannelArray;

class ENGINE_API AttributeSpectrumChannels : public Core::Attribute
{
	public:
		enum { TYPE_ID = 0x00001007 };

		AttributeSpectrumChannels()											 : Core::Attribute()			{}
		AttributeSpectrumChannels(const SpectrumChannelArray& channelArray)	 : Core::Attribute()			{ mValue = channelArray; }
		virtual ~AttributeSpectrumChannels()																{}

		static AttributeSpectrumChannels* Create()															{ return new AttributeSpectrumChannels(); }
		static AttributeSpectrumChannels* Create(const SpectrumChannelArray& channelArray)					{ return new AttributeSpectrumChannels(channelArray); }

		void SetValue(const SpectrumChannelArray& value)													{ mValue = value; }
		const SpectrumChannelArray& GetValue() const														{ return mValue; }
		const SpectrumChannelArray& GetValue()																{ return mValue; }

		// helper modification functions
		void SetNumChannels(uint32 num)																		{ mValue.Resize(num); }
		uint32 GetNumChannels() const																		{ return mValue.Size(); }

		void AddChannel(Channel<Spectrum>* channel)															{ mValue.Add(channel); }
		Channel<Spectrum>* GetChannel(uint32 index) const													{ return mValue[index]; }
		void SetChannel(uint32 index, Channel<Spectrum>* channel)											{ mValue[index] = channel; }
		void ClearChannels()																				{ mValue.Clear(); }

		// overloaded from the attribute base class
		Core::Attribute* Clone() const override																{ return Create(mValue); }
		const char* GetTypeString() const override															{ return "AttributeSpectrumChannels"; }
		uint32 GetType() const override																		{ return TYPE_ID; }
		bool InitFrom(const Core::Attribute* other) override												{ if (other->GetType() != TYPE_ID) return false; const AttributeSpectrumChannels* attribute = static_cast<const AttributeSpectrumChannels*>(other); mValue = attribute->mValue; return true; }

		// not yet supported:
		bool InitFromString(const Core::String& valueString) override										{ return false; }
		bool ConvertToString(Core::String& outString) const override										{ return false; }
		void Write(const Core::Json& jsonParser, const Core::Json::Item& parentItem, const char* name) const override	{}
		void Read(const Core::Json& jsonParser, const Core::Json::Item& valueItem) override								{}

	private:
		SpectrumChannelArray mValue;
};
#endif

#endif
