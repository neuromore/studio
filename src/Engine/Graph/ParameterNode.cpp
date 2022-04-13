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
#include "ParameterNode.h"
#include "../Core/Math.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
ParameterNode::ParameterNode(Graph* graph) : InputNode(graph)
{
	UseChannelMetadataPropagation(false);

	mDefaultValue = 0.0;
	mValues.Add(mDefaultValue);

	mShowControls = false;
	mShowInputs = false;
	mSampleRate = 128;

	mShowWidget = true;
	mEnableWidget = true;

	// color the output channel same as the node
	UseChannelColoring();
}


// destructor
ParameterNode::~ParameterNode()
{
}


// initialize the node
void ParameterNode::Init()
{
	// PORTS

	// register ports
	UpdateInputPorts();
	InitOutputPorts(1);
	GetOutputPort(OUTPUTPORT_VALUE).Setup("Out", "y", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_VALUE);

	// ATTRIBUTES

	// baseclass attributes
	InputNode::Init();

	// description
	AttributeSettings* attributeDescription = RegisterAttribute("Description", "desc", "Description of the parameter. Think of how you can descripe the given parameter to a user that has never used it before.", ATTRIBUTE_INTERFACETYPE_STRING);
	attributeDescription->SetDefaultValue( Core::AttributeString::Create("") );

	// register attributes
	Core::AttributeSettings* staticValue = RegisterAttribute("Value", "value", "The default static value to output.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	staticValue->SetDefaultValue( Core::AttributeFloat::Create(mDefaultValue) );
	staticValue->SetMinValue( Core::AttributeFloat::Create(-DBL_MAX) );
	staticValue->SetMaxValue( Core::AttributeFloat::Create(+DBL_MAX) );

	// sample rate
	Core::AttributeSettings* attributeSampleRate = RegisterAttribute("Sample Rate", "sampleRate", "Sample rate of the output channel.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeSampleRate->SetDefaultValue( Core::AttributeFloat::Create(mSampleRate) );
	attributeSampleRate->SetMinValue( Core::AttributeFloat::Create(0) );
	attributeSampleRate->SetMaxValue( Core::AttributeFloat::Create(DBL_MAX) );

	// number of channels
	Core::AttributeSettings* attributeNumChannels = RegisterAttribute("Number of Channels", "numChannels", "Number of channels in the output multichannel. ", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	attributeNumChannels->SetDefaultValue(Core::AttributeInt32::Create(1));
	attributeNumChannels->SetMinValue(Core::AttributeInt32::Create(1));
	attributeNumChannels->SetMaxValue(Core::AttributeInt32::Create(CORE_INT32_MAX));
	
	// show controls								// NOTE: write translator tp rename this parameter; this is legacy stuff ('isRanged' is now 'showControls')
	AttributeSettings* attributeShowControls = RegisterAttribute("Show Controls", "isRanged", "Show a control widget for the parameter in the Parameter Plugin.", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeShowControls->SetDefaultValue(AttributeBool::Create(mShowControls));

	// show inputs
	AttributeSettings* attributeShowInputs = RegisterAttribute("Show Inputs", "showInputs", "Show inputs to control the parameter and it's behavior.", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeShowInputs->SetDefaultValue(AttributeBool::Create(mShowInputs));
	attributeShowInputs->SetVisible(mShowControls);

	// control type
	AttributeSettings* attributeControlType = RegisterAttribute("Control Type", "controlType", "The type of input control used in the Parameter Plugin.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attributeControlType->ResizeComboValues(NUM_CONTROLTYPES);
	attributeControlType->SetComboValue(CONTROLTYPE_SEPARATOR , "Separator (Dummy)");
	attributeControlType->SetComboValue(CONTROLTYPE_SLIDER, "Value Slider");
	attributeControlType->SetComboValue(CONTROLTYPE_CHECKBOX, "Checkboxes");
	attributeControlType->SetComboValue(CONTROLTYPE_DROPDOWN , "Dropdown Menu");
	attributeControlType->SetComboValue(CONTROLTYPE_SELECTION_BUTTONS , "Selection Buttons");
	attributeControlType->SetComboValue(CONTROLTYPE_EVENT_BUTTONS , "Event Buttons");
	attributeControlType->SetDefaultValue( AttributeInt32::Create(CONTROLTYPE_SLIDER) );
	attributeControlType->SetVisible(mShowControls);

	// range min attribute
	AttributeSettings* attributeRangeMin = RegisterAttribute("Range Min", "rangeMin", "Minimum possible feedback value. Values smaller than this will be clamped", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeRangeMin->SetDefaultValue( AttributeFloat::Create(0.0) );
	attributeRangeMin->SetMinValue( AttributeFloat::Create(-FLT_MAX) );
	attributeRangeMin->SetMaxValue( AttributeFloat::Create(+FLT_MAX) );
	attributeRangeMin->SetVisible(mShowControls);

	// range max attribute
	AttributeSettings* attributeRangeMax = RegisterAttribute("Range Max", "rangeMax", "Maximum possible feedback value. Values bigger than this will be clamped", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeRangeMax->SetDefaultValue( AttributeFloat::Create(1.0) );
	attributeRangeMax->SetMinValue( AttributeFloat::Create(-FLT_MAX) );
	attributeRangeMax->SetMaxValue( AttributeFloat::Create(+FLT_MAX) );
	attributeRangeMax->SetVisible(mShowControls);

	// option list
	AttributeSettings* attributeOptions = RegisterAttribute("Options", "options", "List of options. First option maps to value 0, second to value 1 etc.", ATTRIBUTE_INTERFACETYPE_STRINGARRAY);
	attributeOptions->SetDefaultValue( Core::AttributeStringArray::Create("OFF,ON") );
	attributeOptions->SetVisible(mShowControls);
}


void ParameterNode::Reset()
{
	InputNode::Reset();

	// use default value
	const uint32 numValues = mValues.Size();
	for (uint32 i = 0; i < numValues; ++i)
		mValues[i] = mDefaultValue;

	mClock.Stop();
	mClock.Reset();
}


void ParameterNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	InputNode::ReInit(elapsed, delta);

	UpdateInputPorts();

	PostReInit(elapsed, delta);
}


void ParameterNode::Start(const Time& elapsed)
{
	// create sensors before start 
	UpdateSensors();

	// call baseclass start
	InputNode::Start(elapsed);

	UpdateNames();
	
	// configure clock and start it at current elapsed time
	mClock.Reset();
	mClock.SetFrequency(mSampleRate);
	mClock.SetStartTime(elapsed);
	mClock.Start();
}


// update the node
void ParameterNode::Update(const Time& elapsed, const Time& delta)
{	
	// shared base update helper
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// process the inputs
	if (GetNumInputPorts() == NUM_INPUTPORTS)
	{
		// show/hide widget?
		MultiChannel* showChannels = GetInputPort(INPUTPORT_SHOWCONTROL).GetChannels();
		if (showChannels != NULL && showChannels->GetNumChannels() > 0 && showChannels->GetChannel(0)->GetNumSamples() > 0)
		{
			const bool newValue = (showChannels->GetChannel(0)->AsType<double>()->GetLastSample() >= 1.0);
			if (mShowWidget != newValue)
			{
				mShowWidget = newValue;
				EMIT_EVENT( OnGraphModified(mParentGraph, this) );
			}

		}
		else // default state
		{
			mShowWidget = true;
		}

		// enable widget?	
		MultiChannel* enableChannels = GetInputPort(INPUTPORT_ENABLECONTROL).GetChannels();
		if (enableChannels != NULL && enableChannels->GetNumChannels() > 0 && enableChannels->GetChannel(0)->GetNumSamples() > 0)
		{
			const bool newValue = (enableChannels->GetChannel(0)->AsType<double>()->GetLastSample() >= 1.0);
			if (mEnableWidget != newValue)
			{
				mEnableWidget = newValue;
				EMIT_EVENT( OnGraphModified(mParentGraph, this) );
			}
		}
		else // default state
		{ 
			mEnableWidget = true;
		}
	}
	else // default state
	{
		mShowWidget = true;
		mEnableWidget = true;
	}

	// create output samples
	mClock.Update(elapsed, delta);
	InputNode::Update(elapsed, delta);
}


// update the data
void ParameterNode::OnAttributesChanged()
{
	// check if something has changed
	const double defaultValue = GetFloatAttribute(ATTRIB_DEFAULTVALUE);
	const double sampleRate = GetFloatAttribute(ATTRIB_SAMPLERATE);
	const uint32 numChannels = GetInt32Attribute(ATTRIB_NUMCHANNELS);
	const bool showInputs = GetBoolAttribute(ATTRIB_SHOWINPUTS);
	const bool showControls = GetBoolAttribute(ATTRIB_SHOWCONTROLS);
	const EControlType controlType = (EControlType)GetInt32Attribute(ATTRIB_CONTROLTYPE);
	const Array<String> options = GetStringArrayAttribute(ATTRIB_OPTIONS, Array<String>());

	// check if need to reinitialize the node
	if (GetNumSensors() != numChannels ||
		mSampleRate != sampleRate ||
		numChannels != GetNumSensors())
	{
		ResetAsync();

		mSampleRate = sampleRate;
	}

	// update values if default value was changed
	if (mDefaultValue != defaultValue || 
		mValues.Size() != numChannels)
	{
		mDefaultValue = defaultValue;
		mValues.Resize(numChannels);
		for (uint32 i = 0; i < numChannels; ++i)
			mValues[i] = mDefaultValue;

		// update name
		UpdateNames();
	}


	// show/hide controls depending on type
	mShowControls = showControls;
	if (GetAttributeSettings(ATTRIB_CONTROLTYPE)->IsVisible() != mShowControls)
	{
		GetAttributeSettings(ATTRIB_CONTROLTYPE)->SetVisible( mShowControls );
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_CONTROLTYPE)) );
	}

	if (GetAttributeSettings(ATTRIB_SHOWINPUTS)->IsVisible() != mShowControls)
	{
		GetAttributeSettings(ATTRIB_SHOWINPUTS)->SetVisible( mShowControls );
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_SHOWINPUTS)) );
	}

	// show/hide input ports
	mShowInputs = showInputs;

	// Create input ports (only while loading)  // NOTE: we are not allowed to delete graph objects within this method
	if (GetNumInputPorts() == 0) 
		UpdateInputPorts();

	// Note: emit events only if visibility has changed; we don't keep separate flags for this, 
	
	const bool separatorMode = (controlType == CONTROLTYPE_SEPARATOR && mShowControls == true);
	const bool hasValues = !separatorMode;
	if (GetAttributeSettings(ATTRIB_DEFAULTVALUE)->IsVisible() != hasValues) 
	{
		GetAttributeSettings(ATTRIB_DEFAULTVALUE)->SetVisible( hasValues );
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_DEFAULTVALUE)) );
	}

	if (GetAttributeSettings(ATTRIB_SAMPLERATE)->IsVisible() != hasValues)
	{
		GetAttributeSettings(ATTRIB_SAMPLERATE)->SetVisible( hasValues );
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_SAMPLERATE)) );
	}

	if (GetAttributeSettings(ATTRIB_NUMCHANNELS)->IsVisible() != hasValues)
	{
		GetAttributeSettings(ATTRIB_NUMCHANNELS)->SetVisible( hasValues );
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_NUMCHANNELS)) );
	}

	const bool floatMode = (controlType == CONTROLTYPE_SLIDER);
	const bool showRange = mShowControls && floatMode;
	if (GetAttributeSettings(ATTRIB_RANGEMIN)->IsVisible() != showRange || GetAttributeSettings(ATTRIB_RANGEMAX)->IsVisible() != showRange)
	{
		GetAttributeSettings(ATTRIB_RANGEMIN)->SetVisible( showRange );
		GetAttributeSettings(ATTRIB_RANGEMAX)->SetVisible( showRange );
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_RANGEMIN)) );
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_RANGEMAX)) );
	}

	const bool listMode = (controlType == CONTROLTYPE_CHECKBOX ||controlType == CONTROLTYPE_DROPDOWN || controlType == CONTROLTYPE_SELECTION_BUTTONS || controlType == CONTROLTYPE_EVENT_BUTTONS);
	const bool showOptions = mShowControls && listMode;
	if (GetAttributeSettings(ATTRIB_OPTIONS)->IsVisible() != showOptions)
	{
		GetAttributeSettings(ATTRIB_OPTIONS)->SetVisible( showOptions );
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_OPTIONS)) );
	}
}


