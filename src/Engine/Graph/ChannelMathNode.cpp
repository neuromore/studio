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
#include "ChannelMathNode.h"
#include "../Core/AttributeSettings.h"
#include "Graph.h"


using namespace Core;

// constructor
ChannelMathNode::ChannelMathNode(Graph* graph) : SPNode(graph)
{
	// select sum as default
	mMathFunction = MATHFUNCTION_SUM;
	
	// set any buffersize > 0
	mChannel.SetBufferSize(10);
}


// destructor
ChannelMathNode::~ChannelMathNode()
{
}


// initialize the node
void ChannelMathNode::Init()
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
	GetOutputPort(OUTPUTPORT_RESULT).GetChannels()->AddChannel(&mChannel);

	// ATTRIBUTES

	// hidden port number attribute
	Core::AttributeSettings* attribNumPorts = RegisterAttribute("", "numInputPorts", "", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	attribNumPorts->SetDefaultValue( Core::AttributeInt32::Create(numPortsDefault) );
	attribNumPorts->SetMinValue( Core::AttributeInt32::Create(1) );
	attribNumPorts->SetMaxValue( Core::AttributeInt32::Create(INT_MAX) );
	attribNumPorts->SetVisible(false);

	// Math function combobox
	Core::AttributeSettings* functionParam = RegisterAttribute("Math Function", "mathFunction", "The math function to use.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	functionParam->ResizeComboValues( (uint32)MATHFUNCTION_NUMFUNCTIONS );
	for (uint32 i=0; i<MATHFUNCTION_NUMFUNCTIONS; ++i)
		functionParam->SetComboValue( i, GetFunctionString((EMathFunction)i) );
	functionParam->SetDefaultValue( Core::AttributeInt32::Create(mMathFunction) );

	// set math function
	SetMathFunction(MATHFUNCTION_SUM);
}


void ChannelMathNode::Reset()
{
	SPNode::Reset();

	// clear output channel set (but not delete channel)
	GetOutputPort(OUTPUTPORT_RESULT).GetChannels()->Clear();
}


void ChannelMathNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// dynamic input ports
	EnsureFreeInputPort();

	// reinit baseclass
	SPNode::ReInit(elapsed, delta);

	// TODO shouldn't this be done in Start()? samplerate change will trigger it, so everything should work
	if (mIsInitialized == true)
	{
		// set output channel properties
		if (mInputReader.GetNumChannels() > 0)
		{
			const double outputSampleRate = mInputReader.GetSampleRate();
			mChannel.SetSampleRate(outputSampleRate);
		}
	}

	PostReInit(elapsed, delta);
}


void ChannelMathNode::Start(const Time& elapsed)
{
	// connect channel to output port
	GetOutputPort(OUTPUTPORT_RESULT).GetChannels()->AddChannel(&mChannel);

	SPNode::Start(elapsed);
}



// update the node
void ChannelMathNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update base class
	SPNode::Update(elapsed, delta);

	// do nothing if node is not fully initialized
	if (mIsInitialized == false)
		return;

	// apply calculate function
	MultiChannelReader* input = GetInputReader();
	const uint32 numSamples = input->GetMinNumNewSamples();
	for (uint32 i=0; i<numSamples; ++i)
	{
		const double result = RunCalculation(input);
		mChannel.AddSample(result);
	}
}


// an attribute has changed
void ChannelMathNode::OnAttributesChanged()
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
void ChannelMathNode::UpdateInputPorts()
{
	// set all port names
	const uint32 numPorts = GetNumInputPorts();
	for (uint32 i=0; i<numPorts; ++i)
	{
		mTempString.Format("x%i", i+1);		// is 1-indexed
		GetInputPort(i).SetupAsChannels<double>("In", mTempString.AsChar(), i);
	}
}


// check if there are free input ports; if not, add one more
void ChannelMathNode::EnsureFreeInputPort()
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



//-----------------------------------------------
// math functions
//-----------------------------------------------

void ChannelMathNode::SetMathFunction(EMathFunction function)
{
	mMathFunction = function;
	switch (mMathFunction)
	{
		case MATHFUNCTION_SUM:			{ RunCalculation = CalculateSum; }		break;
		case MATHFUNCTION_PRODUCT:		{ RunCalculation = CalculateProduct; }	break;
		case MATHFUNCTION_AVERAGE:		{ RunCalculation = CalculateAverage; }	break;
		case MATHFUNCTION_MIN:			{ RunCalculation = CalculateMin; }		break;
		case MATHFUNCTION_MAX:			{ RunCalculation = CalculateMax; }		break;
		case MATHFUNCTION_HARMONIC:		{ RunCalculation = CalculateHarmonicMean; }		break;
		case MATHFUNCTION_GEOMETRIC:	{ RunCalculation = CalculateGeometricMean; }	break;
		case MATHFUNCTION_RMS:			{ RunCalculation = CalculateRootMeanSquare; }	break;
		case MATHFUNCTION_SQUARES:		{ RunCalculation = CalculateSumOfSquares; }		break;
		default:						{ CORE_ASSERT(false); }
	}

	// set output port name
	GetOutputPort(OUTPUTPORT_RESULT).SetName(GetFunctionString(mMathFunction));
}


