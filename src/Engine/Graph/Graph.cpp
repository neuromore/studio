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
#include <Engine/Precompiled.h>

// include required headers
#include "Graph.h"
#include "Node.h"
#include "../Core/EventManager.h"
#include "../EngineManager.h"
#include "GraphImporter.h"
#include "GraphExporter.h"


using namespace Core;

// default constructor
Graph::Graph(Graph* parentGraph) : GraphObject(parentGraph)
{
	// initialize to invalid revision
	mRevision = CORE_INVALIDINDEX32;
	mUseGraphSettings = false;
}


// destructor
Graph::~Graph()
{
	// remove all nodes, connections
	DestructArray(mConnections);
	DestructArray(mNodes);
}


// rewind all child nodes and connections
void Graph::Reset()
{
	// get the number of nodes and iterate through them
	const uint32 numNodes = mNodes.Size();
	for (uint32 i=0; i<numNodes; ++i)
		mNodes[i]->Reset();

	const uint32 numConnections = mConnections.Size();
	for (uint32 i=0; i<numConnections; ++i)
		mConnections[i]->Reset();

	// collect all objects again
	CollectObjects();

	EMIT_EVENT( OnGraphReset(this) );
}


void Graph::CollectObjects()
{
	// TODO enable for nested graphs
	/*
	// collect all child graphs
	mGraphs.Clear();
	const uint32 numNodes = mNodes.Size();
	for (uint32 i=0; i<numNodes; ++i)
	{
		if (mNodes[i]->GetBaseType() == NestedGraph::BASE_TYPE_ID)
			mGraphs.Add(static_cast<NestedClassifierNode*>(mNodes[i]));
	}
	*/

	// call CollectObjects of all child objects
	const uint32 numObjects = mObjects.Size();
	for (uint32 i=0; i<numObjects; ++i)
	{
		mObjects[i]->CollectObjects();
	}
}


