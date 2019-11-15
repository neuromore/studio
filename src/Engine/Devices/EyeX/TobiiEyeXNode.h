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

#ifndef __NEUROMORE_TOBIIEYEXNODE_H
#define __NEUROMORE_TOBIIEYEXNODE_H

#include "../../Config.h"
#include "../../Graph/DeviceInputNode.h"
#include "../../Core/StandardHeaders.h"
#include "TobiiEyeXDevice.h"

#ifdef INCLUDE_DEVICE_TOBIIEYEX

class ENGINE_API TobiiEyeXNode : public DeviceInputNode
{
	public:
		enum { TYPE_ID = 0xD00000 | TobiiEyeXDevice::TYPE_ID };
		static const char* Uuid()												{ return "bdc924b5-6e55-4f0c-a8d0-c25a5c05bb6f"; }

		// constructor & destructor
		TobiiEyeXNode(Graph* parentGraph) : DeviceInputNode(parentGraph, TobiiEyeXDevice::TYPE_ID)	{}
		virtual ~TobiiEyeXNode();

		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void OnAttributesChanged() override;

		uint32			GetType() const override								{ return TYPE_ID; }
		const char*		GetTypeUuid() const override final						{ return Uuid(); }
		const char*		GetReadableType() const override						{ return "EyeX"; }
		Core::Color GetColor() const override									{ return Core::RGBA(255, 104, 1); }
		const char* GetRuleName() const override final							{ return TobiiEyeXDevice::GetRuleName(); }
		GraphObject*    Clone(Graph* parentObject) override						{ TobiiEyeXNode* clone = new TobiiEyeXNode(parentObject); return clone; }

};

#endif

#endif
