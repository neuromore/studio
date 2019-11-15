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

// include required headers
#include "HrvProcessor.h"
#include "HrvTimeDomain.h"


using namespace Core;

// constructor
HrvProcessor::HrvProcessor() : ChannelProcessor()
{
	Init();

	mTimeDomainFunction			= NULL;
	mSettings.mTimeDomainMethod	= HrvTimeDomain::METHOD_RMSSD;
	mSettings.mNumRRIntervals	= 10;
	mSettings.mStartTime		= 0;
}


void HrvProcessor::Init()
{
	// one double channel input/output
	AddInput<double>();
	AddOutput<double>();
}


void HrvProcessor::ReInit()
{
	// ReInit baseclass
	ChannelProcessor::ReInit();

	mIsInitialized = false;

	ChannelBase* input = GetInput();
	ChannelBase* output = GetOutput();

	// input (dis)connected?
	if (input == NULL)
	{
		mIsInitialized = false;
		return;
	}
	
	//const double inputSampleRate = input->GetSampleRate();

	// input must be event channel
	// NOTE: why?
	/*if (inputSampleRate != 0)
	{
		mIsInitialized = false;
		return;
	}*/
	
	// output sample rate
	output->SetSampleRate(0);

	// configure input reader epoch
	if (mSettings.mNumRRIntervals <= 2)
		mSettings.mNumRRIntervals = 2;
	GetInputReader()->SetEpochLength(mSettings.mNumRRIntervals);
	GetInputReader()->SetEpochShift(1);
	GetInputReader()->SetEpochZeroPadding(true);

	// update algo function pointer
	mTimeDomainFunction = HrvTimeDomain::GetFunction(mSettings.mTimeDomainMethod);

	mIsInitialized = true;
}


void HrvProcessor::Update()
{
	// update input readers
	ChannelProcessor::Update();

	// call resample method (if set and inputs are connected)
	if (GetInput() != NULL)
	{
		if (mTimeDomainFunction != NULL)
		{
			ChannelReader* inputReader = GetInputReader();
			Channel<double>* output = GetOutput()->AsType<double>();

			(mTimeDomainFunction)(inputReader, output);
		}
	}
}


void HrvProcessor::Update(const Time& elapsed, const Time& delta)
{
	// base update as last stept
	ChannelProcessor::Update(elapsed, delta);
}
