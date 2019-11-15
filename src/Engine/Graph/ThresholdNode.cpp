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
#include "ThresholdNode.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
ThresholdNode::ThresholdNode(Graph* graph) : ProcessorNode(graph, new ThresholdNode::Processor())
{
	mSettings.mIntervalDuration = 1.0;
	mSettings.mNumSamples = 0.0;
	
	mSettings.mUseRange = false;
	mSettings.mLowThreshold = 0.5;
	mSettings.mHighThreshold = 0.8;
	
	mSettings.mCompareFunction = FUNCTION_GREATER;
	mSettings.mCalculateCompareFunc = CompareGreater;
	
	mSettings.mRangeCompareFunction = FUNCTION_WITHIN_INCLUSIVE;
	mSettings.mCalculateRangeCompareFunc = RangeCompareWithinInclusive;
	
}


// destructor
ThresholdNode::~ThresholdNode()
{
}


// initialize the node
void ThresholdNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	// CONFIG SPNODE
	RequireConstantSampleRate();
	RequireSyncedInput();

	// SETUP PORTS

	GetInputPort(INPUTPORT_VALUE).SetupAsChannels<double>("In", "x1", INPUTPORT_VALUE);
	GetInputPort(INPUTPORT_LOW_THRESHOLD).SetupAsChannels<double>("Low", "x2", INPUTPORT_LOW_THRESHOLD);
	GetInputPort(INPUTPORT_HIGH_THRESHOLD).SetupAsChannels<double>("High", "x3", INPUTPORT_HIGH_THRESHOLD);
	GetOutputPort(OUTPUTPORT_RATIO).SetupAsChannels<double>("% Passed", "y1", OUTPUTPORT_RATIO);
	GetOutputPort(OUTPUTPORT_AUGMENT).SetupAsChannels<double>("Augment", "y2", OUTPUTPORT_AUGMENT);
	GetOutputPort(OUTPUTPORT_INHIBIT).SetupAsChannels<double>("Inhibit", "y3", OUTPUTPORT_INHIBIT);
	
	// SETUP ATTRIBUTES

	// interval length
	Core::AttributeSettings* numInputsAttr = RegisterAttribute( "Interval Length", "IntervalLength", "The length of the interval that will be looked at. Set to 0 to use an infinite interval.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSLIDER );
	numInputsAttr->SetDefaultValue(Core::AttributeFloat::Create( mSettings.mIntervalDuration ));
	numInputsAttr->SetMinValue(Core::AttributeFloat::Create( 0.0 ));
	numInputsAttr->SetMaxValue(Core::AttributeFloat::Create( 30 ));

	// range mode checkbox
	Core::AttributeSettings* attribUseRange = RegisterAttribute("Use Range", "useRange", "If enabled a threshold range will be used instead of a single threshold value.", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attribUseRange->SetDefaultValue(Core::AttributeBool::Create( mSettings.mUseRange ));

	// threshold low range spinner
	Core::AttributeSettings* attribLowThreshold = RegisterAttribute("Low Threshold", "lowThreshold", "The threshold value and lower value of the threshold range that is used if the input port is not connected.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attribLowThreshold->SetDefaultValue(Core::AttributeFloat::Create( mSettings.mLowThreshold ));
	attribLowThreshold->SetMinValue(Core::AttributeFloat::Create( -FLT_MAX ));
	attribLowThreshold->SetMaxValue(Core::AttributeFloat::Create( +FLT_MAX ));

	// threshold high range spinner
	Core::AttributeSettings* attribHighThreshold = RegisterAttribute("High Threshold", "highThreshold", "The upper value of the threshold range that is used if the input port is not connected.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attribHighThreshold->SetDefaultValue(Core::AttributeFloat::Create( mSettings.mHighThreshold ));
	attribHighThreshold->SetMinValue(Core::AttributeFloat::Create( -FLT_MAX ));
	attribHighThreshold->SetMaxValue(Core::AttributeFloat::Create( +FLT_MAX ));

	// create the compare function combobox
	Core::AttributeSettings* functionParam = RegisterAttribute("Compare Function", "compareFunction", "The comparator function to use.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	functionParam->ResizeComboValues( (uint32)NUM_FUNCTIONS );
	functionParam->SetComboValue(FUNCTION_EQUAL,			"Equal to \t(x == T)");
	functionParam->SetComboValue(FUNCTION_GREATER,			"Greater than \t(x > T)");
	functionParam->SetComboValue(FUNCTION_LESS,				"Less than \t(x < T)");
	functionParam->SetComboValue(FUNCTION_GREATEROREQUAL,   "Grt. or Equal \t(x >= T)");
	functionParam->SetComboValue(FUNCTION_LESSOREQUAL,		"Less or Equal \t(x <= T)");
	functionParam->SetComboValue(FUNCTION_INEQUAL,			"Inequal to \t(x != T)");
	functionParam->SetDefaultValue(	Core::AttributeInt32::Create(mSettings.mCompareFunction) );

	// create the compare function combobox
	Core::AttributeSettings* rangeFunctionParam = RegisterAttribute("Compare Function", "rangeCompareFunction", "The comparator function to use.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	rangeFunctionParam->ResizeComboValues((uint32)NUM_RANGE_FUNCTIONS);
	rangeFunctionParam->SetComboValue(FUNCTION_WITHIN_EXCLUSIVE, "Within Range (exclusive)\tL < x < H");
	rangeFunctionParam->SetComboValue(FUNCTION_WITHIN_INCLUSIVE, "Within Range (inclusive)\tL <= x <= H");
	rangeFunctionParam->SetComboValue(FUNCTION_WITHOUT_EXCLUSIVE, "Outside Range (exclusive)\tL > x > H");
	rangeFunctionParam->SetComboValue(FUNCTION_WITHOUT_INCLUSIVE, "Outside Range (inclusive)\tL >= x >= H");
	rangeFunctionParam->SetDefaultValue(Core::AttributeInt32::Create(mSettings.mRangeCompareFunction));

	// update ports/attribute visibility for selected default mode
	SetRangeModeEnabled(mSettings.mUseRange);
}


void ThresholdNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	// calculate the interval length in number of samples
	const double sampleRate = mInputChannels.GetSampleRate(); 
	mSettings.mNumSamples = mSettings.mIntervalDuration * sampleRate;

	PostReInit(elapsed, delta);
}


// update the node
void ThresholdNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update the baseclass
	ProcessorNode::Update(elapsed, delta);
}


// attributes have changed
void ThresholdNode::OnAttributesChanged()
{
	// compare function
	const ECompareFunction function = (ECompareFunction)GetInt32Attribute(ATTRIB_THRESHOLD_FUNCTION);
	if (mSettings.mCompareFunction != function)
	{
		mSettings.mCompareFunction = function;

		// set function
		switch (function)
		{
		case FUNCTION_EQUAL:			{ mSettings.mCalculateCompareFunc = CompareEqual; break; }
		case FUNCTION_GREATER:			{ mSettings.mCalculateCompareFunc = CompareGreater; break; }
		case FUNCTION_LESS:				{ mSettings.mCalculateCompareFunc = CompareLess; break; }
		case FUNCTION_GREATEROREQUAL:	{ mSettings.mCalculateCompareFunc = CompareGreaterOrEqual; break; }
		case FUNCTION_LESSOREQUAL:		{ mSettings.mCalculateCompareFunc = CompareLessOrEqual; break; }
		case FUNCTION_INEQUAL:			{ mSettings.mCalculateCompareFunc = CompareInequal; break; }
		default: CORE_ASSERT(false);	// function unknown
		};
	}

	// range compare function
	const ERangeCompareFunction rangeFunction = (ERangeCompareFunction)GetInt32Attribute(ATTRIB_THRESHOLD_RANGE_FUNCTION);
	if (mSettings.mRangeCompareFunction != rangeFunction)
	{
		mSettings.mRangeCompareFunction = rangeFunction;

		// set function
		switch (function)
		{
		case FUNCTION_WITHIN_EXCLUSIVE:	  { mSettings.mCalculateRangeCompareFunc = RangeCompareWithinExclusive;  break; }
		case FUNCTION_WITHIN_INCLUSIVE:	  { mSettings.mCalculateRangeCompareFunc = RangeCompareWithinInclusive;  break; }
		case FUNCTION_WITHOUT_EXCLUSIVE:  { mSettings.mCalculateRangeCompareFunc = RangeCompareOutsideExclusive; break; }
		case FUNCTION_WITHOUT_INCLUSIVE:  { mSettings.mCalculateRangeCompareFunc = RangeCompareOutsideInclusive; break; }
			default: CORE_ASSERT(false);	// function unknown
		};
	}


	mSettings.mLowThreshold		= GetFloatAttribute(ATTRIB_LOW_THRESHOLD);
	mSettings.mHighThreshold    = GetFloatAttribute(ATTRIB_HIGH_THRESHOLD);

	// range mode (call only if mode has changed)
	const bool useRange = GetBoolAttribute(ATTRIB_USE_RANGE);
	if (mSettings.mUseRange != useRange)
	{
		SetRangeModeEnabled(useRange);
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_HIGH_THRESHOLD)) );
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_THRESHOLD_RANGE_FUNCTION)) );
		EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIB_THRESHOLD_FUNCTION)) );
	}


	// need reset node if interval duration has changed (input epoch length increases)
	const double intervalLength = GetFloatAttribute(ATTRIB_INTERVAL_LENGTH);
	if (mSettings.mIntervalDuration != intervalLength)
	{
		mSettings.mIntervalDuration = intervalLength;
		ResetAsync();
		
	}
	else
	{
		// reconfigure processors on the fly
		SetupProcessors();
	}
}


