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
#include "Node.h"
#include "../EngineManager.h"
#include "../Core/EventManager.h"
#include "Graph.h"


using namespace Core;

// default constructor
Node::Node(Graph* parentGraph) : GraphObject(parentGraph)
{
	mIsDirty			= false;
	mPosX				= 0;
	mPosY				= 0;
	mCollapsedState		= COLLAPSE_NONE;
	mIsUpdateReady		= false;
	mIsFirstUpdateReady = true;
	mIsInitialized		= false;

	Reset();
}


// destructor
Node::~Node()
{
}


void Node::Reset()
{
	GraphObject::Reset();
	mDoAsyncReset = false;
}


// set if the graph node is collapsed or not
void Node::SetCollapsedState(ECollapsedState state)
{ 
	mCollapsedState = state;

	// set the dirty flag
	SetIsDirty(true);

	if (mParentGraph != NULL)
		EMIT_EVENT( OnGraphModified(mParentGraph, this) );
}


// adjust the visual position of the graph node
void Node::SetVisualPos(int32 x, int32 y)
{ 
	mPosX=x; mPosY=y;

	const bool wasDirty = mIsDirty;

	// set the dirty flag
	SetIsDirty(true);

	// small trick to supress excessive events while dragging nodes in the graph: only emit the graph change event once when the classifier becomes dirty
	if (mParentGraph != NULL && wasDirty == false)
		EMIT_EVENT( OnGraphModified(mParentGraph, this) );
}


