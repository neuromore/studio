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

// include required files
#include "SPNode.h"
#include "../DSP/Channel.h"
#include "Graph.h"
#include "../EngineManager.h"

using namespace Core;

// constructor
SPNode::SPNode(Graph* graph) : Node(graph)
{
	mLastActivityState = false;

	// default node configuration
	mUseMultiChannelMultiplication = false;
	mUseChannelNamePropagation = false;
	mUseChannelMetadataPropagation = true;			// FIXME: change and clean up the meta data scheme
	mUseChannelColoring = false;
	mRequireInputConnection = false;
	mRequireAllInputConnections = false;
	mRequireMatchingSampleRates = false;
	mRequireConstantSampleRate = false;
	mRequireSyncedInput = false;

	// connect input channel set to input reader
	mInputReader.SetInput(&mInputChannels);
}


// destructor
SPNode::~SPNode()
{
}


// get the name for the given category
const char* SPNode::GetCategoryName(ECategory category)
{
	switch (category)
	{
		case CATEGORY_SENSORS:			return "Sensors";
		case CATEGORY_INPUT:			return "Input";
		case CATEGORY_OUTPUT:			return "Output";
		case CATEGORY_MATH:				return "Math";
		case CATEGORY_DSP:				return "DSP";
		case CATEGORY_BIO:				return "Bio";
		case CATEGORY_UTILS:			return "Utils";
		default:						return "INVALID CATEGORY";
	}
}


// get a short description for the given category
const char* SPNode::GetCategoryDescription(ECategory category)
{
	switch (category)
	{
		case CATEGORY_SENSORS:		return "Input device nodes.";
		case CATEGORY_INPUT:		return "Other input nodes.";
		case CATEGORY_OUTPUT:		return "Output Nodes.";
		case CATEGORY_MATH:			return "Simple mathematical node.";
		case CATEGORY_DSP:			return "Advanced digital signal processing nodes.";
		case CATEGORY_BIO:			return "Nodes for bio feedback and bio signal analysis.";
		case CATEGORY_UTILS:		return "Utilitys and nodes for channel management.";
		default:                    return "INVALID CATEGORY";
	}
}

