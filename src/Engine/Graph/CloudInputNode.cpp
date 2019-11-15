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
#include "CloudInputNode.h"
#include "../Core/Math.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
CloudInputNode::CloudInputNode(Graph* graph) : InputNode(graph)
{
	// default config
	mSampleRate = 128;

	// configure sensor
	mSensor.SetName("Cloud Input");
	mSensor.GetChannel()->SetSampleRate(128);
	mSensor.GetChannel()->SetBufferSize(10);
	mSensor.SetDriftCorrectionEnabled(false);

	// color the output channel same as the node
	UseChannelColoring();

	// configure value array 
	mInputSamples.SetBufferSize(0);
}


// destructor
CloudInputNode::~CloudInputNode()
{
}


// initialize the node
void CloudInputNode::Init()
{
	// PORTS

	// register ports
	InitOutputPorts(1);
	GetOutputPort(OUTPUTPORT_VALUE).Setup("Out", "y", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_VALUE);
	GetOutputPort(OUTPUTPORT_VALUE).GetChannels()->AddChannel(mSensor.GetChannel());

	// ATTRIBUTES

	// baseclass attributes
	InputNode::Init();

	// default value
	Core::AttributeSettings* attribDefaultValue = RegisterAttribute("Default Value", "DefaultNumberValue", "Default value used in case this parameter does not exist on the server.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attribDefaultValue->SetDefaultValue(Core::AttributeFloat::Create(0.0f));
	attribDefaultValue->SetMinValue(Core::AttributeFloat::Create(-DBL_MAX));
	attribDefaultValue->SetMaxValue(Core::AttributeFloat::Create(+DBL_MAX));

	// sample rate
	Core::AttributeSettings* attribSamplerate = RegisterAttribute("Sample Rate", "SampleRate", "Sample rate of the output channel.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attribSamplerate->SetDefaultValue(Core::AttributeFloat::Create(128));
	attribSamplerate->SetMinValue(Core::AttributeFloat::Create(0));
	attribSamplerate->SetMaxValue(Core::AttributeFloat::Create(DBL_MAX));

	// storage type
	Core::AttributeSettings* attribStorageType = RegisterAttribute("Type", "StorageType", "Parameter type.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attribStorageType->ResizeComboValues(2);
	attribStorageType->SetComboValue(0, "User Parameter");
	attribStorageType->SetComboValue(1, "Classifier Parameter");
	attribStorageType->SetDefaultValue(Core::AttributeInt32::Create(0));

	// request mode
	Core::AttributeSettings* attribRequestMode = RegisterAttribute("Request Mode", "RequestMode", "The type of request sent to the server.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attribRequestMode->ResizeComboValues(4);
	attribRequestMode->SetComboValue(REQUEST_CURRENT, "Current Value (no history)");
	attribRequestMode->SetComboValue(REQUEST_COUNT, "Previous Values");
	attribRequestMode->SetComboValue(REQUEST_ALL, "All Values");
	attribRequestMode->SetComboValue(REQUEST_TIMERANGE, "Time Range");
	attribRequestMode->SetDefaultValue(Core::AttributeInt32::Create(0));

	// time range
	Core::AttributeSettings* attribTimeRangeType = RegisterAttribute("Time Range", "TimeRangeType", "Specify the time range you are interested in.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attribTimeRangeType->ResizeComboValues(6);
	attribTimeRangeType->SetComboValue(0, "Custom Range");
	attribTimeRangeType->SetComboValue(1, "One Hour");
	attribTimeRangeType->SetComboValue(2, "One Day");
	attribTimeRangeType->SetComboValue(3, "One Week");
	attribTimeRangeType->SetComboValue(4, "One Month");
	attribTimeRangeType->SetComboValue(5, "One Year");
	/*
	attribRequestMode->SetComboValue(6, "Today");
	attribRequestMode->SetComboValue(7, "Yesterday");
	attribRequestMode->SetComboValue(8, "Last Hour");
	attribRequestMode->SetComboValue(9, "Last Week");
	attribRequestMode->SetComboValue(10, "Last Month");
	attribRequestMode->SetComboValue(11, "Last Year");
	*/
	attribTimeRangeType->SetDefaultValue(Core::AttributeInt32::Create(2));
	attribTimeRangeType->SetVisible(false);

	// custom time range
	Core::AttributeSettings* attribCustomTimeRange = RegisterAttribute("Time Range (Seconds)", "TimeRange", "Length of the time range in seconds.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attribCustomTimeRange->SetDefaultValue(Core::AttributeFloat::Create(1800));
	attribCustomTimeRange->SetMinValue(Core::AttributeFloat::Create(0));
	attribCustomTimeRange->SetMaxValue(Core::AttributeFloat::Create(FLT_MAX));
	attribCustomTimeRange->SetVisible(false);

	// number of samples
	Core::AttributeSettings* attribSampleRange = RegisterAttribute("Number of Values", "SampleRange", "Number of samples to fetch from the cloud.", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	attribSampleRange->SetDefaultValue(Core::AttributeInt32::Create(10));
	attribSampleRange->SetMinValue(Core::AttributeInt32::Create(0));
	attribSampleRange->SetMaxValue(Core::AttributeInt32::Create(CORE_INT32_MAX));
	attribSampleRange->SetVisible(false);

	// hide upload attribute
	GetAttributeSettings(ATTRIB_UPLOAD)->SetVisible(false);
}


void CloudInputNode::Reset()
{
	InputNode::Reset();

	mClock.Stop();
	mClock.Reset();

	mSensor.Reset();

	mInputSamples.Clear();
}



void CloudInputNode::ReInit(const Time& elapsed, const Time& delta)
{
	InputNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


void CloudInputNode::Start(const Time& elapsed)
{
	// call baseclass start
	InputNode::Start(elapsed);

	// configure output channel
	const double sampleRate = GetFloatAttribute(ATTRIB_SAMPLERATE);
	mSensor.GetChannel()->SetSampleRate(sampleRate);
	
	// set sensor / channel name
	mSensor.SetName(GetName());

	// check if node operates in continuous output mode
	if (sampleRate > 0)
	{
		mContinuousOutput = true;

		// configure clock and start it at current elapsed time
		mClock.Reset();
		mClock.SetFrequency(sampleRate);
		mClock.SetStartTime(elapsed);
		mClock.Start();
	}
	else
		mContinuousOutput = false;
}


// update the node
void CloudInputNode::Update(const Time& elapsed, const Time& delta)
{
	// shared base update helper
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// show error if node name is not unique
	if (IsParameterNameUnique(GetName()) == false)
        SetError(ERROR_DUPLICATE_NAME, "Name is not unique.");
	else
		ClearError(ERROR_DUPLICATE_NAME);

	// show warning if node was not updated yet
	if (mInputSamples.GetNumSamples() == 0)
		SetWarning(WARNING_NOT_UPDATED, "Not Updated.");
	else
		ClearWarning(WARNING_NOT_UPDATED);

	if (mContinuousOutput == true)
		mClock.Update(elapsed, delta);

	InputNode::Update(elapsed, delta);
}


// update the node settings
void CloudInputNode::OnAttributesChanged()
{
	const double sampleRate = GetFloatAttribute(ATTRIB_SAMPLERATE);
	const ERequestMode requestMode = (ERequestMode)GetInt32Attribute(ATTRIB_REQUESTMODE);
	const ETimeRangeType timeRangeType = (ETimeRangeType)GetInt32Attribute(ATTRIB_TIMERANGETYPE);
	const double timeRange = GetFloatAttribute(ATTRIB_TIMERANGE);
	const double sampleRange = GetFloatAttribute(ATTRIB_SAMPLERANGE);

	const bool parameterRenamed = (mLastName.Compare(GetName()) != 0);
	if (parameterRenamed == true		||
		sampleRate != mSampleRate		||
		requestMode != mRequestMode		||
		timeRangeType != mTimeRangeType ||
		timeRange != mAttribTimeRange	||
		sampleRange != mSampleRange)
	{
		// reset node immediately so we can start preloading the values
		ResetAsync();

		// update node config
		mSampleRate = sampleRate;
		mRequestMode = requestMode;
		mTimeRangeType = timeRangeType;
		mAttribTimeRange = timeRange;
		mTimeRange = CalculateTimeRange(mTimeRangeType, timeRange);
		mSampleRange = sampleRange;

		// update sensor name
		if (parameterRenamed == true)
		{
			mSensor.SetName(GetName());
			mLastName = GetName();
		}

		// update attribute visibility
		GetAttributeSettings(ATTRIB_SAMPLERANGE)->SetVisible(mRequestMode == REQUEST_COUNT);
		GetAttributeSettings(ATTRIB_TIMERANGETYPE)->SetVisible(mRequestMode == REQUEST_TIMERANGE);
		GetAttributeSettings(ATTRIB_TIMERANGE)->SetVisible(mRequestMode == REQUEST_TIMERANGE && mTimeRangeType == TIMERANGE_CUSTOM);

		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_SAMPLERANGE)) );
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_TIMERANGETYPE)) );
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_TIMERANGE)) );

	}

}


