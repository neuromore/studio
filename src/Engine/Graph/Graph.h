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

#ifndef __NEUROMORE_GRAPH_H
#define __NEUROMORE_GRAPH_H

// include the required headers
#include "../Config.h"
#include "../Core/EventSource.h"
#include "../Core/FpsCounter.h"
#include "GraphObject.h"
#include "GraphSettings.h"
#include "Node.h"


class ENGINE_API Graph : public GraphObject
{
	public:
		enum { BASE_TYPE = 0x001 };

		// constructor & destructor
		Graph(Graph* parentGraph);
		virtual ~Graph();

		// type management
		virtual uint32 GetBaseType() const override						{ return BASE_TYPE; }

		// graph revision
		void SetRevision(uint32 revision)								{ mRevision = revision; }
		uint32 GetRevision() const										{ return mRevision; }

		virtual void Reset() override;

		virtual void CollectObjects() override;

		// permissions
		virtual bool AllowEditing() override;

		// attribute changed
		virtual bool OnAttributeChanged(Core::Attribute* attribute) override;

		// graph settings
		bool GetUseSettings() const										{ return mUseGraphSettings; }
		void SetUseSettings(bool enable=true)							{ mUseGraphSettings = enable; }
		virtual bool ApplySettings(const GraphSettings& settings);
		virtual GraphSettings& CreateSettings(GraphSettings& settings, bool changesOnly);
		
		void ResetUpdateReadyFlags();
		void ResetReInitReadyFlags();

		// performance statistics
		const Core::FpsCounter& GetFpsCounter() const					{ return mFpsCounter; }

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// Child nodes
		///////////////////////////////////////////////////////////////////////////////////////////////////////

		inline uint32 GetNumNodes() const								{ return mNodes.Size(); }
		inline Node* GetNode(uint32 index) const						{ return mNodes[index]; }

		// adding & removing nodes
		void AddNode(Node* node);
		virtual bool RemoveNode(Node* node);

		// node search helpers
		Node* FindNodeByName(const char* name, const uint32 typeId=0);
		Node* FindNodeByUuid(const char* uuid);
		uint32 FindNodeIndexByName(const char* name) const;
		uint32 FindNodeIndex(Node* node) const;

		// node helpers
		void CollectNodesOfType(uint32 nodeTypeID, Core::Array<Node*>* outNodes);	// note: outNodes is NOT cleared internally, nodes are added to the array

		// graph-internal callbacks
		virtual void OnGraphModified(Graph* graph, GraphObject* object) {}
		virtual void OnRenamedNode(Graph* graph, Node* node, const Core::String& oldName) {}
		virtual void OnCreatedNode(Graph* graph, Node* node) {}
		virtual void OnRemoveNode(Graph* graph, Node* nodeToRemove) {}

		bool ContainsDeprecatedGraphObjects();
		bool ContainsUnstableGraphObjects();

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// Connections
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		
		inline uint32 GetNumConnections() const							{ return mConnections.Size(); }
		inline Connection* GetConnection(const uint32 index) const		{ return mConnections[index]; }

		// adding connections
		virtual bool AddConnection(Connection* connection);

		// DEPRECATED
		virtual Connection* AddConnection(Node* sourceNode, uint32 outputPortNr, Node* targetNode, uint32 inputPortNr, int32 startOffsetX = 0, int32 startOffsetY = 0, int32 endOffsetX = 0, int32 endOffsetY = 0);

		// remove connections
		virtual void RemoveConnection(Connection* connection);
		virtual void RemoveConnectionsUsingNode(const Node* source);

		// search connections
		bool HasConnection(Connection* connection) const;
		Connection* FindConnection(const Node* sourceNode, uint16 sourcePort, uint16 targetPort) const;
		inline uint32 FindConnectionIndex(Connection* connection) const										{ return mConnections.Find( connection ); }
		
		// search incoming connections (by port)
		bool HasInputConnection(const Node* targetNode, uint32 targetPort) const;
		uint32 CalcNumInputConnections(const Node* targetNode, uint16 targetPort) const;
		uint32 FindInputConnection(const Node* targetNode, uint16 targetPort, uint32 index) const;
		
		// search outgoing connections (by port)
		bool HasOutputConnection(const Node* sourceNode, uint32 sourcePort) const;
		uint32 CalcNumOutputConnections(const Node* sourceNode, uint16 sourcePort) const;
		uint32 FindOutputConnection(const Node* sourceNode, uint16 sourcePort, uint32 index) const;
		
		// calc num incoming connections (accross all ports)
		uint32 FindInputConnection(const Node* targetNode, uint32 index) const;
		uint32 CalcNumInputConnections(const Node* node) const;

		// calc num outgoing connections (accross all ports)
		uint32 FindOutputConnection(const Node* sourceNode, uint32 index) const;
		uint32 CalcNumOutputConnections(const Node* node) const;

		// connection helpers
		bool ValidateConnections() const;

		void ResetOutputConnections(const Node* node);

		// graph-internal callbacks
		virtual void OnCreatedConnection(Graph* graph, Connection* connection)							{}
		virtual void OnRemoveConnection(Graph* graph, Connection* connection)							{}

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// Serialization
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual Core::Json::Item Save(Core::Json& json, Core::Json::Item& item) override;
		virtual bool Load(const Core::Json& json, const Core::Json::Item& item) override;

		// Is Dirty flags
		virtual bool IsDirty() const override;
		virtual void SetIsDirty(bool isDirty) override;

		// graph object errors (hierarchical)
		virtual bool HasError(uint32 errorId = 0) const override;		
		virtual uint32 GetNumErrors() const override;
		virtual Core::Array<GraphObject::GraphObjectError*>& CollectErrors(Core::Array<GraphObject::GraphObjectError*>& outList) override;
		const Core::Array<Core::String>& CreateErrorList(Core::Array<Core::String>& errorList);

		// graph object warnings (hierarchical)
		virtual bool HasWarning() const override;
		virtual uint32 GetNumWarnings() const override;
		virtual Core::Array<GraphObject::GraphObjectWarning*>& CollectWarnings(Core::Array<GraphObject::GraphObjectWarning*>& outList) override;
		const Core::Array<Core::String>& CreateWarningsList(Core::Array<Core::String>& warningsList);

	protected:
		bool SaveNodes(Core::Json& json, Core::Json::Item& item);
		bool SaveConnections(Core::Json& json, Core::Json::Item& item);

	protected:
		Core::Array<Node*>			mNodes;				// all nodes 
		Core::Array<Connection*>	mConnections;		// all connections
		Core::Array<Graph*>			mGraphs;			// all nested graphs (which are also present in mNodes)

		Core::Array<GraphObject*>	mObjects;			// references to all graph objects (includes all the objects above and everything the derived class uses, e.g. state machine action)

		uint32						mRevision;

		bool						mUseGraphSettings;	// if graph settings were applied

		Core::FpsCounter			mFpsCounter;


		// FIXME find out how to use lambdas correctly as template arguments
		// helper: collect all objects that are matched by the matching function M
		//template<class T, typename M> static uint32 CollectGraphObjects(Core::Array<T*>& outList);
		
		// collect all objects of this type into a list
		/*template<class T> static uint32 CollectGraphObjectsByType(Core::Array<T*>& outList)
		{
			bool typeMatchFunc = [](T*) { return T->GetType() == T::TYPE_ID; };
			return CollectGraphObjects<T, typeMatchFunc>(list);
		}*/

		//template<class T> uint32 CollectGraphObjectsOfType(Core::Array<T*>& outList);

};


#endif
