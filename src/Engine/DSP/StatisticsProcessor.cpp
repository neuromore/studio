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

// include precompiled header
#include <Engine/Precompiled.h>

// include required files
#include "StatisticsProcessor.h"
#include "Channel.h"
#include "../Core/LogManager.h"


using namespace Core;

// constructor
StatisticsProcessor::StatisticsProcessor() : ChannelProcessor()
{
	Init();
}


// destructor
StatisticsProcessor::~StatisticsProcessor()
{

}


void StatisticsProcessor::Init()
{
	AddInput<double>();
	AddOutput<double>();
}


void StatisticsProcessor::ReInit()
{
	// ReInit baseclass
	ChannelProcessor::ReInit();

	mIsInitialized			= false;
	ChannelBase* input		= GetInput();
	ChannelBase* output		= GetOutput();

	// nothing to do until both channels are connected
	if (input == NULL || output == NULL)
		return;

	// calculate and set output sample rate
	const double inputSampleRate = input->GetSampleRate();

	// calculate interval length
	if (mSettings.mSetByTime == true && inputSampleRate > 0.0)
		mSettings.mNumSamples = (uint32)Math::CeilD(inputSampleRate * mSettings.mIntervalDuration);

	// do not start until statistic sample interval is > 0
	if (mSettings.mNumSamples == 0)
	{
		mIsInitialized = false;
		return;
	}

	// set output sample rate
	const double outputSampleRate = inputSampleRate / GetSampleRatio(0, 0);
	output->SetSampleRate(outputSampleRate);

	// configure reader epoching
	ChannelReader*	inputReader = GetInputReader(0);
	inputReader->SetEpochLength(mSettings.mNumSamples);

	// epoch mode
	if (mSettings.mEpochMode == StatisticsSettings::ON)
		inputReader->SetEpochShift(mSettings.mNumSamples);
	else if (mSettings.mEpochMode == StatisticsSettings::CUSTOM)
		inputReader->SetEpochShift(mSettings.mEpochShift);
	else
		inputReader->SetEpochShift(1);

	// zero padding
	inputReader->SetEpochZeroPadding(mSettings.mZeroPadding);

	mIsInitialized = true;
}


void StatisticsProcessor::Update()
{
	if (mIsInitialized == false)
		return;
	
	// update base
	ChannelProcessor::Update();
	
	ChannelReader*	 inputReader = GetInputReader();
	Channel<double>* output = GetOutput()->AsType<double>();

	// number of new epochs we can process
	const uint32 numNewEpochs = inputReader->GetNumEpochs();
	
	// calculate statistics for this epoch
	for (uint32 e=0; e<numNewEpochs; ++e)
	{
		// 1) get the input epoch
		Epoch inputEpoch = inputReader->PopOldestEpoch();
	
		// 2) calculate statistic over epoch
		double statisticValue = 0;
		switch (mSettings.mMethod)
		{

			case Mean:
				statisticValue = inputEpoch.Mean();
				break;

			case Sum:
				statisticValue = inputEpoch.Sum();
				break;

			case Product:
				statisticValue = inputEpoch.Product();
				break;

			case Minimum:
				statisticValue = inputEpoch.Min();
				break;

			case Maximum:
				statisticValue = inputEpoch.Max();
				break;

			case Range:
				statisticValue = inputEpoch.Range();
				break;

			case Variance:
				statisticValue = inputEpoch.Variance();
				break;

			case StandardDeviation:		
				statisticValue = inputEpoch.StdDev();		
				break;

			case RMS:
				statisticValue = inputEpoch.RMS();
				break;

			case Percentile:
				statisticValue = inputEpoch.Percentile(mSettings.mPercentile, mTempArray);
				break;

			case HarmonicMean:
				statisticValue = inputEpoch.HarmonicMean();
				break;

			case GeometricMean:
				statisticValue = inputEpoch.GeometricMean();
				break;

			case Median:
				statisticValue = inputEpoch.Median(mTempArray);
				break;

			// TODO add other quantiles here (needs additional attribute in statistics node

			// does not happen
			default: break;
		}

		output->AddSample(statisticValue);
	}
}


void StatisticsProcessor::Setup(const ChannelProcessor::Settings& settings)
{
	mSettings = static_cast<const StatisticsSettings&>(settings); 
}



uint32 StatisticsProcessor::GetDelay (uint32 inputPortIndex, uint32 outputPortIndex) const 
{
	if (mSettings.mEpochMode == StatisticsSettings::ON)
		return 0;
	else	
		return mSettings.mNumSamples;
}


double StatisticsProcessor::GetSampleRatio(uint32 inputPortIndex, uint32 outputPortIndex) const
{
	if (mSettings.mEpochMode == StatisticsSettings::ON)
		return mSettings.mNumSamples;
	else
		return 1.0;
}


uint32 StatisticsProcessor::GetNumStartupSamples (uint32 inputPortIndex) const 
{
	if (mSettings.mEpochMode == StatisticsSettings::ON)
		return mSettings.mNumSamples;
	else
		return 1.0;
}

const char* StatisticsProcessor::GetStatisticMethodName(EStatisticMethod method)
{
	switch (method)
	{
		case Minimum:			return "Minimum";
		case Maximum:			return "Maximum";
		case Range:				return "Range";
		case Mean:				return "Mean";
		case Median:			return "Median";
		case Variance:			return "Variance";
		case StandardDeviation: return "Standard Deviation";
		case RMS:				return "RMS";
		case Percentile:		return "Percentile";
		case Sum:				return "Sum of Elements";
		case Product:			return "Product of Elements";
		case HarmonicMean:		return "Harmonic Mean";
		case GeometricMean:		return "Geometric Mean";
		default:				return "Unknown";
	}
}


const char* StatisticsProcessor::GetStatisticMethodNameShort(EStatisticMethod method)
{
	switch (method)
	{
		case Minimum:			return "Min";
		case Maximum:			return "Max";
		case Range:				return "Range";
		case Mean:				return "Mean";
		case Median:			return "Median";
		case Variance:			return "Variance";
		case StandardDeviation: return "Std.Dev.";
		case Percentile:		return "P";
		case Sum:				return "Sum";
		case Product:			return "Product";
		case HarmonicMean:		return "Harm.";
		case GeometricMean:		return "Geom.";
		default:				return "Unknown";
	}
}
