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
#include "FFTNode.h"
#include "../Core/Math.h"


using namespace Core;

// constructor
FFTNode::FFTNode(Graph* graph) : ProcessorNode(graph, new FFTProcessor())
{
	// default values
	mSettings.mFFTOrder = 7;
	mSettings.mWindowFunction.SetType(WindowFunction::WINDOWFUNCTION_HANN);
	mSettings.mEpochShift = 1;
	mSettings.mUseZeroPadding = true;
}


// destructor
FFTNode::~FFTNode()
{
}


// initialize the node
void FFTNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	// CONFIG SPNODE
	RequireConstantSampleRate();

	// SETUP PORTS

	GetInputPort(INPUTPORT_CHANNEL).Setup("In", "x", AttributeChannels<double>::TYPE_ID, PORTID_INPUT_SAMPLE);
	GetOutputPort(OUTPUTPORT_SPECTRUM).Setup("Out (Spectrum)", "y", AttributeChannels<Spectrum>::TYPE_ID, PORTID_OUTPUT_SPECTRUM);

	// SETUP ATTRIBUTES

	// FFT order
	Core::AttributeSettings* FFTOrderAttr = RegisterAttribute( "FFT Order", "FFTorder", "Order of the FFT.", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER );
	FFTOrderAttr->SetDefaultValue(Core::AttributeInt32::Create(mSettings.mFFTOrder));
	FFTOrderAttr->SetMinValue(Core::AttributeInt32::Create(2));
	FFTOrderAttr->SetMaxValue(Core::AttributeInt32::Create(20));

	// window type
	Core::AttributeSettings* winFuncAttr = RegisterAttribute( "Window Function", "WindowFunction", "The Time-Domain Window Function that is applied to the input of the FFT.", Core::ATTRIBUTE_INTERFACETYPE_WINDOWFUNCTION );
	winFuncAttr->ResizeComboValues( WindowFunction::WINDOWFUNCTION_NUMFUNCTIONS );
	for (uint32 i = 0; i < WindowFunction::WINDOWFUNCTION_NUMFUNCTIONS; i++)
		winFuncAttr->SetComboValue(i, WindowFunction::GetName((WindowFunction::EWindowFunction)i));
	winFuncAttr->SetDefaultValue(Core::AttributeInt32::Create((uint32)mSettings.mWindowFunction.GetType()));

	// window step size
	Core::AttributeSettings* winShiftAttr = RegisterAttribute( "Window Shift", "NumWindowShiftSamples", "The number of samples the FFT input window advances in each iteration.", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER );
	winShiftAttr->SetDefaultValue(Core::AttributeInt32::Create(mSettings.mEpochShift));
	winShiftAttr->SetMinValue(Core::AttributeInt32::Create(1));
	winShiftAttr->SetMaxValue(Core::AttributeInt32::Create(1024));
}



void FFTNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


void FFTNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::Update(elapsed, delta);
}


// attributes have changed
void FFTNode::OnAttributesChanged()
{
	const uint32 fftOrder = GetInt32Attribute(ATTRIB_FFTORDER);
	const uint32 shiftSteps = GetInt32Attribute(ATTRIB_SHIFTSAMPLES);
	const uint32 windowFunctionID = GetInt32Attribute(ATTRIB_WINDOWFUNCTION);

	// check if settings have changed
	if (mSettings.mFFTOrder == fftOrder && 
		mSettings.mEpochShift == shiftSteps && 
		mSettings.mWindowFunction.GetType() == (WindowFunction::EWindowFunction)windowFunctionID)
	{
		return;
	}

	mSettings.mFFTOrder = fftOrder;
	mSettings.mEpochShift = shiftSteps;
	mSettings.mWindowFunction.SetType((WindowFunction::EWindowFunction)windowFunctionID);

	ResetAsync();
}
