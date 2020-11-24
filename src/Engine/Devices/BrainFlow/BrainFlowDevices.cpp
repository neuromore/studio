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

BrainFlowDevice::BrainFlowDevice(DeviceDriver* deviceDriver)
	: BciDevice(deviceDriver), mBoardId(BoardIds::SYNTHETIC_BOARD), mParams() 
{
	CreateSensors();
}

BrainFlowDevice::BrainFlowDevice(BoardIds boardId, BrainFlowInputParams params, DeviceDriver* deviceDriver)
	: BciDevice(deviceDriver), mBoardId(boardId), mParams(std::move(params))
{
	CreateSensors();
}

bool BrainFlowDevice::Connect()
{
	mBoard = std::make_unique<BoardShim>(getBoardId(), mParams);
	try
	{
		mBoard->prepare_session();
		mBoard->start_stream();
	}
	catch (const BrainFlowException& err)
	{
		mBoard.reset();
		LogError(err.what());
		return false;
	}
	return Device::Connect();
}

bool BrainFlowDevice::Disconnect()
{
	if (mBoard && mBoard->is_prepared())
	{
		try
		{
			mBoard->stop_stream();
			mBoard->release_session();
		}
		catch (const BrainFlowException& err)
		{
			LogError(err.what());
		}
	}
	mBoard.reset();
	return Device::Disconnect();
}

int BrainFlowDevice::getBoardId() const
{
	return static_cast<int>(mBoardId);
}

// get the available electrodes of the neuro headset
void BrainFlowDevice::CreateElectrodes()
{
	mElectrodes.Clear();
	try
	{
		int len = 0;
		std::string* eegNames = BoardShim::get_eeg_names(getBoardId(), &len);
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


void BrainFlowDevice::Update(const Core::Time& elapsed, const Core::Time& delta)
{
	if (!mBoard || !mBoard->is_prepared())
		return;
	try
	{
		int data_count;
		double** board_data;
		board_data = mBoard->get_board_data(&data_count);
		int channels_count;
		int* channels_numbers = BoardShim::get_eeg_channels(getBoardId(), &channels_count);
		for (uint32 i = 0; i < mSensors.Size(); ++i)
		{
			auto* sensor = mSensors[i];
			int channel_number = channels_numbers[i];
			double* channel_data = board_data[channel_number];;
			for (int j = 0; j < data_count; ++j)
			{
				double value = channel_data[j];
				sensor->AddQueuedSample(value);
			}
		}
		// update the neuro headset
		Device::Update(elapsed, delta);
	}
	catch (const BrainFlowException& err)
	{
		LogError(err.what());
		return;
	}
}

double BrainFlowDevice::GetSampleRate() const {
	try
	{
		return BoardShim::get_sampling_rate(getBoardId());
	}
	catch (const BrainFlowException& err)
	{
		LogError(err.what());
	}
};

#endif
