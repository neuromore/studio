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

#ifndef __NEUROMORE_MITSARNODE_H
#define __NEUROMORE_MITSARNODE_H

// include the required headers
#include "../../Config.h"
#include "../../Graph/DeviceInputNode.h"
#include "MitsarDevices.h"

#if defined(NEUROMORE_PLATFORM_WINDOWS)

class ENGINE_API Mitsar201Node : public DeviceInputNode
{
	public:
		enum { TYPE_ID = 0xD00000 | Mitsar201Device::TYPE_ID };
		static const char* Uuid ()												{ return "472ea640-50b2-11e5-885d-feff819cdc9f"; }

		// constructor & destructor
		Mitsar201Node(Graph* parentGraph) : DeviceInputNode(parentGraph, Mitsar201Device::TYPE_ID)		{}
		~Mitsar201Node()				  										{}

		Core::Color GetColor() const override									{ return Core::RGBA(84,218,147); } // one of the colors from the Mitsar logo
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Mitsar-EEG 201"; }
		const char* GetRuleName() const override final							{ return Mitsar201Device::GetRuleName(); }
		GraphObject* Clone(Graph* parentObject) override						{ Mitsar201Node* clone = new Mitsar201Node(parentObject); return clone; }
};

#endif 

#endif
