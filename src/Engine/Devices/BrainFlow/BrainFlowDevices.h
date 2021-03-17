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

#ifndef __NEUROMORE_BRAINFLOWHEADSET_H
#define __NEUROMORE_BRAINFLOWHEADSET_H

// include required headers
#include "../../BciDevice.h"
#include "../../DeviceDriver.h"

#ifdef INCLUDE_DEVICE_BRAINFLOW

#include "brainflow/cpp-package/board_shim.h"
#include <brainflow/utils/brainflow_constants.h>
#include <brainflow/board_controller/brainflow_input_params.h>
#include <future>

class BrainFlowDevice : public BciDevice
{
public:
	enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_BRAINFLOW };

	BrainFlowDevice(DeviceDriver* deviceDriver = nullptr);
	BrainFlowDevice(BoardIds boardId, BrainFlowInputParams params, DeviceDriver* deviceDriver = nullptr);

	bool Connect() override;
	bool Disconnect() override;
	
	uint32 GetType() const override { return TYPE_ID; }
	const char* GetTypeName() const override { return "BrainFlowDevice_type"; }
	const char* GetHardwareName() const override { return "BrainFlowDevice_hardware"; }
	const char* GetUuid() const override { return "5108993a-fe1b-11e4-a322-1697f925e000"; }
	static const char* GetRuleName() { return "BrainFlowDevice_rule"; }
	void Update(const Core::Time& elapsed, const Core::Time& delta) override;

	const BrainFlowInputParams& GetParams() const { return mParams; }
	int GetBoardId() const;

private:
	void CreateElectrodes();
	Device* Clone() override { return new BrainFlowDevice(); }
	bool DoesConnectingFinished() const;
	bool InitAfterConnected();

	// information
	double GetSampleRate() const override;


private:
	const BoardIds mBoardId;
	const BrainFlowInputParams mParams;
	std::future<std::unique_ptr<BoardShim>> mFuture;
	std::unique_ptr<BoardShim> mBoard = nullptr;
};


#endif

#endif