// reinit
void SPNode::ReInit(const Time& elapsed, const Time& delta)
{
	LogTraceRT("ReInit");

	// remember last init state and reset init flag
	mLastActivityState = mIsInitialized;
	mIsInitialized = true;

	// reinit input reader
	CollectInputChannels();
	mInputReader.SetInput(&mInputChannels);
	mInputReader.DetectInputChanges();	// detects changes in the input

	// (re)start the node if requested or input changed
	if (mInputReader.HasInputChanged(ChannelReader::REFERENCE) == true ||
		mInputReader.HasInputChanged(ChannelReader::RESET) == true ||
		mInputReader.HasInputChanged(ChannelReader::SAMPLERATE) == true ||
		mDoAsyncReset == true)
	{
		if (mInputReader.HasInputChanged(ChannelReader::REFERENCE) == true)
			LogDebug("input channel reference change detected, calling Reset()");
		else if (mInputReader.HasInputChanged(ChannelReader::RESET) == true)
			LogDebug("input channel reset detected, calling Reset()", GetName());
		else if (mInputReader.HasInputChanged(ChannelReader::SAMPLERATE) == true)
			LogDebug("input channel samplerate change detected, calling Reset()");
		else if (mDoAsyncReset == true)
			LogDebug("async reset requested, calling Reset()");
			
		// reset and restart in case node is currently running
		Reset();
		mInputReader.DetectInputChanges();

		// stop event must be emitted during async reset. It will be followed by an start event from SPNode::Start()
		if (mLastActivityState == true)
			EMIT_EVENT( OnNodeStopped(mParentGraph, this) );

		// try to restart node immediately
		mIsInitialized = true;
	}

	// don't start node if it is disabled; also stops it if it is currently running
	if (mIsEnabled == false)
	{
		mIsInitialized = false;
		return;
	}

	// stop node if it has not a single input channel connected (but has input ports)
	if (mRequireInputConnection == true)
	{
		if (GetNumInputPorts() > 0 && HasIncomingChannel() == false)
		{
			mIsInitialized = false;
			return;
		}
	}

	// stop node if not all input ports have connections
	if (mRequireAllInputConnections == true)
	{
		const uint32 numInputPorts = GetNumInputPorts();
		for (uint32 i=0; i<numInputPorts; ++i)
		{
			if (GetInputPort(i).GetChannels() == NULL || GetInputPort(i).GetChannels()->GetNumChannels() == 0)
			{
				mIsInitialized = false;
				return;
			}
		}
	}

	
	//
	// check input channel configuration
	//

	// A) Constant Sampelrate: if enabled, require that the input sample rate is > 0
	if (mRequireConstantSampleRate == true)
	{
		const uint32 numChannels = mInputChannels.GetNumChannels();
		for (uint32 i = 0; i < numChannels; ++i)
		{
			if (mInputChannels.GetChannel(i)->GetSampleRate() <= 0)
			{
				//mIsInitialized = false;
				SetError(ERROR_INPUT_CONSTANT_SAMPLERATE, "Input must have a valid sample rate.");
				return;
			}
		}

		ClearError(ERROR_INPUT_CONSTANT_SAMPLERATE);
	}

	// B) make sure all inputs have the same sample rate
	if (mRequireMatchingSampleRates == true)
	{
		if (mInputReader.HasUniformSampleRate() == false)
		{
			//mIsInitialized = false;
			SetError(ERROR_INPUT_MATCHING_SAMPLERATES, "Input sample rates are incompatible.");
			return;
		}
		
		ClearError(ERROR_INPUT_MATCHING_SAMPLERATES);
	}



	// C) check that all channels are synchronous
	if (mRequireSyncedInput == true)
	{
		const uint32 numChannels = mInputChannels.GetNumChannels();
		for (uint32 i = 0; i < numChannels; ++i)
		{
			if (mInputReader.GetChannel(i)->IsIndependent() == true)
			{
				mIsInitialized = false;
				SetError(ERROR_INPUT_SYNCHRONIZED, "Input channel is not synchronized to engine.");
				return;
			}
		}

		ClearError(ERROR_INPUT_SYNCHRONIZED);
	}
	else // D) make sure that all input channels are all either synced or independent
	{
		// start with a positive flag
		bool allIndependent = true;
		bool allSynced = true;

		// go through channels and set flags to false if check fails
		// Note: the correct way would be comparing the channels for each processor individually, not all of them. But this is simpler.
		const uint32 numChannels = mInputChannels.GetNumChannels();
		for (uint32 i = 0; i < numChannels; ++i)
		{
			if (mInputChannels.GetChannel(i)->IsIndependent() == true)
				allSynced = false;
			else
				allIndependent = false;
		}

		if (allIndependent == false && allSynced == false)
		{
			mIsInitialized = false;
			SetError(ERROR_INPUT_INCOMPATIBLE, "Input channels are incompatible. Must be either synced or independent.");
			return;
		}
		else
		{
			ClearError(ERROR_INPUT_INCOMPATIBLE);
		}
	}


	// D) size of input multichannels, if multichannel multiplication is activated
	if (mUseMultiChannelMultiplication == true)
	{
		const uint32 maxSize = FindMaxInputMultiChannelSize();
		
		const uint32 numInputs = GetNumInputPorts();
		for (uint32 i = 0; i < numInputs; ++i)
		{
			// skip ports w/o connections
			Port& inPort = GetInputPort(i);
			if (inPort.HasConnection() == false || inPort.GetChannels() == NULL)
				continue;

			// get multichannel size
			MultiChannel* channels = inPort.GetChannels();
			const uint32 size = channels->GetNumChannels();

			// skip multichannels of size 0 and 1 (0 will always be skipped, and the size 1 channels will be multiplied)
			if (size == 0 || size == 1)
				continue;

// don't check multichannels sizes in development version
//#ifdef PRODUCTION_BUILD
			// all other multichannels must match in size (which means the size must equal maxSize)
			if (size != maxSize)
			{
				mIsInitialized = false;

				SetError(ERROR_INPUT_INCOMPATIBLE_MULTICHANNELS, "Sizes of input multichannels are incompatible.");

				return;
			}
			else
			{
				ClearError(ERROR_INPUT_INCOMPATIBLE_MULTICHANNELS);
			}
// #endif
		}
	}

#ifdef CORE_DEBUG

	// check reader timing accuracy (right now only for development)
	Time offset = mInputReader.CalcReaderSyncOffset();
	const double maxTolerance = 5.0;
	if (offset > maxTolerance) // arbitrary thresholds to catch really bad cases
	{
		SetWarning(WARNING_INPUT_TIMING_MISSMATCH, "Input Timing missmatch!");
		LogWarning("SPNode %s (Type %x): Input Timing missmatch!", GetName(), GetType());
	}
	else
	{
		ClearWarning(WARNING_INPUT_TIMING_MISSMATCH);
	}
#endif

}


// reset node state
void SPNode::Reset()
{
	LogTrace("Reset");

	// call baseclass reset
	Node::Reset();

	// reset input readers
	mInputReader.Reset();

	// enter uninitialized state
	mIsInitialized = false;
	mLastActivityState = false;
}


// update: keep input synchrony
void SPNode::Update(const Time& elapsed, const Time& delta)
{
	LogTraceRT("Update");

	// update input reader
	mInputReader.Update();

	// clear inputs if node is inactive or a forwarding node
	if (mIsInitialized == false)
		mInputReader.Flush();

	// if this node has its own channels, reset output channel sample counter and update elapsed time and latency
	const uint32 numOutputs = GetNumOutputPorts();
	for (uint32 i = 0; i < numOutputs; ++i)
	{
		Port& output = GetOutputPort(i);

		MultiChannel* channels = output.GetChannels();
		const uint32 numChannels = channels->GetNumChannels();
		for (uint32 c = 0; c < numChannels; c++)
		{
			channels->GetChannel(c)->BeginAddSamples();
			channels->GetChannel(c)->SetElapsedTime(elapsed);
			channels->GetChannel(c)->UpdateLatency();
		}
	}
}


