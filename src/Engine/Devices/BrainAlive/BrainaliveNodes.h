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

#ifndef __NEUROMORE_BRAINALIVENODE_H
#define __NEUROMORE_BRAINALIVENODE_H

// include the required headers
#include "../../Config.h"
#include "BrainaliveDevices.h"
#include "../../Graph/DeviceInputNode.h"

#ifdef INCLUDE_DEVICE_BRAINALIVE


// normal OpenBCI device node
class ENGINE_API BrainAliveNode : public DeviceInputNode
{
public:
	enum { TYPE_ID = 0xD00000 | BrainAliveDevice::TYPE_ID };
	static const char* Uuid() { return "0000fe40-cc7a-482a-984a-7f2ed5b3e58f"; }

	BrainAliveNode(Graph* parentGraph) : DeviceInputNode(parentGraph, BrainAliveDevice::TYPE_ID) {}
	~BrainAliveNode() {}

	Core::Color GetColor() const override { return Core::RGBA(60, 120, 210); }
	uint32 GetType() const override { return TYPE_ID; }
	const char* GetTypeUuid() const override final { return Uuid(); }
	const char* GetReadableType() const override { return "BrainAlive"; }
	const char* GetRuleName() const override final { return BrainAliveDevice::GetRuleName(); }
	GraphObject* Clone(Graph* parentGraph) override { BrainAliveNode* clone = new BrainAliveNode(parentGraph); return clone; }

};


#endif

#endif
