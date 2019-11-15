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

#ifndef __CORE_ATTRIBUTEFACTORY_H
#define __CORE_ATTRIBUTEFACTORY_H

// include the required headers
#include "StandardHeaders.h"
#include "Array.h"
#include "Attribute.h"


namespace Core
{

class ENGINE_API AttributeFactory
{
	public:
		// constructor & destructor
		AttributeFactory();
		~AttributeFactory();

		// creator helpers
		Attribute* CreateByType(uint32 typeID) const;
		Attribute* CreateByTypeString(const char* typeString) const;
		Attribute* CreateByAttribute(const Attribute* attributeType) const			{ return CreateByType( attributeType->GetType() ); }

	private:
		Core::Array<Attribute*>	mRegisteredAttributes;
};

} // namespace Core

#endif
