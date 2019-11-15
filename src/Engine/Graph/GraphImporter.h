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

#ifndef __NEUROMORE_GRAPHIMPORTER_H
#define __NEUROMORE_GRAPHIMPORTER_H

// include required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../Core/Json.h"
#include "Graph.h"
#include "Classifier.h"
#include "StateMachine.h"


class ENGINE_API GraphImporter
{
	friend class StateMachine;
	friend class Classifier;
	friend class Graph;

	public:
		static bool LoadFromJSON(const Core::Json& json, const Core::Json::Item& rootItem, Graph* rootNode);
		static bool LoadFromFile(const char* filename, Graph* rootNode);
		static bool LoadFromString(const char* jsonString, Graph* rootNode);

	private:
		// nodes
		static bool LoadNode(const Core::Json& json, const Core::Json::Item& nodeItem, Graph* graph);
		static bool LoadNodes(const Core::Json& json, const Core::Json::Item& item, Graph* graph);
	
		// connection loading helpers
		static Connection* LoadConnection(const Core::Json& json, const Core::Json::Item& connectionItem, Graph* graph);
		static bool LoadConnections(const Core::Json& json, const Core::Json::Item& item, Graph* graph);

		// transition loading helpers
		static bool LoadTransition(const Core::Json& json, const Core::Json::Item& transitionItem, StateMachine* stateMachine);
		static bool LoadTransitions(const Core::Json& json, const Core::Json::Item& item, Graph* graph);
};


#endif
