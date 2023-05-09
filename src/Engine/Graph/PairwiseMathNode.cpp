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

// include precompiled header
#include <Engine/Precompiled.h>

// include required headers
#include "PairwiseMathNode.h"
#include "../Core/AttributeSettings.h"
#include "Graph.h"


using namespace Core;

// constructor
PairwiseMathNode::PairwiseMathNode(Graph* graph) : SPNode(graph)
{
	// select sum as default
	mMathFunction = MATHFUNCTION_EUCLID_DISTANCE;
}


// destructor
PairwiseMathNode::~PairwiseMathNode()
{
}


// initialize the node
void PairwiseMathNode::Init()
{
	// init base class first
	SPNode::Init();

	// configure SPNode behaviour
	UseChannelNamePropagation();
	UseChannelColoring();
	RequireMatchingSampleRates();
	RequireInputConnection();

	// PORTS
	const uint32 numPortsDefault = 1;
	InitInputPorts(numPortsDefault);
	UpdateInputPorts();
	
	InitOutputPorts(1);
	GetOutputPort(OUTPUTPORT_RESULT).SetupAsChannels<double>("Out", "y1", OUTPUTPORT_RESULT);

	// ATTRIBUTES

	// hidden port number attribute
	Core::AttributeSettings* attribNumPorts = RegisterAttribute("", "numInputPorts", "", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	attribNumPorts->SetDefaultValue( Core::AttributeInt32::Create(numPortsDefault) );
	attribNumPorts->SetMinValue( Core::AttributeInt32::Create(1) );
	attribNumPorts->SetMaxValue( Core::AttributeInt32::Create(INT_MAX) );
	attribNumPorts->SetVisible(false);

	// Math function combobox
	Core::AttributeSettings* functionParam = RegisterAttribute("Math Function", "mathFunction", "The function to use.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	functionParam->ResizeComboValues( (uint32)NUM_MATHFUNCTIONS );
	for (uint32 i=0; i<NUM_MATHFUNCTIONS; ++i)
		functionParam->SetComboValue( i, GetFunctionString((EMathFunction)i) );
	functionParam->SetDefaultValue( Core::AttributeInt32::Create(mMathFunction) );

	// set math function
	SetMathFunction(MATHFUNCTION_SUM);
}


void PairwiseMathNode::Reset()
{
	SPNode::Reset();

	// clear output channel set (but not delete channel)
	const uint32 numChannels = mOutputChannels.Size();
	for (uint32 i = 0; i < numChannels; ++i)
		mOutputChannels[i].Clear();

	// reset output port
	GetOutputPort(OUTPUTPORT_RESULT).GetChannels()->Clear();

}


void PairwiseMathNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// dynamic input ports
	EnsureFreeInputPort();

	// reinit baseclass
	SPNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


void PairwiseMathNode::Start(const Time& elapsed)
{
	
	ReInitOutputChannels();

	SPNode::Start(elapsed);
}



// update the node
void PairwiseMathNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update base class
	SPNode::Update(elapsed, delta);

	// do nothing if node is not fully initialized
	if (mIsInitialized == false)
		return;

	// apply calculate function
	const uint32 numInputs = mInputReader.GetNumChannels();
	const uint32 numOutputs = mOutputChannels.Size();
	
	CORE_ASSERT(numOutputs == CalcNumOutputs(numInputs, true));

	const uint32 numSamples = mInputReader.GetMinNumNewSamples();
	for (uint32 i=0; i<numSamples; ++i)
	{
		// Step 1: process all pairs of inputs
		uint32 index = 0;
		for (uint32 j = 0; j < numInputs; ++j)
		{
			ChannelReader* channelReaderA = mInputReader.GetReader(j);
			for (uint32 k = 0; k < j; k++)
			{
				ChannelReader* channelReaderB = mInputReader.GetReader(k);

				const double valueA = channelReaderA->GetOldestSample<double>();
				const double valueB = channelReaderB->GetOldestSample<double>();
				const double result = RunCalculation(valueA, valueB);

				mOutputChannels[index].AddSample(result);

				index++;
			}
		}

		// Step 2: advance all input channel readers (they have to be read multiple during Step 1, so we can't pop the samples there)
		for (uint32 j = 0; j < numInputs; ++j)
			mInputReader.GetReader(j)->Advance(1);
	}
}


// an attribute has changed
void PairwiseMathNode::OnAttributesChanged()
{
	// init 
	const uint32 numPorts = GetInt32Attribute(ATTRIB_NUMINPUTPORTS);
	InitInputPorts(numPorts);
	UpdateInputPorts();

	// updated selected math function pointer
	EMathFunction function = (EMathFunction)GetInt32Attribute(ATTRIB_FUNCTION);
	if (mMathFunction != function)
		SetMathFunction(function);
}


// update input port names
void PairwiseMathNode::UpdateInputPorts()
{
	// set all port names
	const uint32 numPorts = GetNumInputPorts();
	for (uint32 i=0; i<numPorts; ++i)
	{
		mTempString.Format("x%i", i+1);		// is 1-indexed
		GetInputPort(i).SetupAsChannels<double>("In", mTempString.AsChar(), i);
	}
}


void PairwiseMathNode::ReInitOutputChannels()
{
	const uint32 numInputs = mInputReader.GetNumChannels();
	const uint32 numOutputs = CalcNumOutputs(numInputs, true);
	
	mOutputChannels.Resize(numOutputs);

	uint32 index = 0;
	for (uint32 i = 0; i < numInputs; ++i)
	{
		ChannelBase* channelA = mInputReader.GetChannel(i);
		for (uint32 j = 0; j < i; j++)
		{
			ChannelBase* channelB = mInputReader.GetChannel(j);
			
			// set output channel name
			mTempString.Format("%s, %s", channelA->GetName(), channelB->GetName());
			mOutputChannels[index].SetName(mTempString.AsChar());
			index++;
		}
	}

	CORE_ASSERT(index == numOutputs);

	// add to output channels
	GetOutputPort(OUTPUTPORT_RESULT).GetChannels()->Clear();
	for (uint32 i = 0; i < numOutputs; ++i)
	{

		ChannelBase* channelOut = &mOutputChannels[index];
		channelOut->Clear();
		GetOutputPort(OUTPUTPORT_RESULT).GetChannels()->AddChannel(channelOut);
	}
}


// check if there are free input ports; if not, add one more
void PairwiseMathNode::EnsureFreeInputPort()
{
	bool haveFreePort = false;
	const uint32 numPorts = GetNumInputPorts();
	for (uint32 i=0; i<numPorts; ++i)
	{
		if (GetInputPort(i).HasConnection() == false)
		{
			haveFreePort = true;

			// if the free port is the last one, we can exit immediately
			if (i == numPorts - 1)
				return;

			break;
		}
	}

	// calculate new number of ports
	uint32 newNumPorts = numPorts;
	
	// add one
	if (haveFreePort == false)
	{
		newNumPorts++;
	}
	else   // remove all free ports at the end, except one
	{
		while (newNumPorts > 1)
		{
			if (GetInputPort(newNumPorts-1).HasConnection() == false && 
				GetInputPort(newNumPorts-2).HasConnection() == false)
			{
				newNumPorts--;
			}
			else
				break;
		}
	}
	
	// update number of ports
	if (newNumPorts != numPorts)
	{
		SetInt32Attribute("numInputPorts", newNumPorts);
		InitInputPorts(newNumPorts);
	}

	UpdateInputPorts();
}


uint32 PairwiseMathNode::CalcNumOutputs(const uint32 numInputs, bool commutative)
{
	if (commutative == true)
		return numInputs * (numInputs - 1);			// full 'matrix' without diagonal
	else
		return numInputs * (numInputs - 1) / 2;		// triangle matric without diagonal
}


//-----------------------------------------------
// math functions
//-----------------------------------------------

void PairwiseMathNode::SetMathFunction(EMathFunction function)
{
	mMathFunction = function;
	switch (mMathFunction)
	{
		case MATHFUNCTION_SUM:				{ RunCalculation = CalculateSum; }						break;
		case MATHFUNCTION_PRODUCT:			{ RunCalculation = CalculateProduct; }					break;
		case MATHFUNCTION_AVERAGE:			{ RunCalculation = CalculateAverage; }					break;
		case MATHFUNCTION_MIN:				{ RunCalculation = CalculateMin; }						break;
		case MATHFUNCTION_MAX:				{ RunCalculation = CalculateMax; }						break;
		case MATHFUNCTION_EUCLID_DISTANCE:	{ RunCalculation = CalculateEuclideanDistance; }		break;
		case MATHFUNCTION_L2_DISTANCE:		{ RunCalculation = CalculateL2Distance; }				break;
		case MATHFUNCTION_AND:				{ RunCalculation = CalculateAnd; }						break;
		case MATHFUNCTION_OR:				{ RunCalculation = CalculateOr; }						break;
		case MATHFUNCTION_XOR:				{ RunCalculation = CalculateXor; }						break;
		default:							{ CORE_ASSERT(false); }
	}

	// set output port name
	GetOutputPort(OUTPUTPORT_RESULT).SetName(GetFunctionString(mMathFunction));
}


const char* PairwiseMathNode::GetFunctionString(EMathFunction function)
{
	switch (function)
	{
		case MATHFUNCTION_SUM:				{ return "Sum";		}
		case MATHFUNCTION_PRODUCT:			{ return "Product"; }
		case MATHFUNCTION_AVERAGE:			{ return "Average"; }
		case MATHFUNCTION_MIN:				{ return "Minimum"; }
		case MATHFUNCTION_MAX:				{ return "Maximum"; }
		case MATHFUNCTION_EUCLID_DISTANCE:	{ return "Euclidean Distance";	 }
		case MATHFUNCTION_L2_DISTANCE:		{ return "L2 Distance"; }
		case MATHFUNCTION_AND:				{ return "AND";	 }
		case MATHFUNCTION_OR:				{ return "OR";	 }
		case MATHFUNCTION_XOR:				{ return "XOR";	 }
		default:							{ return "Undefined"; }
	}
}

//-----------------------------------------------
// the math functions
//-----------------------------------------------
double PairwiseMathNode::CalculateSum(double x, double y)						{ return x + y; }
double PairwiseMathNode::CalculateProduct(double x, double y)					{ return x * y; }
double PairwiseMathNode::CalculateAverage(double x, double y)					{ return (x + y)*0.5f; }
double PairwiseMathNode::CalculateMin(double x, double y)						{ return Core::Min<double>(x, y); }
double PairwiseMathNode::CalculateMax(double x, double y)						{ return Core::Max<double>(x, y); }
double PairwiseMathNode::CalculateEuclideanDistance(double x, double y)			{ return Core::Math::AbsD(x - y); }
double PairwiseMathNode::CalculateL2Distance(double x, double y)				{ return Core::Math::Sqrt( x*x + y*y ); }
double PairwiseMathNode::CalculateAnd(double x, double y)						{ return ( (x >= 1 && y >=1) ? 1.0 : 0.0); }					// return 1.0 if x and y is >= 1.0
double PairwiseMathNode::CalculateOr(double x, double y)						{ return ( (x >= 1 || y >= 1) ? 1.0 : 0.0); }					// return 1.0 if x or y is >= 1.0
double PairwiseMathNode::CalculateXor(double x, double y)						{ return Core::Math::FModD(x + y, 2.0) >= 1 ? 1.0 : 0.0; }		// uses algebraic xor

