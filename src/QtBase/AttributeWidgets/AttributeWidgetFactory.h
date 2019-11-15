/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_ATTRIBUTEWIDGETFACTORY_H
#define __NEUROMORE_ATTRIBUTEWIDGETFACTORY_H

// include required headers
#include "AttributeWidgetCreators.h"


class QTBASE_API AttributeWidgetFactory
{
	public:
		AttributeWidgetFactory();
		~AttributeWidgetFactory();

		enum AttributeWidgetType
		{
			ATTRIBUTE_NORMAL	= 0,
			ATTRIBUTE_DEFAULT	= 1,
			ATTRIBUTE_MIN		= 2,
			ATTRIBUTE_MAX		= 3
		};

		bool RegisterCreator(AttributeWidgetCreator* creator);
		bool HasRegisteredCreatorByTypeID(uint32 nodeTypeID) const;
		uint32 FindRegisteredCreatorByTypeID(uint32 typeID, AttributeWidgetType attributeWidgetType=ATTRIBUTE_NORMAL) const;


		AttributeWidget* CreateAttributeWidget(Core::Array<Core::Attribute*>& attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool forceInitMinMaxAttributes=false, bool resetMinMaxAttributes=true, AttributeWidgetType attributeWidgetType=ATTRIBUTE_NORMAL, bool creationMode=false);
		AttributeWidget* CreateAttributeWidget(Core::Attribute* attribute, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool forceInitMinMaxAttributes=false, bool resetMinMaxAttributes=true, AttributeWidgetType attributeWidgetType=ATTRIBUTE_NORMAL, bool creationMode=false);
		void InitAttributes(Core::Array<Core::Attribute*>& attributes, Core::AttributeSettings* attributeSettings, bool forceInitMinMaxAttributes=false, AttributeWidgetType attributeWidgetType=ATTRIBUTE_NORMAL);


		// callbacks
		class Callback
		{
            public:
                Callback()              {}
                virtual ~Callback()     {}
            
				virtual void OnAttributeChanged(Core::Attribute* attribute) = 0;
		};

		void ClearCallbacks();
		void OnAttributeChanged(Core::Attribute* attribute); // inform all callbacks about an attribute change
		void RemoveCallback(Callback* callback, bool delFromMem=true);
		void AddCallback(Callback* callback)										{ mCallbacks.Add(callback); }

	private:
		Core::Array<AttributeWidgetCreator*>	mRegisteredCreators;
		Core::Array<Callback*>					mCallbacks;
};


#endif
