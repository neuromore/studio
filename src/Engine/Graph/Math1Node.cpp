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

// include required headers
#include "Math1Node.h"
#include "../Core/Math.h"


using namespace Core;

// constructor
Math1Node::Math1Node(Graph* graph) : ProcessorNode(graph, new Math1Node::Processor())
{
	// default on addition
	mSettings.mMathFunction	 = MATHFUNCTION_SIN;
	mSettings.mCalculateFunc = CalculateSin;
}


// destructor
Math1Node::~Math1Node()
{
}


// initialize the node
void Math1Node::Init()
{
	// init base class first
	ProcessorNode::Init();

	// SETUP PORTS

	// setup the input ports
	GetInputPort(INPUTPORT).Setup("x", "x", AttributeChannels<double>::TYPE_ID, INPUTPORT);

	// setup the output ports
	GetOutputPort(OUTPUTPORT_RESULT).Setup(GetFunctionString(mSettings.mMathFunction), "y", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_RESULT);

	// ATTRIBUTES

	Core::AttributeSettings* functionParam = RegisterAttribute("Math Function", "mathFunction", "The math function to use.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	functionParam->ResizeComboValues( (uint32)MATHFUNCTION_NUMFUNCTIONS );
	for (uint32 i=0; i<MATHFUNCTION_NUMFUNCTIONS; ++i)
		functionParam->SetComboValue( i, GetFunctionString((EMathFunction)i) );
	functionParam->SetDefaultValue( Core::AttributeInt32::Create(mSettings.mMathFunction) );
}


void Math1Node::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


// update the node
void Math1Node::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update the baseclass
	ProcessorNode::Update(elapsed, delta);
}


// update the data
void Math1Node::OnAttributesChanged()
{
	EMathFunction function = (EMathFunction)GetInt32Attribute(ATTRIB_MATHFUNCTION);

	// if it didn't change, don't update anything
	if (function == mSettings.mMathFunction)
		return;
	
	mSettings.mMathFunction = function;
	switch (mSettings.mMathFunction)
	{
		case MATHFUNCTION_SIN:				mSettings.mCalculateFunc = CalculateSin;				break;
		case MATHFUNCTION_COS:				mSettings.mCalculateFunc = CalculateCos;				break;
		case MATHFUNCTION_TAN:				mSettings.mCalculateFunc = CalculateTan;				break;
		case MATHFUNCTION_SQR:				mSettings.mCalculateFunc = CalculateSqr;				break;
		case MATHFUNCTION_SQRT:				mSettings.mCalculateFunc = CalculateSqrt;				break;
		case MATHFUNCTION_ABS:				mSettings.mCalculateFunc = CalculateAbs;				break;
		case MATHFUNCTION_FLOOR:			mSettings.mCalculateFunc = CalculateFloor;				break;
		case MATHFUNCTION_CEIL:				mSettings.mCalculateFunc = CalculateCeil;				break;
		case MATHFUNCTION_ONEOVERINPUT:		mSettings.mCalculateFunc = CalculateOneOverInput;		break;
		case MATHFUNCTION_INVSQRT:			mSettings.mCalculateFunc = CalculateInvSqrt;			break;
		case MATHFUNCTION_LOG:				mSettings.mCalculateFunc = CalculateLog;				break;
		case MATHFUNCTION_LOG10:			mSettings.mCalculateFunc = CalculateLog10;				break;
		case MATHFUNCTION_EXP:				mSettings.mCalculateFunc = CalculateExp;				break;
		case MATHFUNCTION_FRACTION:			mSettings.mCalculateFunc = CalculateFraction;			break;
		case MATHFUNCTION_SIGN:				mSettings.mCalculateFunc = CalculateSign;				break;
		case MATHFUNCTION_ISPOSITIVE:		mSettings.mCalculateFunc = CalculateIsPositive;			break;
		case MATHFUNCTION_ISNEGATIVE:		mSettings.mCalculateFunc = CalculateIsNegative;			break;
		case MATHFUNCTION_ISNEARZERO:		mSettings.mCalculateFunc = CalculateIsNearZero;			break;
		case MATHFUNCTION_RANDOMFLOAT:		mSettings.mCalculateFunc = CalculateRandomFloat;		break;
		case MATHFUNCTION_RADTODEG:			mSettings.mCalculateFunc = CalculateRadToDeg;			break;
		case MATHFUNCTION_DEGTORAD:			mSettings.mCalculateFunc = CalculateDegToRad;			break;
		case MATHFUNCTION_SMOOTHSTEPCOS:	mSettings.mCalculateFunc = CalculateSmoothStepCos;		break;
		case MATHFUNCTION_ACOS:				mSettings.mCalculateFunc = CalculateACos;				break;
		case MATHFUNCTION_ASIN:				mSettings.mCalculateFunc = CalculateASin;				break;
		case MATHFUNCTION_ATAN:				mSettings.mCalculateFunc = CalculateATan;				break;
		case MATHFUNCTION_SMOOTHSTEPPOLY:	mSettings.mCalculateFunc = CalculateSmoothStepPoly;		break;

		default: CORE_ASSERT(1==0);	// function unknown
	};

	// output port name
	OutputPort& outputPort = GetOutputPort(OUTPUTPORT_RESULT);
	outputPort.SetName( GetFunctionString(function) );

	// reconfigure processors
	SetupProcessors();
}



