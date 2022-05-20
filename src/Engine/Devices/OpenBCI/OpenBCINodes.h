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

#ifndef __NEUROMORE_OPENBCINODE_H
#define __NEUROMORE_OPENBCINODE_H

// include the required headers
#include "../../Config.h"
#include "OpenBCIDevices.h"
#include "../../Graph/DeviceInputNode.h"


// normal OpenBCI device node
class ENGINE_API OpenBCINode : public DeviceInputNode
{
	public:
		enum { TYPE_ID = 0xD00000 | OpenBCIDevice::TYPE_ID };
		static const char* Uuid () { return "283fc2da-fe1b-11e4-a322-1697f925ec7b"; }

		OpenBCINode(Graph* parentGraph) : DeviceInputNode(parentGraph, OpenBCIDevice::TYPE_ID)		{}
		~OpenBCINode()				   															{}

		Core::Color GetColor() const override									{ return Core::RGBA(60,120,210); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "OpenBCI"; }
		const char* GetRuleName() const override final							{ return OpenBCIDevice::GetRuleName(); }
		GraphObject* Clone(Graph* parentGraph) override							{ OpenBCINode* clone = new OpenBCINode(parentGraph); return clone; }
};


// OpenBCI + Daisy module device node
class OpenBCIDaisyNode : public DeviceInputNode
{
	public:
		enum { TYPE_ID = 0xD00000 | OpenBCIDaisyDevice::TYPE_ID };
		static const char* Uuid() { return "e136ae40-8ec9-11e5-8994-feff819cdc9f"; }

		OpenBCIDaisyNode(Graph* parentGraph) : DeviceInputNode(parentGraph, OpenBCIDaisyDevice::TYPE_ID)			{}
		~OpenBCIDaisyNode()				   																			{}

		Core::Color GetColor() const override									{ return Core::RGBA(60, 120, 210); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "OpenBCI Daisy"; }
		const char* GetRuleName() const override final							{ return OpenBCIDaisyDevice::GetRuleName(); }
		GraphObject* Clone(Graph* parentGraph) override							{ OpenBCIDaisyNode* clone = new OpenBCIDaisyNode(parentGraph); return clone; }
};

#endif
