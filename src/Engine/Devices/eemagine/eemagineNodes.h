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

class ENGINE_API eemagine8Node : public DeviceInputNode
{
public:
   enum { TYPE_ID = 0xD00000 | eemagine8Device::TYPE_ID };
   static const char* Uuid() { return "4d7c5af3-39bc-4f5d-8a27-bdefa6df404c"; }

   // constructor & destructor
   eemagine8Node(Graph* parentGraph) : DeviceInputNode(parentGraph, eemagine8Device::TYPE_ID) {}
   ~eemagine8Node() {}

   Core::Color GetColor() const override { return Core::RGBA(255, 104, 1); }
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetTypeUuid() const override final { return Uuid(); }
   const char* GetReadableType() const override { return "eego 8ch"; }
   const char* GetRuleName() const override final { return eemagine8Device::GetRuleName(); }
   GraphObject* Clone(Graph* parentObject) override { eemagine8Node* clone = new eemagine8Node(parentObject); return clone; }
};

class ENGINE_API eemagine16Node : public DeviceInputNode
{
public:
   enum { TYPE_ID = 0xD00000 | eemagine16Device::TYPE_ID };
   static const char* Uuid() { return "deac9236-467c-4aae-884f-f449df7ff87f"; }

   // constructor & destructor
   eemagine16Node(Graph* parentGraph) : DeviceInputNode(parentGraph, eemagine16Device::TYPE_ID) {}
   ~eemagine16Node() {}

   Core::Color GetColor() const override { return Core::RGBA(255, 104, 1); }
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetTypeUuid() const override final { return Uuid(); }
   const char* GetReadableType() const override { return "eego 16ch"; }
   const char* GetRuleName() const override final { return eemagine16Device::GetRuleName(); }
   GraphObject* Clone(Graph* parentObject) override { eemagine16Node* clone = new eemagine16Node(parentObject); return clone; }
};

class ENGINE_API eemagine32Node : public DeviceInputNode
{
public:
   enum { TYPE_ID = 0xD00000 | eemagine32Device::TYPE_ID };
   static const char* Uuid() { return "dca2c60e-35a7-45b0-a3fa-1f7244193f10"; }

   // constructor & destructor
   eemagine32Node(Graph* parentGraph) : DeviceInputNode(parentGraph, eemagine32Device::TYPE_ID) {}
   ~eemagine32Node() {}

   Core::Color GetColor() const override { return Core::RGBA(255, 104, 1); }
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetTypeUuid() const override final { return Uuid(); }
   const char* GetReadableType() const override { return "eego 32ch"; }
   const char* GetRuleName() const override final { return eemagine32Device::GetRuleName(); }
   GraphObject* Clone(Graph* parentObject) override { eemagine32Node* clone = new eemagine32Node(parentObject); return clone; }
};

class ENGINE_API eemagine64Node : public DeviceInputNode
{
public:
   enum { TYPE_ID = 0xD00000 | eemagine64Device::TYPE_ID };
   static const char* Uuid() { return "815cd8a2-6a43-49db-b1ca-6beca340c6d0"; }

   // constructor & destructor
   eemagine64Node(Graph* parentGraph) : DeviceInputNode(parentGraph, eemagine64Device::TYPE_ID) {}
   ~eemagine64Node() {}

   Core::Color GetColor() const override { return Core::RGBA(255, 104, 1); }
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetTypeUuid() const override final { return Uuid(); }
   const char* GetReadableType() const override { return "eego 64ch"; }
   const char* GetRuleName() const override final { return eemagine64Device::GetRuleName(); }
   GraphObject* Clone(Graph* parentObject) override { eemagine64Node* clone = new eemagine64Node(parentObject); return clone; }
};

#endif

#endif