//-----------------------------------------------
// the math functions
//-----------------------------------------------
double Math1Node::CalculateSin(double input)					{ return Core::Math::SinD( input ); }
double Math1Node::CalculateCos(double input)					{ return Core::Math::CosD( input ); }
double Math1Node::CalculateTan(double input)					{ return Core::Math::TanD( input ); }
double Math1Node::CalculateSqr(double input)					{ return (input * input); }
double Math1Node::CalculateSqrt(double input)					{ return Core::Math::SafeSqrtD( input ); }
double Math1Node::CalculateAbs(double input)					{ return Core::Math::AbsD( input ); }
double Math1Node::CalculateFloor(double input)					{ return Core::Math::FloorD( input ); }
double Math1Node::CalculateCeil(double input)					{ return Core::Math::CeilD( input ); }
double Math1Node::CalculateOneOverInput(double input)			{ if (input > Core::Math::epsilon) return 1.0 / input; return 0.0; }
double Math1Node::CalculateInvSqrt(double input)				{ if (input > Core::Math::epsilon) return Core::Math::InvSqrtD( input ); return 0.0; }
double Math1Node::CalculateLog(double input)					{ if (input > Core::Math::epsilon) return Core::Math::LogD(input); return 0.0; }
double Math1Node::CalculateLog10(double input)					{ if (input > Core::Math::epsilon) return log10(input); return 0.0; }
double Math1Node::CalculateExp(double input)					{ return Core::Math::ExpD(input); }
double Math1Node::CalculateFraction(double input)				{ return Core::Math::FModD(input, 1.0); }
double Math1Node::CalculateSign(double input)					{ if (input < 0.0) return -1.0; if (input > 0.0) return 1.0; return 0.0; }
double Math1Node::CalculateIsPositive(double input)				{ if (input >= 0.0) return 1.0; return 0.0; }
double Math1Node::CalculateIsNegative(double input)				{ if (input < 0.0) return 1.0; return 0.0; }
double Math1Node::CalculateIsNearZero(double input)				{ if ((input > -Core::Math::epsilon) && (input < Core::Math::epsilon)) return 1.0; return 0.0; }
double Math1Node::CalculateRandomFloat(double input)			{ return Core::Math::RandF(0.0, input);  }
double Math1Node::CalculateRadToDeg(double input)				{ return Core::Math::RadiansToDegrees( input ); }
double Math1Node::CalculateDegToRad(double input)				{ return Core::Math::DegreesToRadians( input ); }
double Math1Node::CalculateSmoothStepCos(double input)			{ const double x=Core::Clamp<double>(input, 0.0, 1.0); return Core::CosineInterpolate<double>(0.0, 1.0, x); }
double Math1Node::CalculateSmoothStepPoly(double input)			{ const double x=Core::Clamp<double>(input, 0.0, 1.0); return x*x*(3.0 - 2.0*input); }
double Math1Node::CalculateACos(double input)					{ return Core::Math::ACosD(input); }
double Math1Node::CalculateASin(double input)					{ return Core::Math::ASinD(input); }
double Math1Node::CalculateATan(double input)					{ return Core::Math::ATanD(input); }


