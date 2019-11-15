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

#ifndef __NEUROMORE_GRAPHMANAGER_H
#define __NEUROMORE_GRAPHMANAGER_H

// include required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../Core/String.h"
#include "../Core/Array.h"
#include "Graph.h"

// holds all loaded graphs
class ENGINE_API GraphManager
{
	friend class EngineManager;

	public:
		// constructor & destructor
		GraphManager();
		~GraphManager();

		uint32 GetNumGraphs() const														{ return mGraphs.Size(); }
		Graph* GetGraph(uint32 index) const												{ return mGraphs[index]; }
		
		const char* GetGraphUuid(uint32 index) const									{ return mGraphs[index]->GetUuid(); }
		
		const char* FindGraphUuid(Graph* graph) const;
		Graph* FindGraphByUuid(const char* uuid) const;
		uint32 FindGraphIndexByUuid(const char* uuid) const;
		uint32 FindGraphIndex(Graph* graph) const;

	protected:
		// used by engine manager
		bool AddGraph(Graph* graph, bool replaceIfExists=true);
		bool RemoveGraph(Graph* graph);
		bool RemoveGraphById(const char* uuid);
		void Clear();

	private:
		Core::Array<Graph*>	mGraphs;
};


#endif
