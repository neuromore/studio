/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "AttributeWidgetFactory.h"
#include <Core/LogManager.h>
#include "AttributeWidgetCreators.h"


using namespace Core;

// constructor
AttributeWidgetFactory::AttributeWidgetFactory()
{
	LogDetailedInfo("Constructing attribute widget factory ...");

	mRegisteredCreators.Reserve( 12 );
	RegisterCreator( new CheckboxAttributeWidgetCreator() );
	RegisterCreator( new FloatSpinnerAttributeWidgetCreator() );
	RegisterCreator( new IntSpinnerAttributeWidgetCreator() );
	RegisterCreator( new FloatSliderAttributeWidgetCreator() );
	RegisterCreator( new FloatSliderLabelAttributeWidgetCreator() );
	RegisterCreator( new IntSliderAttributeWidgetCreator() );
	RegisterCreator( new ComboBoxAttributeWidgetCreator() );
	RegisterCreator( new StringAttributeWidgetCreator() );
	RegisterCreator( new StringArrayAttributeWidgetCreator() );
	RegisterCreator( new ColorAttributeWidgetCreator() );
	RegisterCreator( new ColorMappingAttributeWidgetCreator() );
	RegisterCreator( new WindowFunctionAttributeWidgetCreator() );
}


// destructor
AttributeWidgetFactory::~AttributeWidgetFactory()
{
	LogDetailedInfo("Destructing attribute widget factory ...");

	const uint32 numCreators = mRegisteredCreators.Size();
	for (uint32 i=0; i<numCreators; ++i)
		delete mRegisteredCreators[i];

	// clear the array
	mRegisteredCreators.Clear();

	ClearCallbacks();
}


// create a given attribute widget creator
AttributeWidget* AttributeWidgetFactory::CreateAttributeWidget(Core::Array<Core::Attribute*>& attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool forceInitMinMaxAttributes, bool resetMinMaxAttributes, AttributeWidgetType attributeWidgetType, bool creationMode)
{
	// try to find the registered creator index
	const uint32 index = FindRegisteredCreatorByTypeID( attributeSettings->GetInterfaceType(), attributeWidgetType );
	if (index == CORE_INVALIDINDEX32)
	{
		Core::LogWarning("AttributeWidgetFactory::CreateAttributeWidgetByTypeID() - failed to create attribute widget creator with type ID %d, as no such node type has been registered", attributeSettings->GetInterfaceType());
		return NULL;
	}

	// init the attributes
	mRegisteredCreators[index]->InitAttributes(attributes, attributeSettings, forceInitMinMaxAttributes, resetMinMaxAttributes);

	// init the min values
	if (attributeWidgetType == ATTRIBUTE_MIN && resetMinMaxAttributes == true)
	{
		const uint32 numAttributes = attributes.Size();
		for (uint32 i=0; i<numAttributes; ++i)
			if (attributes[i] != NULL)
				attributes[i]->InitFrom( mRegisteredCreators[index]->GetInitialMinValue() );
	}

	// init the max values
	if (attributeWidgetType == ATTRIBUTE_MAX && resetMinMaxAttributes == true)
	{
		const uint32 numAttributes = attributes.Size();
		for (uint32 i=0; i<numAttributes; ++i)
			if (attributes[i] != NULL)
				attributes[i]->InitFrom( mRegisteredCreators[index]->GetInitialMaxValue() );
	}

	// create a clone of the registered attribute widget creator
	AttributeWidget* result = mRegisteredCreators[index]->Clone( attributes, attributeSettings, customData, readOnly, creationMode );
	return result;
}


AttributeWidget* AttributeWidgetFactory::CreateAttributeWidget(Core::Attribute* attribute, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool forceInitMinMaxAttributes, bool resetMinMaxAttributes, AttributeWidgetType attributeWidgetType, bool creationMode)
{
	Core::Array<Core::Attribute*> attributes;
	attributes.Add(attribute);

	return CreateAttributeWidget( attributes, attributeSettings, customData, readOnly, forceInitMinMaxAttributes, resetMinMaxAttributes, attributeWidgetType, creationMode );
}


