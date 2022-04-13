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
#include "LinearFilterProcessor.h"
#include "Channel.h"
#include "../Core/LogManager.h"


using namespace Core;

// constructor
LinearFilterProcessor::LinearFilterProcessor() : ChannelProcessor()
{
	Init();
	mFilter = NULL;
}


// destructor
LinearFilterProcessor::~LinearFilterProcessor()
{
	delete mFilter;
}


// init Filter processor 
void LinearFilterProcessor::Init()
{
	// one input
	AddInput<double>();
	
	// one output
	AddOutput<double>();
}


void LinearFilterProcessor::ReInit()
{
	// ReInit baseclass
	ChannelProcessor::ReInit();

	mIsInitialized		= false;
	ChannelBase* input	= GetInput();
	ChannelBase* output	= GetOutput();

	// nothing to do until both channels are connected
	if (input == NULL || output == NULL)
		return;
	
	// output sample rate is same as input
	mSettings.mSampleRate = input->GetSampleRate();
	output->SetSampleRate(mSettings.mSampleRate);

	// create new filter
	delete mFilter;
	mFilter = mFilterGenerator.CreateFilter(&mSettings);

	mIsInitialized = true;
}


void LinearFilterProcessor::Update()
{
	if (mIsInitialized == false || mFilter == NULL)
		return;

	// update base
	ChannelProcessor::Update();
	
	ChannelReader*		input			= GetInputReader(0);
	Channel<double>*	output			= GetOutput()->AsType<double>();
	const uint32		numNewSamples	= input->GetNumNewSamples();

	// TODO: implement other filters and remove this
	if (mSettings.mFilterMethod != Filter::BUTTERWORTH)
		return;

	if (numNewSamples == 0)
		return;

	double result = 0;

	// for each new sample: shift in, evaluate filter, output sample to outputchannel
	for (uint32 i=0; i<numNewSamples; ++i)
	{
		// get sample, add it to channel buffer
		const double& sample = input->PopOldestSample<double>();
		
		// apply filter
		result = mFilter->Evaluate(sample);

		// ignore invalid samples
		if (Math::IsValidNumberD(result) == false || Math::AbsD(result) > 10e12)		// NOTE arbitrary max value of 10e12!
			result = 0;

		// add output samples
		output->AddSample(result);
	}
}