// set the name of the graph node
void Node::SetName(const char* name)
{
	// remember the old name for the event
	String oldName = GetName();

	// actually set the name
	GraphObject::SetName(name);

	if (mParentGraph != NULL)
		mParentGraph->OnRenamedNode( mParentGraph, this, oldName );
	
	EMIT_EVENT( OnNodeRenamed( mParentGraph, this, oldName ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ports
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// find a given input port number
uint32 Node::FindInputPortIndex(const char* internalName) const
{
	const uint32 numPorts = mInputPorts.Size();
	for (uint32 i=0; i<numPorts; ++i)
	{
		// if the port name is equal to the name we are searching for, return the index
		if (mInputPorts[i].GetInternalNameString().IsEqual(internalName) == true)
			return i;
	}

	return CORE_INVALIDINDEX32;
}


// find the input port index, based on the port id
uint32 Node::FindInputPortByID(uint32 portID) const
{
	// get the number of input ports and iterate through them
	const uint32 numPorts = mInputPorts.Size();
	for (uint32 i=0; i<numPorts; ++i)
		if (mInputPorts[i].GetPortId() == portID)
			return i;

	return CORE_INVALIDINDEX32;
}


// find a given output port number
uint32 Node::FindOutputPortIndex(const char* internalName) const
{
	const uint32 numPorts = mOutputPorts.Size();
	for (uint32 i=0; i<numPorts; ++i)
	{
		// if the port name is equal to the name we are searching for, return the index
		if (mOutputPorts[i].GetInternalNameString().IsEqual(internalName) == true)
			return i;
	}

	return CORE_INVALIDINDEX32;
}


// find the output port index, based on the port id
uint32 Node::FindOutputPortByID(uint32 portID) const
{
	// get the number of output ports and iterate through them
	const uint32 numPorts = mOutputPorts.Size();
	for (uint32 i=0; i<numPorts; ++i)
		if (mOutputPorts[i].GetPortId() == portID)
			return i;

	return CORE_INVALIDINDEX32;
}


// remove input port and the incoming connection
void Node::RemoveInputPort(uint32 index)
{
	// find and remove incoming connection (if any)
	uint32 conIndex = mParentGraph->FindInputConnection(this, index);
	if (conIndex != CORE_INVALIDINDEX32)
		mParentGraph->RemoveConnection(mParentGraph->GetConnection(conIndex));

	// remove port
	mInputPorts.Remove(index);
}


// remove output port and all connected connections
void Node::RemoveOutputPort(uint32 index)
{
	// remove all connections from this port until they are gone
	uint32 conIndex = mParentGraph->FindOutputConnection(this, index, 0);
	while (conIndex != CORE_INVALIDINDEX32)
	{
		// remove connection
		mParentGraph->RemoveConnection( mParentGraph->GetConnection(conIndex) );

		// find next connection to remove
		conIndex = mParentGraph->FindOutputConnection(this, index, 0);
	}

	// remove port
	mOutputPorts.Remove(index);
}

	// check if we already registered this port ID
	//const uint32 duplicatePort = FindOutputPortByID( portID );
	//if (duplicatePort != CORE_INVALIDINDEX32)
	//	LogWarning("NodeSetOutputPort() - There is already a port with the same ID (portID=%d existingPort='%s' newPort='%s' name='%s')", portID, mOutputPorts[duplicatePort].GetName(), name, GetTypeString());


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Connections
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// checks if there are connections connected to any port
bool Node::HasConnections() const
{
	// get the number of output ports and iterate through them
	const uint32 numOutputPorts = mOutputPorts.Size();
	for (uint32 i = 0; i < numOutputPorts; ++i)
		if (mOutputPorts[i].HasConnection() == true)
			return true;
	
	// get the number of input ports and iterate through them
	const uint32 numInputPorts = mInputPorts.Size();
	for (uint32 i=0; i<numInputPorts; ++i)
		if (mInputPorts[i].HasConnection())
			return true;

	return false;
}


// check if any of the input ports has a connection attached
bool Node::HasIncomingConnections() const
{
	// get the number of input ports and iterate through them
	const uint32 numInputPorts = mInputPorts.Size();
	for (uint32 i=0; i<numInputPorts; ++i)
		if (mInputPorts[i].HasConnection() == true)
			return true;

	return false;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Serialization
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// save the node
Json::Item Node::Save(Json& json, Json::Item& item)
{
	// save graph object and attributes
	Json::Item objectItem = GraphObject::Save(json, item);

	objectItem.AddInt( "posX",		GetVisualPosX() );
	objectItem.AddInt( "posY",		GetVisualPosY() );
	objectItem.AddInt( "collapsed",	GetCollapsedState() );

	return objectItem;
}


// load the node
bool Node::Load(const Json& json, const Json::Item& item)
{
	bool success = true;

	// load graph object and attributes
	if (GraphObject::Load(json, item) == false)
		success = false;
	
	SetEmitEvents(false);

	// set the visual position
	Json::Item posXItem = item.Find("posX");
	Json::Item posYItem = item.Find("posY");
	if (posXItem.IsNumber() == true && posYItem.IsNumber() == true)
		SetVisualPos( posXItem.GetInt(), posYItem.GetInt() );

	// set the is collapsed flag
	Json::Item collapseItem = item.Find("collapsed");
	if (collapseItem.IsInt() == true)
		SetCollapsedState( (Node::ECollapsedState)collapseItem.GetInt() );

	mIsDirty = false;

	SetEmitEvents(true);

	return success;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// recursive Updating and ReInitialization
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// update all incoming nodes
void Node::UpdateAllIncomingNodes(const Time& elapsed, const Time& delta)
{
	// get the number of connections, iterate through them and update all source nodes
	const uint32 numPorts = mInputPorts.Size();
	for (uint32 i=0; i<numPorts; ++i)
	{
		Connection* connection = mInputPorts[i].GetConnection();
		if (connection != NULL)
			connection->GetSourceNode()->Update(elapsed, delta);
	}
}


// shared basis update helper
bool Node::BaseUpdate(const Time& elapsed, const Time& delta)
{
	// skip the very first update of this node
	if (mIsFirstUpdateReady == false)
	{
		mIsFirstUpdateReady = true;
		return false;
	}

	// node was already updated
	if (IsUpdateReady() == true)
		return false;

	SetUpdateReady(true);

	// update all inputs
	UpdateAllIncomingNodes(elapsed, delta);
	return true;
}


// recursive reinit
bool Node::BaseReInit(const Time& elapsed, const Time& delta)
{
	if (IsReInitReady() == true)
		return false;

	SetReInitReady(true);

	ReInitAllIncomingNodes(elapsed, delta);
	return true;
}


// reinit all parent nodes
void Node::ReInitAllIncomingNodes(const Time& elapsed, const Time& delta)
{
	// propagate ReInit to all source nodes recursively
	const uint32 numInPorts = GetNumInputPorts();
	for (uint32 i=0; i<numInPorts; ++i)
	{
		Connection* connection = mInputPorts[i].GetConnection();
		if (connection != NULL)
			connection->GetSourceNode()->ReInit(elapsed, delta);
	}
}
	

void Node::InitInputPorts(uint32 numPorts)
{
	uint32 currentNumPorts = GetNumInputPorts();

	// shrink number of input ports -> remove the connections first
	while (currentNumPorts > numPorts)
	{
		const uint32 portIndex = mInputPorts.Size() - 1;
		RemoveInputPort(portIndex);
		currentNumPorts--;
	}
		
	// call resize (for increasing number of ports)
	mInputPorts.Resize( numPorts ); 
}


void Node::InitOutputPorts(uint32 numPorts)
{
	uint32 currentNumPorts = GetNumOutputPorts();

	// shrink number of output ports -> remove the connections first
	while (currentNumPorts > numPorts)
	{
		const uint32 portIndex = mOutputPorts.Size() - 1;
		RemoveOutputPort(portIndex);
		currentNumPorts--;
	}
		
	// call resize (for increasing number of ports)
	mOutputPorts.Resize( numPorts ); 
}

// find if a node is a parent of another node (also true if node == this)
bool Node::FindNodeOnInputs(Node* node)
{
	if (this == node)
		return true;

	const uint32 numInPorts = GetNumInputPorts();
	if (numInPorts == 0)
		return false;

	// check parent nodes recursively
	bool found = false;
	for (uint32 i=0; i<numInPorts && !found; ++i)
		if (GetInputPort(i).HasConnection() == true)
			found |= GetSourceNode(i)->FindNodeOnInputs(node);

	return found;
}

// find if a node is a child of another node (also true if node == this)
bool Node::FindNodeOnOutputs(Node* node)
{
	if (this == node)
		return true;

	const uint32 numOutPorts = GetNumOutputPorts();
	if (numOutPorts == 0)
		return false;

	// check child nodes recursively
	bool found = false;
	for (uint32 i=0; i<numOutPorts && !found; ++i)
	{
		const uint32 numChildNodes = GetNumChildNodes(i);
		for (uint32 n=0; n<numChildNodes && !found; ++n)
			found |= GetChildNode(i, n)->FindNodeOnOutputs(node);
	}

	return found;
}


// get 
Node* Node::GetSourceNode(uint32 inputPortIndex)
{
	if (GetInputPort(inputPortIndex).HasConnection() == false)
		return NULL;

	return GetInputPort(inputPortIndex).GetConnection()->GetSourceNode();
}


uint32 Node::GetNumChildNodes(uint32 outputPortIndex)
{
	return mParentGraph->CalcNumOutputConnections(this, outputPortIndex);
}


Node* Node::GetChildNode(uint32 outputPortIndex, uint32 nodeIndex)
{
	const uint32 connectionIndex = mParentGraph->FindOutputConnection(this, outputPortIndex, nodeIndex);
	Connection* connection = mParentGraph->GetConnection(connectionIndex);
	return connection->GetTargetNode();
}
