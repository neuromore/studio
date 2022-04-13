/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "GraphObjectViewWidget.h"
#include <Core/EventManager.h>
//#include <Graph/FilterNode.h>
//#include "FilterPlotWidget.h"
#include "../QtBaseManager.h"


using namespace Core;

// constructor
GraphObjectViewWidget::GraphObjectViewWidget(QWidget* parent) : QWidget(parent)
{
	mObject		= NULL;

	// empty layout
	mGridLayout	= new QGridLayout();
	mGridLayout->setContentsMargins(0,0,0,0);
	mGridLayout->setMargin(0);
	setLayout(mGridLayout);

	// init
	show();
}


// destructor
GraphObjectViewWidget::~GraphObjectViewWidget()
{
}


// init for a given object
void GraphObjectViewWidget::ReInit(GraphObject* object)
{
	// create the widgets, if not done already
	if (mObject != object)
	{
		mObject = object;

		hide();

		// remove old widgets first
		Clear();
	
		// empty widget
		if (mObject == NULL)
			return;
	
		mGridLayout->setMargin(0);
		mGridLayout->setSpacing(0);
		setLayout(mGridLayout);

		// add the widgets
		switch (object->GetType())
		{
			/*case FilterNode::TYPE_ID: 
			{
				FilterPlotWidget* widget = new FilterPlotWidget();
				widget->setMinimumHeight(150);
				mGridLayout->addWidget(widget);
				mWidgets.Add(widget);
				break;
			}*/
			default: break;
		}

		show();
	}

	// update the widgets
	switch (object->GetType())
	{
	/*	case FilterNode::TYPE_ID: 
		{
			CORE_ASSERT(mWidgets.Size() == 1);

			FilterPlotWidget* widget = static_cast<FilterPlotWidget*>(mWidgets[0]);
			FilterNode* filterNode = static_cast<FilterNode*>(object);
			widget->ReInit( filterNode->GetFilterSettings() );
			break;
		}*/

		default: break;
	}
}


void GraphObjectViewWidget::Clear()
{
	// remove all widgets
	QLayoutItem *layoutItem;
	while ((layoutItem = mGridLayout->takeAt(0)) != 0)
		delete layoutItem;

	DestructArray<QWidget*>(mWidgets);
}