const char* Math1Node::GetFunctionString(EMathFunction function)
{
	switch (function)
	{
		case MATHFUNCTION_SIN:				{ return "Sine"; }
		case MATHFUNCTION_COS:				{ return "Cosine"; }
		case MATHFUNCTION_TAN:				{ return "Tan"; }
		case MATHFUNCTION_SQR:				{ return "Square"; }
		case MATHFUNCTION_SQRT:				{ return "Square Root"; }
		case MATHFUNCTION_ABS:				{ return "Absolute"; }
		case MATHFUNCTION_FLOOR:			{ return "Floor"; }
		case MATHFUNCTION_CEIL:				{ return "Ceil"; }
		case MATHFUNCTION_ONEOVERINPUT:		{ return "One Over X"; }
		case MATHFUNCTION_INVSQRT:			{ return "Inverse Square Root"; }
		case MATHFUNCTION_LOG:				{ return "Natural Log"; }
		case MATHFUNCTION_LOG10:			{ return "Log Base 10"; }
		case MATHFUNCTION_EXP:				{ return "Exponent"; }
		case MATHFUNCTION_FRACTION:			{ return "Fraction"; }
		case MATHFUNCTION_SIGN:				{ return "Sign"; }
		case MATHFUNCTION_ISPOSITIVE:		{ return "Is Positive"; }
		case MATHFUNCTION_ISNEGATIVE:		{ return "Is Negative"; }
		case MATHFUNCTION_ISNEARZERO:		{ return "Is Near Zero"; }
		case MATHFUNCTION_RANDOMFLOAT:		{ return "Random Float"; }
		case MATHFUNCTION_RADTODEG:			{ return "Radians to Degrees"; }
		case MATHFUNCTION_DEGTORAD:			{ return "Degrees to Radians"; }
		case MATHFUNCTION_SMOOTHSTEPCOS:	{ return "Smooth Step Cosine [0..1]"; }
		case MATHFUNCTION_ACOS:				{ return "Arc Cosine"; }
		case MATHFUNCTION_ASIN:				{ return "Arc Sine"; }
		case MATHFUNCTION_ATAN:				{ return "Arc Tan"; }
		case MATHFUNCTION_SMOOTHSTEPPOLY:	{ return "Smooth Step Poly [0..1]"; }
        default:							{ return ""; }
	}
}
//-----------------------------------------------
// the node's processor implementation
//-----------------------------------------------

void Math1Node::Processor::ReInit()					
{ 
	// ReInit baseclass
	ChannelProcessor::ReInit();

	mIsInitialized = false;
				
	ChannelBase* input = GetInput();
	ChannelBase* output = GetOutput();

	// nothing to do until a channel is connected
	if (input == NULL || output == NULL)
		return;

	double outputSampleRate = input->GetSampleRate();
	output->SetSampleRate(outputSampleRate);
				
	mIsInitialized = true;
}


void Math1Node::Processor::Update()
{
	if (mIsInitialized == false)
		return;
	
	// update base
	ChannelProcessor::Update();
	
	// channel input reader
	ChannelReader* input = GetInputReader(0);

	// input/output channels
	ChannelBase* output = GetOutput();
				
	// get number of samples
	uint32 numNewSamples = input->GetNumNewSamples();
	// process input samples
	for (uint32 i=0; i<numNewSamples; ++i)
	{
		// apply math function
		const double value = input->PopOldestSample<double>();
		const double result = mSettings.mCalculateFunc(value);

		// add result to output
		output->AsType<double>()->AddSample(result);
	}
}
