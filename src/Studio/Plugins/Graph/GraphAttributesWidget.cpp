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

// include required headers
#include "GraphAttributesWidget.h"
#include <AttributeWidgets/AttributeWidgetFactory.h>
#include <QtBaseManager.h>
#include <EngineManager.h>
#include "../../AppManager.h"
#include "../../MainWindow.h"


using namespace Core;

// constructor
GraphAttributesWidget::GraphAttributesWidget(QWidget* parent) : QScrollArea(parent)
{
	LogDebug("Constructing attributes widget ...");
	mGraphObject	= NULL;
	mNameProperty	= NULL;
	mParentGroupName= "";

	setContentsMargins(0,0,0,0);
	setViewportMargins(0,0,0,0);
	setObjectName("TransparentWidget");
	setWidgetResizable( true );

	// create the new property widget
	mPropertyTreeWidget = new PropertyTreeWidget();
	connect( mPropertyTreeWidget->GetPropertyManager(), SIGNAL(ValueChanged(Property*)), this, SLOT(OnValueChanged(Property*)) );
	
	// create the graph object view widget
	mGraphObjectView = new GraphObjectViewWidget();

	// create the main vertical layout
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->setMargin(0);
	vLayout->setContentsMargins(0,0,0,0);

	vLayout->addWidget(mPropertyTreeWidget);
	vLayout->addWidget(mGraphObjectView);
	setLayout( vLayout );


	// register event handler
	CORE_EVENTMANAGER.AddEventHandler(this);

	// init
	InitForNode( NULL );
	show();
}


// destructor
GraphAttributesWidget::~GraphAttributesWidget()
{
	// unregister event handler
	CORE_EVENTMANAGER.RemoveEventHandler(this);
}


// event handler callback: an attribute has changed, check if it was one of the displayed ones
void GraphAttributesWidget::OnAttributeUpdated(Graph* graph, GraphObject* object, Attribute* attribute)
{
	// no attribute set loaded yet
	if (mAttributeSet == NULL)
		return;

	// refresh interface if attribute is currently dislayed
	if (mAttributeSet->HasAttribute(attribute) == true)
		UpdateInterface();
}


// event handler callback: graph was modified; check if graph or graphobject was displayed
void GraphAttributesWidget::OnGraphModified(Graph* graph, GraphObject* object)
{
	if (mGraphObject != NULL && (mGraphObject == graph || mGraphObject == object) )
	{
		ForceReInit();
	}
}


void GraphAttributesWidget::InitForGraphObject(GraphObject* object, bool force)
{
	if (object == NULL)
	{
		InitForNode(NULL);
		return;
	}
	else if (object == mGraphObject && force == false)
	{
		// don't update if we already display this object
		return;
	}

	if (object->GetBaseType() == Graph::BASE_TYPE)				InitForGraph( static_cast<Graph*>(object) );				// classifier
	else if (object->GetBaseType() == Node::BASE_TYPE)			InitForNode( static_cast<Node*>(object) );					// classifier node
	else if (object->GetType() == StateMachine::TYPE_ID)		InitForGraph( static_cast<StateMachine*>(object), false );	// state machine
	else if (object->GetBaseType() == State::BASE_TYPE)			InitForNode(static_cast<State*>(object));					// state
	else if (object->GetBaseType() == Connection::BASE_TYPE)	InitForConnection( static_cast<Connection*>(object) );		// classifier connection
	else														InitForNode(NULL);
}


// init for a given graph (classifier or statemachine)
void GraphAttributesWidget::InitForGraph(Graph* graph, bool showName)
{
	mPropertyTreeWidget->Clear();
	mAttributeLinks.Clear(false);
	mGraphObject = graph;
	mAttributeSet = NULL;

	// if there is no node, leave
	if (graph == NULL)
		return;
	
	// is the attribute editable?
	const bool allowsEditing = graph->AllowEditing();
	const bool sessionRunning = GetSession()->IsRunning();
	const bool isReadOnly = sessionRunning || !allowsEditing;

	// add the name as property first
	if (showName == true)
		mNameProperty = mPropertyTreeWidget->GetPropertyManager()->AddStringProperty( mParentGroupName.AsChar(), "Name", graph->GetName(), 0, isReadOnly);
	else
		mNameProperty = NULL;


	// add attributes
	AddAttributes( graph, mParentGroupName.AsChar(), isReadOnly);	

	if (mParentGroupName.IsEmpty() == false)
		mPropertyTreeWidget->SetExpanded( mParentGroupName.AsChar(), true );

	UpdateInterface();
}


