/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "PropertyManager.h"
#include "AttributeWidgetFactory.h"
#include "../QtBaseManager.h"


// constructor
PropertyManager::PropertyManager(QObject* parent) : QObject(parent)
{
}


// destructor
PropertyManager::~PropertyManager()
{
	Clear();
}


// clear the contents
void PropertyManager::Clear()
{
	// get the number of properties and iterate through them
	const uint32 numProperties = mProperties.Size();
	for (uint32 i=0; i<numProperties; ++i)
		delete mProperties[i];
	mProperties.Clear();
}


// add a new property
Property* PropertyManager::AddProperty(const char* groupName, const char* propertyName, AttributeWidget* attributeWidget, Core::Attribute* attributeValue, Core::AttributeSettings* settings, bool autoDelete, bool emitSignal)
{
	// create a new property
	Property* property = new Property( propertyName, attributeWidget, attributeValue, settings, autoDelete );
	connect( attributeWidget, SIGNAL(ValueChanged()), this, SLOT(OnValueChanged()) );
	mProperties.Add( property );

	if (emitSignal == true)
		emit PropertyAdded(groupName, property);

	return property;
}


// add a new property
Property* PropertyManager::AddProperty(const char* groupName, const char* propertyName, Core::Attribute* attributeValue, Core::AttributeSettings* settings, bool readOnly)
{
	// create the attribute widget
	AttributeWidget* attributeWidget = GetQtBaseManager()->GetAttributeWidgetFactory()->CreateAttributeWidget( attributeValue, settings, NULL, readOnly );

	return AddProperty( groupName, propertyName, attributeWidget, attributeValue, settings, true, true );
}



// find the property based on the given attribute widget
Property* PropertyManager::FindProperty(AttributeWidget* attributeWidget) const
{
	// get the number of properties and iterate through them
	const uint32 numProperties = mProperties.Size();
	for (uint32 i=0; i<numProperties; ++i)
	{
		// get access to the current property and compare it against the given one, if they are the same return directly
		Property* property = mProperties[i];
		if (property->GetAttributeWidget() == attributeWidget)
			return property;
	}

	// attribute widget not found, return failure
	return NULL;
}


// called when a value of a attribute widget got changed
void PropertyManager::OnValueChanged()
{
	// get the attribute widget that fired the value change signal
	AttributeWidget* attributeWidget = static_cast<AttributeWidget*>( sender() );

	// try to find the property based on the attribute widget
	Property* property = FindProperty(attributeWidget);

	// if the property is valid, fire the value changed signal
	if (property != NULL)
		FireValueChangedSignal(property);
}


// create an integer property
Property* PropertyManager::AddIntProperty(const char* groupName, const char* valueName, int32 value, int32 defaultValue, int32 min, int32 max, bool readOnly)
{
	Core::AttributeInt32*		attributeValue		= Core::AttributeInt32::Create();
	Core::AttributeSettings*	attributeSettings	= new Core::AttributeSettings();

	attributeValue->SetValue( value );
	attributeSettings->SetInternalName( valueName );
	attributeSettings->SetDefaultValue( Core::AttributeInt32::Create(defaultValue) );
	attributeSettings->SetMinValue( Core::AttributeInt32::Create(min) );
	attributeSettings->SetMaxValue( Core::AttributeInt32::Create(max) );
	attributeSettings->SetInterfaceType( Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER );

	// create and return the property
	return AddProperty(groupName, valueName, attributeValue, attributeSettings, readOnly );
}


