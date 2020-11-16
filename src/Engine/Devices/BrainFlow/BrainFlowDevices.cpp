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

#include "brainflow/board_shim.h"

#ifdef INCLUDE_DEVICE_BRAINFLOW

using namespace Core;

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


#endif