// the function that fills the sensors with samples
void CloudInputNode::GenerateSamples()
{
	// output samples only in continous mode
	if (mContinuousOutput == true)
	{
		const uint32 numNewSamples = mClock.GetNumNewTicks();

		// number of samples we output in a loop
		const uint32 numInputSamples = mInputSamples.GetNumSamples();
		if (numInputSamples > 0)
		{
			// get samples and output them on the sensor
			for (uint32 i = 0; i < numNewSamples; ++i)
			{
				const uint32 tickIndex = mClock.PopOldestTick();

				//access the sample values in a circular manner
				const uint32 sampleIndex = tickIndex % numInputSamples;
				const double sampleValue = mInputSamples.GetSample(sampleIndex);
				mSensor.AddQueuedSample(sampleValue);
			}
		} 
		else // no input sample yet: use the default value
		{
			const double defaultValue = GetFloatAttribute(ATTRIB_DEFAULTVALUE);
			for (uint32 i = 0; i < numNewSamples; ++i)
				mSensor.AddQueuedSample(defaultValue);

			mClock.ClearNewTicks();
		}
	}
	else  // non-continuous mode: forward values only once
	{
		const uint32 numOutputted = mSensor.GetChannel()->GetNumSamples();
		const uint32 numReceived = mInputSamples.GetNumSamples();
		for (uint32 i = numOutputted; i < numReceived; ++i)
		{
			const double sampleValue = mInputSamples.GetSample(i);
			mSensor.AddQueuedSample(sampleValue);
		}
	}
}


