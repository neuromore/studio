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

#ifndef __NEUROMORE_EEMAGINENODE_H
#define __NEUROMORE_EEMAGINENODE_H

// include the required headers
#include "../../Config.h"
#include "../../Graph/DeviceInputNode.h"
#include "eemagineDevices.h"

#ifdef INCLUDE_DEVICE_EEMAGINE


class ENGINE_API eemagineEE214Node : public DeviceInputNode
{
public:
   enum { TYPE_ID = 0xD00000 | eemagineEE214Device::TYPE_ID };
   static const char* Uuid() { return "f3c996aa-e1a1-4106-ab64-d6f68e5504c2"; }

   // constructor & destructor
   eemagineEE214Node(Graph* parentGraph) : DeviceInputNode(parentGraph, eemagineEE214Device::TYPE_ID) {}
   ~eemagineEE214Node() {}

   Core::Color GetColor() const override { return Core::RGBA(255, 104, 1); }
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetTypeUuid() const override final { return Uuid(); }
   const char* GetReadableType() const override { return "eego EE-214"; }
   const char* GetRuleName() const override final { return eemagineEE214Device::GetRuleName(); }
   GraphObject* Clone(Graph* parentObject) override { eemagineEE214Node* clone = new eemagineEE214Node(parentObject); return clone; }
};


#endif

#endif