// call this at the end of ReInit() in the derived class
void SPNode::PostReInit(const Time& elapsed, const Time& delta)
{
	LogTraceRT("PostReInit");

	if (mIsInitialized != mLastActivityState)
	{
		// transition from inactive to active state
		if (mIsInitialized == true)
		{
			LogDebug("Starting node");

			// set flag so the first update can be skipped
			//mIsFirstUpdateReady = false;
			Start(elapsed);

			// propagate names, colors and other channel config from input to output
			if (mUseChannelMetadataPropagation == true)
				PropagateChannelMetadata();

			// node should set just the the color 
			if (mUseChannelColoring == true)
				UpdateOutputChannelColors();

			// set output source names for rendering
			UpdateOutputChannelSourceNames();

			// fire event
			EMIT_EVENT(OnNodeStarted(mParentGraph, this));
		}
		else  // transition from active to inactive state
		{
			LogDebug("Stopping node");
			Reset();

			// emit event
			EMIT_EVENT( OnNodeStopped(mParentGraph, this) );
		}
	}

}


// called when the node transitions from inactive to active state
void SPNode::Start(const Time& elapsed)
{ 
	LogTrace("Start");

	// sync input readers to earliest last-sample time (use elapsed time if it is not available)
	Time startTime = elapsed;
	if (mInputReader.GetChannels()->GetSampleRate() > 0)
		startTime = mInputReader.FindMinLastSampleTime();
	mInputReader.Start(startTime);
	
	// configure output channels so their timing matches the input
	const uint32 numPorts = GetNumOutputPorts();
	for (uint32 p = 0; p < numPorts; ++p)
	{
		Port& port = GetOutputPort(p);

		MultiChannel* channels = port.GetChannels();
		if (channels == NULL)
			continue;

		// reset channels and set start time
		const uint32 numChannels = channels->GetNumChannels();
		for (uint32 c = 0; c < numChannels; c++)
		{
			ChannelBase* channel = channels->GetChannel(c);
			channel->Reset();
			channel->SetStartTime(startTime);
		}
	}
}


uint32 SPNode::FindMaxInputMultiChannelSize()
{
	uint32 maxSize = 0;
	const uint32 numInputs = GetNumInputPorts();
	for (uint32 i = 0; i < numInputs; ++i)
	{	
		Port& inPort = GetInputPort(i);
		if (inPort.HasConnection() == false || inPort.GetChannels() == NULL)
			continue;

		MultiChannel* channels = inPort.GetChannels();

		maxSize = Max<uint32>(maxSize, channels->GetNumChannels());
	}

	return maxSize;
}


// collect all input channels into mInputChannels array while simuyltaneously detecting if input has changed and return as early as possible
void SPNode::CollectInputChannels()
{
	// step 1: check if collection has changed, skip collecting if it has not changed
	
	bool hasChanged = false;
	uint32 index = 0;	// index in mInputChannels set
	const uint32 numInputChannelsCurrent = mInputChannels.GetNumChannels();
	const uint32 numInputs = GetNumInputPorts();
	for (uint32 i=0; i<numInputs && hasChanged == false; ++i)
	{
		Port& port = GetInputPort(i);
		if (port.HasConnection() == true && port.GetChannels() != NULL)
		{
			// HACKFIX: channels is null in special case
			MultiChannel* channels = port.GetChannels();

			const uint32 numChannels = channels->GetNumChannels();
			for (uint32 c=0; c<numChannels; ++c)
			{
				// check if the number of input channels has increased
				if (index >= numInputChannelsCurrent)
				{
					hasChanged = true;
					break;
				}

				// compare references
				if (mInputChannels.GetChannel(index) != channels->GetChannel(c))
				{
					hasChanged = true;
					break;
				}

				index++;
			}
		}
	}

	// at last: check if number of input channels was too small
	if (index < numInputChannelsCurrent)
		hasChanged = true;

	if (hasChanged == false)
		return;

	uint32 multiplier = FindMaxInputMultiChannelSize();

	// FIXME optimize allocs in MultiChannel::AddChannel()
	// step 2: merge all input channels to one multichannel, apply multichannel multiplication if its activated
	mInputChannels.Clear();
	for (uint32 i=0; i<numInputs; ++i)
	{
		Port& port = GetInputPort(i);
		if (port.HasConnection() == true)
		{
			MultiChannel* channels = port.GetChannels();
			if (channels == NULL)
				continue;

			if (channels->GetNumChannels() == 0)
				continue;

			// multiply single channels so they virtually match the size
			if (mUseMultiChannelMultiplication == true)
			{
				if (channels->GetNumChannels() == 1)
				{
					// multiply single channel by adding it multiple times to the list
					ChannelBase* channel = channels->GetChannel(0);
					for (uint32 j = 0; j < multiplier; ++j)
						mInputChannels.AddChannel(channel);

				}
				else  // just append the set like always normal
				{
					mInputChannels.AddMultiChannel(channels);
				}

			}
			else  // just append the set
			{
				mInputChannels.AddMultiChannel(channels);
			}

		}
	}

	// create the channel reader map for simple access to channelreaders of multiplied input channels
	if (mUseMultiChannelMultiplication == true)
	{
		// clear map first
		mChannelReaderMap.Clear();

		// increasing channel reader index
		uint32 readerIndex = 0;

		for (uint32 i = 0; i < numInputs; ++i)
		{
			Port& port = GetInputPort(i);
			MultiChannel* channels = port.GetChannels();

			bool isEmpty = false;
			if (port.HasConnection() == false)
			{
				isEmpty = true;
			}
			else
			{
				if (channels == NULL)
					isEmpty = true;

				if (channels != NULL && channels->GetNumChannels() == 0)
					isEmpty = true;
			}

			// if input is empty (no connection or empty set), add only INVALID_INDEX values
			if (isEmpty == true)
			{
				// we need to multiply so the map is kept like a 2D array
				for (uint32 c = 0; c < multiplier; ++c)
					mChannelReaderMap.Add(CORE_INVALIDINDEX32);

				// done 
				continue;
			}

			// the rest is multiplied single channels and correct-sized multichannels
			if (channels->GetNumChannels() >= 1)
			{
				for (uint32 c = 0; c < multiplier; ++c)
				{
					mChannelReaderMap.Add(readerIndex);
					readerIndex++;
				}
			}
		}
	}
}