bool Graph::AllowEditing()
{
	// first: the lock state
	if (mIsLocked == true)
		return false;

	// second: no update permissions
	if (mCreud.Update() == false)
		return false;

	// third: graph is in settings mode
	if (mUseGraphSettings == true)
		return false;

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Child nodes
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// add a graph node
void Graph::AddNode(Node* node)
{
	mNodes.Add(node); 
	mObjects.Add(node);
	node->SetParent(this);

	// graph callback
	OnGraphModified(this, node);
	OnCreatedNode(this, node);
	// TODO add tochild graph list if node is a graph

	// fire event
	EMIT_EVENT( OnNodeAdded(this, node) );
	EMIT_EVENT( OnGraphModified(this, node) );

	// set the dirty flag
	SetIsDirty(true);
}


// remove a node by pointer
bool Graph::RemoveNode(Node* node)
{
	// find the index of the given node in the node array and remove it in case the index is valid
	const uint32 index = FindNodeIndex(node);
	if (index == CORE_INVALIDINDEX32)
	{
		LogError( "Graph::RemoveNode(): Cannot remove node '%s' from graph '%s'. Node is not part of the graph.", node->GetName(), GetName() );
		return false;
	}

	// get the node to remove
	Node* nodeToRemove = mNodes[index];
	
	// graph callback
	OnRemoveNode(this, node);

	// fire pre events
	EMIT_EVENT( OnRemoveNode(this, nodeToRemove) );

	// delete all attached connections
	RemoveConnectionsUsingNode(nodeToRemove);

	// delete the node from the array
	mNodes.Remove( index );
	mObjects.RemoveByValue(node);
	// TODO remove from child graph list if node is a graph

	// delete the node from memory
	delete nodeToRemove;

	// graph callback
	OnGraphModified(this, nodeToRemove);

	// fire post events
	EMIT_EVENT( OnNodeRemoved(this, nodeToRemove) );

	// set the dirty flag
	SetIsDirty(true);
	return true;
}


/**
 * Find node by name. This will only iterate through the nodes and isn't a recursive process.
 * @param[in] name The name of the node to search.
 * @return A pointer to the node with the given name in case of success, in the other case a NULL pointer will be returned.
 */
Node* Graph::FindNodeByName(const char* name, const uint32 typeId)
{
	// get the number of nodes and iterate through them
	const uint32 numNodes = mNodes.Size();
	for (uint32 i=0; i<numNodes; ++i)
	{
		// compare the node name with the parameter and return a pointer to the node in case they are equal
		if (mNodes[i]->GetNameString().IsEqualNoCase(name) && (typeId == 0 || mNodes[i]->GetType() == typeId))
			return mNodes[i];
	}

	// failure, return NULL pointer
	return NULL;
}


Node* Graph::FindNodeByUuid(const char* uuid)
{
	// get the number of nodes and iterate through them
	const uint32 numNodes = mNodes.Size();
	for (uint32 i=0; i<numNodes; ++i)
	{
		// compare the node name with the parameter and return a pointer to the node in case they are equal
		if (mNodes[i]->GetUuidString().IsEqualNoCase(uuid))
			return mNodes[i];
	}

	// failure, return NULL pointer
	return NULL;
}


/**
 * Find node index by name. This will only iterate through the nodes and isn't a recursive process.
 * @param[in] name The name of the node to search.
 * @return The index of the node with the given name in case of success, in the other case CORE_INVALIDINDEX32 will be returned.
 */
uint32 Graph::FindNodeIndexByName(const char* name) const
{
	// get the number of nodes and iterate through them
	const uint32 numNodes = mNodes.Size();
	for (uint32 i=0; i<numNodes; ++i)
	{
		// compare the node name with the parameter and return the relative node index in case they are equal
		if (mNodes[i]->GetNameString().IsEqualNoCase(name))
			return i;
	}

	// failure, return invalid index
	return CORE_INVALIDINDEX32;
}


/**
 * Find node index. This will only iterate through the nodes and isn't a recursive process.
 * @param[in] node A pointer to the node for which we want to find the node index.
 * @return The index of the node in case of success, in the other case CORE_INVALIDINDEX32 will be returned.
 */
uint32 Graph::FindNodeIndex(Node* node) const
{
	return mNodes.Find(node);
}


// collect nodes of the given type
void Graph::CollectNodesOfType(uint32 nodeTypeID, Array<Node*>* outNodes)
{
	// get the number of nodes and iterate through them
	const uint32 numNodes = mNodes.Size();
	for (uint32 i=0; i<numNodes; ++i)
	{
		Node* node = mNodes[i];

		// check the current node type and add it to the output array in case they are the same
		if (node->GetType() == nodeTypeID)
			outNodes->Add( node );
	}
}


bool Graph::ContainsDeprecatedGraphObjects()
{
	// get the number of nodes and iterate through them
	const uint32 numNodes = mNodes.Size();
	for (uint32 i=0; i<numNodes; ++i)
	{
		Node* node = mNodes[i];
		if (node->IsDeprecated() == true)
			return true;
	}

	// TODO also check all child graphs recursively by calling this method
    return false;
}


bool Graph::ContainsUnstableGraphObjects()
{
	// get the number of nodes and iterate through them
	const uint32 numNodes = mNodes.Size();
	for (uint32 i=0; i<numNodes; ++i)
	{
		Node* node = mNodes[i];
		if (node->IsUnstable() == true)
			return true;
	}

	// TODO also check all child graphs recursively by calling this method
    return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Connections
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// DEPRECATED
Connection* Graph::AddConnection(Node* sourceNode, uint32 sourcePortNr, Node* targetNode, uint32 targetPortNr, int32 startOffsetX, int32 startOffsetY, int32 endOffsetX, int32 endOffsetY)
{
	// disallow direct loops
	if (sourceNode == targetNode)
	{
		LogError("Graph::AddConnection(): Source and target node are equal '%s'. Cannot create looping connection.", sourceNode->GetName());
		return NULL;
	}

	// make sure the source and target ports are in range
	if (targetNode->IsValidInputPortIndex(targetPortNr) == false || sourceNode->IsValidOutputPortIndex(sourcePortNr) == false)
	{
		LogError("Graph::AddConnection(): Source (%s, %i/%i) or target port (%s, %i/%i) is not in a valid range. Cannot create connection.", sourceNode->GetName(), sourcePortNr, sourceNode->GetNumOutputPorts(), targetNode->GetName(), targetPortNr, targetNode->GetNumInputPorts());
		return NULL;
	}

	Connection* connection = new Connection(sourceNode, targetNode, sourcePortNr, targetPortNr);
	AddConnection(connection);
	return connection;
}


// add the given new connection
bool Graph::AddConnection(Connection* connection)
{
	// create the connection and add it to the graph
	mConnections.Add(connection);
	mObjects.Add(connection);

	// graph callbacks
	OnGraphModified(this, connection);
	OnCreatedConnection(this, connection);

	// fire signal after connection got added
	EMIT_EVENT(OnConnectionAdded(this, connection));
	EMIT_EVENT(OnGraphModified(this, connection));

	// set the dirty flag
	SetIsDirty(true);

	return connection;
}


// remove a given connection
void Graph::RemoveConnection(Connection* connection)
{
	// try to find the connection index by pointer
	const uint32 index = FindConnectionIndex(connection);
	if (index == CORE_INVALIDINDEX32)
	{
		LogError( "Graph::RemoveConnection(): Cannot remove connection 0x%x. Connection is not part of graph '%s'.", connection, GetName() );
		return;
	}

	// actually remove the connection

	// fire pre events
	EMIT_EVENT( OnRemoveConnection(this, connection) );

	// destroy the memory for the connection and remove it from the connection array
	// this also deletes the references in source and target port
	mConnections.Remove(index);
	mObjects.RemoveByValue(connection);

	delete connection;

	// graph callback
	OnGraphModified(this, connection);

	// fire post events
	EMIT_EVENT( OnConnectionRemoved(this, connection) );

	// set the dirty flag
	SetIsDirty(true);
}


// remove all connections connected to a node
void Graph::RemoveConnectionsUsingNode(const Node* node)
{
	for (uint32 i=0; i<mConnections.Size();)
	{
		// if this connection comes from the specified node, delete it 
		if (mConnections[i]->GetSourceNode() == node || mConnections[i]->GetTargetNode() == node)
			RemoveConnection(mConnections[i]);
		else
			i++;
	}
}



/**
 * Find the connection at the given port.
 * Search over the incoming connections that are stored within this node and check if they are connected at the given port.
 * @param[in] The port inside this node of connection to search for.
 * @result A pointer to the connection at the given port, NULL in case there is nothing connected to that port.
 */
Connection* Graph::FindConnection(const Node* sourceNode, uint16 sourcePort, uint16 targetPort) const
{
	const uint32 numConnections = mConnections.Size();
	for (uint32 i=0; i<numConnections; ++i)
		if (mConnections[i]->GetSourceNode() == sourceNode && mConnections[i]->GetSourcePort() == sourcePort && mConnections[i]->GetTargetPort() == targetPort)
			return mConnections[i];

	return NULL;
}


// find the number of connections arriving at a given node and port
uint32 Graph::CalcNumInputConnections(const Node* targetNode, uint16 targetPort) const
{
	uint32 count = 0;
	const uint32 numConnections = mConnections.Size();
	for (uint32 i = 0; i<numConnections; ++i)
		if (mConnections[i]->GetTargetNode() == targetNode && mConnections[i]->GetTargetPort() == targetPort)
			count++;

	return count;
}


// find a given connection going to a given node and port
uint32 Graph::FindInputConnection(const Node* targetNode, uint16 targetPort, uint32 index) const
{
	uint32 currentIndex = 0;
	const uint32 numConnections = mConnections.Size();
	for (uint32 i=0; i<numConnections; ++i)
		if (mConnections[i]->GetTargetNode() == targetNode && mConnections[i]->GetTargetPort() == targetPort)
		{
			if (currentIndex == index)
				return i;
			else
				currentIndex++;
		}

	return CORE_INVALIDINDEX32;
}


// find the number of connections originating at a given node and port
uint32 Graph::CalcNumOutputConnections(const Node* sourceNode, uint16 sourcePort) const
{
	uint32 count = 0;
	const uint32 numConnections = mConnections.Size();
	for (uint32 i=0; i<numConnections; ++i)
		if (mConnections[i]->GetSourceNode() == sourceNode && mConnections[i]->GetSourcePort() == sourcePort)
			count++;

	return count;
}


// find a given connection originating at a given node and port
uint32 Graph::FindOutputConnection(const Node* sourceNode, uint16 sourcePort, uint32 index) const
{
	uint32 currentIndex = 0;
	const uint32 numConnections = mConnections.Size();
	for (uint32 i=0; i<numConnections; ++i)
		if (mConnections[i]->GetSourceNode() == sourceNode && mConnections[i]->GetSourcePort() == sourcePort)
		{
			if (currentIndex == index)
				return i;
			else
				currentIndex++;
		}
	
	return CORE_INVALIDINDEX32;
}


// calculate the number of connections that are ending at the given node
uint32 Graph::CalcNumInputConnections(const Node* node) const
{
	uint32 result = 0;

	// get the number of connections and iterate through them
	const uint32 numConnections = mConnections.Size();
	for (uint32 i = 0; i<numConnections; ++i)
	{
		Connection* connection = mConnections[i];
		if (connection->GetTargetNode() == node)
			result++;
	}

	return result;
}


// find a given connection going into a given node (independent from ports; unordered)
uint32 Graph::FindInputConnection(const Node* targetNode, uint32 index) const
{
	uint32 currentIndex = 0;
	const uint32 numConnections = mConnections.Size();
	for (uint32 i = 0; i<numConnections; ++i)
		if (mConnections[i]->GetTargetNode() == targetNode)
		{
			if (currentIndex == index)
				return i;
			else
				currentIndex++;
		}

	return CORE_INVALIDINDEX32;
}


// calculate the number of connections that are starting at the given node
uint32 Graph::CalcNumOutputConnections(const Node* node) const
{
	uint32 result = 0;

	// get the number of connections and iterate through them
	const uint32 numConnections = mConnections.Size();
	for (uint32 i=0; i<numConnections; ++i)
	{
		Connection* connection = mConnections[i];
		if (connection->GetSourceNode() == node)
			result++;
	}

	return result;
}


// find a connection going out of a node (access them independent from the ports as an unordered list)
uint32 Graph::FindOutputConnection(const Node* sourceNode, uint32 index) const
{
	uint32 currentIndex = 0;
	const uint32 numConnections = mConnections.Size();
	for (uint32 i = 0; i<numConnections; ++i)
		if (mConnections[i]->GetSourceNode() == sourceNode)
		{
			if (currentIndex == index)
				return i;
			else
				currentIndex++;
		}

	return CORE_INVALIDINDEX32;
}


// does this node has a specific connection?
bool Graph::HasConnection(Connection* connection) const
{
	// for all input connections
	const uint32 numConnections = mConnections.Size();
	for (uint32 i=0; i<numConnections; ++i)
		if (mConnections[i] == connection)
			return true;

	return false;
}


// validate the connections
bool Graph::ValidateConnections() const
{
	// validate all connections
	const uint32 numConnections = mConnections.Size();
	for (uint32 i=0; i<numConnections; ++i)
		if (mConnections[i]->IsValid() == false)
			return false;

	return true;
}


// check if the given port already has a connection plugged in
bool Graph::HasInputConnection(const Node* targetNode, uint32 targetPortNr) const
{
	// get the number of connections and iterate through them
	const uint32 numConnections = mConnections.Size();
	for (uint32 i=0; i<numConnections; ++i)
	{
		// get the current connection and check if the connection is connected to the given port
		Connection* connection = mConnections[i];

		// check if the connection properties are equal
		if (connection->GetTargetNode() == targetNode && connection->GetTargetPort() == targetPortNr)
			return true;
	}

	// failure, connection not found
	return false;
}


// check if the given port already has a connection plugged in
bool Graph::HasOutputConnection(const Node* sourceNode, uint32 sourcePortNr) const
{
	// get the number of connections and iterate through them
	const uint32 numConnections = mConnections.Size();
	for (uint32 i = 0; i<numConnections; ++i)
	{
		// get the current connection and check if the connection is connected to the given port
		Connection* connection = mConnections[i];

		// check if the connection properties are equal
		if (connection->GetSourceNode() == sourceNode && connection->GetSourcePort() == sourcePortNr)
			return true;
	}

	// failure, connection not found
	return false;
}


// reset all output connections
void Graph::ResetOutputConnections(const Node* node)
{
	// reset all connections that originate at this node
	const uint32 numConnections = mConnections.Size();
	for (uint32 i = 0; i<numConnections; ++i)
	{
		Connection* connection = mConnections[i];
		if (connection->GetSourceNode() == node)
			connection->Reset();
	}
}


// recursively update attributes of objects that contain the attributes
bool Graph::OnAttributeChanged(Attribute* attribute)
{
	CORE_ASSERT(attribute != NULL);

	bool containsAttribute = false;

	// update this node
	if (HasAttribute(attribute) == true)
	{
		GraphObject::OnAttributesChanged();
		GraphObject::OnAttributeChanged(attribute);
		containsAttribute = true;
	}

	// forward change to all graph objects that contain the attribute
	const uint32 numObjects = mObjects.Size();
	for (uint32 i=0; i<numObjects; ++i)
	{
		if (mObjects[i]->ContainsAttribute(attribute) == true)
		{
#ifdef CORE_DEBUG
			LogDebug("OnAttributesChanged() : Attribute %s of graph object '%s' (at %x) changed", attribute->GetTypeString(), mObjects[i]->GetName(), mObjects[i]);
#endif
			mObjects[i]->OnAttributesChanged();
			mObjects[i]->OnAttributeChanged(attribute);
			
			// mark object as dirty
			mObjects[i]->SetIsDirty(true);

			EMIT_EVENT( OnAttributeUpdated(this, mObjects[i], attribute) );
			
			containsAttribute = true;
		}
	}

	// TODO recurse into child graphs and call withinHierachy |= OnAttributeChanged(attribute)

	// return true if attribute was within this graph hierarchy
	return containsAttribute;
}


bool Graph::ApplySettings(const GraphSettings& settings)
{
	uint32 numApplied = 0;

	const uint32 numObjects = mObjects.Size();
	const uint32 numSettings = settings.Size();

	// for each graph object setting
	for (uint32 i=0; i<numSettings; ++i)
	{
		const char* attributeName = settings.GetName(i);

		// graph's attributes
		const uint32 graphAttribute = FindAttributeIndexByInternalName(attributeName);
		if (graphAttribute != CORE_INVALIDINDEX32)
			GetAttributeValue(graphAttribute)->InitFromString(settings.GetValue(i));

		// find matching object in graph
		for (uint32 j=0; j<numObjects; ++j)
		{
			GraphObject* object = mObjects[j];

			// skip connections, they never have attributes
			if (object->GetBaseType() == Connection::BASE_TYPE)
				continue;
			
			// macht both object UUID and (if nothing matched) object name
			const bool uuidMatched = (settings.HasObjectUuid(i) == true && object->GetUuidString().IsEqualNoCase(settings.GetObjectUUid(i)));
			const bool typeMatched = (settings.HasObjectType(i) == true && object->GetType() == settings.GetObjectType(i));
			const bool nameMatched = (settings.HasObjectName(i) == true && object->GetUuidString().IsEqualNoCase(settings.GetObjectName(i)));

			if (uuidMatched || typeMatched || nameMatched)
			{
				uint32 attributeIndex = object->FindAttributeIndexByInternalName(attributeName);

				// settings not found; this is not an error, we don't know if the graph has the settings or not. Just continue.
				if (attributeIndex == CORE_INVALIDINDEX32)
					continue;

				Attribute* attribute = object->GetAttributeValue(attributeIndex);

				String before; attribute->ConvertToString(before);
				LogDebug("GraphSettings: Changing value of attribute '%s' (in object '%s') from '%s' to '%s'", attributeName, object->GetName(), before.AsChar(), settings.GetValue(i));

				// update attribute
				attribute->InitFromString(settings.GetValue(i));

				// make it behave like a manual change (registers it as a change so we can save graph settings again)
				OnAttributeChanged(attribute);

				numApplied++;
			}
		}
	}

	LogDebug("Changed %i attributes while applying graph settings to the graph '%s'", numApplied, GetName());

	// switch into settings mode only if settings were applied
	if (numApplied > 0)
		mUseGraphSettings = true;

	const bool success = (numApplied == settings.Size());
	return success;
}


// create (append) graph settings 
GraphSettings& Graph::CreateSettings(GraphSettings& settings, bool changesOnly)
{
	String value;
	uint32 numSettings = 0;

	const uint32 numObjects = mObjects.Size();
	for (uint32 i=0; i<numObjects; ++i)
	{
		GraphObject* object = mObjects[i];
		
		// use either all attributes, or only the modified ones
		const AttributeSet* attributes = object;
		if (changesOnly)
			attributes = &object->GetChangedAttributes();

		// add all attributes as settings to the graph
		const uint32 numAttributes = attributes->GetNumAttributes();
		for (uint32 j=0; j<numAttributes; ++j)
		{
			// convert to value string
			attributes->GetAttributeValue(j)->ConvertToString(value);

			LogDebug("GraphSettings: Creating settings value '%s' from attribute '%s' (in object '%s' / '%x')", value.AsChar(), attributes->GetAttribute(j)->GetInternalName(), object->GetName(), object->GetType());

			// add the settings (using object UUID and attribute name)
			settings.Add(object, attributes->GetAttribute(j)->GetInternalName(), value.AsChar());
			
			numSettings++;
		}
	}

	LogInfo("Created %i settings items from graph '%s'", numSettings, GetName());

	return settings;
}


// reset update ready flags for all nodes
void Graph::ResetUpdateReadyFlags()
{
	// get the number of nodes, iterate through them and reset their update ready flag
	const uint32 numNodes = mNodes.Size();
	for (uint32 i=0; i<numNodes; ++i)
		mNodes[i]->SetUpdateReady(false);

	// TODO recurse into child graphs
}


// reset reinit ready flags for all nodes
void Graph::ResetReInitReadyFlags()
{
	// get the number of nodes, iterate through them and reset their update ready flag
	const uint32 numNodes = mNodes.Size();
	for (uint32 i=0; i<numNodes; ++i)
		mNodes[i]->SetReInitReady(false);

	// TODO recurse into child graphs
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Serialization
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Json::Item Graph::Save(Json& json, Json::Item& item)
{	
	// graph base properties
	item.AddString( "name", GetName() );
	item.AddString( "uuid", GetUuid() );
	item.AddString( "type", GetTypeUuid() );

	// graph attributes
	AttributeSet::Write(json, item, false);

	// save child nodes recursively
	if (SaveNodes(json, item) == false)
		return json.NullItem();

	// save connections between the child nodes
	if (SaveConnections(json, item) == false)
		return json.NullItem();

	// reset the dirty flag
	SetIsDirty(false);

	return item;
}


// save all graph nodes
bool Graph::SaveNodes(Json& json, Json::Item& item)
{
	// get the number of nodes and check if we have to save any, if not skip saving
	const uint32 numNodes = mNodes.Size();
	if (numNodes == 0)
		return true;

	// add the nodes array item
	Json::Item nodesItem = item.AddArray("nodes");

	// get the number of nodes and iterate through them
	for (uint32 i=0; i<numNodes; ++i)
		mNodes[i]->Save( json, nodesItem );

	return true;
}


// save all connections between the child nodes
bool Graph::SaveConnections(Json& json, Json::Item& item)
{
	// get the number of connections, if there are none skip directly
	const uint32 numConnections = mConnections.Size();
	if (numConnections == 0)
		return true;

	// add the connections array item
	Json::Item connectionsItem = item.AddArray("connections");

	// iterate through all connections
	for (uint32 i=0; i<numConnections; ++i)
		mConnections[i]->Save( json, connectionsItem );

	return true;
}


// load the classifier
bool Graph::Load(const Json& json, const Json::Item& item)
{
	bool success = true;

	// 0: load attributes
	if (!AttributeSet::Read(json, item, true))
		success = false;

	// 1: load all nodes
	if (GraphImporter::LoadNodes(json, item, this) == false)
		success = false;

	// 2: load all connections
	if (GraphImporter::LoadConnections(json, item, this) == false)
		success = false;

	return success;
}


// does this graph or any of the child objects have an error?
bool Graph::HasError(uint32 errorID) const
{
	// check if any of the child objects is in an error state
	const uint32 numChildObjects = mObjects.Size();
	for (uint32 i=0; i<numChildObjects; ++i)
	{
		if (mObjects[i]->HasError(errorID) == true)
			return true;
	}

	return false;
}


// number of errors this graph and its children have, total
uint32 Graph::GetNumErrors() const
{
	uint32 numErrors = GraphObject::GetNumErrors();

	const uint32 numChildObjects = mObjects.Size();
	for (uint32 i=0; i<numChildObjects; ++i)
		numErrors += mObjects[i]->GetNumErrors();

	return numErrors;
}


Array<Graph::GraphObjectError*>& Graph::CollectErrors(Array<Graph::GraphObjectError*>& outList) 
{
	GraphObject::CollectErrors(outList);

	const uint32 numChildObjects = mObjects.Size();
	for (uint32 i=0; i<numChildObjects; ++i)
		mObjects[i]->CollectErrors(outList);

	return outList;
}


const Core::Array<Core::String>& Graph::CreateErrorList(Core::Array<Core::String>& errorList)
{
	String tmp;
	const uint32 numObjects = mObjects.Size();
	for (uint32 i = 0; i < numObjects; ++i)
	{
		GraphObject* object = mObjects[i];
		const uint32 numErrors = object->GetNumErrors();
		for (uint32 j = 0; j < numErrors; ++j)
		{
			const Graph::GraphObjectError& error = object->GetError(j);
			errorList.AddEmpty();

#ifndef PRODUCTION_BUILD
			errorList.GetLast().Format("%s: %s (E%x)", object->GetName(), error.mMessage.AsChar(), error.mErrorID);
#else		
			errorList.GetLast().Format("%s: %s", object->GetName(), error.mMessage.AsChar());
#endif
		}
	}

	// also append errors of all child graphs
	const uint32 numGraphs = mGraphs.Size();
	for (uint32 i = 0; i < numGraphs; ++i)
		mGraphs[i]->CreateErrorList(errorList);

	return errorList;
}


// does the graph or any of the child objects have a warning?
bool Graph::HasWarning() const
{
	// graph object itself has a warning
	if (GraphObject::HasWarning())
		return true;

	// check if any of the child objects has a warning
	const uint32 numChildObjects = mObjects.Size();
	for (uint32 i=0; i<numChildObjects; ++i)
		if (mObjects[i]->HasWarning() == true)
			return true;

	return false;
}


uint32 Graph::GetNumWarnings() const
{
	uint32 numWarnings = GraphObject::GetNumWarnings();

	const uint32 numChildObjects = mObjects.Size();
	for (uint32 i=0; i<numChildObjects; ++i)
		numWarnings += mObjects[i]->GetNumWarnings();

	return numWarnings;
}


Array<Graph::GraphObjectWarning*>& Graph::CollectWarnings(Array<Graph::GraphObjectWarning*>& outList) 
{
	GraphObject::CollectWarnings(outList);

	const uint32 numChildObjects = mObjects.Size();
	for (uint32 i=0; i<numChildObjects; ++i)
		mObjects[i]->CollectWarnings(outList);

	return outList;
}


const Core::Array<Core::String>& Graph::CreateWarningsList(Core::Array<Core::String>& warningsList)
{
	String tmp;
	const uint32 numObjects = mObjects.Size();
	for (uint32 i = 0; i < numObjects; ++i)
	{
		GraphObject* object = mObjects[i];
		const uint32 numWarnings = object->GetNumWarnings();
		for (uint32 j = 0; j < numWarnings; ++j)
		{
			const Graph::GraphObjectWarning& error = object->GetWarning(j);
			warningsList.AddEmpty();

#ifndef PRODUCTION_BUILD
			warningsList.GetLast().Format("%s: %s (W%x)", object->GetName(), error.mMessage.AsChar(), error.mWarningID);
#else		
			warningsList.GetLast().Format("%s: %s", object->GetName(), error.mMessage.AsChar());
#endif
		}
	}

	// also append errors of all child graphs
	const uint32 numGraphs = mGraphs.Size();
	for (uint32 i = 0; i < numGraphs; ++i)
		mGraphs[i]->CreateWarningsList(warningsList);

	return warningsList;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dirty Management
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// set the dirty flag recursively
void Graph::SetIsDirty(bool isDirty)
{
	// flag of graph itself
	GraphObject::SetIsDirty(isDirty);

	// only for flag reset: reset flag of all child objects
	if (isDirty == false)
	{
		const uint32 numObjects = mObjects.Size();
		for (uint32 i=0; i<numObjects; ++i)
			mObjects[i]->SetIsDirty(isDirty);

		// also treat the reset like a modification event
		if (mIsDirty == false) 
			EMIT_EVENT( OnGraphModified(mParentGraph, NULL) );
	}
}


// get is dirty flag recursively
bool Graph::IsDirty() const
{
	if (GraphObject::IsDirty() == true)
		return true;

	// check if any of the child objects is dirty
	const uint32 numObjects = mObjects.Size();
	for (uint32 i=0; i<numObjects; ++i)
	{
		if (mObjects[i]->IsDirty() == true)
			return true;
	}

	return false;
}


/*template<class T> 
uint32 Graph::CollectGraphObjectsOfType(Array<T*>& list)
{
	// type of objects to collect
	const uint32 type = T::TYPE_ID;

	// count number of objects
	const uint32 numObjects = mObjects.Size();
	uint32 numObjectsOfType = 0;
	for (uint32 i=0; i<numObjects; ++i)
	{
		if (mObjects[i]->GetType() == type)
			numObjectsOfType++;
	}

	// resize output list
	list.Resize(numObjectsOfType);

	// fill list
	uint32 outIndex = 0;
	for (uint32 i=0; i<numObjects; ++i)
	{
		if (mObjects[i]->GetType() == type)
		{
			list[outIndex] = mObjects[i];
			outIndex++;
		}
	}

	return numObjectsOfType;
}*/
