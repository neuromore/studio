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

// include required files
#include "GraphImporter.h"
#include "../Core/LogManager.h"
#include "../Core/Timer.h"
#include "../EngineManager.h"
#include "Classifier.h"
#include "StateMachine.h"
#include "StateTransition.h"
#include "DeprecatedGraphObjects.h"


using namespace Core;


bool GraphImporter::LoadFromJSON(const Json& json, const Json::Item& rootItem, Graph* rootNode)
{
	// disable events when loading the graph
	rootNode->SetEmitEvents( false );

	// construct our new graph
	const bool success = rootNode->Load(json, rootItem);

	// something within the graph could not be loaded. 
	if (success == false)
		LogWarning("GraphImporter::LoadFromJSON(): Errors while loading graph from JSON, it might be incomplete.");

	// reset graph
	rootNode->Init();
	rootNode->CreateDefaultAttributeValues();
	rootNode->Reset();
	rootNode->SetIsDirty(false);

	// re-enable events when loading the graph
	rootNode->SetEmitEvents( true );

	return success;
}


// load the graph from the given json string
bool GraphImporter::LoadFromString(const char* jsonString, Graph* rootNode)
{
	Timer loadTimer;

	// parse our file
	Json json;
	if (json.Parse(jsonString) == false)
	{
		LogWarning("GraphImporter::LoadFromString(): JSON parser failed.");
		return false;
	}

#ifdef CORE_DEBUG
	json.Log();
#endif

	// get access to the root item and load it from there
	Json::Item rootItem = json.GetRootItem();
	if (LoadFromJSON(json, rootItem, rootNode) == false)
	{
		LogWarning("GraphImporter::LoadFromString(): loading graph failed.");
		return false;
	}

	// log the timing information
	const float loadTime = loadTimer.GetTime().InSeconds();
	LogInfo( "Loading %s took %.1f ms.", rootNode->GetReadableType(), loadTime * 1000.0f );

	// return success
	return true;
}


