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

#ifndef __NEUROMORE_GENERICDEVICENODES_H
#define __NEUROMORE_GENERICDEVICENODES_H

// include the required headers
#include "../../Config.h"
#include "../../Graph/DeviceInputNode.h"
#include "GenericDevices.h"

// Heart Rate
class HeartRateNode : public DeviceInputNode
{
	public:
		enum { TYPE_ID = 0xD00000 | HeartRateDevice::TYPE_ID };
		static const char* Uuid ()									{ return "db98a1d4-5d71-11e5-885d-feff819cdc9f"; }

		// constructor & destructor
		HeartRateNode(Graph* parentGraph) : DeviceInputNode(parentGraph, HeartRateDevice::TYPE_ID) 	{}
		~HeartRateNode()				   														{}

		Core::Color GetColor() const override						{ return Core::RGBA(102,45,145); }
		uint32 GetType() const override								{ return TYPE_ID; }
		const char* GetTypeUuid() const override final				{ return Uuid(); }
		const char* GetReadableType() const override				{ return "HeartRate"; }
		const char* GetRuleName() const override final				{ return HeartRateDevice::GetRuleName(); }
		GraphObject* Clone(Graph* parentObject) override			{ HeartRateNode* clone = new HeartRateNode(parentObject); return clone; }
};

////////////////////////////////////////////////////////////////////////////////////

// Accelerometer
class AccelerometerNode : public DeviceInputNode
{
	public:
		enum { TYPE_ID = 0xD00000 | AccelerometerDevice::TYPE_ID };
		static const char* Uuid()									{ return "db98ef86-5d71-11e5-885d-feff819cdc9f"; }

		// constructor & destructor
		AccelerometerNode(Graph* parentGraph) : DeviceInputNode(parentGraph, AccelerometerDevice::TYPE_ID) 	{}
		~AccelerometerNode()				   														{}

		Core::Color GetColor() const override						{ return Core::RGBA(102, 45, 145); }
		uint32 GetType() const override								{ return TYPE_ID; }
		const char* GetTypeUuid() const override final				{ return Uuid(); }
		const char* GetReadableType() const override				{ return "Accelerometer"; }
		const char* GetRuleName() const override final				{ return AccelerometerDevice::GetRuleName(); }
		GraphObject* Clone(Graph* parentObject) override			{ AccelerometerNode* clone = new AccelerometerNode(parentObject); return clone; }
};

////////////////////////////////////////////////////////////////////////////////////


// Gyroscope
class GyroscopeNode : public DeviceInputNode
{
	public:
		enum { TYPE_ID = 0xD00000 | GyroscopeDevice::TYPE_ID };
		static const char* Uuid()									{ return "db98aad0-5d71-11e5-885d-feff819cdc9f"; }

		// constructor & destructor
		GyroscopeNode(Graph* parentGraph) : DeviceInputNode(parentGraph, GyroscopeDevice::TYPE_ID) 	{}
		~GyroscopeNode()				   															{}

		Core::Color GetColor() const override						{ return Core::RGBA(102, 45, 145); }
		uint32 GetType() const override								{ return TYPE_ID; }
		const char* GetTypeUuid() const override final				{ return Uuid(); }
		const char* GetReadableType() const override				{ return "Gyroscope"; }
		const char* GetRuleName() const override final				{ return GyroscopeDevice::GetRuleName(); }
		GraphObject* Clone(Graph* parentObject) override			{ GyroscopeNode* clone = new GyroscopeNode(parentObject); return clone; }
};

#endif
