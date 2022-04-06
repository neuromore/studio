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

// include required files
#include "GraphManager.h"
#include "../Core/LogManager.h"


using namespace Core;

// constructor
GraphManager::GraphManager()
{
}


// destructor
GraphManager::~GraphManager()
{
	Clear();
}


// add a new graph
bool GraphManager::AddGraph(Graph* graph, bool replaceIfExists)
{
	// check if the graph with the given id is already part of the graph manager
	const char* uuid = graph->GetUuid();
	const uint32 graphIndex = FindGraphIndexByUuid(uuid);
	if (graphIndex != CORE_INVALIDINDEX32)
	{
		// check if the graph with the given id is already part of the manager
		if (replaceIfExists == false)
		{
			LogError( "Cannot add graph to graph manager. Graph with id '%s' is already existing.", uuid );
			return false;
		}
		else
		{
			// remove the old graph
			RemoveGraphById(uuid);
		}
	}

	mGraphs.Add( graph );
	return true;
}


// find graph by id
Graph* GraphManager::FindGraphByUuid(const char* uuid) const
{
	// iterate through the graphs and find the one with the given id
	const uint32 numGraphs = mGraphs.Size();
	for (uint32 i=0; i<numGraphs; ++i)
	{
		if (mGraphs[i]->GetUuidString().IsEqual(uuid) == true)
			return mGraphs[i];
	}

	// failure, graph with the given id not found
	return NULL;
}


// find graph index by id
uint32 GraphManager::FindGraphIndexByUuid(const char* uuid) const
{
	// iterate through the graphs and find the one with the given id
	const uint32 numGraphs = mGraphs.Size();
	for (uint32 i=0; i<numGraphs; ++i)
	{
		if (mGraphs[i]->GetUuidString().IsEqual(uuid) == true)
			return i;
	}

	// failure, graph with the given id not found
	return CORE_INVALIDINDEX32;
}


// find graph index by id
uint32 GraphManager::FindGraphIndex(Graph* graph) const
{
	// iterate through the graphs and find the one with the given id
	const uint32 numGraphs = mGraphs.Size();
	for (uint32 i=0; i<numGraphs; ++i)
	{
		if (mGraphs[i] == graph)
			return i;
	}

	// failure, graph with the given id not found
	return CORE_INVALIDINDEX32;
}


// find the uuid for the given graph
const char* GraphManager::FindGraphUuid(Graph* graph) const
{
	// find the graph index
	const uint32 graphIndex = FindGraphIndex(graph);
	if (graphIndex == CORE_INVALIDINDEX32)
		return "";

	// return the graph uuid
	return GetGraphUuid(graphIndex);
}


// remove the given graph
bool GraphManager::RemoveGraph(Graph* graph)
{
	// iterate through the graphs and find the one with the given id
	const uint32 numGraphs = mGraphs.Size();
	for (uint32 i=0; i<numGraphs; ++i)
	{
		if (mGraphs[i] != graph)
			continue;

		delete graph;
		mGraphs.Remove(i);
		return true;
	}

	return false;
}


// remove the given graph
bool GraphManager::RemoveGraphById(const char* uuid)
{
	const uint32 graphIndex = FindGraphIndexByUuid(uuid);
	if (graphIndex == CORE_INVALIDINDEX32)
	{
		LogError( "Cannot remove graph. Graph with the id '%s' does not exist.", uuid );
		return false;
	}

	Graph* graph = mGraphs[graphIndex];

	delete graph;
	mGraphs.Remove(graphIndex);

	return true;
}


// get rid of all graphs
void GraphManager::Clear()
{
	DestructArray(mGraphs);
}
