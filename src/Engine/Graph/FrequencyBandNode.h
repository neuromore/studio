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

#ifndef __NEUROMORE_FREQUENCYBANDNODE_H
#define __NEUROMORE_FREQUENCYBANDNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"
#include "../DSP/ChannelProcessor.h"
#include "../DSP/FrequencyBand.h"


class ENGINE_API FrequencyBandNode : public ProcessorNode
{
	public:
		enum { TYPE_ID = 0x0017 };
		static const char* Uuid () { return "5f6d9a32-bb6a-11e4-8dfc-aa07a5b093db"; }

		enum
		{
			ATTRIB_VALUETYPE		= 0,
			ATTRIB_BAND				= 1,
			ATTRIB_MAXFREQ			= 2,
			ATTRIB_MINFREQ			= 3
		};
		
		enum
		{
			INPUTPORT_SPECTRUM		= 0,
			INPUTPORT_MAX_FREQUENCY,
			INPUTPORT_MIN_FREQUENCY,
			OUTPUTPORT_CHANNEL		= 0
		};

		enum 
		{
			PORTID_INPUT_SPECTRUM	= 0,
			PORTID_OUTPUT_CHANNEL   = 1
		};

		enum EValueType
		{
			VALUE_AMPLITUDE = 0,
			VALUE_POWER,
			VALUE_PHASE
		};

		// constructor & destructor
		FrequencyBandNode(Graph* graph);
		~FrequencyBandNode();

		// initialize
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delt) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;

		Core::Color GetColor() const override								{ return Core::RGBA(156,38,224); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Frequency Band"; }
		const char* GetRuleName() const override final							{ return "NODE_FrequencyBand"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_DSP; }
		GraphObject* Clone(Graph* graph) override								{ FrequencyBandNode* clone = new FrequencyBandNode(graph); return clone; }

		const ChannelProcessor::Settings& GetSettings() override				{ return mSettings; }
	
	private:
		uint32	mSelectedPreset;	// for detecting changes in the preset combobox

		void UpdateInputPorts();

		class FrequencyBandSettings : public ChannelProcessor::Settings
		{
			public:
				enum { TYPE_ID = 0x00017 };

				FrequencyBandSettings()			 {}
				virtual ~FrequencyBandSettings() {}
			
				uint32 GetType() const override		{ return TYPE_ID; }
				
				EValueType		mValueType;
				FrequencyBand	mBand;
		};

		FrequencyBandSettings mSettings;

		// simple spectrum processor for processing incoming spectra
		class Processor : public ChannelProcessor
		{
			enum { TYPE_ID = 0x00017 };

			public:
				Processor() : ChannelProcessor()								{ Init(); }
				~Processor() { }

				uint32 GetType() const override									{ return TYPE_ID; }
				ChannelProcessor* Clone() override								{ Processor* clone = new Processor(); clone->Setup(mSettings); return clone; }

				const FrequencyBand& GetFrequencyBand()							{ return mSettings.mBand; }

				// settings
				void Setup(const ChannelProcessor::Settings& settings) override	{ mSettings = static_cast<const FrequencyBandSettings&>(settings); }
				virtual const Settings& GetSettings() const override			{ return mSettings; }
	
				void Init() override
				{
					AddInput<Spectrum>();
					AddOutput<double>();
				}

				void ReInit() override;
				void Update() override;
				

			private:
				FrequencyBandSettings		mSettings;
		};
};


#endif
