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
#include "LinearFilterNode.h"
#include "../Core/Math.h"
#include "../Core/EventManager.h"
#include "../EngineManager.h"
#include "../DSP/LinearFilterProcessor.h"
#include "../DSP/Filter.h"


using namespace Core;

// constructor
LinearFilterNode::LinearFilterNode(Graph* graph) : ProcessorNode(graph, new LinearFilterProcessor())
{
}


// destructor
LinearFilterNode::~LinearFilterNode()
{
}


// initialize the node
void LinearFilterNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	// CONFIG SPNODE
	RequireConstantSampleRate();

	// SETUP PORTS

	GetInputPort(INPUTPORT_CHANNEL).Setup("In", "x", AttributeChannels<double>::TYPE_ID, PORTID_INPUT);
	GetOutputPort(OUTPUTPORT_CHANNEL).Setup("Out", "y", AttributeChannels<double>::TYPE_ID, PORTID_OUTPUT);
	
	// ATTRIBUTES

	// initialize with default values from settings

	// Filter type (low/high/bandpass/bandstop)
	const uint32 defaultFilterType = (uint32)mSettings.mFilterType;
	Core::AttributeSettings* attr = RegisterAttribute("Filter Type", "FilterType", "The Type of the filter.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attr->ResizeComboValues( Filter::NUM_FILTERTYPES );
	for (uint32 i=0; i<Filter::NUM_FILTERTYPES; i++)
		attr->SetComboValue( i, Filter::GetFilterTypeName((Filter::EFilterType)i) );
	attr->SetDefaultValue( Core::AttributeInt32::Create(defaultFilterType) );
	
	// Filter method (Butterworth, Chebyshev etc)
	const uint32 defaultFilterMethod = (uint32)mSettings.mFilterMethod;
	attr = RegisterAttribute("Filter Method", "FilterMethod", "The Method that should be used to construct the filter.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attr->ResizeComboValues( Filter::NUM_FILTERMETHODS );
	for (uint32 i = 0; i < Filter::NUM_FILTERMETHODS; i++)
		attr->SetComboValue( i, Filter::GetFilterMethodName((Filter::EFilterMethod)i) );
	attr->SetDefaultValue( Core::AttributeInt32::Create( defaultFilterMethod ) );
	
	// Filter order
	const uint32 defaultFilterOrder = mSettings.mFilterOrder;
	attr = RegisterAttribute("Filter Order", "FilterOrder", "The Order of the Filter.", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	attr->SetDefaultValue( Core::AttributeInt32::Create(defaultFilterOrder) );
	attr->SetMinValue( Core::AttributeInt32::Create(1) );
	attr->SetMaxValue( Core::AttributeInt32::Create(10) );

	// Cascade Length
	const uint32 defaultCascadeLength = mSettings.mCascadeLength;
	attr = RegisterAttribute("Cascade Length", "CascadeLength", "Number of chained filters in the cascade.", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	attr->SetDefaultValue( Core::AttributeInt32::Create(defaultCascadeLength) );
	attr->SetMinValue( Core::AttributeInt32::Create(1) );
	attr->SetMaxValue( Core::AttributeInt32::Create(50) );
	attr->SetIsEnabled(false);	// Disabled for now
	attr->SetVisible(false);	// Disabled for now

	// Low Cut Frequency
	const uint32 defaultLowCutFreq = mSettings.mLowCutFrequency;
	attr = RegisterAttribute("Low Cut Frequency", "LowCutFrequency", "The corner frequency of the lowpass / lower cut frequency of bandpass/bandstop", Core::ATTRIBUTE_INTERFACETYPE_FLOATSLIDER);
	attr->SetDefaultValue( Core::AttributeFloat::Create(defaultLowCutFreq) );
	attr->SetMinValue( Core::AttributeFloat::Create(0) );
	attr->SetMaxValue( Core::AttributeFloat::Create(100) );
	
	// High Cut Frequency
	const uint32 defaultHighCutFreq = mSettings.mHighCutFrequency;
	attr = RegisterAttribute("High Cut Frequency", "HighCutFrequency", "The corner frequency of the highpass / upper cut frequency of bandpass/bandstop", Core::ATTRIBUTE_INTERFACETYPE_FLOATSLIDER);
	attr->SetDefaultValue( Core::AttributeFloat::Create(defaultHighCutFreq) );
	attr->SetMinValue( Core::AttributeFloat::Create(0) );
	attr->SetMaxValue( Core::AttributeFloat::Create(100) );

	// show/hide the correct attributes by default
	ShowAttributesForFilterType(mSettings.mFilterType);
	
	// TODO chebychev ripple and more parameters, w/ automatic disabling/enabling depending on selected filter method
}


void LinearFilterNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	// FIXME: for now, we only allow butterworth
	if (mSettings.mFilterMethod != Filter::BUTTERWORTH)
		mIsInitialized = false;

	PostReInit(elapsed, delta);

	// FIXME why is this so ugly
	// HACK: set node error after reset ;)
	if (mSettings.mFilterMethod != Filter::BUTTERWORTH)
	{
		mTempString.Format("%s filter is not implemented", Filter::GetFilterMethodName(mSettings.mFilterMethod));
		SetWarning(WARNING_FILTER_NOT_IMPLEMENTED, mTempString.AsChar());
	}
}


// update the node
void LinearFilterNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update baseclass
	ProcessorNode::Update(elapsed, delta);

	// check filter stability  (FIXME: this is a temporary solution)
	bool isUnstable = false;
	const uint32 numProcessors = mProcessors.Size();
	for (uint32 i = 0; i < numProcessors; ++i)
	{
		Channel<double>* outputChannel = mProcessors[i]->GetOutput()->AsType<double>();
		if (outputChannel->GetNumSamples() > 0)
		{
			const double value = outputChannel->GetLastSample();
			if (Math::AbsD(value) > 10E6 || Math::IsValidNumberD(value) == false)
			{
				// output is very large or invalid -> we assume the filter has become unstable. This is of course not the correct way (because the input may be large, too), so we assume small inputs.
				SetError(ERROR_UNSTABLE_FILTER, "Filter is unstable.");
				isUnstable = true;
				break;
			}
		}
	}

	if (isUnstable == false)
		ClearError(ERROR_UNSTABLE_FILTER);
}


// attributes have changed
void LinearFilterNode::OnAttributesChanged()
{
	const Filter::EFilterType	filterType		= (Filter::EFilterType)GetInt32Attribute(ATTRIB_FILTERTYPE);
	const Filter::EFilterMethod filterMethod	= (Filter::EFilterMethod)GetInt32Attribute(ATTRIB_FILTERMETHOD);
	const uint32				filterOrder		= GetInt32Attribute(ATTRIB_FILTERORDER);
	const uint32				cascadeLength	= GetInt32Attribute(ATTRIB_CASCADELENGTH);
	const double				lowCutFreq		= GetFloatAttribute(ATTRIB_LOWCUTFREQ);
	const double				highCutFreq		= GetFloatAttribute(ATTRIB_HIGHCUTFREQ);

	// check if attributes have changed
	if (mSettings.mFilterType == filterType			&& 
		mSettings.mFilterMethod == filterMethod		&&
		mSettings.mFilterOrder == filterOrder		&&
		mSettings.mCascadeLength == cascadeLength	&&
		mSettings.mLowCutFrequency == lowCutFreq	&&
		mSettings.mHighCutFrequency == highCutFreq)
	{
		// nothing to do
		return;
	}

	// update settings
	mSettings.mFilterType = filterType;
	mSettings.mFilterMethod = filterMethod;
	mSettings.mCoefficients.Clear();
	mSettings.mFilterOrder = filterOrder;
	mSettings.mCascadeLength = cascadeLength;
	mSettings.mLowCutFrequency = lowCutFreq;
	mSettings.mHighCutFrequency = highCutFreq;
	
	// show/hide unused attributes
	ShowAttributesForFilterType(filterType);

	// assert that low < high
	const double minFreqSpacing = 10E-10;
	const bool showBothFreqs = (mSettings.mFilterType == Filter::BANDPASS || mSettings.mFilterType == Filter::BANDSTOP);

	if (showBothFreqs == true)
		if (mSettings.mLowCutFrequency >= mSettings.mHighCutFrequency - minFreqSpacing)
		{
			mSettings.mLowCutFrequency = mSettings.mHighCutFrequency - minFreqSpacing;
	
			// corner case: minfreq was adjusted to a negative value
			if (mSettings.mLowCutFrequency < 0)
			{
				mSettings.mLowCutFrequency = 0.0;
				mSettings.mHighCutFrequency = minFreqSpacing;
			}

			// update attributes
			SetFloatAttribute("LowCutFrequency", mSettings.mLowCutFrequency);
			SetFloatAttribute("HighCutFrequency", mSettings.mHighCutFrequency);

			// fire events
			EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_LOWCUTFREQ)) );
			EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_HIGHCUTFREQ)) );
		}
	

	// update settings and recalculate filters
	ResetAsync();
}


void LinearFilterNode::ShowAttributesForFilterType(Filter::EFilterType type)
{
	// show low frequency slider?
	const bool showLowFreq = (type == Filter::LOWPASS || type == Filter::BANDPASS || type == Filter::BANDSTOP);
	GetAttributeSettings(ATTRIB_LOWCUTFREQ)->SetVisible(showLowFreq);

	// show high frequency slider?
	const bool showHighFreq = (type == Filter::HIGHPASS || type == Filter::BANDPASS || type == Filter::BANDSTOP);
	GetAttributeSettings(ATTRIB_HIGHCUTFREQ)->SetVisible(showHighFreq);
}
