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

#ifndef __CORE_ATTRIBUTE_H
#define __CORE_ATTRIBUTE_H

// include the required headers
#include "StandardHeaders.h"
#include "String.h"
#include "Json.h"


namespace Core
{

class ENGINE_API Attribute
{
	public:
		// constructor & destructor
		Attribute()																						{}
		virtual ~Attribute()																			{}

		virtual Attribute* Clone() const = 0;
		virtual const char* GetTypeString() const = 0;
		virtual uint32 GetType() const = 0;
		virtual bool InitFromString(const Core::String& valueString) = 0;
		virtual bool ConvertToString(Core::String& outString) const = 0;
		virtual bool InitFrom(const Attribute* other) = 0;

		// json serialization
		virtual void Write(Json& json, Json::Item& item, const char* name) const = 0;
		virtual void Read(const Json& json, const Json::Item& item) = 0;

		Attribute& operator=(const Attribute& other)													{ if (&other != this) InitFrom( &other ); return *this; }
};

} // namespace Core

#endif
