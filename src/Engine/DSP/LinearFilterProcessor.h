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

#ifndef __NEUROMORE_LINEARFILTERPROCESSOR_H
#define __NEUROMORE_LINEARFILTERPROCESSOR_H

// include required headers
#include "../Config.h"
#include "ChannelProcessor.h"
#include "Filter.h"
#include "FilterGenerator.h"


// linear filter (IIR/FIR) for a single channel
class ENGINE_API LinearFilterProcessor : public ChannelProcessor
{
	public:
		enum { TYPE_ID = 0x1021 };

		class LinearFilterSettings : public Filter::FilterSettings, public ChannelProcessor::Settings
		{
			public:
				LinearFilterSettings() : Filter::FilterSettings()		{}
				~LinearFilterSettings()									{}

				uint32 GetType() const override							{ return LinearFilterProcessor::TYPE_ID; }
		};

		// constructors & destructor
		LinearFilterProcessor();
		virtual ~LinearFilterProcessor();

		uint32 GetType() const override									{ return TYPE_ID; }
		ChannelProcessor* Clone() override								{ LinearFilterProcessor* clone = new LinearFilterProcessor(); clone->Setup(mSettings); return clone; }

		void Init() override;
		void ReInit() override;
		void Update() override;

		// settings
		void Setup(const ChannelProcessor::Settings& settings) override	{ mSettings = static_cast<const LinearFilterSettings&>(settings); }
		virtual const Settings& GetSettings() const override			{ return mSettings; }
	
		// processor properties
		uint32 GetDelay(uint32 inputPortIndex, uint32 outputPortIndex) const override			{ return 0.0; }
		double GetLatency(uint32 inputPortIndex, uint32 outputPortIndex) const override			{ return (2 * mSettings.mFilterOrder) / mSettings.mSampleRate; /* coarse assumption : double filter length*/; }
		
		double GetSampleRatio(uint32 inputPortIndex, uint32 outputPortIndex) const override		{ return 1.0; }
		uint32 GetNumStartupSamples(uint32 inputPortIndex) const override						{ return 1.0; }
		uint32 GetNumEpochSamples(uint32 inputPortIndex) const override							{ return 1.0; }

	private:
		Filter*					mFilter;			// the active filter
		LinearFilterSettings	mSettings;			// filter specification
		FilterGenerator			mFilterGenerator;	// for creating filters
		
};


#endif
