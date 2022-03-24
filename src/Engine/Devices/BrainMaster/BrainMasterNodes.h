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

#ifndef __NEUROMORE_BRAINMASTERNODES_H
#define __NEUROMORE_BRAINMASTERNODES_H

// include the required headers
#include "../../Config.h"
#include "../../Graph/DeviceInputNode.h"
#include "BrainMasterDevices.h"

#ifdef INCLUDE_DEVICE_BRAINMASTER

class ENGINE_API Discovery20Node : public DeviceInputNode
{
public:
   enum { TYPE_ID = 0xD00000 | Discovery20Device::TYPE_ID };
   static const char* Uuid() { return "fe2edadc-5641-4be7-97c4-caa615adee86"; }

   // constructor & destructor
   Discovery20Node(Graph* parentGraph) : DeviceInputNode(parentGraph, Discovery20Device::TYPE_ID) {}
   ~Discovery20Node() {}

   Core::Color GetColor() const override { return Core::RGBA(255, 104, 1); }
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetTypeUuid() const override final { return Uuid(); }
   const char* GetReadableType() const override { return "Discovery 20"; }
   const char* GetRuleName() const override final { return Discovery20Device::GetRuleName(); }
   GraphObject* Clone(Graph* parentObject) override { Discovery20Node* clone = new Discovery20Node(parentObject); return clone; }
};

#endif

#endif
