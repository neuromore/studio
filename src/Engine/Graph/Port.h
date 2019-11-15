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

#ifndef __NEUROMORE_GRAPH_PORT_H
#define __NEUROMORE_GRAPH_PORT_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../Core/Attribute.h"
#include "../DSP/AttributeChannels.h"
#include "../Core/String.h"
#include "Connection.h"


class ENGINE_API Port
{
	public:

		enum EDirection
		{
			UNDEFINED,
			INPUT, 
			OUTPUT,
		};

		// constructor & destructor
		Port() : Port(Port::UNDEFINED) {}
		Port(EDirection direction);
		virtual ~Port();

		// visual name
		void SetName(const char* name);
		const char* GetName() const;
		const Core::String& GetNameString() const;

		// hide/show node
		bool IsVisible() const										{ return mIsVisible; }
		void SetVisible(bool enabled = true)						{ mIsVisible = enabled; }

		// internal name (used for e.g. serialization)
		void SetInternalName(const char* internal);
		const char* GetInternalName() const;
		const Core::String& GetInternalNameString() const;

		void SetPortId(uint32 portId)								{ mPortId = portId; }
		uint32 GetPortId() const									{ return mPortId; }

		// connections
		bool AddConnection(Connection* connection);
		bool RemoveConnection(Connection* connection);
		bool HasConnection() const									{ return mConnections.IsEmpty() == false; }
		uint32 GetNumConnection() const								{ return mConnections.Size(); }
		Connection* GetConnection(uint32 index = 0) const;
		bool IsCompatibleWith(const Port& otherPort) const;
		void Clear();
		void ClearCompatibleTypes();


		// TODO deprecate these, and implement something like GetChannel() and SetChannel()
		Core::AttributeBool*		GetBool() const;
		Core::AttributeInt32*		GetInt32() const;
		Core::AttributeFloat*		GetFloat() const;
		Core::AttributeString*		GetString() const;
		Core::AttributeColor*		GetColor() const;

		Core::Attribute* GetValueAttribute() const;

		// setup port of given attribute type
		void Setup(const char* name, const char* internalName, uint32 attributeTypeID, uint32 portID);

		// Number Attribute helpers
		void SetupAsNumber(const char* name, const char* internalName, uint32 portID);
		float GetValue() const;
		bool  GetValueAsBool() const;

		// Channel Attribute helpers
		template <class T> 
		  void SetupAsChannels (const char* name, const char* internalName, uint32 portID)				{ return Setup(name, internalName, AttributeChannels<T>::TYPE_ID, portID); }

		MultiChannel* GetChannels() const;

	private:
		Core::String				mName;
		Core::String				mInternalName;

		Core::Attribute*			mValue;					// the value stored inside the port
		Core::Array<Connection*>	mConnections;			// the connections plugged in this port
		uint32						mCompatibleTypes[4];	// four possible compatible types

		uint32						mPortId;				// the unique port ID (unique inside the node input or output port lists)
		EDirection					mPortDirection;

		bool						mIsVisible;				// if the port should be drawn or not
};


// input port
class InputPort : public Port
{
	public:
		InputPort() : Port(Port::INPUT) {}
		virtual ~InputPort() {};
};


// output port
class OutputPort : public Port
{
	public:
		OutputPort() : Port(Port::OUTPUT) {}
		virtual ~OutputPort() {};
};


#endif
