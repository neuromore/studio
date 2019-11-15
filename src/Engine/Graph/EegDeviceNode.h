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

#ifndef __NEUROMORE_EEGDEVICENODE_H
#define __NEUROMORE_EEGDEVICENODE_H

// include the required headers
#include "../Config.h"
#include "../EngineManager.h"
#include "DeviceInputNode.h"

// the SenseLabs Versus graph node
class ENGINE_API EegDeviceNode : public DeviceInputNode
{
	public:
		enum { TYPE_ID = 0xD00000 | 0x336 }; // "336" = "EEG"
		static const char* Uuid () { return "e0dbd00e-89b1-11e6-ae22-56b6b6499611"; }

		enum
		{
			OUTPUTPORT_EEG     = 0
		};

		enum
		{
			ATTRIB_ALLOWEDDEVICES = 0
		};

		EegDeviceNode(Graph* parentGraph) : DeviceInputNode(parentGraph, 0)		{}
		virtual ~EegDeviceNode()				   								{}

		Core::Color GetColor() const override									{ return Core::RGBA(0,114,135); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override								{ return Uuid(); }
		const char* GetReadableType() const override							{ return "EEG Device"; }
		const char* GetRuleName() const override								{ return "DEVICE_Generic"; } // DEVICE_EEG // Note: no rule for now ??? (rules are checked on a per-device basis); node should be available if the user can access at least one EEG device 
		GraphObject* Clone(Graph* parentGraph) override							{ EegDeviceNode* clone = new EegDeviceNode(parentGraph); return clone; }

		void Init() override;
		void Start(const Core::Time& elapsed) override;

		void OnAttributesChanged() override;
		
		Device* FindDevice() override;

		uint32 GetNumSensors() const override;
		Sensor* GetSensor(uint32 index) override;
		virtual bool IsSensorUsed(uint32 index);
};

#endif
