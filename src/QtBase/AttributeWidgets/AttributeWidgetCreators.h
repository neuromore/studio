/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_ATTRIBUTEWIDGETCREATORS_H
#define __NEUROMORE_ATTRIBUTEWIDGETCREATORS_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include "AttributeWidgets.h"



class QTBASE_API AttributeWidgetCreator
{
	public:
		AttributeWidgetCreator();
		virtual ~AttributeWidgetCreator();

		virtual uint32 GetType() const = 0;
		virtual uint32 GetDefaultType() const = 0;
		virtual uint32 GetMinMaxType() const = 0;
		virtual uint32 GetDataType() const = 0;
		virtual bool CanBeParameter() const = 0;
		virtual const char* GetTypeString() const = 0;
		virtual bool HasMinMaxValues() const = 0;
		virtual void InitAttributes(Core::Array<Core::Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool forceInitMinMaxAttributes=false, bool resetMinMaxAttributes=true) = 0;
		
		virtual AttributeWidget* Clone(Core::Array<Core::Attribute*>& attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode=false) = 0;

		void CreateAttributes(Core::Array<Core::Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool initMinMax);
		Core::Attribute* GetInitialMinValue() const	{ return mInitialMinValue; }
		Core::Attribute* GetInitialMaxValue() const	{ return mInitialMaxValue; }

	protected:
		Core::Attribute*	mInitialMinValue;
		Core::Attribute*	mInitialMaxValue;
};


// easy creator class definition
#define DEFINE_ATTRIBUTEWIDGETCREATOR( CLASSNAME, ATTRIBUTEWIDGETCLASSNAME, TYPEID, DEFAULTYPEID, MINMAXTYPEID, DATATYPEID, HASMINMAXVALUES, CANBEPARAMETER, TYPESTRING ) \
	class CLASSNAME : public AttributeWidgetCreator \
	{ \
		public: \
			enum { TYPE_ID = TYPEID }; \
			CLASSNAME() : AttributeWidgetCreator()		{ mInitialMinValue=NULL; mInitialMaxValue=NULL; } \
			~CLASSNAME() {} \
			uint32 GetType() const						{ return TYPEID; }; \
			uint32 GetDefaultType() const				{ return DEFAULTYPEID; }; \
			uint32 GetMinMaxType() const				{ return MINMAXTYPEID; }; \
			uint32 GetDataType() const					{ return DATATYPEID; }; \
			bool CanBeParameter() const					{ return CANBEPARAMETER; } \
			const char* GetTypeString() const			{ return TYPESTRING; }; \
			bool HasMinMaxValues() const				{ return HASMINMAXVALUES; } \
			void InitAttributes(Core::Array<Core::Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool forceInitMinMaxAttributes=false, bool resetMinMaxAttributes=true); \
			AttributeWidget* Clone(Core::Array<Core::Attribute*>& attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode) \
			{ \
				return new ATTRIBUTEWIDGETCLASSNAME(attributes, attributeSettings, customData, readOnly, creationMode); \
			} \
	};

#define DEFINE_NAMEDATTRIBUTEWIDGETCREATOR( CLASSNAME, ATTRIBUTEWIDGETCLASSNAME, TYPEID, DEFAULTYPEID, MINMAXTYPEID, DATATYPEID, HASMINMAXVALUES, CANBEPARAMETER, TYPESTRING ) \
	class CLASSNAME : public AttributeWidgetCreator \
	{ \
		public: \
			enum { TYPE_ID = TYPEID }; \
			CLASSNAME() : AttributeWidgetCreator()		{ mInitialMinValue=NULL; mInitialMaxValue=NULL; } \
			~CLASSNAME() {} \
			uint32 GetType() const						{ return TYPEID; }; \
			uint32 GetDefaultType() const				{ return DEFAULTYPEID; }; \
			uint32 GetMinMaxType() const				{ return MINMAXTYPEID; }; \
			uint32 GetDataType() const					{ return DATATYPEID; }; \
			bool CanBeParameter() const					{ return CANBEPARAMETER; } \
			const char* GetTypeString() const			{ return TYPESTRING; }; \
			bool HasMinMaxValues() const				{ return HASMINMAXVALUES; } \
			void InitAttributes(Core::Array<Core::Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool forceInitMinMaxAttributes=false, bool resetMinMaxAttributes=true); \
			AttributeWidget* Clone(Core::Array<Core::Attribute*>& attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode) \
			{ \
				return new ATTRIBUTEWIDGETCLASSNAME(attributeSettings->GetName(), attributes, attributeSettings, customData, readOnly, creationMode); \
			} \
	};



