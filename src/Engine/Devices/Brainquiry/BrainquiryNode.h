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

#ifndef __NEUROMORE_BRAINQUIRYNODE_H
#define __NEUROMORE_BRAINQUIRYNODE_H

// include the required headers
#include "../../Config.h"
#include "../../Graph/DeviceInputNode.h"
#include "BrainquiryDevice.h"

#ifdef INCLUDE_DEVICE_BRAINQUIRY

class ENGINE_API BrainquiryNode : public DeviceInputNode
{
	public:
		enum { TYPE_ID = 0xD00000 | BrainquiryDevice::TYPE_ID };
		static const char* Uuid () { return "f1f40742-c4f1-11e5-9912-ba0be0483c18"; }
		
		BrainquiryNode(Graph* parentGraph) : DeviceInputNode(parentGraph, BrainquiryDevice::TYPE_ID)		{}
		~BrainquiryNode()																			{}

		Core::Color GetColor() const override									{ return Core::RGBA(228,0,0); } // color from Brainquiry
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "PET 4.0"; }
		const char* GetRuleName() const override final							{ return BrainquiryDevice::GetRuleName(); }
		GraphObject* Clone(Graph* parentGraph) override							{ BrainquiryNode* clone = new BrainquiryNode(parentGraph); return clone; }
};


#endif

#endif
