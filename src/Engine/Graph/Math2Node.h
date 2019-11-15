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

#ifndef __NEUROMORE_MATH2NODE_H
#define __NEUROMORE_MATH2NODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"
#include "../DSP/ChannelProcessor.h"


class ENGINE_API Math2Node : public ProcessorNode
{
	public:
		enum { TYPE_ID = 0x0024 };
		static const char* Uuid () { return "db5e2da4-bb6b-11e4-8dfc-aa07a5b093db"; }

		//
		enum
		{
			INPUTPORT_X			= 0,
			INPUTPORT_Y			= 1,
			OUTPUTPORT_RESULT	= 0
		};

		enum
		{
			PORTID_INPUTPORT_X		 = 0,
			PORTID_INPUTPORT_Y		 = 1,
			PORTID_OUTPUTPORT_RESULT = 2,
		};

		enum
		{
			ATTRIB_MATHFUNCTION	= 0,
			ATTRIB_STATICVALUE	= 1
		};

		// constructor & destructor
		Math2Node(Graph* graph);
		~Math2Node();

		// initialize
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;

		Core::Color GetColor() const override								{ return Core::RGBA(91,255,250); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Operation"; }
		const char* GetRuleName() const override final							{ return "NODE_Math2"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_MATH; }
		GraphObject* Clone(Graph* graph) override								{ Math2Node* clone = new Math2Node(graph); return clone; }

		const ChannelProcessor::Settings& GetSettings() override				{ return mSettings; }

	private:

		// Math functions 

		static double CORE_CDECL CalculateAdd(double x, double y);
		static double CORE_CDECL CalculateSubtract(double x, double y);
		static double CORE_CDECL CalculateMultiply(double x, double y);
		static double CORE_CDECL CalculateDivide(double x, double y);
		static double CORE_CDECL CalculateAverage(double x, double y);
		static double CORE_CDECL CalculateRandomFloat(double x, double y);
		static double CORE_CDECL CalculateMod(double x, double y);
		static double CORE_CDECL CalculateMin(double x, double y);
		static double CORE_CDECL CalculateMax(double x, double y);
		static double CORE_CDECL CalculatePow(double x, double y);

		// available math functions
		enum EMathFunction
		{
			MATHFUNCTION_ADD			= 0,
			MATHFUNCTION_SUBTRACT		= 1,
			MATHFUNCTION_MULTIPLY		= 2,
			MATHFUNCTION_DIVIDE			= 3,
			MATHFUNCTION_AVERAGE		= 4,
			MATHFUNCTION_RANDOMFLOAT	= 5,
			MATHFUNCTION_MOD			= 6,
			MATHFUNCTION_MIN			= 7,
			MATHFUNCTION_MAX			= 8,
			MATHFUNCTION_POW			= 9,
			MATHFUNCTION_NUMFUNCTIONS
		};
		static const char* GetFunctionString(EMathFunction function);
	
		typedef double (CORE_CDECL *Math2Function)(double x, double y);

		class ProcessorSettings : public ChannelProcessor::Settings
		{
			public:
				enum { TYPE_ID = 0x0024 };

				ProcessorSettings()			 {}
				virtual ~ProcessorSettings() {}
			
				uint32 GetType() const override		{ return TYPE_ID; }

				// math function and one default-value
				EMathFunction   mMathFunction;
				Math2Function	mCalculateFunc;
				double			mDefaultValue;
		};

		ProcessorSettings	mSettings;

		class Processor : public ChannelProcessor
		{
			enum { TYPE_ID = 0x00024 };

			public:
				Processor() : ChannelProcessor()                                { Init();  }
				~Processor() { }

				uint32 GetType() const override                                 { return TYPE_ID; }
				ChannelProcessor* Clone() override                              { Processor* clone = new Processor(); clone->Setup(mSettings); return clone; }

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
