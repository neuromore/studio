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

class ENGINE_API eemagineEE211Node : public DeviceInputNode
{
public:
   enum { TYPE_ID = 0xD00000 | eemagineEE211Device::TYPE_ID };
   static const char* Uuid() { return "c5218bdc-a634-4f78-935e-cd658160572f"; }

   // constructor & destructor
   eemagineEE211Node(Graph* parentGraph) : DeviceInputNode(parentGraph, eemagineEE211Device::TYPE_ID) {}
   ~eemagineEE211Node() {}

   Core::Color GetColor() const override { return Core::RGBA(255, 104, 1); }
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetTypeUuid() const override final { return Uuid(); }
   const char* GetReadableType() const override { return "eego EE-211"; }
   const char* GetRuleName() const override final { return eemagineEE211Device::GetRuleName(); }
   GraphObject* Clone(Graph* parentObject) override { eemagineEE211Node* clone = new eemagineEE211Node(parentObject); return clone; }
};

class ENGINE_API eemagineEE212Node : public DeviceInputNode
{
public:
   enum { TYPE_ID = 0xD00000 | eemagineEE212Device::TYPE_ID };
   static const char* Uuid() { return "739fc76e-9cae-486b-8122-a52299ea9a68"; }

   // constructor & destructor
   eemagineEE212Node(Graph* parentGraph) : DeviceInputNode(parentGraph, eemagineEE212Device::TYPE_ID) {}
   ~eemagineEE212Node() {}

   Core::Color GetColor() const override { return Core::RGBA(255, 104, 1); }
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetTypeUuid() const override final { return Uuid(); }
   const char* GetReadableType() const override { return "eego EE-212"; }
   const char* GetRuleName() const override final { return eemagineEE212Device::GetRuleName(); }
   GraphObject* Clone(Graph* parentObject) override { eemagineEE212Node* clone = new eemagineEE212Node(parentObject); return clone; }
};

class ENGINE_API eemagineEE213Node : public DeviceInputNode
{
public:
   enum { TYPE_ID = 0xD00000 | eemagineEE213Device::TYPE_ID };
   static const char* Uuid() { return "d1d464c4-651b-4962-bd75-5e265e20cd52"; }

   // constructor & destructor
   eemagineEE213Node(Graph* parentGraph) : DeviceInputNode(parentGraph, eemagineEE213Device::TYPE_ID) {}
   ~eemagineEE213Node() {}

   Core::Color GetColor() const override { return Core::RGBA(255, 104, 1); }
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetTypeUuid() const override final { return Uuid(); }
   const char* GetReadableType() const override { return "eego EE-213"; }
   const char* GetRuleName() const override final { return eemagineEE213Device::GetRuleName(); }
   GraphObject* Clone(Graph* parentObject) override { eemagineEE213Node* clone = new eemagineEE213Node(parentObject); return clone; }
};

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

class ENGINE_API eemagineEE215Node : public DeviceInputNode
{
public:
   enum { TYPE_ID = 0xD00000 | eemagineEE215Device::TYPE_ID };
   static const char* Uuid() { return "d95163bc-7653-4a99-a1c8-da9ad19af8c8"; }

   // constructor & destructor
   eemagineEE215Node(Graph* parentGraph) : DeviceInputNode(parentGraph, eemagineEE215Device::TYPE_ID) {}
   ~eemagineEE215Node() {}

   Core::Color GetColor() const override { return Core::RGBA(255, 104, 1); }
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetTypeUuid() const override final { return Uuid(); }
   const char* GetReadableType() const override { return "eego EE-215"; }
   const char* GetRuleName() const override final { return eemagineEE215Device::GetRuleName(); }
   GraphObject* Clone(Graph* parentObject) override { eemagineEE215Node* clone = new eemagineEE215Node(parentObject); return clone; }
};

class ENGINE_API eemagineEE221Node : public DeviceInputNode
{
public:
   enum { TYPE_ID = 0xD00000 | eemagineEE221Device::TYPE_ID };
   static const char* Uuid() { return "6c9907d2-5de6-4d21-aedf-d711cc76b212"; }

   // constructor & destructor
   eemagineEE221Node(Graph* parentGraph) : DeviceInputNode(parentGraph, eemagineEE221Device::TYPE_ID) {}
   ~eemagineEE221Node() {}

   Core::Color GetColor() const override { return Core::RGBA(255, 104, 1); }
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetTypeUuid() const override final { return Uuid(); }
   const char* GetReadableType() const override { return "eego EE-221"; }
   const char* GetRuleName() const override final { return eemagineEE221Device::GetRuleName(); }
   GraphObject* Clone(Graph* parentObject) override { eemagineEE221Node* clone = new eemagineEE221Node(parentObject); return clone; }
};

class ENGINE_API eemagineEE222Node : public DeviceInputNode
{
public:
   enum { TYPE_ID = 0xD00000 | eemagineEE222Device::TYPE_ID };
   static const char* Uuid() { return "39f285c1-b070-48a9-b571-81bf05366b55"; }

   // constructor & destructor
   eemagineEE222Node(Graph* parentGraph) : DeviceInputNode(parentGraph, eemagineEE222Device::TYPE_ID) {}
   ~eemagineEE222Node() {}

   Core::Color GetColor() const override { return Core::RGBA(255, 104, 1); }
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetTypeUuid() const override final { return Uuid(); }
   const char* GetReadableType() const override { return "eego EE-222"; }
   const char* GetRuleName() const override final { return eemagineEE222Device::GetRuleName(); }
   GraphObject* Clone(Graph* parentObject) override { eemagineEE222Node* clone = new eemagineEE222Node(parentObject); return clone; }
};

