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

#ifndef __NEUROMORE_GRAPHOBJECTFACTORY_H
#define __NEUROMORE_GRAPHOBJECTFACTORY_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "Node.h"


class ENGINE_API GraphObjectFactory
{
	public:
		// constructor & destructor
		GraphObjectFactory();
		virtual ~GraphObjectFactory();

		// object construction helpers
		GraphObject* CreateObjectByTypeUuid(Graph* graph, const char* typeUuid);
		GraphObject* CreateObjectByTypeID(Graph* graph, uint32 typeID);
		GraphObject* CreateObject(Graph* graph, GraphObject* registeredObject);

		// object registering
		bool RegisterObjectType(GraphObject* object);
		
		// accessors
		inline uint32 GetNumRegisteredObjects() const								{ return mRegisteredObjects.Size(); }
		inline GraphObject* GetRegisteredObject(uint32 index)						{ return mRegisteredObjects[index]; }

	private:
		// object find helpers
		uint32 FindRegisteredObject(GraphObject* object) const						{ for (uint32 i=0; i<mRegisteredObjects.Size(); ++i) if (mRegisteredObjects[i] == object) return i; return CORE_INVALIDINDEX32; }
		uint32 FindRegisteredObjectByTypeID(uint32 typeID) const					{ for (uint32 i=0; i<mRegisteredObjects.Size(); ++i) if (mRegisteredObjects[i]->GetType() == typeID) return i; return CORE_INVALIDINDEX32; }
		uint32 FindRegisteredObjectByTypeUuid(const char* typeUuid) const			{ for (uint32 i=0; i<mRegisteredObjects.Size(); ++i) if (strcmp(typeUuid, mRegisteredObjects[i]->GetTypeUuid()) == 0) return i; return CORE_INVALIDINDEX32; }

		Core::Array<GraphObject*> mRegisteredObjects;
};


#endif
