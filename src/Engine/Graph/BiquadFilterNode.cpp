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
#include "BiquadFilterNode.h"
#include "../Core/Math.h"
#include "../DSP/LinearFilterProcessor.h"
#include "../DSP/Filter.h"


using namespace Core;

// constructor
BiquadFilterNode::BiquadFilterNode(Graph* graph) : ProcessorNode(graph, new LinearFilterProcessor())
{
	// biquad filter configuration
	mSettings.mCoefficients.mNumPoles  = 3;
	mSettings.mCoefficients.mPoles.Resize(3);
	mSettings.mCoefficients.mPoles[2] = 1;		// paramter A0

	mSettings.mCoefficients.mNumZeroes = 3;
	mSettings.mCoefficients.mZeroes.Resize(3);
}


// destructor
BiquadFilterNode::~BiquadFilterNode()
{
}


// initialize the node
void BiquadFilterNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	// SETUP PORTS

	GetInputPort(INPUTPORT_CHANNEL).Setup("In", "x", AttributeChannels<double>::TYPE_ID, PORTID_INPUT);
	GetOutputPort(OUTPUTPORT_CHANNEL).Setup("Out", "y", AttributeChannels<double>::TYPE_ID, PORTID_OUTPUT);
	
	// ATTRIBUTES

	// add biquad parameters
	AttributeSettings* attr;
	attr = RegisterAttribute("B0", "b0", "Biquad Coefficient B 0", Core::ATTRIBUTE_INTERFACETYPE_STRING);
	attr->SetDefaultValue( Core::AttributeString::Create("0.0") );

	attr = RegisterAttribute("B1", "b1", "Biquad Coefficient B 1", Core::ATTRIBUTE_INTERFACETYPE_STRING);
	attr->SetDefaultValue( Core::AttributeString::Create("0.0") );

	attr = RegisterAttribute("B2", "b2", "Biquad Coefficient B 2", Core::ATTRIBUTE_INTERFACETYPE_STRING);
	attr->SetDefaultValue( Core::AttributeString::Create("0.0") );

	attr = RegisterAttribute("A1", "a1", "Biquad Coefficient A 1", Core::ATTRIBUTE_INTERFACETYPE_STRING);
	attr->SetDefaultValue( Core::AttributeString::Create("0.0") );

	attr = RegisterAttribute("A2", "a2", "Biquad Coefficient A 2", Core::ATTRIBUTE_INTERFACETYPE_STRING);
	attr->SetDefaultValue( Core::AttributeString::Create("0.0") );

	attr = RegisterAttribute("Gain", "gain", "Biquad Filter Gain", Core::ATTRIBUTE_INTERFACETYPE_STRING);
	attr->SetDefaultValue( Core::AttributeString::Create("1.0") );
}


void BiquadFilterNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


// update the node
void BiquadFilterNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update channel processors
	ProcessorNode::Update(elapsed, delta);

	// check filter stability  (FIXME: this is a temporary solution)
	bool isUnstable = false;
	const uint32 numProcessors = mProcessors.Size();
	for (uint32 i=0; i<numProcessors; ++i)
	{
		Channel<double>* outputChannel = mProcessors[i]->GetOutput()->AsType<double>();
		if ( outputChannel->GetNumSamples() > 0 )
		{
			if ( Math::AbsD(outputChannel->GetLastSample()) > 10E6)
			{
				// output is very large -> we assume the filter has become unstable. This is of course not the correct way (because the input may be large, too), so we assume small inputs.
				SetError(ERROR_UNSTABLE_FILTER, "Filter is unstable");
				isUnstable = true;
				break;
			} 
		}
	}

	if (isUnstable == false)
		ClearError(ERROR_UNSTABLE_FILTER);
}


// attributes have changed
void BiquadFilterNode::OnAttributesChanged()
{
	double b0 = atof(GetStringAttribute(ATTRIB_B0));
	double b1 = atof(GetStringAttribute(ATTRIB_B1));
	double b2 = atof(GetStringAttribute(ATTRIB_B2));
	double a1 = atof(GetStringAttribute(ATTRIB_A1));
	double a2 = atof(GetStringAttribute(ATTRIB_A2));
	double gain = atof(GetStringAttribute(ATTRIB_GAIN));

	// check if attributes have changed
	if (mSettings.mCoefficients.mZeroes[2] == b0 && 
		mSettings.mCoefficients.mZeroes[1] == b1 && 
		mSettings.mCoefficients.mZeroes[0] == b2 && 
		mSettings.mCoefficients.mPoles[1]  == a1 && 
		mSettings.mCoefficients.mPoles[0]  == a2 &&
		mSettings.mGain					   == gain)
	{
		return;
	}
	
	mSettings.mCoefficients.mZeroes[2] = b0;
	mSettings.mCoefficients.mZeroes[1] = b1;
	mSettings.mCoefficients.mZeroes[0] = b2;
	mSettings.mCoefficients.mPoles[1]  = a1;
	mSettings.mCoefficients.mPoles[0]  = a2;
	mSettings.mGain					   = gain;

	ResetAsync();
}
