/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "AttributeSetGridWidget.h"
#include "AttributeWidgetFactory.h"
#include <Core/EventManager.h>
#include "../QtBaseManager.h"


using namespace Core;

// constructor
AttributeSetGridWidget::AttributeSetGridWidget(QWidget* parent) : QWidget(parent)
{
	mWidget		= NULL;
	mGridLayout	= NULL;
	mCurrentRow	= 0;

	// create the property manager
	mPropertyManager = new PropertyManager(this);
	connect(mPropertyManager, SIGNAL(PropertyAdded(const char*, Property*)), this, SLOT(OnPropertyAdded(const char*, Property*)));

	// register event handler
	CORE_EVENTMANAGER.AddEventHandler(this);

	// init
	show();
}


// destructor
AttributeSetGridWidget::~AttributeSetGridWidget()
{
	// unregister event handler
	CORE_EVENTMANAGER.RemoveEventHandler(this);

	// destruct the property manager
	delete mPropertyManager;
}


// init for a given node
void AttributeSetGridWidget::ReInit(AttributeSet* attributeSet)
{
	CORE_ASSERT( mWidget == NULL );
	if (mWidget != NULL)
		return;
	
	mGridLayout = new QGridLayout();
	mGridLayout->setMargin(0);
	mGridLayout->setSpacing(0);
	setLayout(mGridLayout);

	mAttributeSet = attributeSet;

	// if there is no valid attribute set, leave
	if (mAttributeSet == NULL)
		return;

	void* customData = NULL;
	QWidget* lastTablableWidget = NULL;

	// add all attributes
	mAttributeLinks.Clear(false);
	Core::String labelString;
	const uint32 numAttributes = mAttributeSet->GetNumAttributes();
	for (uint32 i=0; i<numAttributes; ++i)
	{
		// get the attribute and the corresponding attribute settings
		Core::Attribute*			attribute			= mAttributeSet->GetAttributeValue(i);
		Core::AttributeSettings*	attributeSettings	= mAttributeSet->GetAttributeSettings(i);

		// create the attribute and add it to the layout
		AttributeWidget* attributeWidget = GetQtBaseManager()->GetAttributeWidgetFactory()->CreateAttributeWidget( attribute, attributeSettings, customData, !attributeSettings->IsEnabled() );
		Property* property = AddProperty( attributeSettings->GetName(), attributeWidget, attribute, attributeSettings, false);
		mAttributeLinks.AddEmpty();
		mAttributeLinks.GetLast().mAttributeIndex	= i;
		mAttributeLinks.GetLast().mCustomData		= customData;
		mAttributeLinks.GetLast().mWidget			= attributeWidget;
		mAttributeLinks.GetLast().mProperty			= property;

		// set tab order
		QWidget* tablableWidget = attributeWidget->GetTablableWidget();

		if (lastTablableWidget != NULL && tablableWidget != NULL)
		{
			setTabOrder( lastTablableWidget, tablableWidget );
			lastTablableWidget = tablableWidget;
		}
		else if (lastTablableWidget == NULL && tablableWidget != NULL)
		{
			lastTablableWidget = tablableWidget;
		}
	}

	UpdateInterface();
}


// event handler callback
void AttributeSetGridWidget::OnAttributeUpdated(Graph* graph, GraphObject* object, Core::Attribute* attribute)
{
	if (mAttributeSet->HasAttribute(attribute))
		UpdateInterface();
}


// called in case any of the properties gets changed
void AttributeSetGridWidget::OnValueChanged(Property* property)
{
	// when changing the node name
	//if (property == mNameProperty)
	//	mNode->SetName( property->AsString().AsChar() );
}


// update the states
void AttributeSetGridWidget::UpdateInterface()
{
	const uint32 numLinks = mAttributeLinks.Size();
	for (uint32 i=0; i<numLinks; ++i)
	{
		const AttributeLink&	curLink				= mAttributeLinks[i];
		//Property*				property			= curLink.mProperty;
		Attribute*				attribute			= mAttributeSet->GetAttributeValue( curLink.mAttributeIndex );
		AttributeSettings*		attributeSettings	= mAttributeSet->GetAttributeSettings( curLink.mAttributeIndex );
		const bool				isEnabled			= attributeSettings->IsEnabled();
		const bool				isVisible			= attributeSettings->IsVisible();

		curLink.mWidget->EnableWidgets( isEnabled );
		curLink.mWidget->SetValue( attribute );

		curLink.mWidget->SetVisible( isVisible );
		mGridLayout->itemAtPosition(i, 0)->widget()->setVisible( isVisible );
	}
}


// add a new property
Property* AttributeSetGridWidget::AddProperty(const char* propertyName, AttributeWidget* attributeWidget, Core::Attribute* attributeValue, Core::AttributeSettings* settings, bool autoDelete)
{
	mGridLayout->addWidget( new QLabel(propertyName), mCurrentRow, 0 );
	mGridLayout->addWidget( attributeWidget, mCurrentRow, 1 );
	mCurrentRow++;

	// create a new property
	return mPropertyManager->AddProperty( "", propertyName, attributeWidget, attributeValue, settings, autoDelete );
}


void AttributeSetGridWidget::OnPropertyAdded(const char* groupName, Property* newProperty)
{
	mGridLayout->addWidget( new QLabel(newProperty->GetName()), mCurrentRow, 0 );
	mGridLayout->addWidget(  newProperty->GetAttributeWidget(), mCurrentRow, 1 );
	mCurrentRow++;
}
