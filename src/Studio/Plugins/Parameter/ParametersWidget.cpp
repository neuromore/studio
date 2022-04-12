/****************************************************************************
**
** Copyright 2019 neuromore co
** Contact: https://neuromore.com/contact
**
** Commercial License Usage
** Licensees holding valid commercial neuromore licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and neuromore. For licensing terms
** and conditions see https://neuromore.com/licensing. For further
** information use the contact form at https://neuromore.com/contact.
**
** neuromore Public License Usage
** Alternatively, this file may be used under the terms of the neuromore
** Public License version 1 as published by neuromore co with exceptions as 
** appearing in the file neuromore-class-exception.md included in the 
** packaging of this file. Please review the following information to 
** ensure the neuromore Public License requirements will be met: 
** https://neuromore.com/npl
**
****************************************************************************/

// include precompiled header
#include <Studio/Precompiled.h>

// include required headers
#include "ParametersWidget.h"

using namespace Core;

// constructor
ParametersWidget::ParametersWidget(QWidget* parent) : QScrollArea(parent)
{
	mClassifier = NULL;

	setObjectName("TransparentWidget");
	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	setWidgetResizable( true );

	// create our main widget which we'll set as scroll area
	QWidget* mainWidget = new QWidget();
	setWidget( mainWidget );

	// create the vertical layout for the main widget
	mMainLayout = new QVBoxLayout();
	mMainLayout->setMargin(3);
	mMainLayout->setSpacing(1);
	mainWidget->setLayout(mMainLayout);


	// info label and spacer widget
	mInfoLabel = new QLabel("No parameters in classifier");
	mInfoLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	mInfoLabel->setAlignment(Qt::AlignCenter);
	
	mSpacerWidget = new QWidget();
	mSpacerWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
	mSpacerWidget->setObjectName("TransparentWidget");

	// add event handler
	CORE_EVENTMANAGER.AddEventHandler(this);

	// add Widgets immediately
	ReInit(GetEngine()->GetActiveClassifier());

	// init
	show();
}


// destructor
ParametersWidget::~ParametersWidget()
{
	CORE_EVENTMANAGER.RemoveEventHandler(this);
}


// reinitialize parameters for new classifier
void ParametersWidget::ReInit(Classifier* classifier)
{
	// classifier has not changed
	if (mClassifier == classifier && classifier != NULL)
		return;
	
	mClassifier = classifier;

	// clear layout, will add all Widgets later
	ClearLayout();
	DestructArray(mWidgets);

	// no classifier -> no Widgets
	if (mClassifier != NULL)
	{
		// create all Widgets
		const uint32 numParameters = classifier->GetNumParameterNodes();
		for (uint32 i = 0; i < numParameters; ++i)
			AddWidget(classifier->GetParameterNode(i));
	}
	
	FillLayout();
}


// update interface information
void ParametersWidget::UpdateInterface()
{
	const uint32 numWidgets = mWidgets.Size();
	for (uint32 i = 0; i < numWidgets; ++i)
		mWidgets[i]->UpdateInterface();
}


void ParametersWidget::ClearLayout()
{
	// clear main layout first and remove all Widgets from it 
	QLayoutItem *layoutItem;
	while ((layoutItem = mMainLayout->takeAt(0)) != 0)
		delete layoutItem;
}


void ParametersWidget::FillLayout()
{
	const uint32 numWidgets = mWidgets.Size();

	// expanding label as the only widget in the layout so the text is centered
	mMainLayout->addWidget(mInfoLabel);
	mInfoLabel->setVisible(numWidgets == 0);
	
	// add all Widgets
	for (uint32 i=0; i<numWidgets; ++i)
		mMainLayout->addWidget(mWidgets[i]);

	// add an expanding spacer at the end
	mMainLayout->addWidget(mSpacerWidget);
	mSpacerWidget->setVisible(numWidgets > 0);
}


