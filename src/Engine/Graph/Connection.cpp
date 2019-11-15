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
#include "Connection.h"
#include "Node.h"
#include "Port.h"


using namespace Core;

// default constructor
Connection::Connection() : GraphObject()
{
	mSourceNode		= NULL;
	mTargetNode		= NULL;
	mSourcePort		= CORE_INVALIDINDEX16;
	mTargetPort		= CORE_INVALIDINDEX16;
}


// extended constructor
Connection::Connection(Node* sourceNode, Node* targetNode, uint16 sourcePort, uint16 targetPort) : GraphObject()
{
	CORE_ASSERT( sourceNode != NULL && targetNode != NULL );

	mSourceNode		= sourceNode;
	mTargetNode		= targetNode;
	mSourcePort		= sourcePort;
	mTargetPort		= targetPort;

	// link the ports to the connection
	mSourceNode->GetOutputPort(mSourcePort).AddConnection(this);
	mTargetNode->GetInputPort(mTargetPort).AddConnection(this);
}


// destructor
Connection::~Connection()
{
	// unlink the connection from the source node port
	if (mSourceNode != NULL)
	{
		if (mSourcePort != CORE_INVALIDINDEX16)
			if (mSourceNode->GetOutputPort(mSourcePort).RemoveConnection(this) == false)
				LogError( "Connection::~Connection(): Cannot unlink connection from source node port (SourceNodeName=%s, OutputPortNr=%i)", mSourceNode->GetName(), mSourcePort );
	}

	// unlink the connection from the target node port
	if (mTargetNode != NULL)
	{
		if (mTargetPort != CORE_INVALIDINDEX16)
			if (mTargetNode->GetInputPort(mTargetPort).RemoveConnection(this) == false)
				LogError( "Connection::~Connection(): Cannot unlink connection from target node port (TargetNodeName=%s, InputPortNr=%i)", mTargetNode->GetName(), mTargetPort );
	}
}


// relink the connection to another source node
void Connection::SetSourceNode(Node* node)
{
	// unlink the connection from the old port
	if (mSourceNode != NULL)
	{
		if (mSourcePort != CORE_INVALIDINDEX16)
			if (mSourceNode->GetOutputPort(mSourcePort).RemoveConnection(this) == false)
				LogError( "Connection::SetSourceNode(): Cannot unlink connection from old port (OldNode: Name=%s, OutputPortNr=%i)", mSourceNode->GetName(), mSourcePort );
	}

	// set the new source node
	mSourceNode = node;

	// link the connection to the new port
	if (mSourceNode != NULL)
	{
		if (mSourcePort != CORE_INVALIDINDEX16)
			if (mSourceNode->GetOutputPort(mSourcePort).AddConnection(this) == false)
				LogError( "Connection::SetSourceNode(): Cannot link connection to new port (NewNode: Name=%s, InputPortNr=%i)", mSourceNode->GetName(), mSourcePort );
	}

	// adjust the is dirty flag
	SetIsDirty(true);
}


// relink the connection to another target node
void Connection::SetTargetNode(Node* node)
{
	// unlink the connection from the old port
	if (mTargetNode != NULL)
	{
		if (mTargetPort != CORE_INVALIDINDEX16)
			if (mTargetNode->GetInputPort(mTargetPort).RemoveConnection(this) == false)
				LogError( "Connection::SetTargetNode(): Cannot unlink connection from old port (OldNode: Name=%s, OutputPortNr=%i)", mTargetNode->GetName(), mTargetPort );
	}

	// set the new target node
	mTargetNode = node;

	// link the connection to the new port
	if (mTargetNode != NULL)
	{
		if (mTargetPort != CORE_INVALIDINDEX16)
			if (mTargetNode->GetInputPort(mTargetPort).AddConnection(this) == false)
				LogError( "Connection::SetTargetNode(): Cannot link connection to new port (NewNode: Name=%s, OutputPortNr=%i)", mTargetNode->GetName(), mTargetPort );
	}

	// adjust the is dirty flag
	SetIsDirty(true);
}


// relink the connection to another source port
void Connection::SetSourcePort(uint16 sourcePort)
{
	// unlink the connection from the old port
	if (mSourceNode != NULL)
	{
		if (mSourcePort != CORE_INVALIDINDEX16)
			if (mSourceNode->GetOutputPort(mSourcePort).RemoveConnection(this) == false)
				LogError( "Connection::SetSourcePort(): Cannot unlink connection from old port (OldNode: Name=%s, OutputPortNr=%i)", mSourceNode->GetName(), mSourcePort );
	}

	// set the new source port
	mSourcePort = sourcePort;

	// link the connection to the new port
	if (mSourceNode != NULL)
	{
		if (mSourcePort != CORE_INVALIDINDEX16)
			if (mSourceNode->GetOutputPort(mSourcePort).AddConnection(this) == false)
				LogError( "Connection::SetSourcePort(): Cannot link connection to new port (NewNode: Name=%s, OutputPortNr=%i)", mSourceNode->GetName(), mSourcePort );
	}

	// adjust the is dirty flag
	SetIsDirty(true);
}


// relink the connection to another target port
void Connection::SetTargetPort(uint16 targetPort)
{
	// unlink the connection from the old port
	if (mTargetNode != NULL)
	{
		if (mTargetPort != CORE_INVALIDINDEX16)
			if (mTargetNode->GetInputPort(mTargetPort).RemoveConnection(this) == false)
				LogError( "Connection::SetTargetPort(): Cannot unlink connection from old port (OldNode: Name=%s, OutputPortNr=%i)", mTargetNode->GetName(), mTargetPort );
	}

	// set the new target port
	mTargetPort = targetPort;

	// link the connection to the new port
	if (mTargetNode != NULL)
	{
		if (mTargetPort != CORE_INVALIDINDEX16)
			if (mTargetNode->GetInputPort(mTargetPort).AddConnection(this) == false)
				LogError( "Connection::SetTargetPort(): Cannot link connection to new port (NewNode: Name=%s, OutputPortNr=%i)", mTargetNode->GetName(), mTargetPort );
	}

	// adjust the is dirty flag
	SetIsDirty(true);
}


// check if this connection is valid
bool Connection::IsValid() const
{
	// make sure the node and input numbers are valid
	if (mSourceNode == NULL || mTargetNode == NULL ||mSourcePort == CORE_INVALIDINDEX16 || mTargetPort == CORE_INVALIDINDEX16)
		return false;

	// the connection is valid
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Serialization
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// save the given graph node
Json::Item Connection::Save(Json& json, Json::Item& item)
{
	CORE_ASSERT( item.IsArray() == true );

	// add the connection item
	Json::Item connectionItem = item.AddObject();
	if (connectionItem.IsObject() == false)
		return json.NullItem();

	// prepare information
	Node*			sourceNode		= GetSourceNode();
	Node*			targetNode		= GetTargetNode();
	const Port&		sourceNodePort	= sourceNode->GetOutputPort( GetSourcePort() );
	const Port&		targetNodePort	= targetNode->GetInputPort( GetTargetPort() );

	// add the attributes
	connectionItem.AddString( "sourceNode", sourceNode->GetUuid() );
	connectionItem.AddString( "targetNode", targetNode->GetUuid() );
	connectionItem.AddString( "sourcePort", sourceNodePort.GetInternalName() );
	connectionItem.AddString( "targetPort", targetNodePort.GetInternalName() );

	// add attributes from the connection attribute set
	Write( json, connectionItem, true );

	return connectionItem;
}