// the function that fills the sensors with samples
void ParameterNode::GenerateSamples()
{
	const uint32 numNewSamples = mClock.GetNumNewTicks();			// number of samples in this interval
	mClock.ClearNewTicks();

	// add the samples
	const uint32 numSensors = mSensors.Size();
	for (uint32 s = 0; s < numSensors; ++s)
	{
		const Channel<double>* channel = GetSensor(s)->GetChannel();

		// get current value
		double currentValue = mValues[s];

		for (uint32 i = 0; i < numNewSamples; ++i)
		{
			// Special Mode 1: if control type is 'event' style (e.g. event buttons), we output only a single sample of the value and then reset it back to 0.0
			if (GetControlType() == CONTROLTYPE_EVENT_BUTTONS && currentValue != 0.0 && channel->GetNumSamples() > 0)
			{
				// check if we have outputted at least one sample of that value
				if ( channel->GetLastSample() == currentValue)
				{
					mValues[s] = 0.0;
					currentValue = 0.0;
				}
			}

			mSensors[s].AddQueuedSample(currentValue);
		}

		// Special Mode 2: if clock is not running (samplerate = 0), we output a sample only if the parameter was changed
		if (channel->GetSampleRate() == 0)
		{
			if (channel->GetNumSamples() == 0)
				mSensors[s].AddQueuedSample(currentValue);
			else if (channel->GetLastSample() != currentValue)
				mSensors[s].AddQueuedSample(currentValue);
		}
	}
}


