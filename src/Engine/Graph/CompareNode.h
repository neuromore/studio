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

#ifndef __NEUROMORE_COMPARENODE_H
#define __NEUROMORE_COMPARENODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"
#include "../DSP/ChannelProcessor.h"


class ENGINE_API CompareNode : public ProcessorNode
{
	public:
		enum { TYPE_ID = 0x0005 };
		static const char* Uuid () { return "a7cd7d14-bb6b-11e4-8dfc-aa07a5b093db"; }

		enum
		{
			INPUTPORT_X			= 0,
			INPUTPORT_Y			= 1,
			OUTPUTPORT_VALUE	= 0,
		};

	
		enum 
		{
			ATTRIB_FUNCTION			= 0,
			ATTRIB_STATICVALUE		= 1,
			ATTRIB_TRUEMODE			= 2,
			ATTRIB_TRUEVALUE		= 3,
			ATTRIB_FALSEMODE		= 4,
			ATTRIB_FALSEVALUE		= 5
		};

		enum EReturnMode
		{
			MODE_VALUE	= 0,
			MODE_X		= 1,
			MODE_Y		= 2
		};

		// constructor & destructor
		CompareNode(Graph* graph);
		~CompareNode();

		// initialize & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;

		Core::Color GetColor() const override								{ return Core::RGBA(255,100,50); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Compare"; }
		const char* GetRuleName() const override final							{ return "NODE_Compare"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_MATH; }
		GraphObject* Clone(Graph* graph) override								{ CompareNode* clone = new CompareNode(graph); return clone; }
		
		const ChannelProcessor::Settings& GetSettings() override				{ return mSettings; }

	private:
		typedef bool (CORE_CDECL *CompareFunction)(double x, double y);

		// Compare Functions
		static bool CORE_CDECL CompareEqual(double x, double y);
		static bool CORE_CDECL CompareGreater(double x, double y);
		static bool CORE_CDECL CompareLess(double x, double y);
		static bool CORE_CDECL CompareGreaterOrEqual(double x, double y);
		static bool CORE_CDECL CompareLessOrEqual(double x, double y);
		static bool CORE_CDECL CompareInequal(double x, double y);

		enum ECompareFunction
		{
			FUNCTION_EQUAL			= 0,
			FUNCTION_GREATER		= 1,
			FUNCTION_LESS			= 2,
			FUNCTION_GREATEROREQUAL	= 3,
			FUNCTION_LESSOREQUAL	= 4,
			FUNCTION_INEQUAL		= 5,
			NUM_FUNCTIONS
		};
		static const char* GetFunctionString(ECompareFunction function);

		// settings fot the node processor
		class ProcessorSettings : public ChannelProcessor::Settings
		{
			public:
				enum { TYPE_ID = 0x0005 };

				ProcessorSettings()			 {}
				virtual ~ProcessorSettings() {}
			
				uint32 GetType() const override		{ return TYPE_ID; }

				// math function and one default-value
				ECompareFunction	mFunction;
				CompareFunction		mCalculateFunc;

				double				mStaticValue;
				double				mTrueValue;
				double				mFalseValue;
				EReturnMode			mTrueReturnMode;
				EReturnMode			mFalseReturnMode;
		};
		
		ProcessorSettings	mSettings;

		// the node's channel processor
		class Processor : public ChannelProcessor
		{
			enum { TYPE_ID = 0x0005 };

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
