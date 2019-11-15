/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_PROPERTYTREEWIDGET_H
#define __NEUROMORE_PROPERTYTREEWIDGET_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include "Property.h"
#include "PropertyManager.h"
#include "AttributeWidgets.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>


// forward declarations
QT_FORWARD_DECLARE_CLASS(QWidget)

class QTBASE_API PropertyTreeWidget : public QTreeWidget
{
	Q_OBJECT

	public:
		PropertyTreeWidget(QWidget* parent=NULL);
		virtual ~PropertyTreeWidget();

		// expand or collapse groups
		void SetExpanded(const char* groupName, bool isExpanded);
		bool IsExpanded(const char* groupName);

		PropertyManager* GetPropertyManager() const					{ return mPropertyManager; }

		void Clear();

		// helpers
		Property* AddProperty(const char* groupName, const char* propertyName, AttributeWidget* attributeWidget, Core::Attribute* attributeValue, Core::AttributeSettings* settings, bool autoDelete=true);

		void SetVisible(Property* property, bool isVisible);

	private slots:
		void OnPropertyAdded(const char* groupName, Property* newProperty);

	private:
		struct PropertyTreeItemLink
		{
			PropertyTreeItemLink()																{ mTreeItem = NULL; mProperty = NULL; }
			PropertyTreeItemLink(QTreeWidgetItem* treeItem, Property* property)					{ mTreeItem = treeItem; mProperty = property; }
			bool IsValid()																		{ return (mTreeItem != NULL && mProperty != NULL); }

			QTreeWidgetItem*	mTreeItem;
			Property*			mProperty;
		};

		// helper functions
		QTreeWidgetItem* AddTreeWidgetItem(const QString& name, QTreeWidgetItem* parentItem=NULL, const QString& valueText="");
		QTreeWidgetItem* AddTreeWidgetItem(const QString& name, QTreeWidgetItem* parentItem, QWidget* widget);
		QTreeWidgetItem* GetGroupWidgetItem(const Core::String& groupName);
		QTreeWidgetItem* GetGroupWidgetItem(Core::Array<Core::String>& groupNames, QTreeWidgetItem* parentItem);

		PropertyTreeItemLink FindPropertyTreeItemLink(Property* property);

		PropertyManager*					mPropertyManager;
		Core::Array<PropertyTreeItemLink>	mPropertyTreeItemLinks;
};


#endif