DEFINE_ATTRIBUTEWIDGETCREATOR( CheckboxAttributeWidgetCreator,			CheckBoxAttributeWidget,		Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX,			Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX,			Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX,			Core::AttributeBool::TYPE_ID,			false,	true,	"Checkbox" );
DEFINE_ATTRIBUTEWIDGETCREATOR( FloatSpinnerAttributeWidgetCreator,		FloatSpinnerAttributeWidget,	Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER,		Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER,		Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER,		Core::AttributeFloat::TYPE_ID,			true, 	true,	"FloatSpinner" );
DEFINE_ATTRIBUTEWIDGETCREATOR( FloatSliderAttributeWidgetCreator,		FloatSliderAttributeWidget,		Core::ATTRIBUTE_INTERFACETYPE_FLOATSLIDER,		Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER,		Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER,		Core::AttributeFloat::TYPE_ID,			true, 	true,	"FloatSlider" );
DEFINE_ATTRIBUTEWIDGETCREATOR( FloatSliderLabelAttributeWidgetCreator,	FloatSliderLabelAttributeWidget,Core::ATTRIBUTE_INTERFACETYPE_FLOATSLIDERLABEL,	Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER,		Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER,		Core::AttributeFloat::TYPE_ID,			true, 	true,	"FloatSliderLabel" );
DEFINE_ATTRIBUTEWIDGETCREATOR( IntSpinnerAttributeWidgetCreator,		IntSpinnerAttributeWidget,		Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER,		Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER,		Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER,		Core::AttributeInt32::TYPE_ID,			true, 	true,	"IntSpinner" );
DEFINE_ATTRIBUTEWIDGETCREATOR( IntSliderAttributeWidgetCreator,			IntSliderAttributeWidget,		Core::ATTRIBUTE_INTERFACETYPE_INTSLIDER,		Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER,		Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER,		Core::AttributeInt32::TYPE_ID,			true, 	true,	"IntSlider" );
DEFINE_ATTRIBUTEWIDGETCREATOR( ComboBoxAttributeWidgetCreator,			ComboBoxAttributeWidget,		Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX,			Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER,		Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX,			Core::AttributeInt32::TYPE_ID,			false, 	false,	"ComboBox" );
DEFINE_ATTRIBUTEWIDGETCREATOR( StringAttributeWidgetCreator,			StringAttributeWidget,			Core::ATTRIBUTE_INTERFACETYPE_STRING,			Core::ATTRIBUTE_INTERFACETYPE_STRING,			Core::ATTRIBUTE_INTERFACETYPE_STRING,			Core::AttributeString::TYPE_ID,			false, 	true,	"String" );
DEFINE_ATTRIBUTEWIDGETCREATOR( StringArrayAttributeWidgetCreator,		StringArrayAttributeWidget,		Core::ATTRIBUTE_INTERFACETYPE_STRINGARRAY,		Core::ATTRIBUTE_INTERFACETYPE_STRINGARRAY,		Core::ATTRIBUTE_INTERFACETYPE_STRINGARRAY,		Core::AttributeStringArray::TYPE_ID,	false, 	true,	"StringArray" );
DEFINE_ATTRIBUTEWIDGETCREATOR( ColorAttributeWidgetCreator,				ColorAttributeWidget,			Core::ATTRIBUTE_INTERFACETYPE_COLOR,			Core::ATTRIBUTE_INTERFACETYPE_COLOR,			Core::ATTRIBUTE_INTERFACETYPE_COLOR,			Core::AttributeColor::TYPE_ID,			true, 	true,	"Color" );
DEFINE_ATTRIBUTEWIDGETCREATOR( ColorMappingAttributeWidgetCreator,		ColorMappingAttributeWidget,	Core::ATTRIBUTE_INTERFACETYPE_COLORMAPPING,		Core::ATTRIBUTE_INTERFACETYPE_COLORMAPPING,		Core::ATTRIBUTE_INTERFACETYPE_COLORMAPPING,		Core::AttributeInt32::TYPE_ID,			false, 	false,	"ColorMapping" );
DEFINE_ATTRIBUTEWIDGETCREATOR( WindowFunctionAttributeWidgetCreator,	WindowFunctionAttributeWidget,	Core::ATTRIBUTE_INTERFACETYPE_WINDOWFUNCTION,	Core::ATTRIBUTE_INTERFACETYPE_WINDOWFUNCTION,	Core::ATTRIBUTE_INTERFACETYPE_WINDOWFUNCTION,	Core::AttributeInt32::TYPE_ID,			false, 	false,	"WindowFunction" );
DEFINE_NAMEDATTRIBUTEWIDGETCREATOR(ButtonAttributeWidgetCreator, ButtonAttributeWidget, Core::ATTRIBUTE_INTERFACETYPE_BUTTON, Core::ATTRIBUTE_INTERFACETYPE_BUTTON, Core::ATTRIBUTE_INTERFACETYPE_BUTTON, Core::AttributeBool::TYPE_ID, false, false, "Button");

#endif