void ParametersWidget::OnGraphModified(Graph* graph, GraphObject* object)
{
	if (object == NULL)
		return;

	if (object->GetType() != ParameterNode::TYPE_ID)
		return;

	ParameterNode* paramNode = static_cast<ParameterNode*>(object);

	UpdateWidget(paramNode);
}


// apply parameters after graph was reset
void ParametersWidget::OnGraphReset(Graph* graph)
{
	// FIXME: ApplyParameters() somehow doesn't work here?
	//if (mClassifier == graph)
	//	UpdateWidgets();
}


void ParametersWidget::OnRemoveNode(Graph* graph, Node* node)
{
	if (node == NULL)
		return;

	if (node->GetType() != ParameterNode::TYPE_ID)
		return;

	ParameterNode* paramNode = static_cast<ParameterNode*>(node);

	RemoveWidget(paramNode);
}


void ParametersWidget::OnNodeStarted(Graph* graph, SPNode* node)
{
	if (node == NULL)
		return;

	if (node->GetType() != ParameterNode::TYPE_ID)
		return;
	
	ParameterNode* paramNode = static_cast<ParameterNode*>(node);

	AddWidget(paramNode);
}


void ParametersWidget::OnNodeStopped(Graph* graph, SPNode* node)
{
	if (node == NULL)
		return;

	if (node->GetType() != ParameterNode::TYPE_ID)
		return;
	
	ParameterNode* paramNode = static_cast<ParameterNode*>(node);

	RemoveWidget(paramNode);
}


// update Widgets if an attribute of the controlled node has changed
void ParametersWidget::OnAttributeUpdated(Graph* graph, GraphObject* object, Core::Attribute* attribute)
{
	if (object == NULL)
		return;

	if (object->GetType() != ParameterNode::TYPE_ID)
		return;

	ParameterNode* node = static_cast<ParameterNode*>(object);

	UpdateWidget(node);
}

// set parameters on session start
void ParametersWidget::OnStartSession()
{
	ApplyParameters();
}


void ParametersWidget::ApplyParameters()
{
	// set all parameters from the widget state
	const uint32 numWidgets = mWidgets.Size();
	for (uint32 i = 0; i < numWidgets; ++i)
		mWidgets[i]->ApplyParameter();
}


void ParametersWidget::UpdateWidgets()
{
	// update all widgets and set them to the default state
	const uint32 numWidgets = mWidgets.Size();
	for (uint32 i = 0; i < numWidgets; ++i)
		mWidgets[i]->UpdateFromNode();
}


void ParametersWidget::UpdateWidget(ParameterNode* node)
{
	// do we have a Widget for this node? if so, update or remove it it
	ParameterWidget* widget = FindWidgetForNode(node);
	if (widget != NULL)
	{
		if (node->IsWidgetVisible() == true)
			widget->UpdateFromNode(node);
		else
		{
			RemoveWidget(node);
			widget = NULL;
		}
	}
	else // try to add the widget
	{
		widget = AddWidget(node);
	}

	// widget was removed or simply not added
	if (widget == NULL)
		return;

	// re-add widget if the type of control has changed (we need to switch widgets)
	if ( (node->GetControlType() == ParameterNode::CONTROLTYPE_SEPARATOR		&& widget->GetType() != ParameterSeparatorWidget::TYPE_ID)			||
		 (node->GetControlType() == ParameterNode::CONTROLTYPE_SLIDER			&& widget->GetType() != ParameterSliderWidget::TYPE_ID)				||
		 (node->GetControlType() == ParameterNode::CONTROLTYPE_CHECKBOX			&& widget->GetType() != ParameterCheckBoxWidget::TYPE_ID)			||
		 (node->GetControlType() == ParameterNode::CONTROLTYPE_DROPDOWN			&& widget->GetType() != ParameterComboBoxWidget::TYPE_ID)			||
		 (node->GetControlType() == ParameterNode::CONTROLTYPE_SELECTION_BUTTONS&& widget->GetType() != ParameterSelectionButtonWidget::TYPE_ID)	 ||
		 (node->GetControlType() == ParameterNode::CONTROLTYPE_EVENT_BUTTONS	&& widget->GetType() != ParameterEventButtonWidget::TYPE_ID)	 )
	{
		RemoveWidget(node);
		widget = AddWidget(node);
	}

	// change enable statestate
	if (widget != NULL)
		widget->setEnabled(node->IsWidgetEnabled());
}


