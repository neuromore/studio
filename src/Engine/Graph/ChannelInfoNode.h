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

#ifndef __NEUROMORE_CHANNELINFONODE_H
#define __NEUROMORE_CHANNELINFONODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"
#include "../DSP/ChannelProcessor.h"


class ENGINE_API ChannelInfoNode : public ProcessorNode
{
	public:
		enum { TYPE_ID = 0x0044 };
		static const char* Uuid () { return "4eae880a-96de-11e5-8994-feff819cdc9f"; }
		
		//
		enum
		{
			INPUTPORT_DOUBLE   = 0,
			INPUTPORT_SPECTRUM = 1,
			OUTPUTPORT_RESULT  = 0
		};

		enum
		{
			ATTRIB_INFOTYPE = 0,
		};

		// constructor & destructor
		ChannelInfoNode(Graph* graph);
		~ChannelInfoNode();

		// initialize & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;
		
		Core::Color GetColor() const override									{ return Core::RGBA(0, 159, 227); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Channel Info"; }
		const char* GetRuleName() const override final							{ return "NODE_ChannelInfo"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_UTILS; }
		GraphObject* Clone(Graph* graph) override								{ ChannelInfoNode* clone = new ChannelInfoNode(graph); return clone; }

		const ChannelProcessor::Settings& GetSettings() override				{ return mSettings; }

	private:
		// available math functions
		enum EInfoType
		{
			INFOTYPE_SAMPLETIME,
			INFOTYPE_SAMPLEINDEX,
			INFOTYPE_SAMPLERATE,
			INFOTYPE_NUMSAMPLES,
			INFOTYPE_NUMNEWSAMPLES,
			INFOTYPE_BUFFERSIZE,
			INFOTYPE_ELAPSEDTIME,
			INFOTYPE_LASTSAMPLETIME,
			INFOTYPE_STARTTIME,
			INFOTYPE_ISINDEPENDENT, 
			NUM_INFOTYPES
		};
		static const char* GetInfoTypeString(EInfoType type);
	
		// settings fot the node processor
		class ProcessorSettings : public ChannelProcessor::Settings
		{
			public:
				enum { TYPE_ID = 0x0044 };

				ProcessorSettings()			 {}
				virtual ~ProcessorSettings() {}
			
				uint32 GetType() const override		{ return TYPE_ID; }

				// math function and one default-value
				EInfoType	mInfoType;
		};
		
		ProcessorSettings	mSettings;

		// the node's channel processor
		class Processor : public ChannelProcessor
		{
			enum { TYPE_ID = 0x0044 };

			public:
				Processor() : ChannelProcessor()                                { Init(); }
				~Processor() { }

				uint32 GetType() const override                                 { return TYPE_ID; }
				ChannelProcessor* Clone() override                              { Processor* clone = new Processor(); clone->Setup(mSettings); return clone; }

				// settings
				void Setup(const ChannelProcessor::Settings& settings) override	{ mSettings = static_cast<const ProcessorSettings&>(settings); }
				virtual const Settings& GetSettings() const override			{ return mSettings; }

				void Init() override
				{
					AddInput<double>();
					AddOutput<double>();
				}

				void ReInit() override;
				void Update() override;

			private:
				ProcessorSettings		mSettings;

				double GetInfoValue(EInfoType type, uint64 sampleIndex);

		};
};


#endif