// experimental: check for identical input timings
bool SPNode::ValidateInputTiming()
{
	// iterate over all inputs and compare the timestamp of the next sample the input reader will read
	Time targetTime = 0.0;

	bool isFirstValidInput = false;
	
	// for safety
	if (mInputReader.GetChannels() == NULL)
		return true;

	const uint32 numChannels = mInputReader.GetChannels()->GetNumChannels();
	for (uint32 i = 0; i<numChannels; ++i)
	{
		ChannelReader* reader = mInputReader.GetReader(i);
		ChannelBase* channel = reader->GetChannel();

		if (reader->GetNumNewSamples() == 0)
			continue;

		// get timestamp of the oldest readable sample
		Time sampleTime = reader->GetOldestSampleTime();

		// first iteration defines required sample rate
		if (isFirstValidInput == false)
		{
			targetTime = sampleTime;
			isFirstValidInput = true;
		}
		else
		{
			// timing dos not match
			if (Math::AbsD(sampleTime.InSeconds() - targetTime.InSeconds()) > 1.0 / channel->GetSampleRate())
			{
				LogDebug("input samples are off by %.6f ms", (sampleTime.InSeconds() - targetTime.InSeconds() * 1000.0));
				return false;
			}
		}
	}

	return true;
}


void SPNode::UpdateChannelActivity(double timePassedInSeconds)
{
	// update output channel for activity detection (always do this here instead of in update())
	const uint32 numOutPorts = GetNumOutputPorts();
	for (uint32 i=0; i<numOutPorts; ++i)
	{
		MultiChannel* channels = GetOutputPort(i).GetChannels();
		if (channels != NULL)
		{
			const uint32 numChannels = channels->GetNumChannels();
			for (uint32 c=0; c<numChannels; c++)
				channels->GetChannel(c)->UpdateActivity(timePassedInSeconds);
		}
	}
}


// this resizes the output buffers recursively
void SPNode::ResizeBuffers(double seconds)
{
	LogTraceRT("ResizeBuffers");

	// buffers already resized -> do nothing
	if (IsResizeBuffersReady() == true)
		return;

	SetResizeBuffersReady(true);

	// depth-first recursive: call ResizeBuffers of all parent nodes
	const uint32 numInPorts = GetNumInputPorts();
	for (uint32 i=0; i<numInPorts; ++i)
	{
		if (mInputPorts[i].HasConnection() == false)
			continue;

		Connection* connection = mInputPorts[i].GetConnection();
		if (connection != NULL)
		{
			Node* parentNode = connection->GetSourceNode();
			CORE_ASSERT(parentNode->GetNodeType() != Node::NODE_TYPE);
			
			SPNode* parentSPNode = static_cast<SPNode*>(parentNode);
			parentSPNode->ResizeBuffers(seconds);
		}
	}

	// resize this node's buffers 
	const uint32 numOutPorts = GetNumOutputPorts();
	for (uint32 i = 0; i < numOutPorts; ++i)
	{
		Port& port = GetOutputPort(i);

		// get the output channel (if its a multichannel, use the first in the set)
		MultiChannel* channels = port.GetChannels();

		// skip empty sets
		if (channels->GetNumChannels() == 0)
			continue;

		// skip channels that are not buffers (for example sensor channels)
		if (channels->IsBuffer() == false)
			continue;

		uint32 minBufferSizeForReader = 1;

		// 1) find the number of minimum required samples by iterating over all child nodes
		const uint32 numChildren = mParentGraph->CalcNumOutputConnections(this, i);
		for (uint32 c = 0; c < numChildren; c++)
		{
			Connection* connection = mParentGraph->GetConnection(mParentGraph->FindOutputConnection(this, i, c));
			Node* childNode = connection->GetTargetNode();
			CORE_ASSERT(childNode->GetNodeType() != Node::NODE_TYPE);
			SPNode* childSPNode = static_cast<SPNode*>(childNode);

			// find max buffer size of all children
			const uint32 portIndex = connection->GetTargetPort();
			minBufferSizeForReader = Max(minBufferSizeForReader, childSPNode->GetNumEpochSamples(portIndex));
		}

		// 2) calculate minimum buffer size, by time
		const uint32 defaultMinBufferSize = 500;
		const double sampleRate = channels->GetSampleRate();
		const uint32 minBufferSizeForRealtime = (sampleRate > 0 ? seconds * sampleRate : defaultMinBufferSize);
		const uint32 newBufferSize = ::std::max(minBufferSizeForReader, minBufferSizeForRealtime);

		// 3) resize all buffers, but DO NOT RESET
		channels->SetBufferSize(newBufferSize, false);
	}
}



