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

// include required headers
#include "HrvTimeDomain.h"


using namespace Core;

// the square root of the mean of the squares of the successive differences between adjacent RRs
void HrvTimeDomain::RMSSD(ChannelReader* inputReader, Channel<double>* output)
{
	double sumSSD = 0;

	// process all input epochs
	const uint32 numEpochs = inputReader->GetNumEpochs();
	for (uint32 i = 0; i < numEpochs; i++)
	{
		Epoch epoch = inputReader->PopOldestEpoch();
		CORE_ASSERT(epoch.GetLength() > 1);

		sumSSD = 0;

		// calculate result by iteration over successive pairs
		const uint32 numSamples = epoch.GetNumSamples();
		for (uint32 s = 0; s < numSamples - 1; s++)		// Note: epoch length is always > 1!
		{
			// 1) successive difference (sign doesnt matter)
			const double sd = epoch.GetSample(s) - epoch.GetSample(s + 1);

			// 2) sum squares
			sumSSD += sd * sd;
		}

		// 3) root of mean SSD
		const double rmssd = Math::SqrtD( sumSSD / (numSamples - 1) ); 

		output->AddSample(rmssd);
	}
}


// the standard deviation of the successive differences between adjacent RRs.
void HrvTimeDomain::SDSD(ChannelReader* inputReader, Channel<double>* output)
{
	// buffer for mean and variance sums
	double buff = 0;

	// process all input epochs
	const uint32 numEpochs = inputReader->GetNumEpochs();
	for (uint32 i = 0; i < numEpochs; i++)
	{
		Epoch epoch = inputReader->PopOldestEpoch();
		CORE_ASSERT(epoch.GetLength() > 1);

		const uint32 numSamples = epoch.GetNumSamples();

		// 1) calculate mean of successive differences
		buff = 0;
		for (uint32 s = 0; s < numSamples - 1; s++)		// Note: epoch length is always > 1!
		{
			const double sd = epoch.GetSample(s) - epoch.GetSample(s + 1);
			buff += sd;
		}
		const double mean = buff / (numSamples - 1);

		// 2) calculate variance
		buff = 0;
		for (uint32 s = 0; s < numSamples - 1; s++)		// Note: epoch length is always > 1!
		{
			const double sd = epoch.GetSample(s) - epoch.GetSample(s + 1);
			const double diff = mean - sd;
			buff += diff * diff;
		}
		const double variance = buff / (numSamples - 1);

		// 3) standard deviation
		const double sdsd = Math::Sqrt(variance);

		output->AddSample(sdsd);
	}
}


void HrvTimeDomain::EBC(ChannelReader* inputReader, Channel<double>* output)
{
	// process all input epochs
	const uint32 numEpochs = inputReader->GetNumEpochs();
	for (uint32 i = 0; i < numEpochs; i++)
	{
		// ebc = maximum value in epoch - minimum value in epoch
		Epoch epoch = inputReader->PopOldestEpoch();
		const double ebc = epoch.Max() - epoch.Min();
		output->AddSample(ebc);
	}
}


void HrvTimeDomain::RR50(ChannelReader* inputReader, Channel<double>* output)
{
	RRX(inputReader, output, 50.0);
}


void HrvTimeDomain::pRR50(ChannelReader* inputReader, Channel<double>* output)
{
	pRRX(inputReader, output, 50.0);
}


void HrvTimeDomain::pRR20(ChannelReader* inputReader, Channel<double>* output)
{
	pRRX(inputReader, output, 20.0);
}


// RXX
void HrvTimeDomain::RRX(ChannelReader* inputReader, Channel<double>* output, double millisecs)
{
	// for counting samples
	uint32 count = 0;
	const double seconds = millisecs / 1000;
	
	// process all input epochs
	const uint32 numEpochs = inputReader->GetNumEpochs();
	for (uint32 i = 0; i < numEpochs; i++)
	{
		Epoch epoch = inputReader->PopOldestEpoch();
		CORE_ASSERT(epoch.GetLength() > 1);

		count = 0;

		// count number of successive differences that are > X ms
		const uint32 numSamples = epoch.GetNumSamples();
		for (uint32 s = 0; s < numSamples - 1; s++)		// Note: epoch length is always > 1!
		{
			const double sd = epoch.GetSample(s) - epoch.GetSample(s + 1);
			if (sd > seconds)
				count++;
		}

		output->AddSample(count);
	}
}


// pRR50, pRR20, etc.. whatever :)
void HrvTimeDomain::pRRX(ChannelReader* inputReader, Channel<double>* output, double millisecs)
{
	// for counting samples
	uint32 count = 0;
	const double seconds = millisecs / 1000;

	// process all input epochs
	const uint32 numEpochs = inputReader->GetNumEpochs();
	for (uint32 i = 0; i < numEpochs; i++)
	{
		Epoch epoch = inputReader->PopOldestEpoch();
		CORE_ASSERT(epoch.GetLength() > 1);

		count = 0;

		// count number of successive differences that are > X ms
		const uint32 numSamples = epoch.GetNumSamples();
		for (uint32 s = 0; s < numSamples - 1; s++)		// Note: epoch length is always > 1!
		{
			const double sd = epoch.GetSample(s) - epoch.GetSample(s + 1);
			if (sd > seconds)
				count++;
		}

		const double pRR = (double)count / (double)(numSamples - 1);
		output->AddSample(pRR);
	}
}


const char* HrvTimeDomain::GetName(EMethod method)
{
	switch (method)
	{
		case METHOD_RMSSD:		return "RMSSD";
		case METHOD_SDSD:		return "SDSD";
		case METHOD_EBC:		return "EBC";
		case METHOD_RR50:		return "RR50";
		case METHOD_pRR50:		return "pRR50";
		case METHOD_pRR20:		return "pRR20";
		default:				return "Unknown";
	}
}


bool HrvTimeDomain::GetIntervalRequirement(EMethod method)
{
	switch (method)
	{
		// methods working on successive differences
		case METHOD_RMSSD:
		case METHOD_SDSD:
			return false;

		// method working on intervals
		case METHOD_EBC:
		case METHOD_RR50:
		case METHOD_pRR50:
		case METHOD_pRR20:
			return true;

		default:			return false;
	}
}


// get function pointer
HrvTimeDomain::Function HrvTimeDomain::GetFunction(EMethod method)
{
	switch (method)
	{
		case METHOD_RMSSD:		return (&RMSSD);
		case METHOD_SDSD:		return (&SDSD);
		case METHOD_EBC:		return (&EBC);
		case METHOD_RR50:		return (&RR50);
		case METHOD_pRR50:		return (&pRR50);
		case METHOD_pRR20:		return (&pRR20);
		default:		
			CORE_ASSERT(false); 
			return (&RMSSD);
	}
}
