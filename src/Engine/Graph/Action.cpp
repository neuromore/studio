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

// include the required headers
#include "Action.h"
#include "StateMachine.h"


using namespace Core;

// constructor
Action::Action(Graph* graph) : GraphObject(graph)
{
	mParentStateMachine = NULL;
	if (graph != NULL)
		mParentStateMachine = static_cast<StateMachine*>(graph);
}


// destructor
Action::~Action()
{
}


// save the action
Json::Item Action::Save(Json& json, Json::Item& item)
{
	// add the action item
	Json::Item actionItem = item.AddObject();

	// add type
	actionItem.AddString( "type", GetTypeUuid() );

	// add attributes from attribute set
	Write( json, actionItem, true );

	return actionItem;
}
