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

#ifndef __NEUROMORE_Connection_H
#define __NEUROMORE_Connection_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "GraphObject.h"


// forward declaration
class Node;
class Graph;

class ENGINE_API Connection : public GraphObject
{
	public:
		enum { BASE_TYPE		= 0x003 };
		enum { TYPE_ID			= 0x050 };
		static const char* Uuid () { return "4107b810-bb6b-11e4-8dfc-aa07a5b093db"; }

		// constructors & destructor
		Connection();
		Connection(Node* sourceNode, Node* targetNode, uint16 sourcePort, uint16 targetPort);
		virtual ~Connection();

		// type management
		virtual uint32 GetBaseType() const override					{ return BASE_TYPE; }
		virtual uint32 GetType() const override						{ return TYPE_ID; }
		const char* GetTypeUuid() const override					{ return "7b69fb84-bb6c-11e4-8dfc-aa07a5b093db"; }
		virtual const char* GetReadableType() const override		{ return "Connection"; }

		virtual void Update(const Core::Time& elapsed, const Core::Time& delta) override				{}
		GraphObject* Clone(Graph* graph) override					{ Connection* clone = new Connection(); return clone; }

		// source & target nodes
		Node* GetSourceNode() const									{ return mSourceNode; }
		Node* GetTargetNode() const									{ return mTargetNode; }
		void SetSourceNode(Node* node);
		void SetTargetNode(Node* node);

		// source & target ports
		void SetSourcePort(uint16 sourcePort);
		void SetTargetPort(uint16 targetPort);
		uint16 GetSourcePort() const								{ return mSourcePort; }
		uint16 GetTargetPort() const								{ return mTargetPort; }

		bool IsValid() const;

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// Serialization
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual Core::Json::Item Save(Core::Json& json, Core::Json::Item& item) override;

	protected:
		Node*					mSourceNode;		// The source node from which the incoming connection comes.
		Node*					mTargetNode;		// The target node to which the connection goes to.
		uint16					mSourcePort;		// The source port number, so the output port number of the node where the connection comes from.
		uint16					mTargetPort;		// The target port number, which is the input port number of the target node.
};


#endif
