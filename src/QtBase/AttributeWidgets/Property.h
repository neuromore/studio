/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_PROPERTY_H
#define __NEUROMORE_PROPERTY_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include <Core/AttributeBool.h>
#include <Core/AttributeInt32.h>
#include <Core/AttributeFloat.h>
#include <Core/AttributeString.h>
#include <Core/AttributeColor.h>
#include <Core/AttributeSettings.h>
#include <Core/AttributeSet.h>
#include "AttributeWidgets.h"


class QTBASE_API Property
{
	public:
		Property(const char* name, AttributeWidget* attributeWidget, Core::Attribute* attributeValue, Core::AttributeSettings* settings, bool autoDelete=true);
		virtual ~Property();

		inline AttributeWidget* GetAttributeWidget() const						{ return mAttributeWidget; }
		inline Core::Attribute* GetAttributeValue() const						{ return mAttributeValue; }
		inline Core::AttributeSettings* GetAttributeSettings() const			{ return mAttributeSettings; }

		const char* GetName() const;
		const Core::String& GetNameString() const;

		void SetValue(bool value)												{ if (mAttributeValue->GetType() == Core::AttributeBool::TYPE_ID) static_cast<Core::AttributeBool*>(mAttributeValue)->SetValue(value); }
		void SetValue(int32 value)												{ if (mAttributeValue->GetType() == Core::AttributeInt32::TYPE_ID) static_cast<Core::AttributeInt32*>(mAttributeValue)->SetValue(value); }
		void SetValue(float value)												{ if (mAttributeValue->GetType() == Core::AttributeFloat::TYPE_ID) static_cast<Core::AttributeFloat*>(mAttributeValue)->SetValue(value); }
		void SetValue(Core::String value)										{ if (mAttributeValue->GetType() == Core::AttributeString::TYPE_ID) static_cast<Core::AttributeString*>(mAttributeValue)->SetValue(value); }
		void SetValue(Core::Color value)										{ if (mAttributeValue->GetType() == Core::AttributeColor::TYPE_ID) static_cast<Core::AttributeColor*>(mAttributeValue)->SetValue(value); }

		bool AsBool() const														{ if (mAttributeValue->GetType() == Core::AttributeBool::TYPE_ID) return static_cast<Core::AttributeBool*>(mAttributeValue)->GetValue(); CORE_ASSERT( false ); return false; }
		int32 AsInt() const														{ if (mAttributeValue->GetType() == Core::AttributeInt32::TYPE_ID) return static_cast<Core::AttributeInt32*>(mAttributeValue)->GetValue(); CORE_ASSERT( false ); return 0; }
		double AsFloat() const													{ if (mAttributeValue->GetType() == Core::AttributeFloat::TYPE_ID) return static_cast<Core::AttributeFloat*>(mAttributeValue)->GetValue(); CORE_ASSERT( false ); return 0.0; }
		Core::String AsString() const											{ if (mAttributeValue->GetType() == Core::AttributeString::TYPE_ID) return static_cast<Core::AttributeString*>(mAttributeValue)->GetValue(); CORE_ASSERT( false ); return Core::String(); }
		Core::Color AsColor() const												{ if (mAttributeValue->GetType() == Core::AttributeColor::TYPE_ID) return static_cast<Core::AttributeColor*>(mAttributeValue)->GetValue(); CORE_ASSERT( false ); return Core::Color(0.0f, 0.0f, 0.0f, 1.0f); }

	private:
		Core::String				mName;
		AttributeWidget*			mAttributeWidget;
		Core::Attribute*			mAttributeValue;
		Core::AttributeSettings*	mAttributeSettings;
		bool						mAutoDelete;
};


#endif