double CloudInputNode::CalculateTimeRange(ETimeRangeType timeRangeType, double customTimeRange)
{
	switch (timeRangeType)
	{
		case TIMERANGE_CUSTOM:	 return customTimeRange;
		
		case TIMERANGE_ONEHOUR:  return 3600.0;
		case TIMERANGE_ONEDAY:   return 24.0 * 3600.0;
		case TIMERANGE_ONEWEEK:  return 7.0 * 24.0 * 3600.0;
		case TIMERANGE_ONEMONTH: return 30.0 * 24.0 * 3600.0;
		case TIMERANGE_ONEYEAR:  return 365.0 * 24.0 * 3600.0;

		default: return 0.0;
	}
}


bool CloudInputNode::IsParameterNameUnique(const char* name)
{
	// make sure the feedback node is part of a valid classifier
	if (mParentGraph->GetType() != Classifier::TYPE_ID)
	{
		LogError("Cannot check parameter name for uniqueness, parent is not a classifier");
		CORE_ASSERT(false);
		return "";
	}

	// down-cast graph to a classifier
	Classifier* classifier = static_cast<Classifier*>(mParentGraph);

	// get the number of custom feedback nodes in the classifier and iterate through them
	const uint32 numCloudNodes = classifier->GetNumCloudInputNodes();
	for (uint32 i = 0; i<numCloudNodes; ++i)
	{
		// get the feedback node and check if it is another node than the current one
		CloudInputNode* node = classifier->GetCloudInputNode(i);
		if (node == this)
			continue;

		// match agains this node's name (= parameter name)
		if (node->GetNameString().IsEqual(name) == true)
			return false;
	}

	return true;
}


uint32 CloudInputNode::LoadCloudParameters(const CloudParameters& parameters)
{
	// clear data before loading
	Reset();

	// search for items that belong to the classifier (or have no itemId at all)
	Core::Array<const CloudParameters::Parameter*> inputParameters = parameters.Find(GetName(), mParentGraph->GetUuid(), false);

	// count number of parameters matched
	uint32 numMatched  = 0;

	// TODO we could sort the values here if we don't want to do it in the backend
	// process all parameters (in given order)
	const uint32 numParams = inputParameters.Size();
	for (uint32 i=0; i<numParams; ++i)
	{
		// not of type float
		if (inputParameters[i]->mTypeId != CloudParameters::TYPE_FLOAT)
			continue;

		// compare names
		if (inputParameters[i]->mName.IsEqual(GetName()) == false)
			continue;

		// add the value to the node input
		const double value = inputParameters[i]->GetValue<double>();
		AddInputSample(value);
		
		numMatched++;
	}

	return numMatched;
}
