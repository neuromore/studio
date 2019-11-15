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

#ifndef __NEUROMORE_ADVANCEDBRAINMONITORING_NODE_H
#define __NEUROMORE_ADVANCEDBRAINMONITORING_NODE_H

// include the required headers
#include "../../Config.h"
#include "../../Graph/DeviceInputNode.h"
#include "AbmDevices.h"

#ifdef INCLUDE_DEVICE_ADVANCEDBRAINMONITORING


class ENGINE_API AbmX10Node : public DeviceInputNode
{
public:
	enum { TYPE_ID = 0xD00000 | AbmX10Device::TYPE_ID };
	static const char* Uuid()														{ return "ccffc386-86ee-11e5-af63-feff819cdc9f"; }

	// constructor & destructor
	AbmX10Node(Graph* parentGraph) : DeviceInputNode(parentGraph, AbmX10Device::TYPE_ID)	{}
	~AbmX10Node()				   													{}
	
	// hide baseclass attribute
	void Init() override
	{
		DeviceInputNode::Init();
		GetAttributeSettings(ATTRIB_DEVICEINDEX)->SetVisible(false);
	}
	
	Core::Color GetColor() const override											{ return Core::RGBA(102, 45, 145); }
	uint32 GetType() const override													{ return TYPE_ID; }
	const char* GetTypeUuid() const override final									{ return Uuid(); }
	const char* GetReadableType() const override									{ return "B-Alert X10"; }
	const char* GetRuleName() const override final									{ return AbmX10Device::GetRuleName(); }
	GraphObject* Clone(Graph* parentObject) override								{ AbmX10Node* clone = new AbmX10Node(parentObject); return clone; }
};


class AbmX24Node : public DeviceInputNode
{
	public:
		enum { TYPE_ID = 0xD00000 | AbmX24Device::TYPE_ID };
		static const char* Uuid ()														{ return "a43c9614-dd01-11e4-b9d6-1681e6b88ec1"; }

		// constructor & destructor
		AbmX24Node(Graph* parentGraph) : DeviceInputNode(parentGraph, AbmX24Device::TYPE_ID)	{}
		~AbmX24Node()																	{}
		
		// hide baseclass attribute
		void Init() override
		{ 
			DeviceInputNode::Init();
			GetAttributeSettings(ATTRIB_DEVICEINDEX)->SetVisible(false);
		}

		Core::Color GetColor() const override											{ return Core::RGBA(102,45,145); }
		uint32 GetType() const override													{ return TYPE_ID; }
		const char* GetTypeUuid() const override final									{ return Uuid(); }
		const char* GetReadableType() const override									{ return "B-Alert X24"; }
		const char* GetRuleName() const override final									{ return AbmX24Device::GetRuleName(); }
		GraphObject* Clone(Graph* parentObject) override								{ AbmX24Node* clone = new AbmX24Node(parentObject); return clone; }

};

#endif

#endif
