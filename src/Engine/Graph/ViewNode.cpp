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
#include "ViewNode.h"
#include "../Core/Math.h"
#include "../DSP/Spectrum.h"
#include "../EngineManager.h"

using namespace Core;

// constructor
ViewNode::ViewNode(Graph* graph) : SPNode(graph)
{
	// max view duration
	mViewDuration = VIEWDURATION;
	mMaxViewDuration = VIEWDURATIONMAX; // same as in plugin
}


// destructor
ViewNode::~ViewNode()
{
}


// initialize the node
void ViewNode::Init()
{
	// init base class first
	SPNode::Init();

	// CONFIG SPNODE
	RequireSyncedInput();

	// SETUP PORTS
	
	// setup the input ports
	InitInputPorts(4);
	GetInputPort(INPUTPORT_DOUBLE).Setup("In", "x1", AttributeChannels<double>::TYPE_ID, INPUTPORT_DOUBLE);
	GetInputPort(INPUTPORT_SPECTRUM).Setup("In (Spectrum)", "x2", AttributeChannels<Spectrum>::TYPE_ID, INPUTPORT_SPECTRUM);
	GetInputPort(INPUTPORT_ENABLE).Setup("Enable", "enable", AttributeChannels<double>::TYPE_ID, INPUTPORT_ENABLE);
	GetInputPort(INPUTPORT_LENGTH).Setup("Length in mins", "length", AttributeChannels<double>::TYPE_ID, INPUTPORT_LENGTH);

	// ATTRIBUTES

	// autoscaling mode
	const uint32 defaultScaling = SCALING_PEAK;
	AttributeSettings* attributeAutoscaling = RegisterAttribute("Scaling", "scaling", "How the waveform should be scaled in the view.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attributeAutoscaling->AddComboValue("Auto (Peak)");
	attributeAutoscaling->AddComboValue("Auto (StdDev)");
	attributeAutoscaling->AddComboValue("Custom Range");
	attributeAutoscaling->SetDefaultValue(AttributeInt32::Create(SCALING_PEAK));

	// custom range
	AttributeSettings* attributeRangeMin = RegisterAttribute("Range Min", "rangeMin", "Lower value of the displayed value range.", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeRangeMin->SetDefaultValue(AttributeFloat::Create(0));
	attributeRangeMin->SetMinValue(AttributeFloat::Create(-FLT_MAX));
	attributeRangeMin->SetMaxValue(AttributeFloat::Create(FLT_MAX));
	attributeRangeMin->SetVisible(defaultScaling == SCALING_CUSTOM);
	AttributeSettings* attributeRangeMax = RegisterAttribute("Range Max", "rangeMax", "Upper value of the displayed value range.", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeRangeMax->SetDefaultValue(AttributeFloat::Create(1));
	attributeRangeMax->SetMinValue(AttributeFloat::Create(-FLT_MAX));
	attributeRangeMax->SetMaxValue(AttributeFloat::Create(FLT_MAX));
	attributeRangeMax->SetVisible(defaultScaling == SCALING_CUSTOM);

	// custom color
	const bool defaultUseColor = false;
	AttributeSettings* attributeUseColor = RegisterAttribute("Custom Color", "useCustomColor", "Use a user defined color for signal display.", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeUseColor->SetDefaultValue(AttributeBool::Create(defaultUseColor));
	AttributeSettings* attributeColorPick = RegisterAttribute("Color", "customColor", "The color used for signal display.", ATTRIBUTE_INTERFACETYPE_COLOR);
	attributeColorPick->SetDefaultValue( AttributeColor::Create(Color(0, 159.0f / 255.0f, 227.0f / 255.0f)) );	// neuromore color
	attributeColorPick->SetVisible(defaultUseColor == true);

	// combined view
	const bool defaultCombinedView = false;
	AttributeSettings* attributeCombinedView = RegisterAttribute("Combined View", "combineView", "Display all input channels in a single chart.", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeCombinedView->SetDefaultValue( AttributeBool::Create(defaultCombinedView) );

	AttributeSettings* fixedLength = RegisterAttribute( "Fixed Length in mins", "fixedLength", "Fix length in Seconds.", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	fixedLength->SetDefaultValue( AttributeBool::Create(false) );

	// sync buffer size to initial value
	SyncBufferSize(true);
}


void ViewNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	SPNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


// update the node
void ViewNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update the baseclass
	SPNode::Update(elapsed, delta);

	// get the enable value
	InputPort& enablePort = GetInputPort(INPUTPORT_ENABLE);
	if (enablePort.HasConnection() == false)
	{
		mEnableValue = true;
	}
	else
	{
		if (enablePort.GetChannels()->GetNumChannels() > 0 && enablePort.GetChannels()->GetChannel(0)->IsEmpty() == false)
		{
			const double sampleValue = enablePort.GetChannels()->GetChannel(0)->AsType<double>()->GetLastSample();
			mEnableValue = (sampleValue >= 1);
		}
		else
		{
			mEnableValue = false;
		}
	}

	auto fixedLengthCache = mFixedLengthDuration;
	InputPort& lengthPort = GetInputPort(INPUTPORT_LENGTH);
	if (lengthPort.HasConnection() == false)
	{
		GetAttributeSettings(ATTRIB_LENGTH)->SetIsEnabled(true);
		mFixedLengthDuration = -1.;
	}
	else
	{
		if (lengthPort.GetChannels()->GetNumChannels() > 0 && lengthPort.GetChannels()->GetChannel(0)->IsEmpty() == false)
		{
			GetAttributeSettings(ATTRIB_LENGTH)->SetIsEnabled(false);
			const double sampleValue = lengthPort.GetChannels()->GetChannel(0)->AsType<double>()->GetLastSample();
			if (sampleValue > 0.)
				mFixedLengthDuration = sampleValue;
		}
		else
		{
			mFixedLengthDuration = -1.;
		}
	}

	// If the duration is changed, update buffer size
	if (abs(fixedLengthCache - mFixedLengthDuration) > 0.0001)
		SyncBufferSize(false);

	// update scaling range (i.e. the auto modes need to recalculate the range)
	CalculateScalingRange();

	// apply scaling range to the channels for the view plugin

	const uint32 numValues = GetNumDoubleChannels();
	if (numValues > 0)
	{
		if (CombinedView() == true)
		{
			// find the range
			double minRange = DBL_MAX;
			double maxRange = -DBL_MAX;
			for (uint32 i=0; i<numValues; ++i)
			{
				minRange = Min( minRange, mRangesMin[i] );
				maxRange = Max( maxRange, mRangesMax[i] );
			}

			// apply the range
			for (uint32 i=0; i<numValues; ++i)
			{
				GetDoubleChannel(i)->SetMinValue(minRange);
				GetDoubleChannel(i)->SetMaxValue(maxRange);
			}
		}
		else
		{
			// apply the range
			for (uint32 i=0; i<numValues; ++i)
			{
				GetDoubleChannel(i)->SetMinValue(mRangesMin[i]);
				GetDoubleChannel(i)->SetMaxValue(mRangesMax[i]);
			}
		}
	}
}


void ViewNode::OnAttributesChanged()
{
	// show/hide range attributes, depending on selected scaling mode
	const EScalingMode scalingMode = GetScalingMode();
	const bool useCustomRange = (scalingMode == SCALING_CUSTOM);
	GetAttributeSettings(ATTRIB_RANGEMIN)->SetVisible(useCustomRange);
	GetAttributeSettings(ATTRIB_RANGEMAX)->SetVisible(useCustomRange);

	// show/hide color picker
	GetAttributeSettings(ATTRIB_COLORPICK)->SetVisible( CustomColor() );

	InputPort &lengthPort = GetInputPort(INPUTPORT_LENGTH);
	if (FixedLength())
		lengthPort.SetVisible(true);
	else
		lengthPort.SetVisible(false);

	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_COLORPICK)) );
}


uint32 ViewNode::GetNumDoubleChannels()
{
	Port& port = GetInputPort(INPUTPORT_DOUBLE);
	if (port.HasConnection() == false || port.GetChannels() == NULL)
		return 0;

	MultiChannel* channels = port.GetChannels();
	return channels->GetNumChannels();
}


Channel<double>* ViewNode::GetDoubleChannel(uint32 index)
{
	Port& port = GetInputPort(INPUTPORT_DOUBLE);
	return port.GetChannels()->GetChannel(index)->AsType<double>();
}


// get reference to the multichannel on first input
MultiChannel* ViewNode::GetDoubleChannels()
{
	Port& port = GetInputPort(INPUTPORT_DOUBLE);
	MultiChannel* channels = port.GetChannels();
	return channels;
}


uint32 ViewNode::GetNumSpectrumChannels()
{
	Port& port = GetInputPort(INPUTPORT_SPECTRUM);
	if (port.HasConnection() == false || port.GetChannels() == NULL)
		return 0;

	MultiChannel* channels = port.GetChannels();

	return channels->GetNumChannels();
}


Channel<Spectrum>* ViewNode::GetSpectrumChannel(uint32 index)
{
	Port& port = GetInputPort(INPUTPORT_SPECTRUM);
	return port.GetChannels()->GetChannel(index)->AsType<Spectrum>();
}


// get reference to the multichannel on spectrum input
MultiChannel* ViewNode::GetSpectrumChannels()
{
	Port& port = GetInputPort(INPUTPORT_SPECTRUM);
	MultiChannel* channels = port.GetChannels();
	return channels;
}


uint32 ViewNode::GetNumEpochSamples(uint32 inputPortIndex) const
{
	const Port& port = mInputPorts[inputPortIndex];

	if (port.HasConnection() == false)
		return 0;

	if (port.GetChannels() == NULL)
		return 0;

	if (inputPortIndex == INPUTPORT_DOUBLE)
	{
		const double sampleRate = port.GetChannels()->GetSampleRate();
		const uint32 numSamples = mMaxViewDuration * sampleRate;
		return numSamples;
	}
	
	return 1;
}


// scaling (only for doubles right now)
void ViewNode::CalculateScalingRange()
{
	const uint32 numChannels = GetNumDoubleChannels();
	mRangesMin.Resize(numChannels);
	mRangesMax.Resize(numChannels);

	const EScalingMode mode = GetScalingMode();
	switch (mode)
	{
		case SCALING_CUSTOM:
			for (uint32 i = 0; i < numChannels; ++i)
			{
				mRangesMin[i] = GetFloatAttribute(ATTRIB_RANGEMIN);
				mRangesMax[i] = GetFloatAttribute(ATTRIB_RANGEMAX);
			}
			break;

		case SCALING_PEAK:

			// calculate peak-to-peak range over displayed timerange
			for (uint32 i = 0; i < numChannels; ++i)
			{
				Channel<double>* channel = GetDoubleChannel(i);
				const uint32 numSamples = GetViewDuration() * channel->GetSampleRate();
				Epoch epoch(channel, numSamples);
				epoch.SetPositionByOffset(0);

				// peak-to-peak range
				mRangesMin[i] = epoch.Min();
				mRangesMax[i] = epoch.Max();
			}
			break; 

		case SCALING_STDDEV:

			// calculate peak-to-peak range over displayed timerange
			for (uint32 i = 0; i < numChannels; ++i)
			{
				Channel<double>* channel = GetDoubleChannel(i);
				const uint32 numSamples = GetViewDuration() * channel->GetSampleRate();
				Epoch epoch(channel, numSamples);
				epoch.SetPositionByOffset(0);

				// RMS-based range
				const double stdDev = epoch.StdDev();
				const double avg = epoch.Mean();
				mRangesMin[i] = avg - (0.5 * stdDev);
				mRangesMax[i] = avg + (0.5 * stdDev);
			}
			break;

		default:
			for (uint32 i = 0; i < numChannels; ++i)
			{
				mRangesMin[i] = 0.0;
				mRangesMax[i] = 1.0;
			}
	}
}

void ViewNode::SyncBufferSize(bool discard)
{
	InputPort& doublePort = GetInputPort(INPUTPORT_DOUBLE);
	InputPort& specPort   = GetInputPort(INPUTPORT_SPECTRUM);
	InputPort& enablePort = GetInputPort(INPUTPORT_ENABLE);
	InputPort& lengthPort = GetInputPort(INPUTPORT_LENGTH);
	MultiChannel* channels;

	auto duration = GetViewDuration();
	if (doublePort.HasConnection() && (channels = doublePort.GetChannels()))
		channels->SetBufferSizeInSeconds(duration, discard);

	if (specPort.HasConnection() && (channels = specPort.GetChannels()))
		channels->SetBufferSizeInSeconds(duration, discard);

	if (enablePort.HasConnection() && (channels = enablePort.GetChannels()))
		channels->SetBufferSizeInSeconds(duration, discard);
}
