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

// include required files
#include "BrainFlowDevices.h"
#include "../../EngineManager.h"
#include "../../Core/LogManager.h"

#ifdef INCLUDE_DEVICE_BRAINFLOW

using namespace Core;

BrainFlowDeviceBase::BrainFlowDeviceBase(int boardId, DeviceDriver* driver)
	: mBoardId(boardId), mBoardShim(nullptr)
{
	mState = STATE_IDLE;
	CreateSensors();
}

void BrainFlowDeviceBase::CreateBoardShim(BrainFlowInputParams params)
{
	CORE_ASSERT(mBoardShim == NULL);
	mBoardShim = new BoardShim(mBoardId, params); 
}

BoardShim* BrainFlowDeviceBase::GetBoardShim() { 
	return mBoardShim; 
}

double BrainFlowDeviceBase::GetSampleRate() const
{
	try
	{
		double samplingRate = BoardShim::get_sampling_rate(mBoardId);
		return samplingRate;
	}
	catch (const BrainFlowException& err)
	{
		LogError(err.what());
		return 250;
	}
}

void BrainFlowDeviceBase::ReleaseBoardShim()
{
	if (mBoardShim != NULL)
	{
		// boardshim destructor doesnt call release_session, it should be called manually before
		try
		{
			if (mBoardShim->is_prepared())
			{
				LogError("Removing BoardShim object with prepared session!");
			}
		}
		catch (const BrainFlowException& err)
		{
			// do nothing
		}
		delete mBoardShim;
		mBoardShim = NULL;
	}
}

// get the available electrodes of the neuro headset
void BrainFlowDeviceBase::CreateElectrodes()
{
	mElectrodes.Clear();

	try
	{
		int len = 0;
		std::string* eegNames = BoardShim::get_eeg_names(mBoardId, &len);
		mElectrodes.Reserve(len);
		// todo check that BrainFlow IDs match studio IDs
		for (int i = 0; i < len; i++)
		{
			mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID(eegNames[i].c_str()));
		}
	}
	catch (const BrainFlowException& err)
	{
		LogError(err.what());
	}
}

BrainFlowDeviceCyton::BrainFlowDeviceCyton(DeviceDriver* driver)
	: BrainFlowDeviceBase((int)BoardIds::CYTON_BOARD, driver) {}

void BrainFlowDeviceCyton::Update(const Core::Time& elapsed, const Core::Time& delta)
{
	const double a = 100; // amplitude +-300uv
	for (uint32 i = 0; i < mSensors.Size(); ++i)
	{
		auto* sensor = mSensors[i];
		auto offsetSampleTime = elapsed.InSeconds();
		auto value = 100 * sin(2.0 * Math::pi * 1 * offsetSampleTime);
		// ac noise
		value += 0.3 * a * ((double)rand() / RAND_MAX - 0.5) * 2.0;
		sensor->AddQueuedSample(value);
	}
	// update the neuro headset
	BciDevice::Update(elapsed, delta);
}
#endif