// reconfigure ports/attributes for range mode (or normal mode)
void ThresholdNode::SetRangeModeEnabled(bool enabled)
{
	mSettings.mUseRange = enabled;

	// remove input connections on second threshold port if range mode is disabled
	if (enabled == false)
	{
		uint32 conIndex = mParentGraph->FindInputConnection(this, INPUTPORT_HIGH_THRESHOLD);
		if (conIndex != CORE_INVALIDINDEX32)
			mParentGraph->RemoveConnection(mParentGraph->GetConnection(conIndex));
	}

	// show port only if mode is enabled
	GetInputPort(INPUTPORT_HIGH_THRESHOLD).SetVisible(enabled);

	// rename first threshold port
	if (enabled)
		GetInputPort(INPUTPORT_LOW_THRESHOLD).SetName("Low");
	else
		GetInputPort(INPUTPORT_LOW_THRESHOLD).SetName("Thresh");

	// rename the first threshold attribute
	if (enabled == true)
		GetAttributeSettings(ATTRIB_LOW_THRESHOLD)->SetName("Low Threshold");
	else
		GetAttributeSettings(ATTRIB_LOW_THRESHOLD)->SetVisible("Threshold");

	// show/hide attributes
	GetAttributeSettings(ATTRIB_HIGH_THRESHOLD)->SetVisible(enabled == true);
	GetAttributeSettings(ATTRIB_THRESHOLD_RANGE_FUNCTION)->SetVisible(enabled == true);
	GetAttributeSettings(ATTRIB_THRESHOLD_FUNCTION)->SetVisible(enabled == false);
}