//
// Recursive Max Delay Calculations
//


// resursively calculate the maxmimum delay starting at one input port
double SPNode::FindMaximumDelayForInput(uint32 inputPortIndex)
{
	// get port
	Port& input = GetInputPort(inputPortIndex);
	if (input.HasConnection() == false)
		return 0.0;

	// find source node an output port
	Node* sourceNode = input.GetConnection()->GetSourceNode();
	uint32 sourcePortIndex = input.GetConnection()->GetSourcePort();

	// in case source node is not an SPNode (this can only be a deprecated node or a bug), do nothing
	if (sourceNode->GetNodeType() == Node::NODE_TYPE)
		return 0.0;

	// recursively calculate max delay of this input (output of parent node)
	SPNode* sourceSPNode = static_cast<SPNode*>(sourceNode);
	const double maxInputDelay = sourceSPNode->FindMaximumDelayForOutput(sourcePortIndex);

	return maxInputDelay;
}


// resursively calculate the maxmimum delay starting at one output port
double SPNode::FindMaximumDelayForOutput(uint32 outputPortIndex)
{
	// recursively find maximum delay of all input ports to the specified output port
	double maxDelay = 0.0;
	const uint32 numInputs = GetNumInputPorts();
	const uint32 numOutputs = GetNumOutputPorts();

	// the node has no delay if it does not have inputs
	if (numInputs == 0)
		return 0.0;

	for (uint32 i=0; i<numInputs; ++i)
	{
		const double maxInputDelay = FindMaximumDelayForInput(i);
		
		// if node has outputs, include the node delay in the calculations; otherwise, skip it (so we can reuse the code for the output nodes)
		if (numOutputs == 0)
		{
			maxDelay = Max(maxDelay, maxInputDelay);
		}
		else
		{
			
			// skip input in case there are no channels, or an invalid sample rate
			if (mInputPorts[i].HasConnection() == false)
				continue;

			// delay from this input to the output
			const double nodeDelay = GetDelay(i, outputPortIndex);

			// delay of this node between the given input and output, in seconds
			const double totalDelay = maxInputDelay + nodeDelay;

			// calculate maximum delay 
			maxDelay = Max(maxDelay, totalDelay);
		}
	}

	return maxDelay;
}


// calculate maxmimum delay accross all output ports of this node
double SPNode::FindMaximumDelay()
{
	double maxDelay = 0.0;
	const uint32 numOutputs = GetNumOutputPorts();

	// find max delay accross all outputs, if there are any
	if (numOutputs != 0)
	{
		for (uint32 i=0; i<numOutputs; ++i)
			maxDelay = Max(maxDelay, FindMaximumDelayForOutput(i));
	}
	else
	{
		// we reuse the code of the find-for-output method, it does not matter what output index we specify here
		maxDelay = FindMaximumDelayForOutput(0);	
	}

	return maxDelay;
}



//
// Recursive Max Latency Calculations
//

// resursively calculate the maxmimum latency starting at one input port
double SPNode::FindMaximumLatencyForInput(uint32 inputPortIndex)
{
	// get port
	Port& input = GetInputPort(inputPortIndex);
	if (input.HasConnection() == false)
		return 0.0;

	// find source node an output port
	Node* sourceNode = input.GetConnection()->GetSourceNode();
	uint32 sourcePortIndex = input.GetConnection()->GetSourcePort();

	// in case source node is not an SPNode (this can only be a deprecated node or a bug), do nothing
	if (sourceNode->GetNodeType() == Node::NODE_TYPE)
		return 0.0;

	// recursively calculate max latency of this input (output of parent node)
	SPNode* sourceSPNode = static_cast<SPNode*>(sourceNode);
	const double maxInputLatency = sourceSPNode->FindMaximumLatencyForOutput(sourcePortIndex);

	return maxInputLatency;
}


