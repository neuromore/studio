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

#ifndef __NEUROMORE_FREQUENCYBAND_H
#define __NEUROMORE_FREQUENCYBAND_H

// include required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../Core/String.h"
#include "../Core/Color.h"


// forward declaration
class Spectrum;

// the frequency band class
/*
Delta (0.5-4Hz) - indicating deep sleep, restfulness, and conversely excitement or agitation when delta waves are suppressed 
Theta (4-8Hz) - indicating deep meditative states, daydreaming and automatic tasks 
Alpha (8-15Hz) - indicating relaxed alertness, restful and meditative states 
Beta (15-30Hz) - indicating wakefulness, alertness, mental engagement and conscious processing of information. 
*/
class ENGINE_API FrequencyBand
{
	public:
		// constructor & destructor
		FrequencyBand();
		FrequencyBand(double minFrequency, double maxFrequency, const Core::Color& color, const char* name, const char* description);
		virtual ~FrequencyBand();

		// copy properties from other frequency band
		void SetFromFrequencyBand(FrequencyBand* band);

		// calculate the value of this band (average over all bins contained within the band)
		double CalcMagnitude(const Spectrum* spectrum);

		// calculate the average power within the band
		double CalcPower(const Spectrum* spectrum);

		// calculate the average phase within the band
		double CalcPhase(const Spectrum* spectrum);

		// name description and color
		inline const char* GetName() const					{ return mName.AsChar(); }
		inline void SetName(const char*  name) 				{ mName = name; }
		inline const char* GetDescription() const			{ return mDescription.AsChar(); }
		inline void SetDescription(const char* descr) 		{ mDescription = descr; }
		inline const Core::Color& GetColor() const			{ return mColor; }
		inline void SetColor(const Core::Color& color)		{ mColor = color; }

		// lower/upper frequency of the bands range
		inline double GetMinFrequency() const				{ return mMinFrequency; }
		inline void SetMinFrequency(double freq)			{ mMinFrequency = freq; }
		inline double GetMaxFrequency() const				{ return mMaxFrequency; }
		inline void SetMaxFrequency(double freq)			{ mMaxFrequency = freq; }

		// TODO implement some windowing capabilities here

	private:
		Core::String		mName;
		Core::String		mDescription;
		Core::Color			mColor;
		double 				mMinFrequency;
		double 				mMaxFrequency;
};


#endif
