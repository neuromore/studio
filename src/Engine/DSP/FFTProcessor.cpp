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
#include "FFTProcessor.h"
#include "../Core/LogManager.h"
#include "Channel.h"
#include "ChannelProcessor.h"


using namespace Core;

// constructor
FFTProcessor::FFTProcessor()
{
	Init();
	mIsInitialized = false;
}


// destructor
FFTProcessor::~FFTProcessor()
{
}


// init DFT processor 
void FFTProcessor::Init()
{
	// one double channel input
	AddInput<double>();
	
	// one spectrum channel output
	AddOutput<Spectrum>();
}


// reinit processor internals
void FFTProcessor::ReInit()
{
	// ReInit baseclass
	ChannelProcessor::ReInit();

	mIsInitialized = false;

	ChannelBase* input = GetInput();
	ChannelBase* output = GetOutput();

	// nothing to do until both channels are connected
	if (input == NULL || output == NULL)
		return;

	// clamp shift parameter to > 0 
	if (mSettings.mEpochShift == 0)
		mSettings.mEpochShift = 1;

	// clamp FFT order to > 0 
	if (mSettings.mFFTOrder == 0)
		mSettings.mFFTOrder = 1;

	// calculate number of FFT input samples
	mSettings.mNumFFTSamples = Math::Pow(2, mSettings.mFFTOrder);

	// configure input reader epoching
	ChannelReader*	inputReader = GetInputReader(0);
	inputReader->SetEpochLength(mSettings.mNumFFTSamples);
	inputReader->SetEpochZeroPadding(mSettings.mUseZeroPadding);
	inputReader->SetEpochShift(mSettings.mEpochShift);

	// TODO implement full epoch support like in statistics node
	/*if (mSettings.mEpochMode == StatisticsSettings::ON)
	inputReader->SetEpochShift(mSettings.mNumSamples);
	else if (mSettings.mEpochMode == StatisticsSettings::CUSTOM)
	inputReader->SetEpochShift(mSettings.mEpochShift);
	else
	inputReader->SetEpochShift(1);*/

	// init FFT
	mFFT.Init(mSettings.mNumFFTSamples);

	// calculate output sample rate
	double outputSampleRate = input->GetSampleRate() / (double)mSettings.mEpochShift;

	// set output sample rate
	output->SetSampleRate(outputSampleRate);

	mIsInitialized = true;
}


// main update function
void FFTProcessor::Update()
{
	if (mIsInitialized == false)
		return;

	// update input readers
	ChannelProcessor::Update();

	ChannelBase* inputChannel = GetInput();
	ChannelReader* inputReader = GetInputReader();

	// calculate number of bins
	const uint32 numBins = mSettings.mNumFFTSamples / 2 + 1;

	// FFT requires at least two bins
	if (numBins <= 1) 
		return;

	// number of new epochs we have to perform the FFT on
	uint32 numNewEpochs = inputReader->GetNumEpochs();
	
	Channel<double>* input = inputChannel->AsType<double>();
	Channel<Spectrum>* output = GetOutput()->AsType<Spectrum>();

	// calculate FFT of all epochs and output the values
	for (uint32 i=0; i<numNewEpochs; ++i)
	{
		// 1) get the input epoch
		Epoch inputEpoch = inputReader->PopOldestEpoch();
	
		// 2) copy values to FFT input
		double* inputBuffer = mFFT.GetInput();
		for (uint32 s=0; s < mSettings.mNumFFTSamples; ++s)
			inputBuffer[s] = inputEpoch.GetSample(s);

		// 3) calculate Discrete Fourier Transform
		mFFT.CalcFFT();

		// 4) get a free spectrum from the buffer
		Spectrum* spectrum = output->GetNextSampleRef();
		spectrum->SetMaxFrequency(inputChannel->GetSampleRate() / 2.0);
		spectrum->SetNumBins(numBins);

		// 5) calculate spectrum power from complex spectrum
		// 5.1) copy over 0Hz bin (DC part; scaled by 2 due to half symmetry of complex spectrum)
		const Complex* complexSpectrum = mFFT.GetOutput();
		spectrum->SetBin(0, complexSpectrum[0].mReal / numBins / 2.0);

		// 5.2) calculate real-valued power spectrum (L2-Norm of complex frequency values), scale by mNumBins/2, and double the value (due to spectrum symmetrie)
		const double scalingFactor = 1.0 / (numBins-1) / 2.0 * 2.0;		// for clarity (is optimized by compiler)
		for (uint32 b = 1; b < numBins; b++)
			spectrum->SetBin(b, complexSpectrum[b] * scalingFactor);
		
		// TODO deprecate spectrum time?!
		spectrum->SetTime(input->GetSampleTime(inputEpoch.GetPosition()).InSeconds());
	}
}




uint32 FFTProcessor::GetDelay(uint32 inputPortIndex, uint32 outputPortIndex) const
{
	// zero padding reduces the delay to zero
	if (mSettings.mUseZeroPadding == true)
		return 0;
	else
		return mSettings.mNumFFTSamples;

}
