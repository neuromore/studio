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

#ifndef __NEUROMORE_DOMINANTFREQUENCYNODE_H
#define __NEUROMORE_DOMINANTFREQUENCYNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"
#include "../DSP/ChannelProcessor.h"
#include "../DSP/FrequencyBand.h"

// NOTE: this node is in large parts just a copy of the frequency band processor

class ENGINE_API DominantFrequencyNode : public ProcessorNode
{
	public:
		enum { TYPE_ID = 0x00029 };
		static const char* Uuid () { return "1c9c0568-bb6a-11e4-8dfc-aa07a5b093db"; }

		enum
		{
			ATTRIB_PRESET			= 0,
			ATTRIB_MINFREQ			= 1,
			ATTRIB_MAXFREQ			= 2
		};
		
		enum
		{
			INPUTPORT_SPECTRUM		= 0,
			OUTPUTPORT_CHANNEL		= 0
		};

		enum 
		{
			PORTID_INPUT_SPECTRUM	= 0,
			PORTID_OUTPUT_CHANNEL   = 1
		};

		// constructor & destructor
		DominantFrequencyNode(Graph* graph);
		~DominantFrequencyNode();

		// initialize & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;

		Core::Color GetColor() const override								{ return Core::RGBA(157,236,0); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Dominant Frequency"; }
		const char* GetRuleName() const override final							{ return "NODE_DominantFrequency"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_DSP; }
		GraphObject* Clone(Graph* graph) override								{ DominantFrequencyNode* clone = new DominantFrequencyNode(graph); return clone; }

		const ChannelProcessor::Settings& GetSettings() override				{ return mSettings; }

	private:
		// for detecting changes in the preset combobox
		uint32				mSelectedPreset;


		//
		// The node's processor implementation
		//
		
		class ProcessorSettings : public ChannelProcessor::Settings
		{
			public:
				enum { TYPE_ID = 0x00025 };

				ProcessorSettings()			 {}
				virtual ~ProcessorSettings() {}
			
				uint32 GetType() const override		{ return TYPE_ID; }

				FrequencyBand	mBand;
		};
		
		ProcessorSettings mSettings;


		// calculate dominant frequency of input specturm within a selected frequency band
		class Processor : public ChannelProcessor
		{
			enum { TYPE_ID = 0x00025 };

			public:
				// constructor & destructor
				Processor() : ChannelProcessor()						{ Init(); }
				~Processor()											{}

				uint32 GetType() const override							{ return TYPE_ID; }
				ChannelProcessor* Clone() override						{ Processor* clone = new Processor(); clone->Setup(mSettings); return clone; }

				const FrequencyBand& GetFrequencyBand()					{ return mSettings.mBand; }
				
				// settings
				void Setup(const ChannelProcessor::Settings& settings) override	{ mSettings = static_cast<const ProcessorSettings&>(settings); }
				virtual const Settings& GetSettings() const override            { return mSettings; }
	
				void Init() override
				{
					AddInput<Spectrum>();
					AddOutput<double>();
				}

				void ReInit() override;
				void Update() override;

			private:
				ProcessorSettings		mSettings;
		};

};


#endif
