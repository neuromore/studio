/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "PropertyTreeWidget.h"
#include "AttributeWidgetFactory.h"
#include "../QtBaseManager.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QHeaderView>


using namespace Core;

// constructor
PropertyTreeWidget::PropertyTreeWidget(QWidget* parent) : QTreeWidget(parent)
{
	// setup the behavior
	setSelectionBehavior( QAbstractItemView::SelectRows );
	setSelectionMode( QAbstractItemView::NoSelection );
	setAlternatingRowColors(true);
	setExpandsOnDoubleClick(true);
	//setAnimated(true);

	// setup the headers
	QStringList headerList;
	headerList.append("Name");
	headerList.append("Value");
	setHeaderLabels(headerList);
	header()->setSectionResizeMode( QHeaderView::ResizeToContents );

	// create the property manager
	mPropertyManager = new PropertyManager(this);
	connect(mPropertyManager, SIGNAL(PropertyAdded(const char*, Property*)), this, SLOT(OnPropertyAdded(const char*, Property*)));
}


// destructor
PropertyTreeWidget::~PropertyTreeWidget()
{
	Clear();

	// destruct the property manager
	delete mPropertyManager;
}



// clear the contents
void PropertyTreeWidget::Clear()
{
	mPropertyTreeItemLinks.Clear();
	mPropertyManager->Clear();
	clear();
}



// find the tree widget item for the given group name, create it if it doesn't exist yet
QTreeWidgetItem* PropertyTreeWidget::GetGroupWidgetItem(const String& groupName)
{
	// split the groun names into their different hierarchy levels
	Array<String> groupNames = groupName.Split( StringCharacter('.') );
	if (groupNames.IsEmpty() == true)
		return NULL;

	// get the group root level name and directly remove it from the array
	String rootLevelName = groupNames[0];
	groupNames.RemoveFirst();

	// get the number of top level items and iterate through them
	const int32 numTopLevelItems = QTreeWidget::topLevelItemCount();
	for (int32 i=0; i<numTopLevelItems; ++i)
	{
		QTreeWidgetItem* topLevelItem = QTreeWidget::topLevelItem(i);

		// compare the names of the existing top level items with the one we are searching, if it already exists, just return it
		if (topLevelItem->text(0) == rootLevelName.AsChar())
			return GetGroupWidgetItem( groupNames, topLevelItem );
	}

	// if we reach this line it means that the root level item does not exist, so create it
	QTreeWidgetItem* newItem = new QTreeWidgetItem();
	newItem->setText( 0, rootLevelName.AsChar() );
	addTopLevelItem( newItem );

	// recursive call
	return GetGroupWidgetItem( groupNames, newItem );
}


// find the tree widget item for the given group name, create it if it doesn't exist yet
QTreeWidgetItem* PropertyTreeWidget::GetGroupWidgetItem(Array<String>& groupNames, QTreeWidgetItem* parentItem)
{
	// if there are no more levels to deal with, return directly
	if (groupNames.IsEmpty() == true)
		return parentItem;

	// get the first of the group name levels and directly remove it from the array
	String currentLevelName = groupNames[0];
	groupNames.RemoveFirst();

	// get the number of child items and iterate through them
	const int32 numChilds = parentItem->childCount();
	for (int32 i=0; i<numChilds; ++i)
	{
		QTreeWidgetItem* childItem = parentItem->child(i);

		// compare the names of the existing top level items with the one we are searching, if it already exists, just return it
		if (childItem->text(0) == currentLevelName.AsChar())
			return GetGroupWidgetItem( groupNames, childItem );
	}

	// if we reach this line it means that the item does not exist, so create it
	QTreeWidgetItem* newItem = new QTreeWidgetItem();
	newItem->setText( 0, currentLevelName.AsChar() );
	parentItem->addChild( newItem );

	// recursive call
	return GetGroupWidgetItem( groupNames, newItem );
}


