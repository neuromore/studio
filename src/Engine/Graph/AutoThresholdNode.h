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

#ifndef __NEUROMORE_AUTOTHRESHOLDNODE_H
#define __NEUROMORE_AUTOTHRESHOLDNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"
#include "../DSP/ChannelProcessor.h"
#include "../DSP/Histogram.h"


class ENGINE_API AutoThresholdNode : public ProcessorNode
{
	public:
		enum { TYPE_ID = 0x0057 };
		static const char* Uuid () { return "93bd87e2-624a-11e6-8b77-86f30ca893d3"; }
		
		enum
		{
			INPUTPORT_SIGNAL = 0,
			INPUTPORT_TARGET,
			INPUTPORT_HIGH,
			INPUTPORT_LOW,
			OUTPUTPORT_HIGH	= 0,
			OUTPUTPORT_LOW
		};

		enum
		{
			ATTRIB_NUMBINS= 0,
			ATTRIB_NUMSAMPLES,
			ATTRIB_TARGETMODE,
			ATTRIB_INVERT_TARGET,
			ATTRIB_THRESHOLDINPUTMODE,
		};

		enum ETargetMode			{ TARGETMODE_SCORE, TARGETMODE_TIME	};
		enum EThresholdInputMode	{ THRESHOLDINPUTMODE_ABSOLUTE, THRESHOLDINPUTMODE_RELATIVE };
		enum EOutputMode			{ OUTPUTMODE_HIGH, OUTPUTMODE_LOW };

		enum EError
		{
			ERROR_NO_THRESHOLD_INPUT		= GraphObjectError::ERROR_CONFIGURATION | 0x01,
			ERROR_SINGLE_THRESHOLD_INPUT	= GraphObjectError::ERROR_CONFIGURATION | 0x02,
			ERROR_CONTROL_CHANNEL_MISSMATCH = GraphObjectError::ERROR_CONFIGURATION | 0x03,
			ERROR_CONTROL_PORT_SAMPLERATE	= GraphObjectError::ERROR_CONFIGURATION | 0x04
		};

		// constructor & destructor
		AutoThresholdNode(Graph* graph);
		~AutoThresholdNode();

		// initialize & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;
		
		Core::Color GetColor() const override									{ return Core::RGBA(227, 165, 77); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Auto Threshold"; }
		const char* GetRuleName() const override final							{ return "NODE_AutoThreshold"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_BIO; }
		GraphObject* Clone(Graph* graph) override								{ AutoThresholdNode* clone = new AutoThresholdNode(graph); return clone; }

		const ChannelProcessor::Settings& GetSettings() override				{ return mSettings; }

	private:

		// settings for the node processor
		class ProcessorSettings : public ChannelProcessor::Settings
		{
			public:
				enum { TYPE_ID = 0x0057 };

				ProcessorSettings()			 {}
				virtual ~ProcessorSettings() {}
			
				uint32 GetType() const override		{ return TYPE_ID; }

				ETargetMode				mTargetMode;
				bool					mInvertTarget;
				EThresholdInputMode		mThresholdInputMode;
				uint32					mNumBins;					// total number of histogram bins
				uint32					mNumSamples;				// size of the interval we base the thresholding calc on
				uint32					mRequiredEpochLength;		// number of samples that must be present in the signal input buffer (interval length + safety buffer)
		};
		
		ProcessorSettings	mSettings;

		// the node's channel processor
		class Processor : public ChannelProcessor
		{
			enum { TYPE_ID = 0x0057 };

			public:
				Processor() : ChannelProcessor()												{ Init(); }
				~Processor() { }

				uint32 GetType() const override													{ return TYPE_ID; }
				ChannelProcessor* Clone() override												{ Processor* clone = new Processor(); clone->Setup(mSettings); return clone; }

				// settings
				void Setup(const ChannelProcessor::Settings& settings) override					{ mSettings = static_cast<const ProcessorSettings&>(settings); }
				const Settings& GetSettings() const	override									{ return mSettings; }
		
				uint32 GetNumEpochSamples(uint32 inputPortIndex) const override final			{ if (inputPortIndex == 0) return mSettings.mRequiredEpochLength; else return 0; }

				void Init() override
				{
					AddInput<double>();
					AddInput<double>();
					AddInput<double>();
					AddInput<double>();

					AddOutput<double>();
					AddOutput<double>();
				}

				void ReInit() override;
				void Update() override;

			private:
				ProcessorSettings		mSettings;

				Histogram				mHistogram;

				double CalcHighInputThreshold (double inputValue);
				double CalcLowInputThreshold (double inputValue);
				double CalcHighAutoThreshold (double lowThreshold, double target);
				double CalcLowAutoThreshold (double highThreshold, double target);
		};
};


#endif
