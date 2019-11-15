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

#ifndef __NEUROMORE_LOGICNODE_H
#define __NEUROMORE_LOGICNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"
#include "../DSP/ChannelProcessor.h"


class ENGINE_API LogicNode : public ProcessorNode
{
	public:
		enum { TYPE_ID = 0x0002 };
		static const char* Uuid () { return "73b7ecb2-bb6b-11e4-8dfc-aa07a5b093db"; }

		enum
		{
			INPUTPORT_X			= 0,
			INPUTPORT_Y			= 1,
			OUTPUTPORT_VALUE	= 0,
			OUTPUTPORT_BOOL		= 1
		};

		enum 
		{
			PORTID_INPUT_X		= 0,
			PORTID_INPUT_Y		= 1,
			PORTID_OUTPUT_VALUE = 0,
			PORTID_OUTPUT_BOOL	= 1
		};

		enum 
		{
			ATTRIB_FUNCTION			= 0,
			ATTRIB_STATICVALUE		= 1,
			ATTRIB_TRUEVALUE		= 2,
			ATTRIB_FALSEVALUE		= 3
		};

		// constructor & destructor
		LogicNode(Graph* graph);
		~LogicNode();

		// initialize & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;

		Core::Color GetColor() const override								{ return Core::RGBA(234,229,129); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Logic"; }
		const char* GetRuleName() const override final							{ return "NODE_Logic"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_MATH; }
		GraphObject* Clone(Graph* graph) override								{ LogicNode* clone = new LogicNode(graph); return clone; }
		
		const ChannelProcessor::Settings& GetSettings() override				{ return mSettings; }

	private:

		typedef bool (CORE_CDECL *LogicFunction)(bool x, bool y);

		// Logic functions
		static bool CORE_CDECL BoolLogicAND(bool x, bool y);
		static bool CORE_CDECL BoolLogicOR(bool x, bool y);
		static bool CORE_CDECL BoolLogicXOR(bool x, bool y);

		enum ELogicFunction
		{
			FUNCTION_AND		= 0,
			FUNCTION_OR			= 1,
			FUNCTION_XOR		= 2,
			NUM_FUNCTIONS
		};
		static const char* GetFunctionString(ELogicFunction function);

		
		class ProcessorSettings : public ChannelProcessor::Settings
		{
			public:
				enum { TYPE_ID = 0x0002 };

				ProcessorSettings()			 {}
				virtual ~ProcessorSettings() {}
			
				uint32 GetType() const override		{ return TYPE_ID; }

				// logic function and one default-value
				ELogicFunction  mFunction;
				LogicFunction	mCalculateFunc;

				uint32			mStaticValue;
				double			mTrueValue;
				double			mFalseValue;
		};
		ProcessorSettings	mSettings;


		// the node's channel processor
		class Processor : public ChannelProcessor
		{
			enum { TYPE_ID = 0x0002 };

			public:
				Processor() : ChannelProcessor()						{ Init(); }
				~Processor() { }

				uint32 GetType() const override							{ return TYPE_ID; }
				ChannelProcessor* Clone() override						{ Processor* clone = new Processor(); clone->Setup(mSettings); return clone; }

				// settings
				void Setup(const ChannelProcessor::Settings& settings) override	{ mSettings = static_cast<const ProcessorSettings&>(settings); }
				virtual const Settings& GetSettings() const override			{ return mSettings; }

				void Init() override
				{
					AddInput<double>();
					AddInput<double>();
					AddOutput<double>();
				}

				void ReInit() override;
				void Update() override;

			private:
				ProcessorSettings		mSettings;
		};

};


#endif
