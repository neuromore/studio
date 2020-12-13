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

namespace
{
	std::unique_ptr<BoardShim> connectToBoard(std::unique_ptr<BoardShim>&& board)
	{
		board->prepare_session();
		board->start_stream();
		return std::move(board);
	}
}

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
	mBoard = std::make_unique<BoardShim>(GetBoardId(), mParams);
	BoardShim::enable_dev_board_logger();
	BoardShim::set_log_file("brainflow.log");
	mFuture = std::async(&connectToBoard, std::move(mBoard));
	return Device::Connect();
}

bool BrainFlowDevice::Disconnect()
{
	if (mBoard && mBoard->is_prepared())
	{
		try
		{
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

int BrainFlowDevice::GetBoardId() const
{
	return static_cast<int>(mBoardId);
}

// get the available electrodes of the neuro headset
void BrainFlowDevice::CreateElectrodes()
{
	// clear existed sensors/electrodes
	mElectrodes.Clear();
	mDefaultElectrodes.Clear();
	mNeuroSensors.Clear();
	mInputSensors.Clear();
	mOutputSensors.Clear();
	for (unsigned i = 0; i < mSensors.Size(); ++i)
		delete mSensors[i];
	mSensors.Clear();

	int len = 0;
	std::string* eegNames;
	
	if (mBoard && mBoard->is_prepared())
	{
		try
		{
			eegNames = BoardShim::get_eeg_names(GetBoardId(), &len);
		}
		catch (const BrainFlowException& err)
		{
			LogError(err.what());
			BoardShim::get_eeg_channels(GetBoardId(), &len);
			eegNames = new std::string[len];
			for (int i = 0; i < len; ++i)
				eegNames[i] = "";
		}
	}
	else
	{
		// create dummy electrode
		len = 1;
		eegNames = new std::string[len];
		eegNames[len - 1] = "";
	}
	mElectrodes.Reserve(len);
	// todo check that BrainFlow IDs match studio IDs
	for (int i = 0; i < len; i++)
	{
		mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID(eegNames[i].c_str()));
	}
	delete[] eegNames;
}
	


void BrainFlowDevice::Update(const Core::Time& elapsed, const Core::Time& delta)
{
	if (!InitAfterConnected())
		return;
	try
	{
		int data_count;
		double** board_data;
		board_data = mBoard->get_board_data(&data_count);
		int channels_count;
		int* channels_numbers = BoardShim::get_eeg_channels(GetBoardId(), &channels_count);
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
	}
	catch (const BrainFlowException& err)
	{
		LogError(err.what());
		return;
	}
	// update the neuro headset
	Device::Update(elapsed, delta);
}

bool BrainFlowDevice::DoesConnectingFinished() const
{
	return mFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

bool BrainFlowDevice::InitAfterConnected()
{
	if (mBoard && mFuture.valid())
	{
		return DoesConnectingFinished();
	}
	else if (mBoard && !mFuture.valid())
	{
		// had already been connected and initialized
		return true;
	}
	else if (!mBoard && !mFuture.valid())
	{
		// no device and no trying to connect
		return false;
	}
	else if (!mBoard && mFuture.valid() && !DoesConnectingFinished())
	{
		// connecting has not been not finished yet
		return false;
	}

	// initialize device after connecting finished
	try
	{
		mBoard = std::move(mFuture.get());
		// device is connected successfully
		CreateSensors();
		GetEngine()->SetActiveBci(this);
		return true;
	}
	catch (const BrainFlowException& err)
	{
		// device is failed to connect
		Disconnect();
		GetDeviceManager()->RemoveDeviceAsync(this);
		mBoard.reset();
		LogError(err.what());
		return false;
	}
}

double BrainFlowDevice::GetSampleRate() const {
	try
	{
		return BoardShim::get_sampling_rate(GetBoardId());
	}
	catch (const BrainFlowException& err)
	{
		LogError(err.what());
		return 200;
	}
};

#endif