// init for a given node (from classifier or state machine)
void GraphAttributesWidget::InitForNode(Node* node, bool showName)
{
	mPropertyTreeWidget->Clear();
	mAttributeLinks.Clear(false);
	mGraphObject = node;
	mAttributeSet = NULL;

	// if there is no node, leave
	if (node == NULL)
		return;

	// we can't handle nodes without parents here (they always belong to a graph anyways...)
	Graph* parent = node->GetParent();
	if (parent == NULL)
		return;

	// graph editable? (Read access and not locked)
	const bool graphNotEditable = (parent->AllowEditing() == false);
	
	// FIXME this is a little hacky:
	//Experience* experience = GetEngine()->GetActiveExperience();

	const bool graphSettingsMode = GetMainWindow()->IsSettingsModeEnabled();
	const bool sessionRunning = GetSession()->IsRunning();
	const bool nodeLocked = node->IsLocked();

	const bool isAttributeReadOnly = sessionRunning || nodeLocked || ( !graphSettingsMode && graphNotEditable);
	const bool isNameReadOnly = sessionRunning || nodeLocked || graphNotEditable;

	// add the name as property first
	if (showName == true)
		mNameProperty = mPropertyTreeWidget->GetPropertyManager()->AddStringProperty( mParentGroupName.AsChar(), "Node Name", node->GetName(), 0, isNameReadOnly);
	else
		mNameProperty = NULL;

	// node attributes
	AddAttributes( node, mParentGroupName.AsChar(), isAttributeReadOnly );

	// if node is a state: add actions
	if (node->GetBaseType() == State::BASE_TYPE)
		AddActions( node, isAttributeReadOnly );

	if (mParentGroupName.IsEmpty() == false)
		mPropertyTreeWidget->SetExpanded( mParentGroupName.AsChar(), true );

	UpdateInterface();
}


// called in case any of the properties gets changed
void GraphAttributesWidget::OnValueChanged(Property* property)
{
	// when changing the node name
	if (property == mNameProperty)
		mGraphObject->SetName( property->AsString().AsChar() );

	UpdateInterface();
}


// update the states
void GraphAttributesWidget::UpdateInterface()
{
	const uint32 numLinks = mAttributeLinks.Size();
	for (uint32 i=0; i<numLinks; ++i)
	{
		const AttributeLink& curLink = mAttributeLinks[i];
		if (curLink.mWidget == NULL)
			continue;

		GraphObject*			object				= curLink.mGraphObject;
		Property*				property			= curLink.mProperty;
		Attribute*				attribute			= object->GetAttributeValue( curLink.mAttributeIndex );
		AttributeSettings*		attributeSettings	= object->GetAttributeSettings( curLink.mAttributeIndex );
		const bool				isEnabled			= attributeSettings->IsEnabled();
		const bool				isVisible			= attributeSettings->IsVisible();

		curLink.mWidget->EnableWidgets( isEnabled );
		curLink.mWidget->SetValue( attribute );

		mPropertyTreeWidget->SetVisible( property, isVisible );
	}

	// update widget view
	mGraphObjectView->ReInit(mGraphObject);
}


void GraphAttributesWidget::AddAttributes(GraphObject* object, const char* parentGroupName, bool disableAttributes)
{
	mAttributeSet = object;

	QWidget* lastTablableWidget = NULL;

	// add all attributes
	Core::String labelString;
	const uint32 numAttributes = object->GetNumAttributes();
	for (uint32 i=0; i<numAttributes; ++i)
	{
		// get the attribute and the corresponding attribute settings
		Core::Attribute*			attribute			= object->GetAttributeValue(i);
		Core::AttributeSettings*	attributeSettings	= object->GetAttributeSettings(i);

		// create the attribute and add it to the layout
		const bool isReadOnly = (attributeSettings->IsEnabled() == false) || (disableAttributes == true);
		AttributeWidget* attributeWidget = GetQtBaseManager()->GetAttributeWidgetFactory()->CreateAttributeWidget( attribute, attributeSettings, object, isReadOnly);
		Property* property = mPropertyTreeWidget->AddProperty( parentGroupName, attributeSettings->GetName(), attributeWidget, attribute, attributeSettings, false);
		mAttributeLinks.AddEmpty();
		mAttributeLinks.GetLast().mProperty = property;
		mAttributeLinks.GetLast().mAttributeIndex = i;
		mAttributeLinks.GetLast().mGraphObject = object;
		mAttributeLinks.GetLast().mWidget = attributeWidget;

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

			if (mNameProperty != NULL)
				setTabOrder( mNameProperty->GetAttributeWidget(), tablableWidget );
		}
	}
}


