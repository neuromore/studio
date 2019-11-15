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

#ifndef __CORE_ATTRIBUTEDOUBLECHANNELS_H
#define __CORE_ATTRIBUTEDOUBLECHANNELS_H

// include the required headers
#include "../Core/Array.h"
#include "../Core/Attribute.h"
#include "Channel.h"
#include "AttributeChannels.h"

typedef  AttributeChannels<double> AttributeDoubleChannels;
#if FALSE

// helper typedef for an array containing spectrum channel pointers
typedef Core::Array<Channel<double>*> DoubleChannelArray;


class ENGINE_API AttributeDoubleChannels : public Core::Attribute
{
	public:
		enum { TYPE_ID = 0x00001008 };

		AttributeDoubleChannels()										 : Core::Attribute()				{}
		AttributeDoubleChannels(const DoubleChannelArray& channelArray)	 : Core::Attribute()				{ mValue = channelArray; }
		virtual ~AttributeDoubleChannels()																	{}

		static AttributeDoubleChannels* Create()															{ return new AttributeDoubleChannels(); }
		static AttributeDoubleChannels* Create(const DoubleChannelArray& channelArray)						{ return new AttributeDoubleChannels(channelArray); }

		void SetValue(const DoubleChannelArray& value)														{ mValue = value; }
		const DoubleChannelArray& GetValue() const															{ return mValue; }
		const DoubleChannelArray& GetValue()																{ return mValue; }

		// helper modification functions
		void SetNumChannels(uint32 num)																		{ mValue.Resize(num); }
		uint32 GetNumChannels() const																		{ return mValue.Size(); }

		void AddChannel(Channel<double>* channel)															{ mValue.Add(channel); }
		Channel<double>* GetChannel(uint32 index) const														{ return mValue[index]; }
		void SetChannel(uint32 index, Channel<double>* channel)												{ mValue[index] = channel; }
		void ClearChannels()																				{ mValue.Clear(); }

		// overloaded from the attribute base class
		Core::Attribute* Clone() const override																{ return Create(mValue); }
		const char* GetTypeString() const override															{ return "AttributeDoubleChannels"; }
		uint32 GetType() const override																		{ return TYPE_ID; }
		bool InitFrom(const Core::Attribute* other) override												{ if (other->GetType() != TYPE_ID) return false; const AttributeDoubleChannels* attribute = static_cast<const AttributeDoubleChannels*>(other); mValue = attribute->mValue; return true; }

		// not yet supported:
		bool InitFromString(const Core::String& valueString) override										{ return false; }
		bool ConvertToString(Core::String& outString) const override										{ return false; }
		void Write(const Core::Json& jsonParser, const Core::Json::Item& parentItem, const char* name) const override	{}
		void Read(const Core::Json& jsonParser, const Core::Json::Item& parentItem valueItem) override								{}

	private:
		DoubleChannelArray mValue;
};
#endif

#endif