ParameterWidget* ParametersWidget::FindWidgetForNode(ParameterNode* node) const
{
	const uint32 numWidgets = mWidgets.Size();
	for (uint32 i = 0; i < numWidgets; ++i)
		if (mWidgets[i]->GetParameterNode() == node)
			return mWidgets[i];

	return NULL;
}


// create Widget for parameter node, if its settings allow it 
ParameterWidget* ParametersWidget::AddWidget(ParameterNode* node)
{
	// Widget already added
	if (FindWidgetForNode(node) != NULL)
		return NULL;

	// show control for this node?
	if (node->ShowControls() == false)
		return NULL;

	// create new Widget depending on parameter type
	ParameterWidget* widget = NULL;
	switch (node->GetControlType())
	{
		case ParameterNode::CONTROLTYPE_SEPARATOR:			widget = new ParameterSeparatorWidget(node, ParameterWidget::POSITION_BELOW);		break;
		case ParameterNode::CONTROLTYPE_SLIDER:				widget = new ParameterSliderWidget(node, ParameterWidget::POSITION_BELOW);			break;
		case ParameterNode::CONTROLTYPE_CHECKBOX:			widget = new ParameterCheckBoxWidget(node, ParameterWidget::POSITION_RIGHT);		break;
		case ParameterNode::CONTROLTYPE_DROPDOWN:			widget = new ParameterComboBoxWidget(node, ParameterWidget::POSITION_RIGHT);		break;
		case ParameterNode::CONTROLTYPE_SELECTION_BUTTONS:	widget = new ParameterSelectionButtonWidget(node, ParameterWidget::POSITION_RIGHT);	break;
		case ParameterNode::CONTROLTYPE_EVENT_BUTTONS:		widget = new ParameterEventButtonWidget(node, ParameterWidget::POSITION_BELOW);		break;
        default:                                            LogError("ParametersWidget::AddWidget(): Unsupported control type");                break;
	}
	
	if (widget != NULL)
	{
		widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
		ClearLayout();
		mWidgets.Add(widget);
		SortWidgets();
		FillLayout();
	}

	return widget;
}


bool ParametersWidget::RemoveWidget(ParameterNode* node)
{
	// Widget for node not found
	ParameterWidget* widget = FindWidgetForNode(node);
	if (widget == NULL)
		return false;

	// remove from layout
	ClearLayout();

	// remove Widget
	delete widget;
	mWidgets.RemoveByValue(widget);

	// update layout
	FillLayout();

	return true;
}


// sort Widget array so they are in the same order as the nodes (which are sorted by visual height)
// TODO we can think about using other orders
void ParametersWidget::SortWidgets()
{
	if (mClassifier == NULL || mWidgets.Size() == 0)
		return;

	// make copy of widget array
	Array<ParameterWidget*> WidgetsCopy = mWidgets;
	const uint32 numWidgets = WidgetsCopy.Size();
	mWidgets.Clear();

	// fill mWidget array in correct order (same as classifier)
	const uint32 numNodes = mClassifier->GetNumParameterNodes();
	for (uint32 i=0; i<numNodes; ++i)
	{
		// find Widget for this node
		for (uint32 j=0; j<numWidgets; ++j)
		{
			if (WidgetsCopy[j]->GetParameterNode() == mClassifier->GetParameterNode(i))
			{
				mWidgets.Add(WidgetsCopy[j]);

				break; // assumes there is exactly one Widget per node
			}
		}
	}
}