void ThresholdNode::Processor::ReInit()
{
	// ReInit baseclass
	ChannelProcessor::ReInit();

	mIsInitialized = false;
	ChannelBase* input = GetInput();
	ChannelBase* output = GetOutput();

	// nothing to do until both channels are connected
	if (input == NULL || output == NULL)
		return;

	// forward input sample rates
	const double sampleRate = input->GetSampleRate();
	GetOutput(OUTPUTPORT_RATIO)->SetSampleRate(sampleRate);
	GetOutput(OUTPUTPORT_AUGMENT)->SetSampleRate(sampleRate);
	GetOutput(OUTPUTPORT_INHIBIT)->SetSampleRate(sampleRate);
	
	mIsInitialized = true;
}


void ThresholdNode::Processor::Update()
{
	if (mIsInitialized == false)
		return;

	// update base
	ChannelProcessor::Update();

	ChannelReader*	 inputReader = GetInputReader(INPUTPORT_VALUE);
	ChannelReader*	 lowThresholdReader = GetInputReader(INPUTPORT_LOW_THRESHOLD);
	ChannelReader*	 highThresholdReader = GetInputReader(INPUTPORT_HIGH_THRESHOLD);
	Channel<double>* ratioOutput = GetOutput(OUTPUTPORT_RATIO)->AsType<double>();
	Channel<double>* augmentOutput = GetOutput(OUTPUTPORT_AUGMENT)->AsType<double>();
	Channel<double>* inhibitOutput = GetOutput(OUTPUTPORT_INHIBIT)->AsType<double>();

	// number of new epochs we can process
	const uint32 numNewEpochs = inputReader->GetNumEpochs();

	// threshold will be overwritten with threshold input sample, if there is something connected
	double lowThreshold = mSettings.mLowThreshold;
	double highThreshold = mSettings.mHighThreshold;

	// calculate statistics for this epoch
	for (uint32 e = 0; e < numNewEpochs; ++e)
	{
		// get the input epoch
		Epoch inputEpoch = inputReader->PopOldestEpoch();

		// get the first threshold value from the input (if there is a sample aligned to the epoc (time of last sample))
		ChannelBase* lowChannel = lowThresholdReader->GetChannel();
		if (lowChannel != NULL && lowChannel->IsEmpty() == false)
		{
			const uint64 sampleIndex = lowChannel->FindIndexByTime(inputReader->GetOldestSampleTime(), true);
			if (lowChannel->IsValidSample(sampleIndex))
				lowThreshold = lowChannel->AsType<double>()->GetSample(sampleIndex);
		}

		// get high threshold only if range mode is on
		if (mSettings.mUseRange == true)
		{
			ChannelBase* highChannel = highThresholdReader->GetChannel();
			if (highChannel != NULL && highChannel->IsEmpty() == false)
			{
				const uint64 sampleIndex = highChannel->FindIndexByTime(inputReader->GetOldestSampleTime(), true);
				if (highChannel->IsValidSample(sampleIndex))
					highThreshold = highChannel->AsType<double>()->GetSample(sampleIndex);
			}
		}

		// compare all samples in epoch against thresholds and count (use one loop for each mode to reduce branching)
		const uint32 numSamples = inputEpoch.GetLength();
		uint32 numPassed = 0;

		// normal mode
		if (mSettings.mUseRange == false)
		{
			for (uint32 i = 0; i < numSamples; ++i)
			{
				const double sample = inputEpoch.GetSample(i);

				// check if sample passes the threshold: compare values
				if (mSettings.mCalculateCompareFunc(sample, lowThreshold) == true)
					numPassed++;
			}
		}
		else  
		{
			// ranged mode
			for (uint32 i = 0; i < numSamples; ++i)
			{
				const double sample = inputEpoch.GetSample(i);

				// check if sample passes the threshold: compare values
				if (mSettings.mCalculateRangeCompareFunc(sample, lowThreshold, highThreshold) == true)
					numPassed++;
			}

		}

		// output ratio sample
		ratioOutput->AddSample((double)numPassed / (double)numSamples);

		// calculate and output on/off value (check the last sample)
		const double sampleValue = inputEpoch.GetLastSample();

		// output augment/inhibit value
		double augmentValue = 0;
		double inhibitValue = 0;

		// single-threshold mode
		if (mSettings.mUseRange == false)
		{
			const bool compared = mSettings.mCalculateCompareFunc(sampleValue, lowThreshold);
			augmentValue = (compared ? 1.0 : 0.0);
			inhibitValue = 1.0 - augmentValue;
		}
		else // ranged mode
		{
			const bool withinRange = mSettings.mCalculateRangeCompareFunc(sampleValue, lowThreshold, highThreshold);
			augmentValue = (withinRange == true ? RemapRange(sampleValue, lowThreshold, highThreshold, 0.0, 1.0) : 0.0);
			inhibitValue = 1.0 - augmentValue;
		}
			
		augmentOutput->AddSample(augmentValue);
		inhibitOutput->AddSample(inhibitValue);
	}
}


