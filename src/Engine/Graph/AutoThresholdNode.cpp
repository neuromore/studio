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
#include "AutoThresholdNode.h"
#include "../Core/EventManager.h"
#include "../Core/Math.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
AutoThresholdNode::AutoThresholdNode(Graph* graph) : ProcessorNode(graph, new AutoThresholdNode::Processor())
{
	mSettings.mNumBins = 10000;
	mSettings.mNumSamples = 128;
	mSettings.mRequiredEpochLength = 256;
	mSettings.mTargetMode = TARGETMODE_SCORE;
	mSettings.mThresholdInputMode = THRESHOLDINPUTMODE_RELATIVE;
	mSettings.mInvertTarget = false;
}


// destructor
AutoThresholdNode::~AutoThresholdNode()
{
}


// initialize the node
void AutoThresholdNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	UseMultiChannelMultiplication();

	// SETUP PORTS

	// setup the input ports
	GetInputPort(INPUTPORT_SIGNAL).Setup("Signal", "signal", AttributeChannels<double>::TYPE_ID, INPUTPORT_SIGNAL);
	GetInputPort(INPUTPORT_TARGET).Setup("Target", "target", AttributeChannels<double>::TYPE_ID, INPUTPORT_TARGET);
	GetInputPort(INPUTPORT_HIGH).Setup("High", "highIn", AttributeChannels<double>::TYPE_ID, INPUTPORT_HIGH);
	GetInputPort(INPUTPORT_LOW).Setup("Low", "lowIn", AttributeChannels<double>::TYPE_ID, INPUTPORT_LOW);

	// setup the output ports
	GetOutputPort(OUTPUTPORT_HIGH).Setup("High", "highOut", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_HIGH);
	GetOutputPort(OUTPUTPORT_LOW).Setup("Low", "lowOut", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_LOW);

	// ATTRIBUTES
	Core::AttributeSettings* numBinsAttrib = RegisterAttribute("Number of Bins", "numBins", "", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	numBinsAttrib->SetDefaultValue( Core::AttributeInt32::Create(mSettings.mNumBins) );
	numBinsAttrib->SetMinValue( Core::AttributeInt32::Create(100) );
	numBinsAttrib->SetMaxValue( Core::AttributeInt32::Create(CORE_INT32_MAX) );

	Core::AttributeSettings* numSamplesAttrib = RegisterAttribute("Sample Count", "numSamples", "", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	numSamplesAttrib->SetDefaultValue( Core::AttributeInt32::Create(mSettings.mNumSamples) );
	numSamplesAttrib->SetMinValue( Core::AttributeInt32::Create(2) );
	numSamplesAttrib->SetMaxValue( Core::AttributeInt32::Create(CORE_INT32_MAX) );

	Core::AttributeSettings* targetAttrib = RegisterAttribute("Target", "target", "", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	targetAttrib->ResizeComboValues( 2 );
	targetAttrib->SetComboValue(0, "Average Score");
	targetAttrib->SetComboValue(1, "Trigger Ratio");
	targetAttrib->SetDefaultValue( Core::AttributeInt32::Create(mSettings.mTargetMode) );

	Core::AttributeSettings* invertTargetAttrib = RegisterAttribute("Invert Target", "invertTarget", "", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	invertTargetAttrib->SetDefaultValue( Core::AttributeBool::Create(mSettings.mInvertTarget) );

	Core::AttributeSettings* inputModeAttrib = RegisterAttribute("Threshold Input", "thresholdInputMode", "", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	inputModeAttrib->ResizeComboValues( 2 );
	inputModeAttrib->SetComboValue(0, "Absolute");
	inputModeAttrib->SetComboValue(1, "Relative");
	inputModeAttrib->SetDefaultValue( Core::AttributeInt32::Create(mSettings.mThresholdInputMode) );
}


void AutoThresholdNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	// check inputs
	MultiChannel* signalChannels = GetInputPort(INPUTPORT_SIGNAL).GetChannels();
	MultiChannel* targetChannels = GetInputPort(INPUTPORT_TARGET).GetChannels();
	MultiChannel* highInChannels = GetInputPort(INPUTPORT_HIGH).GetChannels();
	MultiChannel* lowInChannels = GetInputPort(INPUTPORT_LOW).GetChannels();
	const bool hasSignal = signalChannels != NULL && signalChannels->GetNumChannels() > 0;
	const bool hasTarget = targetChannels != NULL && (targetChannels->GetNumChannels() == 1 || targetChannels->GetNumChannels() == signalChannels->GetNumChannels()); 
	const bool hasHighIn = highInChannels != NULL && (highInChannels->GetNumChannels() == 1 || highInChannels->GetNumChannels() == signalChannels->GetNumChannels()); 
	const bool hasLowIn	 = lowInChannels  != NULL && (lowInChannels->GetNumChannels()  == 1 || lowInChannels->GetNumChannels()  == signalChannels->GetNumChannels()); 

	// signal and target must be present
	if (hasSignal == false || hasTarget == false)
		mIsInitialized = false;
	
	// only one threshold can be specified
	if (mIsInitialized && (hasHighIn == false && hasLowIn == false))
	{
		mIsInitialized = false;
		SetError(ERROR_NO_THRESHOLD_INPUT, "Missing threshold input");
	}
	else 
	{
		ClearError(ERROR_NO_THRESHOLD_INPUT);
	}


	// only one threshold can be specified
	if (mIsInitialized && hasHighIn && hasLowIn)
	{
		mIsInitialized = false;
		SetError(ERROR_SINGLE_THRESHOLD_INPUT, "More than one threshold defined");
	}
	else 
	{
		ClearError(ERROR_SINGLE_THRESHOLD_INPUT);
	}

	// control channels must have the same samplerate
	if (mIsInitialized == true && hasTarget == true)
	{
		const double controlInputSampleRate = targetChannels->GetSampleRate();
		if ( (hasHighIn == true && highInChannels->GetSampleRate() != controlInputSampleRate) || 
			 (hasLowIn == true  && lowInChannels->GetSampleRate() != controlInputSampleRate) )
		{
			SetError(ERROR_CONTROL_PORT_SAMPLERATE, "Samplerates of control ports don't match");
		}
		else
		{
			ClearError(ERROR_CONTROL_PORT_SAMPLERATE);
		}
	}
	else
	{
		ClearError(ERROR_CONTROL_PORT_SAMPLERATE);
	}
	
	//// check size of control ports (they must either match the size of the signals channel, or all be of size 1)s
	//if (mIsInitialized && hasTarget && (hasHighIn || hasLowIn))
	//{
	//	const uint32 numChannels = targetChannels->GetNumChannels();
	//	
	//	// control channels don't match in size (Note: we already checked for size=1 above, don't need to do it here)
	//	const bool match = (hasHighIn && highInChannels->GetNumChannels() == numChannels) || (hasLowIn && lowInChannels->GetNumChannels() == numChannels);
	//	if (match == false)
	//	{
	//		mIsInitialized = false;
	//		SetError(ERROR_CONTROL_CHANNEL_MISSMATCH, "Sizes of control channels don't match");
	//	}
	//	else
	//	{
	//		ClearError(ERROR_CONTROL_CHANNEL_MISSMATCH);
	//	}
	//}

	// calculate the required input buffer size (we need an epoch longer than mNumSamples because we need to remove the old ones too, 
	const double sampleRate = (signalChannels != NULL ? signalChannels->GetSampleRate() : 0.0);
	if (sampleRate > 0)
		mSettings.mRequiredEpochLength = mSettings.mNumSamples + sampleRate * 5.0; // allow 5 second lag
	else
		mSettings.mRequiredEpochLength = mSettings.mNumSamples + Max<uint32>(mSettings.mNumSamples * 2, 100);

	PostReInit(elapsed, delta);
}


void AutoThresholdNode::Start(const Time& elapsed)
{
	ProcessorNode::Start(elapsed);
}


// update the node
void AutoThresholdNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update the baseclass
	ProcessorNode::Update(elapsed, delta);
}


void AutoThresholdNode::OnAttributesChanged()
{
	mSettings.mNumBins			  = GetInt32Attribute(ATTRIB_NUMBINS);
	mSettings.mNumSamples		  = GetInt32Attribute(ATTRIB_NUMSAMPLES);
	mSettings.mTargetMode		  = (ETargetMode)GetInt32Attribute(ATTRIB_TARGETMODE);
	mSettings.mInvertTarget		  = GetBoolAttribute(ATTRIB_INVERT_TARGET);
	mSettings.mThresholdInputMode = (EThresholdInputMode)GetInt32Attribute(ATTRIB_THRESHOLDINPUTMODE);

	// always need to reset
	ResetAsync();
}


//-----------------------------------------------
// the node's processor implementation
//-----------------------------------------------

void AutoThresholdNode::Processor::ReInit()					
{ 
	// ReInit baseclass
	ChannelProcessor::ReInit();

	mIsInitialized = false;

	mHistogram.Clear();
	
	// NOTE: don't check inputs here, everything was checked in the nodes ReInit function


	// configure input epoch for signal port
	ChannelReader* signalInputReader = GetInputReader(INPUTPORT_SIGNAL);
	signalInputReader->SetEpochLength(mSettings.mNumSamples);
	signalInputReader->SetEpochShift(1);
	signalInputReader->SetEpochZeroPadding(false);

	// forward sample rate (from control ports, not the signal port)
	ChannelBase* targetInput = GetInput(INPUTPORT_TARGET)->AsType<double>();
	ChannelBase* highOutput = GetOutput(OUTPUTPORT_HIGH)->AsType<double>();
	ChannelBase* lowOutput = GetOutput(OUTPUTPORT_LOW)->AsType<double>();
	const double outputSampleRate = targetInput->GetSampleRate();
	highOutput->SetSampleRate(outputSampleRate);
	lowOutput->SetSampleRate(outputSampleRate);

	mIsInitialized = true;
}


void AutoThresholdNode::Processor::Update()
{
	if (mIsInitialized == false)
		return;

	// update base
	ChannelProcessor::Update();
				
	// check which value we have to calculate (high or low threshold)
	const EOutputMode outputMode = (GetInput(INPUTPORT_HIGH) != NULL ? OUTPUTMODE_LOW: OUTPUTMODE_HIGH);

	// input/output channels
	Channel<double>* signalInput = GetInput(INPUTPORT_SIGNAL)->AsType<double>();

	ChannelReader* signalInputReader = GetInputReader(INPUTPORT_SIGNAL);
	ChannelReader* targetInputReader = GetInputReader(INPUTPORT_TARGET);
	ChannelReader* thresholdInputReader = (outputMode == OUTPUTMODE_LOW ? GetInputReader(INPUTPORT_HIGH) : GetInputReader(INPUTPORT_LOW)); 

	Channel<double>* highOutput = GetOutput(OUTPUTPORT_HIGH)->AsType<double>();
	Channel<double>* lowOutput = GetOutput(OUTPUTPORT_LOW)->AsType<double>();

	// get number of signal and control samples we can process. Control samples define the number of output samples
	const uint32 numSignalSamples = signalInputReader->GetNumNewSamples();
	const uint32 numControlSamples = Min(targetInputReader->GetNumNewSamples(), thresholdInputReader->GetNumNewSamples());

	//LogDebug("AutoThresholdNode::Processor::Update(): have %i signal samples, %i control samples", numSignalSamples, numControlSamples);

	// Step 1: check if histogram is still sufficient for the sample range by checking the new samples first
	double minValue = mHistogram.GetMinValue();
	double maxValue = mHistogram.GetMaxValue();
	for (uint32 i=0; i<numSignalSamples; ++i)
	{
		const double value = signalInputReader->GetSample<double>(i);
		minValue = Min(minValue, value);
		maxValue = Max(maxValue, value);
	}

	// Step 2: Reinitialize Histogram if necessary
	const bool canRebuildHistogram = signalInputReader->GetNumEpochs() > 0;
	
	const bool isEmpty = (mHistogram.GetNumValues() == 0);
	const bool isTooSmall = (minValue < mHistogram.GetMinValue() || maxValue > mHistogram.GetMaxValue());				// histogram range doesn't cover the input values
	//const bool isTooLarge = (mHistogram.GetMaxValue() - mHistogram.GetMinValue()) > 20.0 * (maxValue - minValue);		// only 5% of the histogram are actually used
	const bool isTooLarge = (maxValue - minValue) / mHistogram.GetBinWidth();		// value range is covered by less than 100 bins (= possible 'steps' in the output)
	const bool needRebuildHistogram = isEmpty || isTooSmall || isTooLarge;

	if (needRebuildHistogram && canRebuildHistogram)
	{
		//LogDebug("Rebuilding Histogram");

		// recalculate histogram by using the newest epoch
		const uint32 newestEpochIndex = signalInputReader->GetNumEpochs() - 1;
		Epoch epoch = signalInputReader->GetEpoch(newestEpochIndex);

		const double epochMin = epoch.Min();
		const double epochMax = epoch.Max();
		const double newRange = epochMax - epochMin;

		const double headroom = newRange / 4.0;					// init new histogram with a certain amount of free space in both directions to reduce number of reinitializations
		const double newMinValue = epochMin - headroom;
		const double newMaxValue = epochMax + headroom;

		//LogDebug("Reinitializing Histogram %i (%f .. %f)", mSettings.mNumBins, newMinValue, newMaxValue);
		mHistogram.Init(mSettings.mNumBins, newMinValue, newMaxValue);

		// add all the samples
		const uint32 numEpochSamples = epoch.GetNumSamples();
		for (uint32 i=0; i<numEpochSamples; ++i)
			mHistogram.AddValue(epoch.GetSample(i));

		//LogDebug("%i samples added to Histogram", numEpochSamples);


		// Note: we don't have to process new samples, they are already contained in the processed epoch
	}
	else if (needRebuildHistogram == false) // update histogram from the signal by adding new and removing old values
	{
		// Step 2.1: add new samples and remove old ones
		for (uint32 i=0; i<numSignalSamples; ++i)
		{
			// add new sample to histogram
			const uint64 index = signalInputReader->GetSampleIndex(i);
			//LogDebug("Adding Sample %i", index);
			mHistogram.AddValue(signalInput->GetSample(index));

			// remove old values that 'leave' the interval
			if (mHistogram.GetNumValues() >= mSettings.mNumSamples && index >= mSettings.mNumSamples)
			{
				const uint32 removeIndex = index - mSettings.mNumSamples;
				CORE_ASSERT(signalInput->IsValidSample(removeIndex));

				//LogDebug("Removing Sample %i", removeIndex);
				mHistogram.RemoveValue(signalInput->GetSample(removeIndex));
			}
		}
	}

	// all input from signal port is now processed
	signalInputReader->Flush();

	// Step 3: process control port samples and produce outputs (based on the latest histogram that we get after processing all signal samples)
	
	//
	// run algorithm and produce output samples
	//

	for (uint32 i=0; i<numControlSamples; ++i)
	{
		const double thresholdInputValue = thresholdInputReader->PopOldestSample<double>();
		const double targetInputValue = targetInputReader->PopOldestSample<double>();
		const double targetValue = (mSettings.mInvertTarget ? 1.0 - targetInputValue : targetInputValue);


		double highThreshold = 0;
		double lowThreshold = 0;
		
		// TODO skip calculation if target/threshold input values are the same as in the previous loop iteration

		// Mode one: high threshold is given, low must be calculated
		if (outputMode == OUTPUTMODE_LOW)
		{
			highThreshold = CalcHighInputThreshold(thresholdInputValue);
			lowThreshold = CalcLowAutoThreshold(highThreshold, targetValue);

		}
		// Mode two: low threshold is given, high must be calculated
		else // if (outputMode == OUTPUTMODE_HIGH)
		{
			lowThreshold = CalcLowInputThreshold(thresholdInputValue);
			highThreshold = CalcHighAutoThreshold (lowThreshold, targetValue);
		}

		// output the results
		highOutput->AddSample( highThreshold );
		lowOutput->AddSample( lowThreshold );
	}

}


	
double AutoThresholdNode::Processor::CalcHighInputThreshold (double inputValue)
{
	// select high threshold relativ to the upper value of the high bin of the current histogram
	if (mSettings.mThresholdInputMode == THRESHOLDINPUTMODE_RELATIVE)
	{
		const double highBinValue = mHistogram.GetBinMaxValue( mHistogram.FindHighBin() );
		const double offset = highBinValue * inputValue;
		return highBinValue + offset;
	}		
	else  // THRESHOLDINPUTMODE_ABSOLUTE
	{
		return inputValue;
	}
}

	
double AutoThresholdNode::Processor::CalcLowInputThreshold (double inputValue)
{
	// select low threshold relativ to the lower value of the low bin of the current histogram
	if (mSettings.mThresholdInputMode == THRESHOLDINPUTMODE_RELATIVE)
	{
		const double lowBinValue = mHistogram.GetBinMinValue( mHistogram.FindLowBin() );
		const double offset = lowBinValue * inputValue;
		return lowBinValue + offset;
	}		
	else  // THRESHOLDINPUTMODE_ABSOLUTE
	{
		return inputValue;
	}
}


double AutoThresholdNode::Processor::CalcHighAutoThreshold (double lowThreshold, double target)
{
	if (mHistogram.GetNumValues() == 0)
		return 0;

	// targets for both mode
	const uint32 numSamples = mHistogram.GetNumValues();
	const uint32 scoreTargetSampleCount = numSamples * target;

	// find threshold so the target is matched as closely as possible
	uint32 currentSampleCount = 0;
	double currentInverseArea = 0;

	const int32 highBinIndex = mHistogram.GetNumBins()-1;;//mHistogram.FindHighBin(); 
	const int32 lowBinIndex = mHistogram.CalcBinIndex(lowThreshold);

	for (int32 i=lowBinIndex; i<=highBinIndex; ++i)
	{
		// move low threshold one bin down and increase the current sample count
		const uint32 binSize = mHistogram.GetBin(i);

		// proposed low threshold that will be tested for the target
		const double highThreshold = mHistogram.GetBinMaxValue(i);

		// current sample count
		currentSampleCount += binSize;

		if (mSettings.mTargetMode == TARGETMODE_TIME)
		{
			// reached target count?
			if (currentSampleCount >= scoreTargetSampleCount)
			{
				// fine-adjust the threshold within BinMinValue and BinMaxValue by assuming equal sample distribution within the bin;
				const uint32 numSamplesOvershoot = currentSampleCount - scoreTargetSampleCount;
				const double correctionOffset = (binSize == 0 ? 0 : mHistogram.GetBinWidth() *  ( (double)numSamplesOvershoot / (double)binSize)); // Note: this check is probably unnecessary because the currentSampleCount never increases when binSize is zero ?!
				return highThreshold - correctionOffset;
			}
		}
		else // TARGETMODE_SCORE
		{
			const double height = (highThreshold - lowThreshold);

			if (height == 0)
				continue;

			// increment current (inverse) area
			currentInverseArea += height * binSize;

			const double totalArea = height * (double)numSamples;

			// calculate real area taken up by the waveform within the thresholds
			const double nonVisitedInverseArea = height * (numSamples - currentSampleCount);
			const double currentArea = totalArea - (currentInverseArea + nonVisitedInverseArea);

			// calculate target area from current thresholds
			const double targetArea = totalArea * target;
			
			if (currentArea >= targetArea)
			{
				// TODO fine-adjust the threshold if possible? same as above?? -> whiteboard time!
				return highThreshold;
			}
		}
	}

	// goal was not reached, return the largest value we have			// TODO: add some kind of 'success' output to the node? 
	return mHistogram.GetBinMaxValue( highBinIndex );
}


double AutoThresholdNode::Processor::CalcLowAutoThreshold (double highThreshold, double target)
{
	if (mHistogram.GetNumValues() == 0)
		return 0;

	// targets for both mode
	const uint32 numSamples = mHistogram.GetNumValues();
	const uint32 scoreTargetSampleCount = numSamples * target;

	// find threshold so the target is matched as closely as possible
	uint32 currentSampleCount = 0;
	double currentInverseArea = 0;

	const int32 highBinIndex = mHistogram.CalcBinIndex(highThreshold);
	const int32 lowBinIndex = 0;//mHistogram.FindLowBin();

	for (int32 i=highBinIndex; i>=lowBinIndex; --i)
	{
		// move low threshold one bin down and increase the current sample count
		const uint32 binSize = mHistogram.GetBin(i);

		// proposed low threshold that will be tested for the target
		const double lowThreshold = mHistogram.GetBinMinValue(i);

		// current sample count
		currentSampleCount += binSize;

		if (mSettings.mTargetMode == TARGETMODE_TIME)
		{
			// reached target count?
			if (currentSampleCount >= scoreTargetSampleCount)
			{
				// fine-adjust the threshold within BinMinValue and BinMaxValue by assuming equal sample distribution within the bin;
				const uint32 numSamplesOvershoot = currentSampleCount - scoreTargetSampleCount;
				const double correctionOffset = (binSize == 0 ? 0 : mHistogram.GetBinWidth() *  ( (double)numSamplesOvershoot / (double)binSize)); // Note: this check is probably unnecessary because the currentSampleCount never increases when binSize is zero ?!
				return lowThreshold + correctionOffset;
			}
		}
		else // TARGETMODE_SCORE
		{
			const double height = (highThreshold - lowThreshold);

			if (height == 0)
				continue;

			// increment current (inverse) area
			currentInverseArea += height * binSize;

			const double totalArea = height * (double)numSamples;

			// calculate real area taken up by the waveform within the thresholds
			const double nonVisitedInverseArea = height * (numSamples - currentSampleCount);
			const double currentArea = totalArea - (currentInverseArea + nonVisitedInverseArea);

			// calculate target area from current thresholds
			const double targetArea = totalArea * target;
			
			if (currentArea >= targetArea)
			{
				// TODO fine-adjust the threshold if possible? same as above?? -> whiteboard time!
				return lowThreshold;
			}
		}
	}

	// goal was not reached, return the smallest value we have			// TODO: add some kind of 'success' output to the node? 
	return mHistogram.GetBinMinValue( lowBinIndex );
}