// set the expanded flag to a given item
void PropertyTreeWidget::SetExpanded(const char* groupName, bool isExpanded)
{
	// get the tree widget item based on the group name, and return directly if it doesn't exist
	QTreeWidgetItem* treeWidgetItem = GetGroupWidgetItem(groupName);
	if (treeWidgetItem == NULL)
	{
		CORE_ASSERT(false);
		return;
	}

	treeWidgetItem->setExpanded(isExpanded);
}


// get the expanded flag from a given item
bool PropertyTreeWidget::IsExpanded(const char* groupName)
{
	// get the tree widget item based on the group name, and return directly if it doesn't exist
	QTreeWidgetItem* treeWidgetItem = GetGroupWidgetItem(groupName);
	if (treeWidgetItem == NULL)
		return false;

	return treeWidgetItem->isExpanded();
}


// build a property with some readonly text as value
QTreeWidgetItem* PropertyTreeWidget::AddTreeWidgetItem(const QString& name, QTreeWidgetItem* parentItem, const QString& valueText)
{
	// create the new item
	QTreeWidgetItem* newItem;
	if (parentItem == NULL)
		newItem = new QTreeWidgetItem( this );
	else
		newItem = new QTreeWidgetItem( parentItem );

	// set the name
	newItem->setText( 0, name );

	// if we specified some text to use as value, set it
	if (valueText.length() > 0)
		newItem->setText(1, valueText);


	// return the new item
	return newItem;
}


// add a property with a given widget
QTreeWidgetItem* PropertyTreeWidget::AddTreeWidgetItem(const QString& name, QTreeWidgetItem* parentItem, QWidget* widget)
{
	// create the new item
	QTreeWidgetItem* newItem = AddTreeWidgetItem(name, parentItem);
	
	// set the item widget
	setItemWidget(newItem, 1, widget);

	// return the new item
	return newItem;
}


// add a new property
Property* PropertyTreeWidget::AddProperty(const char* groupName, const char* propertyName, AttributeWidget* attributeWidget, Attribute* attributeValue, AttributeSettings* settings, bool autoDelete)
{
	// find the corresponding parent item based on the group name
	QTreeWidgetItem* parentItem = GetGroupWidgetItem( groupName );

	// create the tree widget item and put the attribute widget in it
	QTreeWidgetItem* treeItem = AddTreeWidgetItem( propertyName, parentItem, attributeWidget );

	// create a new property
	Property* property = mPropertyManager->AddProperty( groupName, propertyName, attributeWidget, attributeValue, settings, autoDelete );

	// create the link
	mPropertyTreeItemLinks.Add( PropertyTreeItemLink(treeItem, property) );


	return property;
}


void PropertyTreeWidget::OnPropertyAdded(const char* groupName, Property* newProperty)
{
	// find the corresponding parent item based on the group name
	QTreeWidgetItem* parentItem = GetGroupWidgetItem( groupName );

	// create the tree widget item and put the attribute widget in it
	AddTreeWidgetItem( newProperty->GetName(), parentItem, newProperty->GetAttributeWidget() );
	
	// expand parent if its not null
	if (parentItem != NULL)
		parentItem->setExpanded(true);
}


// find the link between a property and a tree widget item
PropertyTreeWidget::PropertyTreeItemLink PropertyTreeWidget::FindPropertyTreeItemLink(Property* property)
{
	const uint32 numLinks = mPropertyTreeItemLinks.Size();
	for (uint32 i=0; i<numLinks; ++i)
	{
		if (mPropertyTreeItemLinks[i].mProperty == property)
			return mPropertyTreeItemLinks[i];
	}

	return PropertyTreeItemLink();
}


// set the property visibility (including its description)
void PropertyTreeWidget::SetVisible(Property* property, bool isVisible)
{
	if (property == NULL)
		return;

	PropertyTreeItemLink link = FindPropertyTreeItemLink( property );
	if (link.IsValid() == false)
		return;

	const bool isHidden = !isVisible;
	if (link.mTreeItem->isHidden() != isHidden)
		link.mTreeItem->setHidden( isHidden );
}
