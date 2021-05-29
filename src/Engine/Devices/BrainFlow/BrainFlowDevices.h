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

// the base class for all OpenBCI devices
class ENGINE_API BrainFlowDeviceBase : public BciDevice
{

public:
	BrainFlowDeviceBase(DeviceDriver* driver = NULL);
	BrainFlowDeviceBase(BoardIds boardId, DeviceDriver* deviceDriver = nullptr);
	BrainFlowDeviceBase(BoardIds boardId, BrainFlowInputParams params, DeviceDriver* deviceDriver = nullptr);
	virtual ~BrainFlowDeviceBase() {}

	bool Connect() override;
	bool Disconnect() override;
	int GetBoardId() const;
	double GetSampleRate() const override;
	double GetLatency() const override { return 0.1; }
	double GetExpectedJitter() const override { return 0.1; }
	bool IsWireless() const override { return true; }
	const BrainFlowInputParams& GetParams() const { return mParams; }
	void Update(const Core::Time& elapsed, const Core::Time& delta) override;

	
protected:
	void CreateElectrodes();
	bool DoesConnectingFinished() const;
	bool InitAfterConnected();

private:
	const BoardIds mBoardId;
	const BrainFlowInputParams mParams;
	std::future<std::unique_ptr<BoardShim>> mFuture;
	std::unique_ptr<BoardShim> mBoard = nullptr;

};

class BrainFlowDevice : public BrainFlowDeviceBase
{
public:
	enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_BRAINFLOW };

	BrainFlowDevice(DeviceDriver* deviceDriver = nullptr) : BrainFlowDeviceBase(deviceDriver) {};
	BrainFlowDevice(BoardIds boardId, BrainFlowInputParams params, DeviceDriver* deviceDriver = nullptr) : BrainFlowDeviceBase(boardId, params, deviceDriver) {};
	
	uint32 GetType() const override { return TYPE_ID; }
	const char* GetTypeName() const override { return "BrainFlowDevice_type"; }
	const char* GetHardwareName() const override { return "BrainFlow"; }
	const char* GetUuid() const override { return "5108993a-fe1b-11e4-a322-1697f925e000"; }
	static const char* GetRuleName() { return "BrainFlowDevice_rule"; }

private:
	Device* Clone() override { return new BrainFlowDevice(); }	


};


class BrainFlowCytonDevice : public BrainFlowDeviceBase
{
public:
	enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_BRAINFLOW_CYTON };
	virtual uint32 GetType() const override { return BrainFlowCytonDevice::TYPE_ID; }

	BrainFlowCytonDevice(DeviceDriver* deviceDriver = nullptr) : BrainFlowDeviceBase(BoardIds::CYTON_BOARD, deviceDriver) {}
	BrainFlowCytonDevice(BoardIds boardId, BrainFlowInputParams params, DeviceDriver* deviceDriver = nullptr) : BrainFlowDeviceBase(BoardIds::CYTON_BOARD, params, deviceDriver) {}

	const char* GetTypeName() const override { return "BrainFlowCytonDevice_type"; }
	const char* GetHardwareName() const override { return "BrainFlowCytonDevice_hardware"; }
	const char* GetUuid() const override { return "5308993a-fe1b-11e4-a322-1697f925e000"; }
	static const char* GetRuleName() { return "BrainFlowCytonDevice_rule"; }

protected:
	Device* Clone() override { return new BrainFlowCytonDevice(); }

private:
	const BoardIds mBoardId = BoardIds::CYTON_BOARD;

};

#endif

#endif
