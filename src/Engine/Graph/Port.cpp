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
#include "../EngineManager.h"
#include "Port.h"
#include "../Core/AttributeFactory.h"
#include "../Core/Math.h"
#include "Node.h"
#include "../DSP/AttributeChannels.h"


using namespace Core;

// constructor
Port::Port(EDirection direction) : mPortDirection(direction)
{
	mValue			= NULL;
	mPortId			= CORE_INVALIDINDEX32;
	mIsVisible		= true;

	ClearCompatibleTypes();
}


// destructor
Port::~Port()
{
	Clear();
}


// set the name
void Port::SetName(const char* name)
{
	if (mName.IsEqual(name) == false)
		mName = name;
}


// get the name
const char* Port::GetName() const
{
	return mName.AsChar();
}


// get the name as string object
const String& Port::GetNameString() const
{
	return mName;
}


// set the internal name
void Port::SetInternalName(const char* internalName)
{
	mInternalName = internalName;
}


// get the internal name
const char* Port::GetInternalName() const
{
	return mInternalName.AsChar();
}


// get the internal name as string object
const String& Port::GetInternalNameString() const
{
	return mInternalName;
}


bool Port::AddConnection(Connection* connection)
{
	mConnections.Add(connection);
	return true;
}


bool Port::RemoveConnection(Connection* connection)
{
	return mConnections.RemoveByValue(connection);
}


Connection* Port::GetConnection(uint32 index) const
{
	if (index >= mConnections.Size())
		return NULL;

	return mConnections[index];
}


// is the port compatible with the given other port?
bool Port::IsCompatibleWith(const Port& otherPort) const
{
	// check [prt directions
	if (otherPort.mPortDirection == mPortDirection)
		return false;

	// check the data types
	for (uint32 i=0; i<4; ++i)
	{
		if (otherPort.mValue->GetType() == mCompatibleTypes[i])
			return true;

		// if there aren't any more compatibility types and we haven't found a compatible one so far, return false
		if (mCompatibleTypes[i] == 0)
			return false;
	}

	// not compatible
	return false;
}


// clear compatibility types
void Port::ClearCompatibleTypes()
{
	MemSet(mCompatibleTypes, 0, sizeof(uint32)*4);
}


// clear the port
void Port::Clear()
{
	delete mValue;
	mValue = NULL;
	ClearCompatibleTypes();
}


Core::AttributeBool* Port::GetBool() const
{
	Core::Attribute* attrib = GetValueAttribute();
	if (attrib == NULL)
		return NULL;

	CORE_ASSERT(attrib->GetType() == Core::AttributeBool::TYPE_ID);
	return static_cast<Core::AttributeBool*>(attrib);
}


Core::AttributeInt32* Port::GetInt32() const
{
	Core::Attribute* attrib = GetValueAttribute();
	if (attrib == NULL)
		return NULL;

	CORE_ASSERT(attrib->GetType() == Core::AttributeInt32::TYPE_ID);
	return static_cast<Core::AttributeInt32*>(attrib);
}


Core::AttributeFloat* Port::GetFloat() const
{
	Core::Attribute* attrib = GetValueAttribute();
	if (attrib == NULL)
		return NULL;

	CORE_ASSERT(attrib->GetType() == Core::AttributeFloat::TYPE_ID);
	return static_cast<Core::AttributeFloat*>(attrib);
}


Core::AttributeString* Port::GetString() const
{
	Core::Attribute* attrib = GetValueAttribute();
	if (attrib == NULL)
		return NULL;

	CORE_ASSERT(attrib->GetType() == Core::AttributeString::TYPE_ID);
	return static_cast<Core::AttributeString*>(attrib);
}


Core::AttributeColor* Port::GetColor() const
{
	Core::Attribute* attrib = GetValueAttribute();
	if (attrib == NULL)
		return NULL;

	CORE_ASSERT(attrib->GetType() == Core::AttributeColor::TYPE_ID);
	return static_cast<Core::AttributeColor*>(attrib);
}


