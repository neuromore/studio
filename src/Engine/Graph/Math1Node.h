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

#ifndef __NEUROMORE_MATH1NODE_H
#define __NEUROMORE_MATH1NODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"
#include "../DSP/ChannelProcessor.h"


class ENGINE_API Math1Node : public ProcessorNode
{
	public:
		enum { TYPE_ID = 0x0006 };
		static const char* Uuid () { return "cdc17a98-bb6b-11e4-8dfc-aa07a5b093db"; }
		
		//
		enum
		{
			INPUTPORT			= 0,
			OUTPUTPORT_RESULT	= 0
		};

		enum
		{
			ATTRIB_MATHFUNCTION	= 0,
		};

		// constructor & destructor
		Math1Node(Graph* graph);
		~Math1Node();

		// initialize & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;
		
		Core::Color GetColor() const override								{ return Core::RGBA(125,25,53); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Function"; }
		const char* GetRuleName() const override final							{ return "NODE_Math1"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_MATH; }
		GraphObject* Clone(Graph* graph) override								{ Math1Node* clone = new Math1Node(graph); return clone; }

		const ChannelProcessor::Settings& GetSettings() override				{ return mSettings; }

	private:
		typedef double (CORE_CDECL *Math1Function)(double x);
		
		// Math functions 
		static double CORE_CDECL CalculateSin(double input);
		static double CORE_CDECL CalculateCos(double input);
		static double CORE_CDECL CalculateTan(double input);
		static double CORE_CDECL CalculateSqr(double input);
		static double CORE_CDECL CalculateSqrt(double input);
		static double CORE_CDECL CalculateAbs(double input);
		static double CORE_CDECL CalculateFloor(double input);
		static double CORE_CDECL CalculateCeil(double input);
		static double CORE_CDECL CalculateOneOverInput(double input);
		static double CORE_CDECL CalculateInvSqrt(double input);
		static double CORE_CDECL CalculateLog(double input);
		static double CORE_CDECL CalculateLog10(double input);
		static double CORE_CDECL CalculateExp(double input);
		static double CORE_CDECL CalculateFraction(double input);
		static double CORE_CDECL CalculateSign(double input);
		static double CORE_CDECL CalculateIsPositive(double input);
		static double CORE_CDECL CalculateIsNegative(double input);
		static double CORE_CDECL CalculateIsNearZero(double input);
		static double CORE_CDECL CalculateRandomFloat(double input);
		static double CORE_CDECL CalculateRadToDeg(double input);
		static double CORE_CDECL CalculateDegToRad(double input);
		static double CORE_CDECL CalculateSmoothStepCos(double input);
		static double CORE_CDECL CalculateSmoothStepPoly(double input);
		static double CORE_CDECL CalculateACos(double input);
		static double CORE_CDECL CalculateASin(double input);
		static double CORE_CDECL CalculateATan(double input);

		// available math functions
		enum EMathFunction
		{
			MATHFUNCTION_SIN			= 0,
			MATHFUNCTION_COS			= 1,
			MATHFUNCTION_TAN			= 2,
			MATHFUNCTION_SQR			= 3,
			MATHFUNCTION_SQRT			= 4,
			MATHFUNCTION_ABS			= 5,
			MATHFUNCTION_FLOOR			= 6,
			MATHFUNCTION_CEIL			= 7,
			MATHFUNCTION_ONEOVERINPUT	= 8,
			MATHFUNCTION_INVSQRT		= 9,
			MATHFUNCTION_LOG			= 10,
			MATHFUNCTION_LOG10			= 11,
			MATHFUNCTION_EXP			= 12,
			MATHFUNCTION_FRACTION		= 13,
			MATHFUNCTION_SIGN			= 14,
			MATHFUNCTION_ISPOSITIVE		= 15,
			MATHFUNCTION_ISNEGATIVE		= 16,
			MATHFUNCTION_ISNEARZERO		= 17,
			MATHFUNCTION_RANDOMFLOAT	= 18,
			MATHFUNCTION_RADTODEG		= 19,
			MATHFUNCTION_DEGTORAD		= 20,
			MATHFUNCTION_SMOOTHSTEPCOS	= 21,
			MATHFUNCTION_ACOS			= 22,
			MATHFUNCTION_ASIN			= 23,
			MATHFUNCTION_ATAN			= 24,
			MATHFUNCTION_SMOOTHSTEPPOLY	= 25,
			MATHFUNCTION_NUMFUNCTIONS
		};
		static const char* GetFunctionString(EMathFunction function);
	
		// settings fot the node processor
		class ProcessorSettings : public ChannelProcessor::Settings
		{
			public:
				enum { TYPE_ID = 0x0006 };

				ProcessorSettings()			 {}
				virtual ~ProcessorSettings() {}
			
				uint32 GetType() const override		{ return TYPE_ID; }

				// math function and one default-value
				EMathFunction   mMathFunction;
				Math1Function	mCalculateFunc;
				double			mDefaultValue;
		};
		
		ProcessorSettings	mSettings;

		// the node's channel processor
		class Processor : public ChannelProcessor
		{
			enum { TYPE_ID = 0x0006 };

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
		};
};


#endif
