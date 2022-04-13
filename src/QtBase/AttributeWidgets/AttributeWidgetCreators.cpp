/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include <EngineManager.h>
#include "AttributeWidgetCreators.h"
#include <Core/AttributeFactory.h>



using namespace Core;

// constructor
AttributeWidgetCreator::AttributeWidgetCreator()
{
	mInitialMinValue = NULL;
	mInitialMaxValue = NULL;
}


// destructor
AttributeWidgetCreator::~AttributeWidgetCreator()
{
	delete mInitialMinValue;
	delete mInitialMaxValue;
}


// create the attributes
void AttributeWidgetCreator::CreateAttributes(Core::Array<Core::Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool initMinMax)
{
	// get the attribute type
	const uint32 dataType = GetDataType();	// for example an AttributeFloat::TYPE_ID or something else

	// for all attributes
	const uint32 numAttributes = attributes.Size();
	for (uint32 i=0; i<numAttributes; ++i)
	{
		// if the attribute is not allocated yet or if the type changed, then recreate it
		if (attributes[i] == NULL || attributes[i]->GetType() != dataType)
		{
			delete attributes[i];
			attributes[i] = CORE_ATTRIBUTEFACTORY.CreateByType( dataType );
		}
	}

	// create the min and max values, which are the interface widget min and max values (for example a double spincontrol min and max range)
	if (initMinMax == true)
	{
		// create the minimum value
		if (attributeSettings->GetMinValue() == NULL || attributeSettings->GetMinValue()->GetType() != dataType)
		{
			delete attributeSettings->GetMinValue();
			attributeSettings->SetMinValue( CORE_ATTRIBUTEFACTORY.CreateByType(dataType) );
		}

		// create the maximum value
		if (attributeSettings->GetMaxValue() == NULL || attributeSettings->GetMaxValue()->GetType() != GetDataType())
		{
			delete attributeSettings->GetMaxValue();
			attributeSettings->SetMaxValue( CORE_ATTRIBUTEFACTORY.CreateByType(dataType) );
		}

		// create the initial values for min and max
		// note: this is not the widget range, but the actual initial value
		if (mInitialMinValue == NULL) mInitialMinValue = CORE_ATTRIBUTEFACTORY.CreateByType( dataType );
		if (mInitialMaxValue == NULL) mInitialMaxValue = CORE_ATTRIBUTEFACTORY.CreateByType( dataType );
	}
}


// initialize float attribs
void FloatSpinnerAttributeWidgetCreator::InitAttributes(Array<Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool forceInitMinMaxAttributes, bool resetMinMaxAttributes)
{
	CreateAttributes( attributes, attributeSettings, forceInitMinMaxAttributes );
	if (forceInitMinMaxAttributes == true)
	{
		static_cast<Core::AttributeFloat*>(attributeSettings->GetMinValue())->SetValue( -FLT_MAX );
		static_cast<Core::AttributeFloat*>(attributeSettings->GetMaxValue())->SetValue(  FLT_MAX );

		if (resetMinMaxAttributes == true)
		{
			static_cast<Core::AttributeFloat*>(mInitialMinValue)->SetValue( 0.0f );
			static_cast<Core::AttributeFloat*>(mInitialMaxValue)->SetValue( 1.0f );
		}
	}
}


// initialize checkbox attribs
void CheckboxAttributeWidgetCreator::InitAttributes(Array<Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool forceInitMinMaxAttributes, bool resetMinMaxAttributes)
{
	CreateAttributes( attributes, attributeSettings, forceInitMinMaxAttributes );
}


// init integer attributes
void IntSpinnerAttributeWidgetCreator::InitAttributes(Array<Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool forceInitMinMaxAttributes, bool resetMinMaxAttributes)
{
	CreateAttributes( attributes, attributeSettings, forceInitMinMaxAttributes );
	if (forceInitMinMaxAttributes == true)
	{
		static_cast<Core::AttributeInt32*>(attributeSettings->GetMinValue())->SetValue( -INT_MAX );
		static_cast<Core::AttributeInt32*>(attributeSettings->GetMaxValue())->SetValue(  INT_MAX );

		if (resetMinMaxAttributes == true)
		{
			static_cast<Core::AttributeInt32*>(mInitialMinValue)->SetValue( 0 );
			static_cast<Core::AttributeInt32*>(mInitialMaxValue)->SetValue( 100 );
		}
	}
}


// init float attributes
void FloatSliderAttributeWidgetCreator::InitAttributes(Array<Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool forceInitMinMaxAttributes, bool resetMinMaxAttributes)
{
	CreateAttributes( attributes, attributeSettings, forceInitMinMaxAttributes );
	if (forceInitMinMaxAttributes == true)
	{
		static_cast<Core::AttributeFloat*>(attributeSettings->GetMinValue())->SetValue( -FLT_MAX );
		static_cast<Core::AttributeFloat*>(attributeSettings->GetMaxValue())->SetValue(  FLT_MAX );

		if (resetMinMaxAttributes == true)
		{
			static_cast<Core::AttributeFloat*>(mInitialMinValue)->SetValue( 0.0f );
			static_cast<Core::AttributeFloat*>(mInitialMaxValue)->SetValue( 1.0f );
		}
	}
}


