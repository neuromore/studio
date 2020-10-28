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

#ifndef __NEUROMORE_NOTIONNODE_H
#define __NEUROMORE_NOTIONNODE_H

// include the required headers
#include "../../Config.h"
#include "../../Graph/DeviceInputNode.h"

#ifdef INCLUDE_DEVICE_NEUROSITY_NOTION


// Notion1 device node
class ENGINE_API Notion1Node : public DeviceInputNode
{
    public:
        enum { TYPE_ID = 0xD00000 | Notion1Device::TYPE_ID };
        static const char* Uuid() { return "23d427d8-11d5-417d-80e0-52636ec49339"; }        // Add UUID

        //constructor and destructor
        Notion1Node(Graph* parentGraph) : DeviceInputNode(parentGraph, Notion1Device::TYPE_ID) {}
        ~Notion1Node() {}

        Core::Color GetColor() const override                   { return Core::RGBA(30, 180, 158); }
        uint32 GetType() const override                         { return TYPE_ID; }
        const char* GetTypeUuid() const override final          { return Uuid(); }
        const char* GetReadableType() const override            { return "Notion1"; }
        const char* GetRuleName() const override final          { return Notion1Device::GetRuleName(); }
        GraphObject* Clone(Graph* parentObject) override        { Notion1Node* clone = new Notion1Node(parentObject); return clone; }
};


// Notion2 device node
class ENGINE_API Notion2Node : public DeviceInputNode
{
    public:
        enum { TYPE_ID = 0xD00000 | Notion2Device::TYPE_ID };
        static const char* Uuid() { return "1cd1e229-b6ac-4d70-92c6-9e987f49e9cf"; }        // Add UUID

        // constructor & destructor
        Notion2Node(Graph* parentGraph) : DeviceInputNode(parentGraph, Notion2Device::TYPE_ID) {}
        ~Notion2Node() {}

        Core::Color GetColor() const override                   { return Core::RGBA(30, 180, 158); }    // need to change color at some point to neurosity colors
        uint32 GetType() const override                         { return TYPE_ID; }
        const char* GetTypeUuid() const override final          { return Uuid(); }
        const char* GetReadableType() const override            { return "Notion2"; }
        const char* GetRuleName() const override final          { return Notion2Device::GetRuleName(); }
        GraphObject* Clone(Graph* parentObject) override        { Notion2Node* clone = new Notion2Node(parentObject); return clone; }
};

#endif

#endif