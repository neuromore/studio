/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_PROPERTYMANAGER_H
#define __NEUROMORE_PROPERTYMANAGER_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include "Property.h"
#include "AttributeWidgets.h"



class QTBASE_API PropertyManager : public QObject
{
	Q_OBJECT

	public:
		// constructor & destructor
		PropertyManager(QObject* parent);
		virtual ~PropertyManager();

		void Clear();

		// integer type
		Property* AddIntProperty(const char* groupName, const char* valueName, int32 value, int32 defaultValue, int32 min, int32 max, bool readOnly=false);
		Property* AddReadOnlyIntProperty(const char* groupName, const char* valueName, int32 value)							{ return AddIntProperty( groupName, valueName, value, value, INT_MIN, INT_MAX, true ); }

		// combo box
		Property* AddComboBoxProperty(const char* groupName, const char* valueName, const Core::Array<Core::String>& comboValues, int32 defaultComboIndex, bool readOnly=false);

		//button
		Property* AddButtonProperty(const char* groupName, const char* valueName, const char* Value, bool readOnly = false);

		// float
		Property* AddFloatSpinnerProperty(const char* groupName, const char* valueName, float value, float defaultValue, float min, float max, bool readOnly=false);
		Property* AddReadOnlyFloatSpinnerProperty(const char* groupName, const char* valueName, float value)				{ return AddFloatSpinnerProperty( groupName, valueName, value, value, -FLT_MIN, FLT_MAX, true ); }

		// bool
		Property* AddBoolProperty(const char* groupName, const char* valueName, bool value, bool defaultValue, bool readOnly=false);
		Property* AddBoolProperty(const char* groupName, const char* valueName, bool value)									{ return AddBoolProperty( groupName, valueName, value, value, false ); }
		Property* AddReadOnlyBoolProperty(const char* groupName, const char* valueName, bool value)							{ return AddBoolProperty( groupName, valueName, value, value, true ); }

		// string
		Property* AddStringProperty(const char* groupName, const char* valueName, const char* value, const char* defaultValue=NULL, bool readOnly=false);
		Property* AddReadOnlyStringProperty(const char* groupName, const char* valueName, const char* value)				{ return AddStringProperty( groupName, valueName, value, value, true ); }

		// color
		Property* AddColorProperty(const char* groupName, const char* valueName, const Core::Color& value, const Core::Color& defaultValue, bool readOnly=false);
		Property* AddColorProperty(const char* groupName, const char* valueName, const Core::Color& value)				{ return AddColorProperty (groupName, valueName, value, value, false ); }
		Property* AddReadOnlyColorProperty(const char* groupName, const char* valueName, const Core::Color& value)		{ return AddColorProperty (groupName, valueName, value, value, true ); }

		bool IsEmpty() const								{ return mProperties.IsEmpty(); }

		// helpers
		Property* FindProperty(AttributeWidget* attributeWidget) const;

		// manual property adding
		Property* AddProperty(const char* groupName, const char* propertyName, AttributeWidget* attributeWidget, Core::Attribute* attributeValue, Core::AttributeSettings* settings, bool autoDelete=true, bool emitSignal=false);

	signals:
		void ValueChanged(Property* property);
		void PropertyAdded(const char* groupName, Property* property);

	private slots:
		void FireValueChangedSignal(Property* property)		{ emit ValueChanged(property); }
		void OnValueChanged();

	private:
		Property* AddProperty(const char* groupName, const char* propertyName, Core::Attribute* attributeValue, Core::AttributeSettings* settings, bool readOnly);

		Core::Array<Property*>			mProperties;
};


#endif