const char* ChannelMathNode::GetFunctionString(EMathFunction function)
{
	switch (function)
	{
		case MATHFUNCTION_SUM:			{ return "Sum";		}
		case MATHFUNCTION_PRODUCT:		{ return "Product"; }
		case MATHFUNCTION_AVERAGE:		{ return "Average"; }
		case MATHFUNCTION_MIN:			{ return "Minimum"; }
		case MATHFUNCTION_MAX:			{ return "Maximum"; }
		case MATHFUNCTION_HARMONIC:		{ return "Harmonic Mean";	 }
		case MATHFUNCTION_GEOMETRIC:	{ return "Geometric Mean";	 }
		case MATHFUNCTION_RMS:			{ return "Root Mean Square"; }
		case MATHFUNCTION_SQUARES:		{ return "Sum of Squares";	 }
		default:						{ return "Undefined"; }
	}
}


// Sum: (s1 + s2 + s3 + ...)
double ChannelMathNode::CalculateSum(MultiChannelReader* reader)				
{ 
	const uint32 numChannels = reader->GetChannels()->GetNumChannels();

	double sum = 0;
	for (uint32 c=0; c<numChannels; ++c)
	{
		double sample = reader->GetReader(c)->PopOldestSample<double>();
		sum += sample;
	}

	return sum;	
}

// Product: (s1 * s2 * s3 * ...)
double ChannelMathNode::CalculateProduct(MultiChannelReader* reader)				
{ 
	const uint32 numChannels = reader->GetChannels()->GetNumChannels();

	double product = 1;
	for (uint32 c=0; c<numChannels; ++c)
	{
		double sample = reader->GetReader(c)->PopOldestSample<double>();
		product *= sample;
	}

	return product;	
}

// Average: (s1 + s2 + s3 + ...) / n
double ChannelMathNode::CalculateAverage(MultiChannelReader* reader)				
{ 
	const uint32 numChannels = reader->GetChannels()->GetNumChannels();

	double sum = 0;
	for (uint32 c=0; c<numChannels; ++c)
	{
		double sample = reader->GetReader(c)->PopOldestSample<double>();
		sum += sample;
	}

	double average = sum / (double)numChannels;
	return average;	
}


// Minimum: Min(s1, s2, ...)
double ChannelMathNode::CalculateMin(MultiChannelReader* reader)				
{ 
	const uint32 numChannels = reader->GetChannels()->GetNumChannels();

	// guard agains div by zero
	if (numChannels == 0)
		return 0;

	double minVal = DBL_MAX;
	for (uint32 c=0; c<numChannels; ++c)
	{
		double sample = reader->GetReader(c)->PopOldestSample<double>();
		minVal = Min<double>(minVal, sample);
	}

	return minVal;	
}

// Maximum: Max(s1, s2, ...)
double ChannelMathNode::CalculateMax(MultiChannelReader* reader)				
{ 
	const uint32 numChannels = reader->GetChannels()->GetNumChannels();
	
	double maxVal = -DBL_MAX;
	for (uint32 c=0; c<numChannels; ++c)
	{
		double sample = reader->GetReader(c)->PopOldestSample<double>();
		maxVal = Max<double>(maxVal, sample);
	}

	return maxVal;	
}

// Harmonic Mean: result = n / (1/s1 + 1/s2 + 1/s3 + ...)
double ChannelMathNode::CalculateHarmonicMean(MultiChannelReader* reader)				
{ 
	const uint32 numChannels = reader->GetChannels()->GetNumChannels();

	double result = 0;
	for (uint32 c=0; c<numChannels; ++c)
	{
		const double sample = reader->GetReader(c)->PopOldestSample<double>();

		// harmonic mean is 0 if one term is 0
		if (sample == 0)
			return 0;

		const double reciprocal = 1.0 / sample;
		result += reciprocal;
	}

	// NOTE: result can only be zero if numChannels == 0, and we catch that earlier
	//  Update: can also be zero if sample == DBL_MAX? lets better check to be sure no div by zero happens

	if (result == 0.0)
		return 0;

	// n / sum
	result = (double)numChannels / result;

	return result;	
}


// Geometric Mean: n-th root of ( s1 * s2 * ...)
double ChannelMathNode::CalculateGeometricMean(MultiChannelReader* reader)				
{ 
	const uint32 numChannels = reader->GetChannels()->GetNumChannels();

	// guard agains div by zero
	if (numChannels == 0)
		return 0;

	double result = CalculateProduct(reader);

	// cannot have negative value in geometric mean :) (roots, dude!)
	if (result <= 0)
		return 0;

	// n-th root
	const double exponent = 1.0 / (double)numChannels;
	result = Math::PowD(result, exponent);

	return result;	

}

// Root Mean Square (quadratic mean): root of (mean of (sum of squares))
double ChannelMathNode::CalculateRootMeanSquare(MultiChannelReader* reader)				
{ 
	const uint32 numChannels = reader->GetChannels()->GetNumChannels();
	
	// guard agains div by zero
	if (numChannels == 0)
		return 0;

	// sum of squares
	double result = CalculateSumOfSquares(reader);

	// mean
	result = result / (double)numChannels;

	// square root
	result = Math::SqrtD(result);

	return result;	
}


// Sum of Square: s1*s1 + s2*s2 + s3*s3 + ....
double ChannelMathNode::CalculateSumOfSquares(MultiChannelReader* reader)				
{ 
	const uint32 numChannels = reader->GetChannels()->GetNumChannels();

	double sum = 0;
	for (uint32 c=0; c<numChannels; ++c)
	{
		double sample = reader->GetReader(c)->PopOldestSample<double>();
		const double sampleSquared = sample * sample;
		sum += sampleSquared;
	}
	return sum;	
}


