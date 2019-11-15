/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_ATTRIBUTESETGRIDWIDGET_H
#define __NEUROMORE_ATTRIBUTESETGRIDWIDGET_H

// include required headers
#include "PropertyManager.h"
#include <Core/EventHandler.h>
#include <QGridLayout>
#include <QScrollArea>


class QTBASE_API AttributeSetGridWidget : public QWidget, public Core::EventHandler
{
	Q_OBJECT

	public:
		AttributeSetGridWidget(QWidget* parent=NULL);
		virtual ~AttributeSetGridWidget();

		void ReInit(Core::AttributeSet* attributeSet);

		// event handlers
		void OnAttributeUpdated(Graph* graph, GraphObject* object, Core::Attribute* attribute) override;

		PropertyManager* GetPropertyManager() const							{ return mPropertyManager; }

	public slots:
		void OnValueChanged(Property* property);
		void UpdateInterface();

		void OnPropertyAdded(const char* groupName, Property* newProperty);

	protected:
		Property* AddProperty(const char* propertyName, AttributeWidget* attributeWidget, Core::Attribute* attributeValue, Core::AttributeSettings* settings, bool autoDelete);

		struct AttributeLink
		{
			AttributeWidget*			mWidget;
			Property*					mProperty;
			void*						mCustomData;
			uint32						mAttributeIndex;
		};

		Core::AttributeSet*				mAttributeSet;
		PropertyManager*				mPropertyManager;
		QGridLayout*					mGridLayout;
		uint32							mCurrentRow;
		QWidget*						mWidget;
		Core::Array<AttributeLink>		mAttributeLinks;
};


#endif
