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
#include "FrequencyBand.h"
#include "Spectrum.h"


using namespace Core;

// constructor
FrequencyBand::FrequencyBand()
{
	mName			= "";
	mDescription	= "";
	mMinFrequency	= 0;
	mMaxFrequency	= 0;
}


// constructor
FrequencyBand::FrequencyBand(double minFrequency, double maxFrequency, const Core::Color& color, const char* name, const char* description)
{
	mName			= name;
	mDescription	= description;
	mColor			= color;
	mMinFrequency	= minFrequency;
	mMaxFrequency	= maxFrequency;
}


// destructor
FrequencyBand::~FrequencyBand()
{
}


// copy over values from other frequency band
void FrequencyBand::SetFromFrequencyBand(FrequencyBand* band)
{
	mName = band->GetName();
	mDescription = band->GetDescription();
	mColor = band->GetColor();
	mMinFrequency = band->GetMinFrequency();
	mMaxFrequency = band->GetMaxFrequency();
}


// calculate average band magnitude
double FrequencyBand::CalcMagnitude(const Spectrum* spectrum)
{
	// reset the magnitude and the counter
	double magnitude	= 0.0;
	uint32 counter		= 0;

	const uint32 numValues = spectrum->GetNumBins();
	for (uint32 i=0; i<numValues; ++i)
	{
		const double value = spectrum->GetBin(i);
		const double frequency = spectrum->CalcFrequency(i);

		// skip if the frequency of this band is not within range
		if (frequency > mMaxFrequency || frequency < mMinFrequency)
			continue;

		magnitude += value;
		counter++;
	}

	if (counter == 0)
		return 0;

	return magnitude / (double)counter;
}


// calculate average power
double FrequencyBand::CalcPower(const Spectrum* spectrum)
{
	// reset the magnitude and the counter
	double powerSum		= 0.0;
	uint32 counter		= 0;

	const uint32 numValues = spectrum->GetNumBins();
	for (uint32 i=0; i<numValues; ++i)
	{
		const double power = spectrum->GetComplexBin(i).SquaredNorm();
		const double frequency = spectrum->CalcFrequency(i);

		// skip if the frequency of this band is not within range
		if (frequency > mMaxFrequency || frequency < mMinFrequency)
			continue;

		powerSum += power;
		counter++;
	}

	if (counter == 0)
		return 0.0;

	return powerSum / (double)counter;
}


// calculate average phase
double FrequencyBand::CalcPhase(const Spectrum* spectrum)
{
	// reset the magnitude and the counter
	double phaseSum		= 0.0;
	uint32 counter		= 0;

	const uint32 numValues = spectrum->GetNumBins();
	for (uint32 i=0; i<numValues; ++i)
	{
		const double phase = spectrum->GetComplexBin(i).Arg();
		const double frequency = spectrum->CalcFrequency(i);

		// skip if the frequency of this band is not within range
		if (frequency > mMaxFrequency || frequency < mMinFrequency)
			continue;

		phaseSum += phase;
		counter++;
	}

	if (counter == 0)
		return 0.0;

	return phaseSum / (double)counter;
}