class ENGINE_API eemagineEE223Node : public DeviceInputNode
{
public:
   enum { TYPE_ID = 0xD00000 | eemagineEE223Device::TYPE_ID };
   static const char* Uuid() { return "59b65731-80ca-474e-a072-5517403766f7"; }

   // constructor & destructor
   eemagineEE223Node(Graph* parentGraph) : DeviceInputNode(parentGraph, eemagineEE223Device::TYPE_ID) {}
   ~eemagineEE223Node() {}

   Core::Color GetColor() const override { return Core::RGBA(255, 104, 1); }
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetTypeUuid() const override final { return Uuid(); }
   const char* GetReadableType() const override { return "eego EE-223"; }
   const char* GetRuleName() const override final { return eemagineEE223Device::GetRuleName(); }
   GraphObject* Clone(Graph* parentObject) override { eemagineEE223Node* clone = new eemagineEE223Node(parentObject); return clone; }
};

class ENGINE_API eemagineEE224Node : public DeviceInputNode
{
public:
   enum { TYPE_ID = 0xD00000 | eemagineEE224Device::TYPE_ID };
   static const char* Uuid() { return "82e7aefd-1a7a-4354-a82b-61c0dff4a1cf"; }

   // constructor & destructor
   eemagineEE224Node(Graph* parentGraph) : DeviceInputNode(parentGraph, eemagineEE224Device::TYPE_ID) {}
   ~eemagineEE224Node() {}

   Core::Color GetColor() const override { return Core::RGBA(255, 104, 1); }
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetTypeUuid() const override final { return Uuid(); }
   const char* GetReadableType() const override { return "eego EE-224"; }
   const char* GetRuleName() const override final { return eemagineEE224Device::GetRuleName(); }
   GraphObject* Clone(Graph* parentObject) override { eemagineEE224Node* clone = new eemagineEE224Node(parentObject); return clone; }
};

class ENGINE_API eemagineEE225Node : public DeviceInputNode
{
public:
   enum { TYPE_ID = 0xD00000 | eemagineEE225Device::TYPE_ID };
   static const char* Uuid() { return "0d2c630d-7208-407d-ace3-a1d27d041f92"; }

   // constructor & destructor
   eemagineEE225Node(Graph* parentGraph) : DeviceInputNode(parentGraph, eemagineEE225Device::TYPE_ID) {}
   ~eemagineEE225Node() {}

   Core::Color GetColor() const override { return Core::RGBA(255, 104, 1); }
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetTypeUuid() const override final { return Uuid(); }
   const char* GetReadableType() const override { return "eego EE-225"; }
   const char* GetRuleName() const override final { return eemagineEE225Device::GetRuleName(); }
   GraphObject* Clone(Graph* parentObject) override { eemagineEE225Node* clone = new eemagineEE225Node(parentObject); return clone; }
};

class ENGINE_API eemagineEE410Node : public DeviceInputNode
{
public:
   enum { TYPE_ID = 0xD00000 | eemagineEE410Device::TYPE_ID };
   static const char* Uuid() { return "c1a887a5-2783-448c-865c-430d9b3db7e6"; }

   // constructor & destructor
   eemagineEE410Node(Graph* parentGraph) : DeviceInputNode(parentGraph, eemagineEE410Device::TYPE_ID) {}
   ~eemagineEE410Node() {}

   Core::Color GetColor() const override { return Core::RGBA(255, 104, 1); }
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetTypeUuid() const override final { return Uuid(); }
   const char* GetReadableType() const override { return "eego EE-410"; }
   const char* GetRuleName() const override final { return eemagineEE410Device::GetRuleName(); }
   GraphObject* Clone(Graph* parentObject) override { eemagineEE410Node* clone = new eemagineEE410Node(parentObject); return clone; }
};

class ENGINE_API eemagineEE411Node : public DeviceInputNode
{
public:
   enum { TYPE_ID = 0xD00000 | eemagineEE411Device::TYPE_ID };
   static const char* Uuid() { return "a948f054-f658-4f18-852f-1f62f4d56f6d"; }

   // constructor & destructor
   eemagineEE411Node(Graph* parentGraph) : DeviceInputNode(parentGraph, eemagineEE411Device::TYPE_ID) {}
   ~eemagineEE411Node() {}

   Core::Color GetColor() const override { return Core::RGBA(255, 104, 1); }
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetTypeUuid() const override final { return Uuid(); }
   const char* GetReadableType() const override { return "eego EE-411"; }
   const char* GetRuleName() const override final { return eemagineEE411Device::GetRuleName(); }
   GraphObject* Clone(Graph* parentObject) override { eemagineEE411Node* clone = new eemagineEE411Node(parentObject); return clone; }
};

class ENGINE_API eemagineEE430Node : public DeviceInputNode
{
public:
   enum { TYPE_ID = 0xD00000 | eemagineEE430Device::TYPE_ID };
   static const char* Uuid() { return "679bfc6f-afaf-46a9-b6d4-c353a718861f"; }

   // constructor & destructor
   eemagineEE430Node(Graph* parentGraph) : DeviceInputNode(parentGraph, eemagineEE430Device::TYPE_ID) {}
   ~eemagineEE430Node() {}

   Core::Color GetColor() const override { return Core::RGBA(255, 104, 1); }
   uint32 GetType() const override { return TYPE_ID; }
   const char* GetTypeUuid() const override final { return Uuid(); }
   const char* GetReadableType() const override { return "eego EE-430"; }
   const char* GetRuleName() const override final { return eemagineEE430Device::GetRuleName(); }
   GraphObject* Clone(Graph* parentObject) override { eemagineEE430Node* clone = new eemagineEE430Node(parentObject); return clone; }
};
#endif

#endif