// create a combobox property
Property* PropertyManager::AddComboBoxProperty(const char* groupName, const char* valueName, const Core::Array<Core::String>& comboValues, int32 defaultComboIndex, bool readOnly)
{
	Core::AttributeInt32*		attributeValue		= Core::AttributeInt32::Create();
	Core::AttributeSettings*	attributeSettings	= new Core::AttributeSettings();

	attributeSettings->SetInternalName( valueName );
	attributeSettings->SetInterfaceType( Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX );
	
	const uint32 numComboValues = comboValues.Size();
	attributeSettings->ResizeComboValues( (uint32)numComboValues );
	for (uint32 i=0; i<numComboValues; ++i)
		attributeSettings->SetComboValue( i, comboValues[i].AsChar() );
	attributeValue->SetValue( defaultComboIndex );
	attributeSettings->SetDefaultValue( Core::AttributeInt32::Create(defaultComboIndex) );
	attributeSettings->SetMinValue( Core::AttributeInt32::Create(defaultComboIndex) );
	attributeSettings->SetMaxValue( Core::AttributeInt32::Create(numComboValues-1) );

	// create and return the property
	return AddProperty(groupName, valueName, attributeValue, attributeSettings, readOnly );
}


// create a float property
Property* PropertyManager::AddFloatSpinnerProperty(const char* groupName, const char* valueName, float value, float defaultValue, float min, float max, bool readOnly)
{
	Core::AttributeFloat*		attributeValue		= Core::AttributeFloat::Create();
	Core::AttributeSettings*	attributeSettings	= new Core::AttributeSettings();

	attributeValue->SetValue( value );
	attributeSettings->SetInternalName( valueName );
	attributeSettings->SetDefaultValue( Core::AttributeFloat::Create(defaultValue) );
	attributeSettings->SetMinValue( Core::AttributeFloat::Create(min) );
	attributeSettings->SetMaxValue( Core::AttributeFloat::Create(max) );
	attributeSettings->SetInterfaceType( Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER );

	// create and return the property
	return AddProperty(groupName, valueName, attributeValue, attributeSettings, readOnly );
}


// create a string property
Property* PropertyManager::AddStringProperty(const char* groupName, const char* valueName, const char* value, const char* defaultValue, bool readOnly)
{
	Core::AttributeString*		attributeValue		= Core::AttributeString::Create();
	Core::AttributeSettings*	attributeSettings	= new Core::AttributeSettings();

	attributeValue->SetValue( value );
	attributeSettings->SetInternalName( valueName );
	attributeSettings->SetInterfaceType( Core::ATTRIBUTE_INTERFACETYPE_STRING );

	// in case no default value got specified, just use the value as default
	if (defaultValue == NULL)
		attributeSettings->SetDefaultValue( Core::AttributeString::Create(value) );
	else
		attributeSettings->SetDefaultValue( Core::AttributeString::Create(defaultValue) );

	// create and return the property
	return AddProperty(groupName, valueName, attributeValue, attributeSettings, readOnly);
}


// create a bool property
Property* PropertyManager::AddBoolProperty(const char* groupName, const char* valueName, bool value, bool defaultValue, bool readOnly)
{
	Core::AttributeBool*		attributeValue		= Core::AttributeBool::Create();
	Core::AttributeSettings*	attributeSettings	= new Core::AttributeSettings();

	attributeValue->SetValue( value );
	attributeSettings->SetInternalName( valueName );
	attributeSettings->SetInterfaceType( Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX );

	// in case no default value got specified, just use the value as default
    attributeSettings->SetDefaultValue( Core::AttributeBool::Create(defaultValue) );

	// create and return the property
	return AddProperty(groupName, valueName, attributeValue, attributeSettings, readOnly );
}


// create a color property
Property* PropertyManager::AddColorProperty(const char* groupName, const char* valueName, const Core::Color& value, const Core::Color& defaultValue, bool readOnly)
{
	Core::AttributeColor*		attributeValue		= Core::AttributeColor::Create();
	Core::AttributeSettings*	attributeSettings	= new Core::AttributeSettings();

	attributeValue->SetValue( value );
	attributeSettings->SetInternalName( valueName );
	attributeSettings->SetInterfaceType( Core::ATTRIBUTE_INTERFACETYPE_COLOR );

	// in case no default value got specified, just use the value as default
    attributeSettings->SetDefaultValue( Core::AttributeColor::Create(defaultValue) );

	// create and return the property
	return AddProperty(groupName, valueName, attributeValue, attributeSettings, readOnly );
}