void ThresholdNode::Processor::Setup(const ChannelProcessor::Settings& settings)
{
	mSettings = static_cast<const ProcessorSettings&>(settings);

	// configure input reader epoching
	ChannelReader*	inputReader = GetInputReader(0);
	inputReader->SetEpochLength(mSettings.mNumSamples);
	inputReader->SetEpochShift(1);
}



//-----------------------------------------------
// the comparator functions
//-----------------------------------------------

// single threshold compare
bool ThresholdNode::CompareEqual(double x, double y)				{ return Core::IsClose<double>(x, y, Core::Math::epsilon); }
bool ThresholdNode::CompareGreater(double x, double y)				{ return (x >  y); }
bool ThresholdNode::CompareLess(double x, double y)					{ return (x <  y); }
bool ThresholdNode::CompareGreaterOrEqual(double x, double y)		{ return (x >= y); }
bool ThresholdNode::CompareLessOrEqual(double x, double y)			{ return (x <= y); }
bool ThresholdNode::CompareInequal(double x, double y)				{ return !CompareEqual(x, y); }

// range threshold compare
bool ThresholdNode::RangeCompareWithinExclusive(double x, double a, double b)		{ return (x >  a) && (x <  b); }
bool ThresholdNode::RangeCompareWithinInclusive(double x, double a, double b)		{ return (x >= a) && (x <= b); }
bool ThresholdNode::RangeCompareOutsideExclusive(double x, double a, double b)		{ return (x <  a) && (x >  b); }
bool ThresholdNode::RangeCompareOutsideInclusive(double x, double a, double b)		{ return (x <= a) && (x >= b); }