// load the graph from the given file
bool GraphImporter::LoadFromFile(const char* filename, Graph* rootNode)
{
	// create the file on disk
	FILE* file;
	file = fopen(filename, "rb\0");
	if (file == NULL)
	{
		LogError( "GraphImporter::LoadFromFile(): Cannot open file '%s'.", filename );
		return false;
	}

	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	// load the whole file into a string
	char* stringData = new char[fileSize+1];

	if (fread(stringData, fileSize, 1, file) < 0)
	{
		LogError( "GraphImporter::LoadFromFile(): Cannot read file '%s'.", filename );
		delete[] stringData;
		return false;
	}
	stringData[fileSize] = '\0';

	fclose(file);

	// load the graph from the json string data
	bool result = LoadFromString( stringData, rootNode );

	delete[] stringData;
	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Nodes
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// load the graph nodes
bool GraphImporter::LoadNodes(const Json& json, const Json::Item& item, Graph* graph)
{
	bool success = true;

	// get the nodes item
	Json::Item nodesItem = item.Find("nodes");
	if (nodesItem.IsArray() == false)
	{
		//LogDetailedInfo("GraphImporter::LoadGraphNodes():  Graph does not contain any node.");
		return true;
	}

	// get the number of child items and iterate through them
	const uint32 numNodes = nodesItem.Size();
	for (uint32 i=0; i<numNodes; ++i)
		if (LoadNode( json, nodesItem[i], graph ) == false)
			success = false;

	return success;
}


// load a graph node from the given json item
bool GraphImporter::LoadNode(const Json& json, const Json::Item& nodeItem, Graph* graph)
{
	bool success = true;

	// make sure the given item is valid
	if (nodeItem.IsNull() == true)
		return false;

	// get the node type string
	Json::Item typeItem = nodeItem.Find("type");
	if (typeItem.IsString() == false)
		return false;
	
	// translate deprecated nodes
	DeprecatedGraphObjects::TranslateNode(const_cast<Json::Item&>(nodeItem)); // CONST CAST
	
	// create the node
	GraphObject* object = GetGraphObjectFactory()->CreateObjectByTypeUuid( graph, typeItem.GetString() );
	if (object == NULL)
	{
		LogError( "GraphImporter::LoadNode(): Unknown Object (type='%s'). Skipping...", typeItem.GetString() );
		success =  false;
	}

	// type-cast object to node
	Node* node = static_cast<Node*>(object);
	if (node == NULL)
	{
		LogError( "GraphImporter::LoadNode(): Cannot type-cast created object (type='%s') to a node. Skipping...", typeItem.GetString() );
		delete object;
		success =  false;
	}


	// fill the node with the json information
	else if (success && node->Load(json, nodeItem) == false)
	{
		// error message already fired in Node::Load()
		delete node;
		success =  false;
	}

	// add the node to the graph
	if (success)
		graph->AddNode(node);

	return success;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Connections
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// load the connections
bool GraphImporter::LoadConnections(const Json& json, const Json::Item& item, Graph* graph)
{
	// get the connections item
	Json::Item connectionsItem = item.Find("connections");
	if (connectionsItem.IsArray() == false)
	{
		//LogDetailedInfo("GraphImporter::LoadGraphConnections():  Graph does not contain any connection.");
		return true;
	}

	// get the number of child items and iterate through them
	bool success = true;
	const uint32 numConnections = connectionsItem.Size();
	for (uint32 i = 0; i < numConnections; ++i)
		if (LoadConnection(json, connectionsItem[i], graph) == NULL)
			success = false;

	return success;
}


// load a connection from the given json item
Connection* GraphImporter::LoadConnection(const Json& json, const Json::Item& connectionItem, Graph* graph)
{
	// make sure the given item is valid
	if (connectionItem.IsNull() == true)
		return NULL;

	// get the connection source node uuid
	String sourceNodeUuid;
	Json::Item sourceNodeItem = connectionItem.Find("sourceNode");
	if (sourceNodeItem.IsString() == true)
		sourceNodeUuid = sourceNodeItem.GetString();

	// get the connection target node uuid
	String targetNodeUuid;
	Json::Item targetNodeItem = connectionItem.Find("targetNode");
	if (targetNodeItem.IsString() == true)
		targetNodeUuid = targetNodeItem.GetString();

	// get the source and the target node and check if they are valid
	Node* sourceNode = graph->FindNodeByUuid( sourceNodeUuid.AsChar() );
	Node* targetNode = graph->FindNodeByUuid( targetNodeUuid.AsChar() );
	if (sourceNode == NULL || targetNode == NULL)
	{
		LogError( "GraphImporter::LoadGraphConnection() - Connection cannot be created because the source or target node cannot be found (sourceNodeUuid='%s' targetNodeUuid='%s').", sourceNodeUuid.AsChar(), targetNodeUuid.AsChar() );
		return NULL;
	}

	// get the connection source and target ports
	Json::Item	sourcePortItem	= connectionItem.Find("sourcePort");
	Json::Item	targetPortItem	= connectionItem.Find("targetPort");
	uint32		sourcePortIndex = CORE_INVALIDINDEX32;
	uint32		targetPortIndex = CORE_INVALIDINDEX32;
	if (sourcePortItem.IsString() == true && targetPortItem.IsString() == true)
	{
		sourcePortIndex	= sourceNode->FindOutputPortIndex( sourcePortItem.GetString() );
		targetPortIndex	= targetNode->FindInputPortIndex( targetPortItem.GetString() );
	}

	// make sure the ports indices are valid
	if (sourcePortIndex == CORE_INVALIDINDEX32 || targetPortIndex == CORE_INVALIDINDEX32)
	{
		LogError("GraphImporter::LoadGraphConnection() - Connection cannot be created because the source or target port index is invalid (sourcePort='%s' targetPortIndex='%s' sourceNode='%s' targetNode='%s').", sourcePortItem.GetString(), targetPortItem.GetString(), sourceNode->GetName(), targetNode->GetName());
		return NULL;
	}

	// make sure the channel connects only compatible ports
	if (targetNode->GetInputPort(targetPortIndex).IsCompatibleWith(sourceNode->GetOutputPort(sourcePortIndex)) == false)
	{
		LogError("GraphImporter::LoadGraphConnection() - Connection cannot be created because the source type is incompatible to the target type (sourceType='%s' targetPortIndex='%s' sourceNode='%s' targetNode='%s').", sourcePortItem.GetString(), targetPortItem.GetString(), sourceNode->GetName(), targetNode->GetName());
		return NULL;
	}

	// create the connection
	return graph->AddConnection(sourceNode, sourcePortIndex, targetNode, targetPortIndex);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Transitions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// load the transitions
bool GraphImporter::LoadTransitions(const Json& json, const Json::Item& item, Graph* graph)
{
	// make sure we are dealing with a state machine
	if (graph->GetType() != StateMachine::TYPE_ID)
	{
		LogError("GraphImporter::LoadTransitions(): Given parent node is no valid state machine.");
		CORE_ASSERT(false);
		return false;
	}

	// get the connections item
	Json::Item transitionsItem = item.Find("transitions");
	if (transitionsItem.IsArray() == false)
	{
		//LogDetailedInfo("GraphImporter::LoadTransitions(): State machine does not contain any transitions.");
		return true;
	}

	// type-cast graph to state machine
	StateMachine* stateMachine = static_cast<StateMachine*>(graph);

	// get the number of child items and iterate through them
	bool success = true;
	const uint32 numTransitions = transitionsItem.Size();
	for (uint32 i=0; i<numTransitions; ++i)
	{
		if (LoadTransition(json, transitionsItem[i], stateMachine) == false)
			success = false;
	}

	return success;
}


// load a connection from the given json item
bool GraphImporter::LoadTransition(const Json& json, const Json::Item& transitionItem, StateMachine* stateMachine)
{
	// make sure the given item is valid
	if (transitionItem.IsNull() == true)
		return false;

	//------- copy pasted code start

	// get the connection source node uuid
	String sourceNodeUuid;
	Json::Item sourceNodeItem = transitionItem.Find("sourceNode");
	if (sourceNodeItem.IsString() == true)
		sourceNodeUuid = sourceNodeItem.GetString();

	// get the connection target node uuid
	String targetNodeUuid;
	Json::Item targetNodeItem = transitionItem.Find("targetNode");
	if (targetNodeItem.IsString() == true)
		targetNodeUuid = targetNodeItem.GetString();

	// get the source and the target node and check if they are valid
	Node* sourceNode = stateMachine->FindNodeByUuid( sourceNodeUuid.AsChar() );
	Node* targetNode = stateMachine->FindNodeByUuid( targetNodeUuid.AsChar() );
	if (sourceNode == NULL || targetNode == NULL)
	{
		LogError( "GraphImporter::LoadTransition() - Transition cannot be created because the source or target state cannot be found (sourceStateUuid='%s' targetStateUuid='%s').", sourceNodeUuid.AsChar(), targetNodeUuid.AsChar() );
		return false;
	}

	// get the connection source and target ports
	Json::Item	sourcePortItem	= transitionItem.Find("sourcePort");
	Json::Item	targetPortItem	= transitionItem.Find("targetPort");
	uint32		sourcePortIndex = CORE_INVALIDINDEX32;
	uint32		targetPortIndex = CORE_INVALIDINDEX32;
	if (sourcePortItem.IsString() == true && targetPortItem.IsString() == true)
	{
		sourcePortIndex	= sourceNode->FindOutputPortIndex( sourcePortItem.GetString() );
		targetPortIndex	= targetNode->FindInputPortIndex( targetPortItem.GetString() );
	}

	// make sure all essential information is available
	if (sourcePortIndex == CORE_INVALIDINDEX32 || targetPortIndex == CORE_INVALIDINDEX32)
	{
		LogError("GraphImporter::LoadTransition() - Transition cannot be created because the source or target port index is invalid (sourcePort='%s' targetPortIndex='%s' sourceNode='%s' targetNode='%s').", sourcePortItem.GetString(), targetPortItem.GetString(), sourceNode->GetName(), targetNode->GetName());
		return false;
	}

	//------- copy pasted code end

	// get the visual offsets
	int32 startOffsetX	= 0;
	int32 startOffsetY	= 0;
	int32 endOffsetX	= 0;
	int32 endOffsetY	= 0;

	Json::Item startOffsetXItem = transitionItem.Find( "startOffsetX" );
	Json::Item startOffsetYItem = transitionItem.Find( "startOffsetY" );
	Json::Item endOffsetXItem	= transitionItem.Find( "endOffsetX" );
	Json::Item endOffsetYItem	= transitionItem.Find( "endOffsetY" );

	if (startOffsetXItem.IsNumber() == true)	startOffsetX	= startOffsetXItem.GetInt();
	if (startOffsetYItem.IsNumber() == true)	startOffsetY	= startOffsetYItem.GetInt();
	if (endOffsetXItem.IsNumber() == true)		endOffsetX		= endOffsetXItem.GetInt();
	if (endOffsetYItem.IsNumber() == true)		endOffsetY		= endOffsetYItem.GetInt();

	Connection* newConnection = NULL;
	if (sourcePortIndex != CORE_INVALIDINDEX32 && targetPortIndex != CORE_INVALIDINDEX32)
		newConnection = stateMachine->AddConnection( sourceNode, sourcePortIndex, targetNode, targetPortIndex, startOffsetX, startOffsetY, endOffsetX, endOffsetY );
	else
	{
		LogError("GraphImporter::LoadTransition() - Transition cannot be created because the source or target port doesn't exist (sourcePortID=%d targetPortID=%d sourceNode='%s' targetNode=%s').", sourcePortIndex, targetPortIndex, sourceNode->GetName(), targetNode->GetName());
		return false;
	}

	// load transition related attributes
	if (newConnection != NULL && newConnection->GetType() == StateTransition::TYPE_ID)
	{
		StateTransition* transition = static_cast<StateTransition*>(newConnection);

		// read the attributes
		if (transition->Read(json, transitionItem, true) == false)
		{
			delete transition;
			return false;
		}

		// load conditions
		transition->Load( json, transitionItem );

		// update attributes
		transition->OnAttributesChanged();
	}

	return true;
}
