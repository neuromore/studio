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

#ifndef __NEUROMORE_BRAINFLOW_H
#define __NEUROMORE_BRAINFLOW_H

#include <Config.h>
#include <DeviceDriver.h>
#include <Devices/BrainFlow/BrainFlowDevices.h>
#include <EngineManager.h>
#include <Core/EventHandler.h>
#include <Core/Array.h>

#include <QObject>

#include <brainflow/cpp-package/board_shim.h>

class BrainFlowDriver : public QObject, public DeviceDriver, public Core::EventHandler
{
public:
	BrainFlowDriver();
	virtual ~BrainFlowDriver() = default;

	void DetectDevices() override;

	const char* GetName() const override;
	uint32 GetType() const override;
	bool HasAutoDetectionSupport() const override { return false; }
	bool Init() override;
	void Update(const Core::Time& delta, const Core::Time& elapsed) override {};

	void OnDeviceAdded(Device* device) override;
	void OnRemoveDevice(Device* device) override;

	void OnRemoveNode(Graph* graph, Node* node) override;

	Device* CreateDevice(BoardIds boardId, BrainFlowInputParams params);
private:
	Device* CreateDevice(uint32 deviceTypeID) override { return nullptr; };
};

#endif