void ParameterNode::UpdateSensors()
{
	const uint32 numChannels = GetInt32Attribute(ATTRIB_NUMCHANNELS);
	const uint32 sampleRate = GetFloatAttribute(ATTRIB_SAMPLERATE);

	mValues.Resize(numChannels);
	mSensors.Resize(numChannels);
	GetOutputPort(OUTPUTPORT_VALUE).GetChannels()->Clear();

	for (uint32 i = 0; i<numChannels; ++i)
	{
		mValues[i] = GetFloatAttribute(ATTRIB_DEFAULTVALUE);

		mSensors[i].Reset();
		mSensors[i].GetChannel()->SetBufferSize(10);
		mSensors[i].SetDriftCorrectionEnabled(false);
		mSensors[i].GetChannel()->SetSampleRate(sampleRate);
		mTempString.Format("%.2f", mValues[i]);
		mSensors[i].GetChannel()->SetName(mTempString.AsChar());
		GetOutputPort(OUTPUTPORT_VALUE).GetChannels()->AddChannel(mSensors[i].GetChannel());
	}
}


void ParameterNode::SetValue(double value, uint32 channelIndex)
{
	CORE_ASSERT(channelIndex < mValues.Size());

	if (channelIndex >= mValues.Size())
		return;

	mValues[channelIndex] = value;
	UpdateNames();
}