// resursively calculate the maxmimum latency starting at one output port
double SPNode::FindMaximumLatencyForOutput(uint32 outputPortIndex)
{
	// recursively find maximum latency of all input ports to the specified output port
	double maxLatency = 0.0;
	const uint32 numInputs = GetNumInputPorts();
	const uint32 numOutputs = GetNumOutputPorts();

	// the node has no latency if it does not have inputs
	if (numInputs == 0)
		return 0.0;

	for (uint32 i=0; i<numInputs; ++i)
	{
		const double maxInputLatency = FindMaximumLatencyForInput(i);
		
		// if node has outputs, include the node latency in the calculations; otherwise, skip it (so we can reuse the code for the output nodes)
		if (numOutputs == 0)
		{
			maxLatency = Max(maxLatency, maxInputLatency);
		}
		else
		{
			
			// skip input in case there are no channels, or an invalid sample rate
			if (mInputPorts[i].HasConnection() == false)
				continue;

			// latency from this input to the output
			const double nodeLatency = GetLatency(i, outputPortIndex);

			// latency of this node between the given input and output, in seconds
			const double totalLatency = maxInputLatency + nodeLatency;

			// calculate maximum latency 
			maxLatency = Max(maxLatency, totalLatency);
		}
	}

	return maxLatency;
}


// calculate maxmimum latency accross all output ports of this node
double SPNode::FindMaximumLatency()
{
	double maxLatency = 0.0;
	const uint32 numOutputs = GetNumOutputPorts();

	// find max latency accross all outputs, if there are any
	if (numOutputs != 0)
	{
		for (uint32 i=0; i<numOutputs; ++i)
			maxLatency = Max(maxLatency, FindMaximumLatencyForOutput(i));
	}
	else
	{
		// we reuse the code of the find-for-output method, it does not matter what output index we specify here
		maxLatency = FindMaximumLatencyForOutput(0);	
	}

	return maxLatency;
}


// find if node has a non-empty incoming connection
bool SPNode::HasIncomingChannel()
{
	const uint32 numInputs = GetNumInputPorts();

	for (uint32 i = 0; i < numInputs; ++i)
	{
		Port& port = GetInputPort(i);
		if (port.HasConnection() == false || port.GetChannels() == NULL)
			continue;
		
		if (port.GetChannels()->GetNumChannels() > 0)
			return true;
	}

	return false;
}



/////////////////////////////////////////////////////////////////////////////////////////////


// recursively calculate the startup delay accross all output ports of this node
// TODO deprecate this
double SPNode::FindStartupDelay()
{
	// recursively find maximum delay of all input ports to the specified output port
	double maxDelay = 0.0;
	const uint32 numInputs = GetNumInputPorts();

	// no inputs, no startup delay
	if (numInputs == 0)
		return 0.0;

	for (uint32 i=0; i<numInputs; ++i)
	{
		Port& input = GetInputPort(i);
		if (input.HasConnection() == false)
			continue;

		// find source node an output port
		Node* sourceNode = input.GetConnection()->GetSourceNode();

		// in case source node is not an SPNode (this can only be a deprecated node or a bug), do nothing
		if (sourceNode->GetNodeType() == Node::NODE_TYPE)
			continue;

		// recursively calculate max delay of this input
		SPNode* sourceSPNode = static_cast<SPNode*>(sourceNode);
		const double maxInputDelay = sourceSPNode->FindStartupDelay();

		// calculate startup delay of this node
		const double inputSampleRate = input.GetChannels()->GetSampleRate();

		// default to zero, if the input sample rate is unknown
		const double nodeDelay = (inputSampleRate == 0) ? 0 : GetNumStartupSamples(i) / inputSampleRate;

		// add the delay of this node to the value, and calculate maximum delay
		const double maxOutputDelay = maxInputDelay + nodeDelay;

		maxDelay = Max(maxDelay, maxOutputDelay);
	}

	return maxDelay;
}



//
// Buffer Stuff
//

// clear buffers
void SPNode::ResetBuffers() 
{ LogDebug ("SPNode::ResetBuffers()");
	const uint32 numOutputs = mOutputPorts.Size();
	for (uint32 p=0; p<numOutputs; ++p)
	{
		MultiChannel* channels = mOutputPorts[p].GetChannels();
		if (channels != NULL)
			channels->Reset();
	}
}


uint32 SPNode::CalculateBufferMemoryAllocated() const
{
	uint32 numBytes = 0;

	const uint32 numOutputs = mOutputPorts.Size();
	for (uint32 p=0; p<numOutputs; ++p)
	{
		MultiChannel* channels = mOutputPorts[p].GetChannels();
		if (channels == NULL)
			continue;

		numBytes += channels->CalculateMemoryAllocated(true);
	}

	return numBytes;
}