MultiChannel* Port::GetChannels() const
{
	Core::Attribute* attrib = GetValueAttribute();
	if (attrib == NULL)
		return NULL;

	if (attrib->GetType() == AttributeChannels<double>::TYPE_ID)
		return static_cast<AttributeChannels<double>*>(attrib);

	else if (attrib->GetType() == AttributeChannels<Spectrum>::TYPE_ID)
		return static_cast<AttributeChannels<Spectrum>*>(attrib);

	return NULL;
}


// get the attribute holding the ports value (depends on port direction)
Core::Attribute* Port::GetValueAttribute() const
{
	// Input Port: get the attribute of the connected port (if a connection exists)
	if (mPortDirection == INPUT)
	{
		// check connection
		if (mConnections.IsEmpty() == true)
			return NULL;

		// get connected node
		Node* sourceNode = mConnections[0]->GetSourceNode();
		if (sourceNode == NULL)
			return NULL;

		// get connected port
		const OutputPort& port = sourceNode->GetOutputPort(mConnections[0]->GetSourcePort());

		// return its attribute
		return port.GetValueAttribute();
	}
	// Output Port: just return the port attribute
	else if (mPortDirection == OUTPUT || mPortDirection == UNDEFINED)
	{
		return mValue;
	}

	return NULL;
}


float Port::GetValue() const
{
	Core::Attribute* attribute = GetValueAttribute();
	if (attribute == NULL) return 0.0f;
	const uint32 typeID = attribute->GetType();
	switch (typeID)
	{
		case Core::AttributeFloat::TYPE_ID:	return static_cast<const Core::AttributeFloat*>(attribute)->GetValue();
		case Core::AttributeInt32::TYPE_ID:	return static_cast<const Core::AttributeInt32*>(attribute)->GetValue();
		case Core::AttributeBool::TYPE_ID:	return static_cast<const Core::AttributeBool*>(attribute)->GetValue();
		case AttributeChannels<double>::TYPE_ID_DOUBLE:
		{
			const AttributeChannels<double>* channels = static_cast<const AttributeChannels<double>*>(attribute);
			if (channels != NULL)
				if (channels->GetNumChannels() > 0)
					if (channels->GetChannel(0)->GetNumSamples() > 0)
						return channels->GetChannel(0)->AsType<double>()->GetLastSample();
			return 0;
		}

		default:
			CORE_ASSERT(1==0);	// should never happen
			return 0.0f;
	};
}


bool Port::GetValueAsBool() const
{
	const Core::Attribute* attribute = GetValueAttribute();
	if (attribute == NULL) return false;
	const uint32 typeID = attribute->GetType();
	switch (typeID)
	{
		case Core::AttributeBool::TYPE_ID:		return static_cast<const Core::AttributeBool*>(attribute)->GetValue();
		case Core::AttributeInt32::TYPE_ID:		return (static_cast<const Core::AttributeInt32*>(attribute)->GetValue() != 0);
		case Core::AttributeFloat::TYPE_ID:		return Core::IsClose<double>(static_cast<const Core::AttributeFloat*>(attribute)->GetValue(), 0.0, Core::Math::epsilon);
		default:
			CORE_ASSERT(1==0);	// should never happen
			return false;
	};
}


void Port::Setup(const char* name, const char* internalName, uint32 attributeTypeID, uint32 portID)
{
	SetName(name);
	SetInternalName(internalName);
	Clear();

	mValue				= CORE_ATTRIBUTEFACTORY.CreateByType( attributeTypeID );
	mCompatibleTypes[0]	= attributeTypeID;

	SetPortId( portID );

	// make sure we were able to create the attribute
	CORE_ASSERT(mValue != NULL );
}


void Port::SetupAsNumber(const char* name, const char* internalName, uint32 portID)
{
	SetName(name);
	SetInternalName(internalName);
	Clear();

	mValue				= new Core::AttributeFloat();
	mCompatibleTypes[0]	= AttributeFloat::TYPE_ID;		// setup the compatible types of this port

	SetPortId( portID );
}
