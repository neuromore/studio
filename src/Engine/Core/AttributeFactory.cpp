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

// include required headers
#include "StandardHeaders.h"
#include "AttributeFactory.h"
#include "LogManager.h"

// include default types
#include "AttributeFloat.h"
#include "AttributeInt32.h"
#include "AttributeString.h"
#include "AttributeStringArray.h"
#include "AttributeBool.h"
#include "AttributeColor.h"

// DSP attributes from out of Core (not fully clean yet!)
#include "../DSP/AttributeChannels.h"


namespace Core
{

// constructor
AttributeFactory::AttributeFactory()
{
	mRegisteredAttributes.Reserve( 7 );

	mRegisteredAttributes.Add( AttributeFloat::Create() );
	mRegisteredAttributes.Add( AttributeInt32::Create() );
	mRegisteredAttributes.Add( AttributeString::Create() );
	mRegisteredAttributes.Add( AttributeBool::Create() );
	mRegisteredAttributes.Add( AttributeColor::Create() );

	// DSP attributes from out of Core (not fully clean yet!)
	mRegisteredAttributes.Add( AttributeChannels<double>::Create() );
	mRegisteredAttributes.Add( AttributeChannels<Spectrum>::Create() );
}


// destructor
AttributeFactory::~AttributeFactory() 
{
	for (uint32 i=0; i<mRegisteredAttributes.Size(); ++i)
		delete mRegisteredAttributes[i];

	mRegisteredAttributes.Clear();
}


// return new attribute based on the type id
Attribute* AttributeFactory::CreateByType(uint32 typeID) const
{
	for (uint32 i=0; i<mRegisteredAttributes.Size(); ++i)
		if (mRegisteredAttributes[i]->GetType() == typeID)
			return mRegisteredAttributes[i]->Clone();

	return NULL;
}


// parse string and create attribute based on it
Attribute* AttributeFactory::CreateByTypeString(const char* typeString) const
{
	for (uint32 i=0; i<mRegisteredAttributes.Size(); ++i)
		if (strcmp(mRegisteredAttributes[i]->GetTypeString(), typeString) == 0)
			return mRegisteredAttributes[i]->Clone();

	return NULL;
}

AttributeInt32* AttributeInt32::Create()											{ return new AttributeInt32(); }
AttributeInt32* AttributeInt32::Create(int32 value)									{ return new AttributeInt32(value); }
AttributeFloat* AttributeFloat::Create()											{ return new AttributeFloat(); }
AttributeFloat* AttributeFloat::Create(double value)								{ return new AttributeFloat(value); }

}
