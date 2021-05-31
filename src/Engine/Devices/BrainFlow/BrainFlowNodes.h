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

#ifndef __NEUROMORE_BRAINFLOWNODE_H
#define __NEUROMORE_BRAINFLOWNODE_H

#include "../../Config.h"

#ifdef INCLUDE_DEVICE_BRAINFLOW

#include "BrainFlowDevices.h"
#include "../../Graph/DeviceInputNode.h"
#include <brainflow/board_controller/brainflow_input_params.h>


class BrainFlowNodeBase : public DeviceInputNode
{
public:

	~BrainFlowNodeBase() {}
	BrainFlowNodeBase(Graph* parentGraph, uint32 deviceType) : DeviceInputNode(parentGraph, deviceType) { mBoardID = (int)BoardIds::SYNTHETIC_BOARD; }

	void Init() override;

	const BrainFlowInputParams& GetParams() const { return mParams; }
	int GetBoardID() const { return mBoardID; }

	void OnAttributesChanged() override;

	BrainFlowDevice* GetCurrentDevice() { return dynamic_cast<BrainFlowDevice*>(mCurrentDevice); }

	Device* FindDevice();

	void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;

protected:
	void SynchronizeParams();
	void CreateNewDevice();

	int mBoardID;
	BrainFlowInputParams mParams;
};

class ENGINE_API BrainFlowNode : public BrainFlowNodeBase
{
	public:
		enum { TYPE_ID = 0xD00000 | BrainFlowDevice::TYPE_ID };
		static const char* Uuid()												{ return "283fc2da-fe1b-11e4-a322-1697f925ec7c"; }

		~BrainFlowNode()				   																	{}
		BrainFlowNode(Graph* parentGraph) : BrainFlowNodeBase(parentGraph, BrainFlowDevice::TYPE_ID)		{ mBoardID = (int)BoardIds::SYNTHETIC_BOARD; }

		Core::Color GetColor() const override									{ return Core::RGBA(60,120,210); }
		uint32 GetType() const override											{ return BrainFlowNode::TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "BrainFlow"; }
		const char* GetRuleName() const override final							{ return BrainFlowDevice::GetRuleName(); }
		GraphObject* Clone(Graph* parentGraph) override							{ BrainFlowNode* clone = new BrainFlowNode (parentGraph); return clone; }

};

class ENGINE_API BrainFlowCytonNode : public BrainFlowNodeBase
{
public:
	enum { TYPE_ID = 0xD00000 | BrainFlowCytonDevice::TYPE_ID };
	static const char* Uuid() { return "a8c6808f-3cc8-44e6-8d57-63ec144cbca9"; }
	

	~BrainFlowCytonNode() {}
	BrainFlowCytonNode(Graph* parentGraph) : BrainFlowNodeBase(parentGraph, BrainFlowCytonDevice::TYPE_ID) { mBoardID = (int)BoardIds::CYTON_BOARD; }

	void Init() override;
	void OnAttributesChanged() override;

	Core::Color GetColor() const override { return Core::RGBA(60, 120, 210); }
	uint32 GetType() const override { return BrainFlowCytonNode::TYPE_ID; }
	const char* GetTypeUuid() const override final { return Uuid(); }
	const char* GetReadableType() const override { return "Cyton"; }
	const char* GetRuleName() const override final { return BrainFlowCytonDevice::GetRuleName(); }
	GraphObject* Clone(Graph* parentGraph) override { BrainFlowCytonNode* clone = new BrainFlowCytonNode(parentGraph); return clone; }

};


#endif

#endif
