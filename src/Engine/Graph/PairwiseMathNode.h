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

#ifndef __NEUROMORE_PAIRWISEMATHNODE_H
#define __NEUROMORE_PAIRWISEMATHNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "SPNode.h"


class ENGINE_API PairwiseMathNode : public SPNode
{
	public:
		enum { TYPE_ID = 0x0047 };
		static const char* Uuid () { return "32051a72-a762-11e5-bf7f-feff819cdc9f"; }

		enum
		{
			OUTPUTPORT_RESULT	= 0
		};

		enum
		{
			ATTRIB_NUMINPUTPORTS	= 0,
			ATTRIB_FUNCTION			= 1,
		};
		
		// constructor & destructor
		PairwiseMathNode(Graph* graph);
		~PairwiseMathNode();

		// initialize & update
		void Init() override;
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;

		Core::Color GetColor() const override						{ return Core::RGBA(73,173,255); }
		uint32 GetType() const override									{ return TYPE_ID; }
		const char* GetTypeUuid() const override final					{ return Uuid(); }
		const char* GetReadableType() const override					{ return "Pairwise Math"; }
		const char* GetRuleName() const override final					{ return "NODE_PairwiseMath"; }
		uint32 GetPaletteCategory() const override						{ return CATEGORY_MATH; }
		GraphObject* Clone(Graph* graph) override						{ PairwiseMathNode* clone = new PairwiseMathNode(graph); return clone; }

		bool IsUnstable() const override final							{ return true; }

	private:
		Core::Array<Channel<double>> mOutputChannels;

		void EnsureFreeInputPort();
		void UpdateInputPorts();
		void ReInitOutputChannels();
		uint32 CalcNumOutputs(const uint32 numInputs, bool commutative);


		// note: only commutative math functions for now
		enum EMathFunction
		{
			MATHFUNCTION_SUM = 0,
			MATHFUNCTION_PRODUCT,
			MATHFUNCTION_AVERAGE,
			MATHFUNCTION_MIN,	
			MATHFUNCTION_MAX,
			MATHFUNCTION_EUCLID_DISTANCE,  	// |x - y|		
			MATHFUNCTION_L2_DISTANCE,		// sqrt (x^2 + y^2)

			MATHFUNCTION_AND,
			MATHFUNCTION_OR,
			MATHFUNCTION_XOR,

			NUM_MATHFUNCTIONS
		};

		EMathFunction mMathFunction;

		const char* GetFunctionString(EMathFunction function);

		void SetMathFunction(EMathFunction function);

		// Commutative Math functions 
		typedef double (CORE_CDECL *MathFunction)(double x, double y);
		static double CORE_CDECL CalculateSum(double x, double y);
		static double CORE_CDECL CalculateProduct(double x, double y);
		static double CORE_CDECL CalculateAverage(double x, double y);
		static double CORE_CDECL CalculateMin(double x, double y);
		static double CORE_CDECL CalculateMax(double x, double y);
		static double CORE_CDECL CalculateEuclideanDistance(double x, double y);
		static double CORE_CDECL CalculateL2Distance(double x, double y);
		static double CORE_CDECL CalculateAnd(double x, double y);
		static double CORE_CDECL CalculateOr(double x, double y);
		static double CORE_CDECL CalculateXor(double x, double y);
		
		// Active math function
		MathFunction RunCalculation;
};


#endif
