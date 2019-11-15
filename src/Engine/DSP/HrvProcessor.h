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

#ifndef __NEUROMORE_HRVPROCESSOR_H
#define __NEUROMORE_HRVPROCESSOR_H

// include the required headers
#include "../Config.h"
#include "ChannelProcessor.h"
#include "HrvTimeDomain.h"


// heart rate variability processor
class ENGINE_API HrvProcessor : public ChannelProcessor
{
	public:
		// settings
		class Settings : public ChannelProcessor::Settings
		{
		public:
			enum { TYPE_ID = 0x0039 };

			Settings()									{}
			virtual ~Settings()							{}

			uint32 GetType() const override				{ return TYPE_ID; }

			uint32					mNumRRIntervals;		// number of input samples (RR intervals) used in the calculation

			double					mOutputSampleRate;		// output sample rate
			Core::Time				mStartTime;				// output start time	
			HrvTimeDomain::EMethod 	mTimeDomainMethod;		// which method to use for time domain analysis output
		};

		////////////////////////////////////////////

		enum { TYPE_ID = 0x0039 };

		// constructors & destructor
		HrvProcessor();
		virtual ~HrvProcessor()													{}

		uint32 GetType() const override											{ return TYPE_ID; }
		ChannelProcessor* Clone() override										{ HrvProcessor* clone = new HrvProcessor(); clone->Setup(mSettings); return clone; }

		void Init() override;
		void ReInit() override;
		void Update() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void Setup(const ChannelProcessor::Settings& settings) override			{ mSettings = static_cast<const Settings&>(settings); }
		const Settings& GetSettings() const	override							{ return mSettings; }

		// configure processor
		void SetTimeDomainMethod(HrvTimeDomain::EMethod method)					{ mSettings.mTimeDomainMethod = method; }
		void SetNumRRIntervals(uint32 numRRs)									{ mSettings.mNumRRIntervals = numRRs; }

		// DSP related properties
		//uint32 GetDelay(uint32 inputPortIndex, uint32 outputPortIndex) const override;		// is zero for all methods 
		//double GetLatency(uint32 inputPortIndex, uint32 outputPortIndex) const override;		// this is a hard one, we better not try to calculate it right now, doesn't really matter anyways
		//double GetSampleRatio(uint32 inputPortIndex, uint32 outputPortIndex) const override;	// this is not applicable here! // FIXME sample rate ratio alone is not sufficient in case a node has undefinied input samplerate but fixed output.. BTW: resamplers have the same problem!
		uint32 GetNumEpochSamples(uint32 inputPortIndex) const override						{ return mSettings.mNumRRIntervals + 1; }

	private:
		HrvTimeDomain::Function	mTimeDomainFunction;
		Settings				mSettings;
};


#endif
