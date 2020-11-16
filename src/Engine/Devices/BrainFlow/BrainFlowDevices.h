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

#include "brainflow/board_shim.h"

#ifdef INCLUDE_DEVICE_BRAINFLOW

// the base class for all BrainFlow Devices
class ENGINE_API BrainFlowDeviceBase : public BciDevice
{
	int mBoardId;
	BoardShim* mBoardShim;

	public:
		// constructors & destructor
		BrainFlowDeviceBase(int boardId, DeviceDriver* driver)				{ mState = STATE_IDLE; mBoardId = boardId; mBoardShim = NULL; }
		virtual ~BrainFlowDeviceBase()										{}
		
		// set/get board shim brainflow object
		void CreateBoardShim(BrainFlowInputParams params)					{ CORE_ASSERT(mBoardShim == NULL); mBoardShim = new BoardShim(mBoardId, params); }
		BoardShim* GetBoardShim()											{ return mBoardShim; }
		void ReleaseBoardShim();
		
		// information
		double GetSampleRate() const override;

		void CreateElectrodes () override;
};



// the default OpenBCI device class
class ENGINE_API BrainFlowDeviceCyton : public BrainFlowDeviceBase
{
public:
	enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_BRAINFLOW_CYTON };

	// constructors & destructor
	BrainFlowDeviceCyton (DeviceDriver* driver = NULL) : BrainFlowDeviceBase ((int)BoardIds::CYTON_BOARD, driver)	{}
	virtual ~BrainFlowDeviceCyton ()																				{}

	Device* Clone() override										{ return new BrainFlowDeviceCyton(); }

	// information
	uint32 GetType() const override									{ return TYPE_ID; }
	const char* GetTypeName() const override						{ return "openbci"; }
	const char* GetHardwareName() const override					{ return "OpenBCI"; }
	const char* GetUuid() const override							{ return "5108993a-fe1b-11e4-a322-1697f925ec7c"; }
	static const char* GetRuleName()								{ return "DEVICE_BrainFlowCyton"; }
	double GetLatency () const override								{ return 0.1; }
	double GetExpectedJitter () const override						{ return 0.1; }
	bool IsWireless () const override								{ return true; }
	double GetTimeoutLimit() const override							{ return 60; } // Long timeout limit because channel config takes so long
};


#endif

#endif