// find the index for the given button
uint32 GraphAttributesWidget::FindRemoveButtonIndex(QObject* button) const
{
	// for all table entries
	const uint32 numButtons = mRemoveButtonTable.Size();
	for (uint32 i=0; i<numButtons; ++i)
	{
		if (mRemoveButtonTable[i].mButton == button)	// this is button we search for
			return mRemoveButtonTable[i].mIndex;
	}

	return CORE_INVALIDINDEX32;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Connections
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GraphAttributesWidget::InitForConnection(Connection* connection)
{
	mPropertyTreeWidget->Clear();
	mAttributeLinks.Clear(false);
	mGraphObject = connection;
	mAttributeSet = NULL;
	mNameProperty = NULL;
	
	// if there is no condition, leave
	if (connection == NULL)
		return;

	// add attributes
	bool disableAttributes = GetSession()->IsRunning() == true;
	AddAttributes( connection, mParentGroupName.AsChar(), disableAttributes );

	// add conditions only for state transition objects
	if (connection->GetBaseType() == StateTransition::BASE_TYPE &&
		connection->GetType() == StateTransition::TYPE_ID)
		AddConditions( connection, false );

	if (mParentGroupName.IsEmpty() == false)
		mPropertyTreeWidget->SetExpanded( mParentGroupName.AsChar(), true );

	UpdateInterface();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Transition Conditions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// add the conditions management for to a given layout
void GraphAttributesWidget::AddConditions(GraphObject* object, bool readOnly)
{
	if (object->GetType() != StateTransition::TYPE_ID)
		return;

	mRemoveButtonTable.Clear();

	const char* groupName = "Conditions";


	// create the add condition attribute widget and add it as property to the property widget
	ButtonAttributeWidget* addConditionAttributeWidget = new ButtonAttributeWidget( "", Array<Attribute*>(), NULL, NULL, readOnly );
	
	String conditionsGroupName;
	//conditionsGroupName.Format( "%s.%s", mParentGroupName.AsChar(), groupName );
	conditionsGroupName = groupName;

	mPropertyTreeWidget->AddProperty( conditionsGroupName.AsChar(), "Add Condition", addConditionAttributeWidget, NULL, NULL );

	// get the button from the property widget and adjust it
	QPushButton* addConditionButton = addConditionAttributeWidget->GetButton();
	QtBaseManager::MakeTransparentButton( addConditionButton, "/Images/Icons/Plus.png", "Add new state transition condition." );
	connect(addConditionButton, SIGNAL(clicked()), this, SLOT(OnAddConditionButtonClicked()));


	// convert the object into a state transition
	StateTransition* stateTransition = static_cast<StateTransition*>( object );

	// for all transition conditions
	String conditionName;
	String conditionGroupName;
	const uint32 numConditions = stateTransition->GetNumConditions();
	for (uint32 c=0; c<numConditions; ++c)
	{
		StateTransitionCondition* condition = stateTransition->GetCondition(c);

		// create the condition group name
		conditionName.Format( "#%i: %s", c, condition->GetReadableType() );
		
		//conditionGroupName.Format( "%s.%s.%s", mParentGroupName.AsChar(), groupName, conditionName.AsChar() );
		conditionGroupName.Format( "%s.%s", groupName, conditionName.AsChar() );

		// create the remove condition attribute widget and add it as property to the property widget
		ButtonAttributeWidget* removeConditionAttributeWidget = new ButtonAttributeWidget( "", Array<Attribute*>(), NULL, NULL, readOnly );
		mPropertyTreeWidget->AddProperty( conditionGroupName.AsChar(), "Remove Condition", removeConditionAttributeWidget, NULL, NULL );

		// get the button from the property widget and adjust it
		QPushButton* removeConditionButton = removeConditionAttributeWidget->GetButton();
		QtBaseManager::MakeTransparentButton( removeConditionButton, "/Images/Icons/Remove.png", "Remove condition from the transition." );
		connect(removeConditionButton, SIGNAL(clicked()), this, SLOT(OnRemoveCondition()));

		// add the remove button to the table, so we know this button would remove what condition
		mRemoveButtonTable.AddEmpty();
		mRemoveButtonTable.GetLast().mButton	= removeConditionButton;
		mRemoveButtonTable.GetLast().mIndex		= c;

		// add condition attributes
		AddAttributes( condition, conditionGroupName.AsChar(), readOnly );

		// expand the condition group
		mPropertyTreeWidget->SetExpanded( conditionGroupName.AsChar(), true );
	}

	// expand the conditions group
	//mTempString.Format( "%s.%s", mParentGroupName.AsChar(), groupName );
	mTempString = groupName;
	mPropertyTreeWidget->SetExpanded( mTempString.AsChar(), true );
}


// when we press the add conditions button
void GraphAttributesWidget::OnAddConditionButtonClicked()
{
	if (mGraphObject == NULL || mGraphObject->GetType() != StateTransition::TYPE_ID )
		return;

	// get the sender push button
	CORE_ASSERT( sender()->inherits("QPushButton") == true );
	QPushButton* addConditionButton = qobject_cast<QPushButton*>( sender() );

	// add context menu

	QMenu menu(this);
	menu.setVisible(false);

	// for all registered objects, add it in case the object is a condition
	const uint32 numObjects = GetGraphObjectFactory()->GetNumRegisteredObjects();
	for (uint32 i=0; i<numObjects; ++i)
	{
		// get the object and skip in case the object is no action
		GraphObject* object = GetGraphObjectFactory()->GetRegisteredObject(i);
		if (object->GetBaseType() != StateTransitionCondition::BASE_TYPE)
			continue;

		QAction* addAction = menu.addAction( object->GetReadableType(), this, SLOT(OnAddCondition()) );
		addAction->setData( object->GetType() );
		//addAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/PlusFolder.png") );
	}

	if (menu.isEmpty() == true)
		return;

	menu.exec( addConditionButton->mapToGlobal(addConditionButton->rect().bottomLeft()) );
	menu.setVisible(true);
}


void GraphAttributesWidget::OnAddCondition()
{
	// get the sender action
	CORE_ASSERT( sender()->inherits("QAction") == true );
	QAction* qAction= qobject_cast<QAction*>( sender() );

	// get the selected transition condition type
	const uint32 selectedConditionType = qAction->data().toInt();

	// add it to the transition
	StateTransition* transition = static_cast<StateTransition*>( mGraphObject );
	
	// create the new condition object
	GraphObject* object = GetGraphObjectFactory()->CreateObjectByTypeID( transition->GetStateMachine(), selectedConditionType );
	if (object == NULL)
		return;

	CORE_ASSERT( object->GetBaseType() == StateTransitionCondition::BASE_TYPE );

	// add the condition to the transition
	StateTransitionCondition* condition = static_cast<StateTransitionCondition*>(object);

	// add the condition to the transition
	transition->GetStateMachine()->AddCondition(transition, condition);

	// reinit the interface
	ForceReInit();
}


// when we press the remove condition button
void GraphAttributesWidget::OnRemoveCondition()
{
	if (mGraphObject == NULL || mGraphObject->GetType() != StateTransition::TYPE_ID )
		return;

	// find the condition to remove
	const uint32 index = FindRemoveButtonIndex( sender() );
	if (index == CORE_INVALIDINDEX32)
		return;

	GraphObject* transitionObject = mGraphObject;

	// clear the interface
	InitForGraphObject( NULL, true );

	// remove the condition
	StateTransition* transition = static_cast<StateTransition*>( transitionObject );
	transition->GetStateMachine()->RemoveCondition( transition,  mRemoveButtonTable[index].mIndex );

	// reinit the interface
	InitForGraphObject( transitionObject, true );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// State Actions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// add the actions management for to a given layout
void GraphAttributesWidget::AddActions(GraphObject* object, bool readOnly)
{
	if (object->GetType() != ActionState::TYPE_ID)
		return;

	// convert the object into a action state
	ActionState* state = static_cast<ActionState*>(object);

	mRemoveButtonTable.Clear();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// On Enter Actions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const char* groupName = "On-Enter Actions";

	// create the add action attribute widget and add it as property to the property widget
	ButtonAttributeWidget* addActionAttributeWidget = new ButtonAttributeWidget( "", Array<Attribute*>(), NULL, NULL, readOnly );
	
	String actionsGroupName;
	//actionsGroupName.Format( "%s.%s", mParentGroupName.AsChar(), groupName );
	actionsGroupName = groupName;

	mPropertyTreeWidget->AddProperty( actionsGroupName.AsChar(), "Add Action", addActionAttributeWidget, NULL, NULL );

	// get the button from the property widget and adjust it
	QPushButton* addActionButton = addActionAttributeWidget->GetButton();
	QtBaseManager::MakeTransparentButton( addActionButton, "/Images/Icons/Plus.png", "Add new action." );
	connect(addActionButton, SIGNAL(clicked()), this, SLOT(OnAddEnterActionButtonClicked()));


	// for all state actions
	String actionName;
	String actionGroupName;
	uint32 numActions = state->GetOnEnterActions().GetNumActions();
	for (uint32 a=0; a<numActions; ++a)
	{
		Action* action = state->GetOnEnterActions().GetAction(a);

		// create the action group name
		actionName.Format( "#%i: %s", a, action->GetReadableType() );
		
		//actionGroupName.Format( "%s.%s.%s", mParentGroupName.AsChar(), groupName, actionName.AsChar() );
		actionGroupName.Format( "%s.%s", groupName, actionName.AsChar() );

		// create the remove action attribute widget and add it as property to the property widget
		ButtonAttributeWidget* removeActionAttributeWidget = new ButtonAttributeWidget( "", Array<Attribute*>(), NULL, NULL, readOnly );
		mPropertyTreeWidget->AddProperty( actionGroupName.AsChar(), "Remove Action", removeActionAttributeWidget, NULL, NULL );

		// get the button from the property widget and adjust it
		QPushButton* removeActionButton = removeActionAttributeWidget->GetButton();
		QtBaseManager::MakeTransparentButton( removeActionButton, "/Images/Icons/Remove.png", "Remove action." );
		connect(removeActionButton, SIGNAL(clicked()), this, SLOT(OnRemoveEnterAction()));

		// add the remove button to the table, so we know this button would remove what action
		mRemoveButtonTable.AddEmpty();
		mRemoveButtonTable.GetLast().mButton	= removeActionButton;
		mRemoveButtonTable.GetLast().mIndex		= a;

		// add action attributes
		AddAttributes( action, actionGroupName, readOnly );

		// expand the action group
		mPropertyTreeWidget->SetExpanded( actionGroupName.AsChar(), true );
	}

	// expand the actions group
	//mTempString.Format( "%s.%s", mParentGroupName.AsChar(), groupName );
	mTempString = groupName;
	mPropertyTreeWidget->SetExpanded( mTempString.AsChar(), true );

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// On Exit Actions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	groupName = "On-Exit Actions";

	// create the add action attribute widget and add it as property to the property widget
	addActionAttributeWidget = new ButtonAttributeWidget( "", Array<Attribute*>(), NULL, NULL, readOnly );

	//actionsGroupName.Format( "%s.%s", mParentGroupName.AsChar(), groupName );
	actionsGroupName = groupName;

	mPropertyTreeWidget->AddProperty( actionsGroupName.AsChar(), "Add Action", addActionAttributeWidget, NULL, NULL );

	// get the button from the property widget and adjust it
	addActionButton = addActionAttributeWidget->GetButton();
	QtBaseManager::MakeTransparentButton( addActionButton, "/Images/Icons/Plus.png", "Add new action." );
	connect(addActionButton, SIGNAL(clicked()), this, SLOT(OnAddExitActionButtonClicked()));

	// for all state actions
	numActions = state->GetOnExitActions().GetNumActions();
	for (uint32 a=0; a<numActions; ++a)
	{
		Action* action = state->GetOnExitActions().GetAction(a);

		// create the action group name
		actionName.Format( "#%i: %s", a, action->GetReadableType() );
		
		//actionGroupName.Format( "%s.%s.%s", mParentGroupName.AsChar(), groupName, actionName.AsChar() );
		actionGroupName.Format( "%s.%s", groupName, actionName.AsChar() );

		// create the remove action attribute widget and add it as property to the property widget
		ButtonAttributeWidget* removeActionAttributeWidget = new ButtonAttributeWidget( "", Array<Attribute*>(), NULL, NULL, readOnly );
		mPropertyTreeWidget->AddProperty( actionGroupName.AsChar(), "Remove Action", removeActionAttributeWidget, NULL, NULL );

		// get the button from the property widget and adjust it
		QPushButton* removeActionButton = removeActionAttributeWidget->GetButton();
		QtBaseManager::MakeTransparentButton( removeActionButton, "/Images/Icons/Remove.png", "Remove action." );
		connect(removeActionButton, SIGNAL(clicked()), this, SLOT(OnRemoveExitAction()));

		// add the remove button to the table, so we know this button would remove what action
		mRemoveButtonTable.AddEmpty();
		mRemoveButtonTable.GetLast().mButton	= removeActionButton;
		mRemoveButtonTable.GetLast().mIndex		= a;

		// add action attributes
		AddAttributes( action, actionGroupName, readOnly );

		// expand the action group
		mPropertyTreeWidget->SetExpanded( actionGroupName.AsChar(), true );
	}

	// expand the actions group
	//mTempString.Format( "%s.%s", mParentGroupName.AsChar(), groupName );
	mTempString = groupName;
	mPropertyTreeWidget->SetExpanded( mTempString.AsChar(), true );
}


// when we press the add actions button
void GraphAttributesWidget::OnAddActionButtonClicked(uint32 actionTypeIndex)
{
	if (mGraphObject == NULL || mGraphObject->GetType() != ActionState::TYPE_ID )
		return;

	// get the sender push button
	CORE_ASSERT( sender()->inherits("QPushButton") == true );
	QPushButton* addActionButton = qobject_cast<QPushButton*>( sender() );

	// add context menu

	QMenu menu(this);
	menu.setVisible(false);

	// for all registered objects, add it in case the object is an action
	String actionReadableType;
	const uint32 numObjects = GetGraphObjectFactory()->GetNumRegisteredObjects();
	for (uint32 i=0; i<numObjects; ++i)
	{
		// get the object and skip in case the object is no action
		GraphObject* object = GetGraphObjectFactory()->GetRegisteredObject(i);
		if (object->GetBaseType() != Action::BASE_TYPE)
			continue;

		QAction* addAction = NULL;

		// construct the readable type
		actionReadableType = object->GetReadableType();
		actionReadableType.RemoveAllParts(" Action");

		if (actionTypeIndex == 0)
			addAction = menu.addAction( actionReadableType.AsChar(), this, SLOT(OnAddEnterAction()) );
		else
			addAction = menu.addAction( actionReadableType.AsChar(), this, SLOT(OnAddExitAction()) );

		addAction->setData( object->GetType() );
		//addAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/PlusFolder.png") );
	}

	if (menu.isEmpty() == true)
		return;

	menu.exec( addActionButton->mapToGlobal(addActionButton->rect().bottomLeft()) );
	menu.setVisible(true);
}


// actually adds the action after using the button's context menu
void GraphAttributesWidget::OnAddAction(uint32 actionTypeIndex)
{
	// get the sender action
	CORE_ASSERT( sender()->inherits("QAction") == true );
	QAction* qAction= qobject_cast<QAction*>( sender() );

	// get the selected transition action type
	const uint32 actionType = qAction->data().toInt();

	ActionState* state = static_cast<ActionState*>(mGraphObject);
	
	// create the new action object
	GraphObject* object = GetGraphObjectFactory()->CreateObjectByTypeID( state->GetParent(), actionType );
	if (object == NULL)
		return;

	CORE_ASSERT( object->GetBaseType() == Action::BASE_TYPE );

	// add action to state
	Action* action = static_cast<Action*>(object);

	if (actionTypeIndex == 0)
		state->GetOnEnterActions().Add(action);
	else
		state->GetOnExitActions().Add(action);

	// reinit the interface
	ForceReInit();
}


// when we press the remove action button
void GraphAttributesWidget::OnRemoveAction(uint32 actionTypeIndex)
{
	if (mGraphObject == NULL || mGraphObject->GetType() != ActionState::TYPE_ID)
		return;

	// find the action to remove
	const uint32 index = FindRemoveButtonIndex( sender() );
	if (index == CORE_INVALIDINDEX32)
		return;

	GraphObject* stateObject = mGraphObject;

	// clear the interface
	InitForGraphObject( NULL, true );

	// remove the action
	ActionState* state = static_cast<ActionState*>(stateObject);

	if (actionTypeIndex == 0)
		state->GetOnEnterActions().RemoveByIndex( mRemoveButtonTable[index].mIndex );
	else
		state->GetOnExitActions().RemoveByIndex( mRemoveButtonTable[index].mIndex );

	// reinit the interface
	InitForGraphObject( stateObject, true );
}