double ParameterNode::GetValue(uint32 channelIndex) const
{
	CORE_ASSERT(channelIndex < mValues.Size());

	if (channelIndex >= mValues.Size())
		return 0.0;

	return mValues[channelIndex];
}


void ParameterNode::SetValue(double value)
{
	const uint32 numChannels = mValues.Size();
	for (uint32 i=0; i<numChannels; ++i)
		mValues[i] = value;

	UpdateNames();
}


void ParameterNode::UpdateNames()
{
	// use parameter name as channel name

	const uint32 numSensors = mSensors.Size();
	for (uint32 i = 0; i < numSensors; ++i)
		mSensors[i].SetName(GetName());

	// use parameter value as port name
	if (numSensors <= 1)
		mTempString.Format("%.2f", mValues[0]);
	else
		mTempString.Format("%.2f, ..", mValues[0]);

	GetOutputPort(OUTPUTPORT_VALUE).SetName(mTempString.AsChar());
}


void ParameterNode::UpdateInputPorts()
{
	const uint32 numInputPorts = GetNumInputPorts();
	if (mShowInputs == true && mShowControls == true)
	{
		// create, if necessary
		if (numInputPorts == 0)
		{
			InitInputPorts(NUM_INPUTPORTS);
			GetInputPort(INPUTPORT_ENABLECONTROL).Setup("Enable", "enable", AttributeChannels<double>::TYPE_ID, INPUTPORT_ENABLECONTROL);
			GetInputPort(INPUTPORT_SHOWCONTROL).Setup("Show", "show", AttributeChannels<double>::TYPE_ID, INPUTPORT_SHOWCONTROL);
		}
	}
	else
	{
		// remove ports again
		InitInputPorts(0);
	}
}