String& SPNode::GetDebugString(String& inout)
{
	Node::GetDebugString(inout);

	mTempString.Format("Initialized: %s\n", (mIsInitialized ? "True" : "False") );
	inout += mTempString;
	mTempString.Format("Delay: %.5f s\n", GetDelay(0, 0));
	inout += mTempString;
	mTempString.Format("Latency: %.5f s\n", GetLatency(0, 0));
	inout += mTempString;
	mTempString.Format("Epoch Length: %i\n", GetNumEpochSamples(0));
	inout += mTempString;
	mTempString.Format("SR Ratio: %.5f\n", GetSampleRatio(0,0));
	inout += mTempString;
	
	mTempString.Format("Sync Offset: %.5fs\n", mInputReader.CalcReaderSyncOffset().InSeconds());
	inout += mTempString;
	mTempString.Format("Latest Input: %.5fs\n", mInputReader.FindMinLastSampleTime().InSeconds());
	inout += mTempString;
	mTempString.Format("Earliest Input: %.5fs\n", mInputReader.FindMaxLastSampleTime().InSeconds());
	inout += mTempString;

	//const uint32 numInputs = GetNumInputPorts();
	//for (uint32 i = 0; i < numInputs; ++i)
	//{
	//	// for safety
	//	if (i >= mInputReader.GetChannels()->GetNumChannels())
	//		break;

	//	Port& port = GetInputPort(i);
	//	mTempString.Format("Input %i \"%s\"\n", i, port.GetName());
	//  inout += mTempString;

	//	MultiChannel* channels = port.GetChannels();

	//	if (reader == NULL)
	//		continue;
	//	else if (channels->GetNumChannels() == 0)
	//		mTempString.Format(" (empty)\n");
	//      inout += mTempString;
	//	else
	//	{
	//		ChannelBase* channel = channels->GetChannel(0);
	//	}
	//}

	const uint32 numOutputs = GetNumOutputPorts();
	for (uint32 i = 0; i < numOutputs; ++i)
	{
		Port& port = GetOutputPort(i);
		mTempString.Format("Output %i \"%s\"\n", i, port.GetName());
		inout += mTempString;

		MultiChannel* channels = port.GetChannels();
		if (channels == NULL)
			continue;
		else if (channels->GetNumChannels() == 0)
		{
			mTempString.Format(" (empty)\n");
			inout += mTempString;
		}
		else
		{
			ChannelBase* channel = channels->GetChannel(0);
			mTempString.Format("  Start: %.5fs\n", channel->GetStartTime().InSeconds());
			inout += mTempString;
			mTempString.Format("  Last: %.5fs\n", channel->GetLastSampleTime().InSeconds());
			inout += mTempString;
			mTempString.Format("  Added: %i\n", channel->GetNumNewSamples());			
			inout += mTempString;
			mTempString.Format("  Counter: %i\n", channel->GetSampleCounter());	
			inout += mTempString;
		}
	}

	return inout;
}

uint32 SPNode::CalculateBufferMemoryUsed() const
{
	uint32 numBytes = 0;

	const uint32 numOutputs = mOutputPorts.Size();
	for (uint32 p=0; p<numOutputs; ++p)
	{
		MultiChannel* channels = mOutputPorts[p].GetChannels();
		if (channels == NULL)
			continue;

		numBytes += channels->CalculateMemoryUsed(true);
	}

	return numBytes;
}


