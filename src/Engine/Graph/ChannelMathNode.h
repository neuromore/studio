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

#ifndef __NEUROMORE_CHANNELMATHNODE_H
#define __NEUROMORE_CHANNELMATHNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "SPNode.h"


class ENGINE_API ChannelMathNode : public SPNode
{
	public:
		enum { TYPE_ID = 0x0026 };
		static const char* Uuid () { return "deadc8ee-cd6c-11e4-afdc-1681e6b88ec1"; }

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
		ChannelMathNode(Graph* graph);
		~ChannelMathNode();

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
		const char* GetReadableType() const override					{ return "Channel Math"; }
		const char* GetRuleName() const override final					{ return "NODE_ChannelMath"; }
		uint32 GetPaletteCategory() const override						{ return CATEGORY_MATH; }
		GraphObject* Clone(Graph* graph) override						{ ChannelMathNode* clone = new ChannelMathNode(graph); return clone; }

	private:
		Channel<double> mChannel;

		void EnsureFreeInputPort();
		void UpdateInputPorts();

		// Available math functions
		enum EMathFunction
		{
			MATHFUNCTION_SUM		= 0,
			MATHFUNCTION_PRODUCT	= 1,
			MATHFUNCTION_AVERAGE	= 2,
			MATHFUNCTION_MIN		= 3,
			MATHFUNCTION_MAX		= 4,
			MATHFUNCTION_HARMONIC	= 5,
			MATHFUNCTION_GEOMETRIC  = 6,
			MATHFUNCTION_RMS		= 7,
			MATHFUNCTION_SQUARES    = 8,
			MATHFUNCTION_NUMFUNCTIONS
		};

		EMathFunction mMathFunction;

		void SetMathFunction(EMathFunction function);

		const char* GetFunctionString(EMathFunction function);

		// Math functions 
		typedef double (CORE_CDECL *MathFunction)(MultiChannelReader* reader);
		static double CORE_CDECL CalculateSum(MultiChannelReader* reader);
		static double CORE_CDECL CalculateProduct(MultiChannelReader* reader);

		static double CORE_CDECL CalculateAverage(MultiChannelReader* reader);
		static double CORE_CDECL CalculateMin(MultiChannelReader* reader);
		static double CORE_CDECL CalculateMax(MultiChannelReader* reader);
		
		static double CORE_CDECL CalculateHarmonicMean(MultiChannelReader* reader);
		static double CORE_CDECL CalculateGeometricMean(MultiChannelReader* reader);
		static double CORE_CDECL CalculateRootMeanSquare(MultiChannelReader* reader);
		static double CORE_CDECL CalculateSumOfSquares(MultiChannelReader* reader);
		
		// Active math function
		MathFunction RunCalculation;
};


#endif
