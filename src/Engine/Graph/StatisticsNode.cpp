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
#include "StatisticsNode.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
StatisticsNode::StatisticsNode(Graph* graph) : ProcessorNode(graph, new StatisticsProcessor())
{
	mSettings.mSetByTime = true;
	mSettings.mIntervalDuration = 1.0;
	mSettings.mNumSamples = 100;
	mSettings.mEpochMode = StatisticsProcessor::StatisticsSettings::OFF;
}


// destructor
StatisticsNode::~StatisticsNode()
{
}


// initialize the node
void StatisticsNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	// CONFIG SPNODE
	if (mSettings.mSetByTime == true)
		RequireConstantSampleRate();

	// SETUP PORTS

	GetInputPort(INPUTPORT_CHANNEL).Setup("In", "x", AttributeChannels<double>::TYPE_ID, PORTID_INPUT_CHANNEL);
	
	const char* statisticName = StatisticsProcessor::GetStatisticMethodNameShort(mSettings.mMethod);
	GetOutputPort(OUTPUTPORT_CHANNEL).Setup(statisticName, "y", AttributeChannels<double>::TYPE_ID, PORTID_OUTPUT_CHANNEL);

	// SETUP ATTRIBUTES

	// interval type combo box
	const uint32 defaultType = (mSettings.mSetByTime ? INTERVALTYPE_DURATION : INTERVALTYPE_NUMSAMPLES);
	Core::AttributeSettings* intervalTypeAttr = RegisterAttribute("Interval Type", "IntervalType", "How the interval length should be determined", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	intervalTypeAttr->ResizeComboValues(2);
	intervalTypeAttr->SetComboValue(INTERVALTYPE_DURATION, "Duration");
	intervalTypeAttr->SetComboValue(INTERVALTYPE_NUMSAMPLES, "Sample Count");
	intervalTypeAttr->SetDefaultValue(Core::AttributeInt32::Create(defaultType));

	// interval length in seconds
	Core::AttributeSettings* lengthAttr = RegisterAttribute( "Length (Seconds)", "IntervalLength", "The length of the statistic interval in seconds.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER );
	lengthAttr->SetDefaultValue(Core::AttributeFloat::Create(mSettings.mIntervalDuration));
	lengthAttr->SetMinValue(Core::AttributeFloat::Create(0.0));
	lengthAttr->SetMaxValue(Core::AttributeFloat::Create(CORE_FLOAT_MAX));
	lengthAttr->SetVisible(mSettings.mSetByTime == true);

	// interval length in samples
	Core::AttributeSettings* lengthSamplesAttr = RegisterAttribute("Length (Samples)", "IntervalLengthInSamples", "The length of the statistic interval in samples.", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	lengthSamplesAttr->SetDefaultValue(Core::AttributeInt32::Create(mSettings.mNumSamples));
	lengthSamplesAttr->SetMinValue(Core::AttributeInt32::Create(0));
	lengthSamplesAttr->SetMaxValue(Core::AttributeInt32::Create(CORE_INT32_MAX));
	lengthSamplesAttr->SetVisible(mSettings.mSetByTime == false);

	// statistic type combo box
	const uint32 defaultMethod = (uint32)mSettings.mMethod;
	Core::AttributeSettings* methodAttr = RegisterAttribute( "Statistic", "Statistic", "The statistic over the interval.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX );
	methodAttr->ResizeComboValues(StatisticsProcessor::NUM_STATISTICTYPES);
	for (uint32 i = 0; i < StatisticsProcessor::NUM_STATISTICTYPES; i++)
		methodAttr->SetComboValue(i, StatisticsProcessor::GetStatisticMethodName((StatisticsProcessor::EStatisticMethod)i));
	methodAttr->SetDefaultValue(Core::AttributeInt32::Create(defaultMethod));

	// percentile attribute 
	Core::AttributeSettings* percentileAttr = RegisterAttribute( "Percentile", "Percentile", "Select which percentile to calculate.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	percentileAttr->SetDefaultValue(Core::AttributeFloat::Create(mSettings.mPercentile));
	percentileAttr->SetMinValue(Core::AttributeFloat::Create(0));
	percentileAttr->SetMaxValue(Core::AttributeFloat::Create(100));
	percentileAttr->SetVisible(defaultMethod == StatisticsProcessor::Percentile);

	// epoching mode combo box
	const uint32 defaultEpochMode = (uint32)mSettings.mEpochMode;
	Core::AttributeSettings* epochModeAttr = RegisterAttribute( "Epoching", "Epoching", "If epoching is on, the statistic are taken of consecutive epochs instead of every sample. Disable this if you want fast feedback.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX );
	epochModeAttr->ResizeComboValues(2);
	epochModeAttr->SetComboValue(EPOCHMODE_ON, "On");
	epochModeAttr->SetComboValue(EPOCHMODE_OFF, "Off");
	epochModeAttr->SetDefaultValue( Core::AttributeInt32::Create(defaultEpochMode) );

	// zero padding checkbox
	const bool zeroPadding = false;
	Core::AttributeSettings* zeroPaddingAttr = RegisterAttribute( "Zero Padding", "ZeroPadding", "Fill the statistics array with zeros and output results immediately. If false the node will output 0.0 until enough samples are read.", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX );
	zeroPaddingAttr->SetDefaultValue( Core::AttributeBool::Create(zeroPadding));
}


void StatisticsNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


// update the node
void StatisticsNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update the baseclass
	ProcessorNode::Update(elapsed, delta);
}


// attributes have changed
void StatisticsNode::OnAttributesChanged()
{
	// interval duration was changed
	const bool setByTime = (GetInt32Attribute(ATTRIB_INTERVAL_TYPE) ==  INTERVALTYPE_DURATION);
	const double duration = GetFloatAttribute(ATTRIB_INTERVAL_DURATION);
	const double size = GetInt32Attribute(ATTRIB_INTERVAL_SIZE);
	const uint32 methodID = GetInt32Attribute(ATTRIB_TYPE);
	const double percentile = GetFloatAttribute(ATTRIB_PERCENTILE);
	const uint32 epochMode = GetInt32Attribute(ATTRIB_EPOCHMODE);
	const bool zeroPadding = GetBoolAttribute(ATTRIB_ZEROPADDING);
	// do not reinit if nothing has changed
	if (mSettings.mSetByTime == setByTime &&
		((mSettings.mSetByTime == true && mSettings.mIntervalDuration == duration) ||
		 (mSettings.mSetByTime == false && mSettings.mNumSamples == size) ) &&
		 methodID == (uint32)mSettings.mMethod       &&
		 percentile == (uint32)mSettings.mPercentile &&
		 epochMode == (uint32)mSettings.mEpochMode	 &&
		 zeroPadding == mSettings.mZeroPadding	)
	{
		return;
	}

	// update settings
	mSettings.mSetByTime = setByTime;
	if (mSettings.mSetByTime)
		mSettings.mIntervalDuration = duration;
	else
		mSettings.mNumSamples = size;
	mSettings.mMethod = (StatisticsProcessor::EStatisticMethod)methodID;
	mSettings.mEpochMode = (StatisticsProcessor::StatisticsSettings::EEpochMode)epochMode;
	mSettings.mPercentile = percentile;
	mSettings.mZeroPadding = zeroPadding;

	// update port name
	GetOutputPort(OUTPUTPORT_CHANNEL).SetName(StatisticsProcessor::GetStatisticMethodNameShort(mSettings.mMethod));
	
	// show/hide attributes
	GetAttributeSettings(ATTRIB_INTERVAL_DURATION)->SetVisible( mSettings.mSetByTime == true );
	GetAttributeSettings(ATTRIB_INTERVAL_SIZE)->SetVisible( mSettings.mSetByTime == false);
	GetAttributeSettings(ATTRIB_PERCENTILE)->SetVisible( mSettings.mMethod == StatisticsProcessor::Percentile );

	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_INTERVAL_DURATION)) );
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_INTERVAL_SIZE)) );
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_PERCENTILE)) );


	// reconfigure SPNode
	RequireConstantSampleRate(mSettings.mSetByTime);

	ResetAsync();
}