void AttributeWidgetFactory::InitAttributes(Core::Array<Core::Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool forceInitMinMaxAttributes, AttributeWidgetType attributeWidgetType)
{
	// try to find the registered creator index
	const uint32 index = FindRegisteredCreatorByTypeID( attributeSettings->GetInterfaceType(), attributeWidgetType );
	if (index == CORE_INVALIDINDEX32)
		Core::LogWarning("AttributeWidgetFactory::InitAttributes() - failed to init attribute widget creator with type ID %d, as no such node type has been registered", attributeSettings->GetInterfaceType());

	mRegisteredCreators[index]->InitAttributes(attributes, attributeSettings, forceInitMinMaxAttributes);
}


// register a given creator
bool AttributeWidgetFactory::RegisterCreator(AttributeWidgetCreator* creator)
{
	// make sure we didn't already register this node
	if (HasRegisteredCreatorByTypeID( creator->GetType() ) == true)
	{
		Core::LogWarning("AttributeWidgetFactory::RegisterCreator() - Already registered the given creator, skipping registration (type=%s)", creator->GetTypeString());
		CORE_ASSERT(false);
		return false;
	}

	// register it
	mRegisteredCreators.Add( creator );
	return true;
}


// check if we have a registered creator of the given type
bool AttributeWidgetFactory::HasRegisteredCreatorByTypeID(uint32 typeID) const
{
	return (FindRegisteredCreatorByTypeID(typeID) != CORE_INVALIDINDEX32);
}


// find registered creator by its type ID
uint32 AttributeWidgetFactory::FindRegisteredCreatorByTypeID(uint32 typeID, AttributeWidgetType attributeWidgetType) const
{
	uint32 result = CORE_INVALIDINDEX32;

	// for all registered creators
	const uint32 numCreators = mRegisteredCreators.Size();
	for (uint32 i=0; i<numCreators; ++i)
	{
		if (mRegisteredCreators[i]->GetType() == typeID)
		{
			result = i;
			break;
		}
	}

	switch (attributeWidgetType)
	{
		case ATTRIBUTE_NORMAL:	{ return result; break; }
		case ATTRIBUTE_DEFAULT:	{ return FindRegisteredCreatorByTypeID( mRegisteredCreators[result]->GetDefaultType(), ATTRIBUTE_NORMAL ); break; }
		case ATTRIBUTE_MIN:		{ return FindRegisteredCreatorByTypeID( mRegisteredCreators[result]->GetMinMaxType(), ATTRIBUTE_NORMAL ); break; }
		case ATTRIBUTE_MAX:		{ return FindRegisteredCreatorByTypeID( mRegisteredCreators[result]->GetMinMaxType(), ATTRIBUTE_NORMAL ); break; }
	};

	return CORE_INVALIDINDEX32;
}


// get rid of all callbacks
void AttributeWidgetFactory::ClearCallbacks()
{
	// get the number of callbacks, iterate through and remove them
	const uint32 numCallbacks = mCallbacks.Size();
	for (uint32 i=0; i<numCallbacks; ++i)
		delete mCallbacks[i];

	mCallbacks.Clear();
}


// remove the given callback
void AttributeWidgetFactory::RemoveCallback(Callback* callback, bool delFromMem)
{
	// get the number of callbacks, iterate through and call the callback
	const uint32 numCallbacks = mCallbacks.Size();
	for (uint32 i=0; i<numCallbacks; ++i)
	{
		// check if the current callback is the given one
		if (mCallbacks[i] == callback)
		{
			if (delFromMem == true)
				delete mCallbacks[i];

			mCallbacks.Remove(i);
			return;
		}
	}
}


// inform all callbacks about that an attribute changed
void AttributeWidgetFactory::OnAttributeChanged(Core::Attribute* attribute)
{
	// get the number of callbacks, iterate through and call the callback
	const uint32 numCallbacks = mCallbacks.Size();
	for (uint32 i=0; i<numCallbacks; ++i)
		mCallbacks[i]->OnAttributeChanged(attribute);
}
