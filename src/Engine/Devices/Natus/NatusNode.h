/****************************************************************************
**
** Copyright 2022 neuromore co
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

#ifndef __NEUROMORE_NATUSNODE_H
#define __NEUROMORE_NATUSNODE_H

// include the required headers
#include "../../Config.h"
#include "../../Graph/DeviceInputNode.h"
#ifdef INCLUDE_DEVICE_NATUS


// Natus device node
class ENGINE_API NatusNode : public DeviceInputNode
{
    public:
        enum { TYPE_ID = 0xD00000 | NatusDevice::TYPE_ID };
        static const char* Uuid() { return "24b358c9-7af2-4fab-9c3c-0d47635458ac"; }        // Add UUID

        // constructor & destructor
        NatusNode(Graph* parentGraph) : DeviceInputNode(parentGraph, NatusDevice::TYPE_ID) {}
        ~NatusNode() {}

        Core::Color GetColor() const override                   { return Core::RGBA(30, 180, 158); }
        uint32 GetType() const override                         { return TYPE_ID; }
        const char* GetTypeUuid() const override final          { return Uuid(); }
        const char* GetReadableType() const override            { return "Natus"; }
        const char* GetRuleName() const override final          { return NatusDevice::GetRuleName(); }
        GraphObject* Clone(Graph* parentObject) override        { NatusNode* clone = new NatusNode(parentObject); return clone; }
};

#endif

#endif