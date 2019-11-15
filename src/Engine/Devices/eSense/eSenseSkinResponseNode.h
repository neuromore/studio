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

#ifndef __NEUROMORE_ESENSESKINRESPONSENODE_H
#define __NEUROMORE_ESENSESKINRESPONSENODE_H

// include the required headers
#include "../../Config.h"
#include "../../Graph/DeviceInputNode.h"
#include "eSenseSkinResponseDevice.h"

#ifdef INCLUDE_DEVICE_ESENSESKINRESPONSE


class ENGINE_API eSenseSkinResponseNode : public DeviceInputNode
{
	public:
		enum { TYPE_ID = 0xD00000 | eSenseSkinResponseDevice::TYPE_ID };
		static const char* Uuid ()												{ return "1b4ad4fe-ffcb-11e5-86aa-5e5517507c66"; }

		// constructor & destructor
		eSenseSkinResponseNode(Graph* parentGraph) : DeviceInputNode(parentGraph, eSenseSkinResponseDevice::TYPE_ID)	{}
		~eSenseSkinResponseNode()				 															{}

		Core::Color GetColor() const override									{ return Core::RGBA(155,187,89); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "eSense GSR"; }
		const char* GetRuleName() const override final							{ return eSenseSkinResponseDevice::GetRuleName(); }
		GraphObject* Clone(Graph* parentObject) override						{ eSenseSkinResponseNode* clone = new eSenseSkinResponseNode(parentObject); return clone; }
};


#endif

#endif
