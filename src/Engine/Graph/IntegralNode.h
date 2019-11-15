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

#ifndef __NEUROMORE_INTEGRALNODE_H
#define __NEUROMORE_INTEGRALNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"
#include "../DSP/ChannelProcessor.h"


class ENGINE_API IntegralNode : public ProcessorNode
{
	public:
		enum { TYPE_ID = 0x0053 };
		static const char* Uuid () { return "5968d8e8-df06-11e5-b86d-9a79f06e9478"; }
		
		//
		enum
		{
			INPUTPORT_VALUE		= 0,
			OUTPUTPORT_RESULT	= 0
		};

		// constructor & destructor
		IntegralNode(Graph* graph);
		~IntegralNode();

		// initialize & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;
		
		Core::Color GetColor() const override								{ return Core::RGBA(242, 255, 64); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Integral"; }
		const char* GetRuleName() const override final							{ return "NODE_Integral"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_MATH; }
		GraphObject* Clone(Graph* graph) override								{ IntegralNode* clone = new IntegralNode(graph); return clone; }

		const ChannelProcessor::Settings& GetSettings() override				{ return mSettings; }

	private:

		// settings for the node processor
		class ProcessorSettings : public ChannelProcessor::Settings
		{
			public:
				enum { TYPE_ID = 0x0053 };

				ProcessorSettings()			 {}
				virtual ~ProcessorSettings() {}
			
				uint32 GetType() const override		{ return TYPE_ID; }

				// Note: no settings for now, we'll add things like integral order later (requires ProcessorChain class)
		};
		
		ProcessorSettings	mSettings;

		// the node's channel processor
		class Processor : public ChannelProcessor
		{
			enum { TYPE_ID = 0x0053 };

			public:
				Processor() : ChannelProcessor()												{ Init(); }
				~Processor() { }

				uint32 GetType() const override													{ return TYPE_ID; }
				ChannelProcessor* Clone() override												{ Processor* clone = new Processor(); clone->Setup(mSettings); return clone; }

				// settings
				void Setup(const ChannelProcessor::Settings& settings) override					{ mSettings = static_cast<const ProcessorSettings&>(settings); }
				const Settings& GetSettings() const	override									{ return mSettings; }

				void Init() override
				{
					AddInput<double>();
					AddOutput<double>();
				}

				void ReInit() override;
				void Update() override;

			private:
				ProcessorSettings		mSettings;
				double					mIntegral;					// the integral value

		};
};


#endif
