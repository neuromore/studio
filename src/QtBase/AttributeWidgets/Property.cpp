/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include the required headers
#include "Property.h"
#include "AttributeWidgetFactory.h"
#include "../QtBaseManager.h"


using namespace Core;

// constructor
Property::Property(const char* name, AttributeWidget* attributeWidget, Attribute* attributeValue, AttributeSettings* settings, bool autoDelete)
{
	mAttributeValue		= attributeValue;
	mAttributeWidget	= attributeWidget;
	mAttributeSettings	= settings;
	mName				= name;
	mAutoDelete			= autoDelete;
}


// destructor
Property::~Property()
{
	if (mAutoDelete == true)
	{
		delete mAttributeValue;
		delete mAttributeSettings;
	}
}


// get the name
const char* Property::GetName() const
{ 
	return mName.AsChar();
}


// get the name as string object
const Core::String& Property::GetNameString() const
{
	return mName;
}