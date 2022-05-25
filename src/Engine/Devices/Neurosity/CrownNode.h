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

#ifndef __NEUROMORE_CROWNNODE_H
#define __NEUROMORE_CROWNNODE_H

// include the required headers
#include "../../Config.h"
#include "../../Graph/DeviceInputNode.h"

// Crown device node
class ENGINE_API CrownNode : public DeviceInputNode
{
    public:
        enum { TYPE_ID = 0xD00000 | CrownDevice::TYPE_ID };
        static const char* Uuid() { return "a993213e-bfbf-11ec-9d64-0242ac120002"; }        // Add UUID

        // constructor & destructor
        CrownNode(Graph* parentGraph) : DeviceInputNode(parentGraph, CrownDevice::TYPE_ID) {}
        ~CrownNode() {}

        Core::Color GetColor() const override                   { return Core::RGBA(30, 180, 158); }    // need to change color at some point to neurosity colors
        uint32 GetType() const override                         { return TYPE_ID; }
        const char* GetTypeUuid() const override final          { return Uuid(); }
        const char* GetReadableType() const override            { return "Crown"; }
        const char* GetRuleName() const override final          { return CrownDevice::GetRuleName(); }
        GraphObject* Clone(Graph* parentObject) override        { CrownNode* clone = new CrownNode(parentObject); return clone; }
};

#endif