// propagate input channel names, colors etc to the outputchannels
void SPNode::PropagateChannelMetadata()
{
	const uint32 numInputs = GetNumInputPorts();
	const uint32 numOutputs = GetNumOutputPorts();

	// trivial case: one input port, and one output port -> copy name directly
	if (numInputs == 1 && numOutputs == 1)
	{
		Port& inputPort = GetInputPort(0);
		Port& outputPort = GetOutputPort(0);

		if (inputPort.HasConnection() == false || inputPort.GetChannels() == NULL)
			return;

		const uint32 numInputChannels = inputPort.GetChannels()->GetNumChannels();
		const uint32 numOutputChannels = outputPort.GetChannels()->GetNumChannels();

		// safe channel iteration
		const uint32 minNumChannels = Min(numInputChannels, numOutputChannels);
		for (uint32 i=0; i<minNumChannels; ++i)
		{
			ChannelBase* input = inputPort.GetChannels()->GetChannel(i);
			ChannelBase* output = outputPort.GetChannels()->GetChannel(i);
		
			// forward channel names
			if (mUseChannelNamePropagation == true)
				output->SetName(input->GetName());
/*
			// forward channel colors (only if channel coloring is off)
			if (mUseChannelColoring == false)
			{
				// channel color 
				//  1) use node color if its a single channel
				//  2) forward the input color if its a  multichannel (think of EEG channels, we want to keep the unique colors))
				if (minNumChannels == 1)
					output->SetColor(this->GetColor());
				else
					output->SetColor(input->GetColor());
			}
			*/
			
			// independent/synced
			output->SetIndependent(input->IsIndependent());
		}

	}

	// different number of inputs and outputs : use first name that is not empty
	else if (numInputs > 0 && numOutputs > 0)
	{
		// iterate over output channels, find a name and set it 
		for (uint32 p = 0; p < numOutputs; ++p)
		{
			Port& outPort = GetOutputPort(p);

			// metadata to propagate
			const char* outChannelName = "";
			Color outChannelColor = GetColor();
			bool outChannelIndependence = true;

			// iterate over all channels in the output multichannel and find a name for each one
			const uint32 numOutputChannels = outPort.GetChannels()->GetNumChannels();
			for (uint32 c = 0; c < numOutputChannels; ++c)
			{
				// flag so we can use the fallback method if the first one fails
				bool success = false;

				//  go through input channels and find the first non-empty name of an input;
				//       Important: this step requires that the multichannel size of the input _matches_ the output channel size
				for (uint32 i = 0; i < numInputs; ++i)
				{
					Port& inPort = GetInputPort(i);
					if (inPort.HasConnection() == false || inPort.GetChannels() == NULL)
						continue;

					MultiChannel* inChannels = inPort.GetChannels();
					if (inChannels->GetNumChannels() == 0)
						continue;

					// this produces better results with multichannels, because it prefers the multichannel input that is responsible for the multiplication
					if (inChannels->GetNumChannels() != numOutputChannels)
						continue;

					// make sure that there is an input channel with index c
					if (c >= inChannels->GetNumChannels())
						continue;

					// string is empty -> no valid candidate
					const char* candidateName = inChannels->GetChannel(c)->GetName();
					if (strlen(candidateName) > 0)
						outChannelName = candidateName;

					outChannelColor = inChannels->GetChannel(c)->GetColor();
					outChannelIndependence = inChannels->GetChannel(c)->IsIndependent();

					success = true;
					break;
				}

				// If first method failes, we use the fallback method. It work almost the same, but it ommits the requirement that input multichannel size matches output multichannel size
				// the reason we use two methods is that the first provides better results if the automatic multichannel multiplication is active (but it can fail, so we use the fallback method)
				if (success == false)
				{
					for (uint32 i = 0; i < numInputs; ++i)
					{
						Port& inPort = GetInputPort(i);
						if (inPort.HasConnection() == false || inPort.GetChannels() == NULL)
							continue;

						MultiChannel* inChannels = inPort.GetChannels();
						if (inChannels->GetNumChannels() == 0)
							continue;

						// make sure that there is an input channel with index c
						if (c >= inChannels->GetNumChannels())
							continue;

						// string is empty -> no valid candidate
						const char* candidateName = inChannels->GetChannel(c)->GetName();
						if (strlen(candidateName) > 0)
							outChannelName = candidateName;

						outChannelColor = inChannels->GetChannel(c)->GetColor();
						outChannelIndependence = inChannels->GetChannel(c)->IsIndependent();

						success = true;
						break;
					}
				}

				// last resort in case we had no success: 
				if (success == false)
				{
					//we simply use the channel index (1-indexed)
					mTempString.Format("Channel %i", c + 1);
					outChannelName = mTempString.AsChar();
				}

				//
				// now configure the output channels
				//

				MultiChannel* outChannels = outPort.GetChannels();
				ChannelBase* channel = outChannels->GetChannel(c);

				// channel name
				if (mUseChannelNamePropagation == true)
					channel->SetName(outChannelName);
				
				// forward channel colors (only if channel coloring is off)
				if (mUseChannelColoring == false)
				{
					// channel color 
					//  1) use node color if its a single channel
					//  2) forward the input color if its a  multichannel (think of EEG channels, we want to keep the unique colors))

					if (numOutputChannels == 1)
						channel->SetColor(GetColor());
					else
						channel->SetColor(outChannelColor);
				}

				// independent/synced
				channel->SetIndependent(outChannelIndependence); 

			} // end for all outputchannels

		} // end for all outputs

	}
}


void SPNode::UpdateOutputChannelColors()
{
	Color color;
	const uint32 numOutputs = GetNumOutputPorts();
	for (uint32 p = 0; p < numOutputs; ++p)
	{

		Port& outPort = GetOutputPort(p);
		const uint32 numChannels = outPort.GetChannels()->GetNumChannels();
		if (numChannels == 1)
		{
			outPort.GetChannels()->GetChannel(0)->SetColor(GetColor());
		}
		else
		{
			for (uint32 c = 0; c < numChannels; ++c)
			{
				color.SetUniqueColor(c);
				outPort.GetChannels()->GetChannel(c)->SetColor(color);
			}
		}
	}
}


void SPNode::UpdateOutputChannelSourceNames()
{
	// iterate over all output channels and set a readable source name
	const uint32 numOutputs = GetNumOutputPorts();
	for (uint32 p = 0; p < numOutputs; ++p)
	{
		Port& outPort = GetOutputPort(p);
		MultiChannel* channels = outPort.GetChannels();

		// no channels created yet
		if (channels == NULL)
			return;

		// iterate over all channels in the output multichannel and find a name for each one
		const uint32 numOutputChannels = channels->GetNumChannels();
		for (uint32 c = 0; c < numOutputChannels; ++c)
			outPort.GetChannels()->GetChannel(c)->SetSourceName(GetName());
	}

}