// init float attributes
void FloatSliderLabelAttributeWidgetCreator::InitAttributes(Array<Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool forceInitMinMaxAttributes, bool resetMinMaxAttributes)
{
	CreateAttributes( attributes, attributeSettings, forceInitMinMaxAttributes );
	if (forceInitMinMaxAttributes == true)
	{
		static_cast<Core::AttributeFloat*>(attributeSettings->GetMinValue())->SetValue( -FLT_MAX );
		static_cast<Core::AttributeFloat*>(attributeSettings->GetMaxValue())->SetValue(  FLT_MAX );

		if (resetMinMaxAttributes == true)
		{
			static_cast<Core::AttributeFloat*>(mInitialMinValue)->SetValue( 0.0f );
			static_cast<Core::AttributeFloat*>(mInitialMaxValue)->SetValue( 1.0f );
		}
	}
}


// init int attributes
void IntSliderAttributeWidgetCreator::InitAttributes(Array<Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool forceInitMinMaxAttributes, bool resetMinMaxAttributes)
{
	CreateAttributes( attributes, attributeSettings, forceInitMinMaxAttributes );
	if (forceInitMinMaxAttributes == true)
	{
		static_cast<Core::AttributeInt32*>(attributeSettings->GetMinValue())->SetValue( -INT_MAX );
		static_cast<Core::AttributeInt32*>(attributeSettings->GetMaxValue())->SetValue(  INT_MAX );

		if (resetMinMaxAttributes == true)
		{
			static_cast<Core::AttributeInt32*>(mInitialMinValue)->SetValue( 0 );
			static_cast<Core::AttributeInt32*>(mInitialMaxValue)->SetValue( 100 );
		}
	}
}


// init int attribs
void ComboBoxAttributeWidgetCreator::InitAttributes(Array<Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool forceInitMinMaxAttributes, bool resetMinMaxAttributes)
{
	CreateAttributes( attributes, attributeSettings, forceInitMinMaxAttributes );
	if (forceInitMinMaxAttributes == true)
	{
		static_cast<Core::AttributeInt32*>(attributeSettings->GetMinValue())->SetValue( -INT_MAX );
		static_cast<Core::AttributeInt32*>(attributeSettings->GetMaxValue())->SetValue(  INT_MAX );
	}
}


// string attribute widget
void StringAttributeWidgetCreator::InitAttributes(Array<Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool forceInitMinMaxAttributes, bool resetMinMaxAttributes)
{
	CreateAttributes( attributes, attributeSettings, forceInitMinMaxAttributes );
}


// string array attribute widget
void StringArrayAttributeWidgetCreator::InitAttributes(Array<Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool forceInitMinMaxAttributes, bool resetMinMaxAttributes)
{
	CreateAttributes( attributes, attributeSettings, forceInitMinMaxAttributes );
}


// init color attribs
void ColorAttributeWidgetCreator::InitAttributes(Array<Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool forceInitMinMaxAttributes, bool resetMinMaxAttributes)
{
	CreateAttributes( attributes, attributeSettings, forceInitMinMaxAttributes );
	if (forceInitMinMaxAttributes == true)
	{
		static_cast<Core::AttributeColor*>(attributeSettings->GetMinValue())->SetValue( Core::Color( 0.0f, 0.0f, 0.0f, 0.0f ) );
		static_cast<Core::AttributeColor*>(attributeSettings->GetMaxValue())->SetValue( Core::Color( 1.0f, 1.0f, 1.0f, 1.0f ) );
	}
}


// init int attribs
void ColorMappingAttributeWidgetCreator::InitAttributes(Array<Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool forceInitMinMaxAttributes, bool resetMinMaxAttributes)
{
	CreateAttributes( attributes, attributeSettings, forceInitMinMaxAttributes );
	if (forceInitMinMaxAttributes == true)
	{
		static_cast<Core::AttributeInt32*>(attributeSettings->GetMinValue())->SetValue( 0 );
		static_cast<Core::AttributeInt32*>(attributeSettings->GetMaxValue())->SetValue( INT_MAX );
	}
}


// init int attribs
void WindowFunctionAttributeWidgetCreator::InitAttributes(Array<Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool forceInitMinMaxAttributes, bool resetMinMaxAttributes)
{
	CreateAttributes( attributes, attributeSettings, forceInitMinMaxAttributes );
	if (forceInitMinMaxAttributes == true)
	{
		static_cast<Core::AttributeInt32*>(attributeSettings->GetMinValue())->SetValue( 0 );
		static_cast<Core::AttributeInt32*>(attributeSettings->GetMaxValue())->SetValue( INT_MAX );
	}
}


void ButtonAttributeWidgetCreator::InitAttributes(Array<Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool forceInitMinMaxAttributes, bool resetMinMaxAttributes)
{
	CreateAttributes(attributes, attributeSettings, forceInitMinMaxAttributes);	
}