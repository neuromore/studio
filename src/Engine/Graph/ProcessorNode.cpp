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

// include required files
#include "ProcessorNode.h"
#include "../DSP/Channel.h"
#include "ProcessorNode.h"


using namespace Core;

// constructor
ProcessorNode::ProcessorNode(Graph* graph, ChannelProcessor* processor) : SPNode(graph)
{
	mProcessorPrototype		= processor;
}


// destructor
ProcessorNode::~ProcessorNode()
{
	// get rid of all node processors
	const uint32 numProcs = mProcessors.Size();
	for (uint32 i = 0; i<numProcs; ++i)
		delete mProcessors[i];
	mProcessors.Clear();

	// remove all channels from the output channel sets
	const uint32 numPorts = GetNumOutputPorts();
	for (uint32 p = 0; p < numPorts; ++p)
	{
		Port& port = GetOutputPort(p);
		MultiChannel* channels = port.GetChannels();
		if (channels == NULL)
			continue;

		channels->Clear();
	}
	
	// deallocate processor prototype
	delete mProcessorPrototype;
}


// initialize the node
void ProcessorNode::Init()
{
	UseMultiChannelMultiplication();
	UseChannelNamePropagation();
	UseChannelColoring();
	RequireInputConnection();

	// set number of input and output ports based on processor instance
	InitInputPorts(mProcessorPrototype->GetNumInputs());
	InitOutputPorts(mProcessorPrototype->GetNumOutputs());
}


// check if input connections have changed and reinit the processors accordingly
void ProcessorNode::ReInit(const Time& elapsed, const Time& delta)
{
	// reinit baseclass
	SPNode::ReInit(elapsed, delta);
}


// reset node state
void ProcessorNode::Reset()
{
	// reset SPNode
	SPNode::Reset();

	// get rid of all node processors
	const uint32 numProcs = mProcessors.Size();
	for (uint32 i=0; i<numProcs; ++i)
		delete mProcessors[i];
	mProcessors.Clear();

	// remove all channels from the output channel sets
	const uint32 numPorts = GetNumOutputPorts();
	for (uint32 p = 0; p < numPorts; ++p)
	{
		Port& port = GetOutputPort(p);
		MultiChannel* channels = port.GetChannels();
		if (channels == NULL)
			continue;

		channels->Clear();
	}
}



void ProcessorNode::Start(const Time& elapsed)
{
	//////////////////////////////////////////////////////////////////////////////////////////////
	// 1) find the number of processors we have to instantiate
	//
	//  we require that all ports must have either 1) the same number of channels as all other ports or 2) have a single channel
	//   in case of 1) we will assign each processor a different channel of the multichannel
	//   in case of 2) we will assign each processor the same single channel
	//  note: this is checked in ReInit()
	//
	//////////////////////////////////////////////////////////////////////////////////////////////

	const uint32 numInputs =  mProcessorPrototype->GetNumInputs();
	const uint32 numOutputs = mProcessorPrototype->GetNumOutputs();

	// assert the connections to the processors are valid
	// note: this is no longer required, because SPNode verifies the connections in SPNode::ReInit()
	//CORE_ASSERT(ValidateConnections() == true);

	// find the number of processors we have to create
	const uint32 numProcessors = FindMaxInputMultiChannelSize();

	//////////////////////////////////////////////////////////////////////////////////////////////
	// 2. allocate new processors
	// TODO optimize this by reuse old processors. 
	//      -> We need to check that every processor resets correctly before we can do that.
	//////////////////////////////////////////////////////////////////////////////////////////////

	// clear output channels
	for (uint32 i = 0; i<numOutputs; ++i)
	{
		MultiChannel*  value = GetOutputPort(i).GetChannels();
		CORE_ASSERT(value != NULL);
		value->Clear();
	}

	// get rid of all node processors
	const uint32 numProcs = mProcessors.Size();
	for (uint32 i = 0; i<numProcs; ++i)
		delete mProcessors[i];
	mProcessors.Clear();

	// create new processors
	for (uint32 i = 0; i<numProcessors; ++i)
	{
		ChannelProcessor* processor = mProcessorPrototype->Clone();
		mProcessors.Add(processor);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// 3. connect all input channels to the individual processors
	//////////////////////////////////////////////////////////////////////////////////////////////
	
	// connect inputs
	for (uint32 i = 0; i<numInputs; ++i)
	{
		for (uint32 p = 0; p < numProcessors; ++p)
		{
			// find the index of the channel reader (Multichannel Multiplication comes into play here)
			const uint32 readerIndex = mChannelReaderMap[i * numProcessors + p];

			// connection is missing -> remove delegate input reader and give it no input channel (processors handle this correctly)
			if (GetInputPort(i).HasConnection() == false || readerIndex == CORE_INVALIDINDEX32)
			{
				mProcessors[p]->RemoveDelegateInputReader(i);
				mProcessors[p]->SetInput(NULL, i);
			}
			else
			{
				CORE_ASSERT(readerIndex < mInputReader.GetNumChannels());
				
				// get the reader and tell the processor to use it for reading
				ChannelReader* reader = mInputReader.GetReader(readerIndex);
				mProcessors[p]->SetDelegateInputReader(i, reader);
			}
		}
	}

	// connect outputs
	for (uint32 i = 0; i < numOutputs; ++i)
	{
		for (uint32 p = 0; p < numProcessors; ++p)
		{
			MultiChannel* channels = GetOutputPort(i).GetChannels();
			channels->AddChannel(mProcessors[p]->GetOutput(i));
		}
	}

	SetupProcessors();
	ReInitProcessors();

	// verify that all the processors are initialized
	bool isInitialized = true;
	for (uint32 p = 0; p < numProcessors; ++p)
	{
		if (mProcessors[p]->IsInitialized() == false)
			isInitialized = false;
	}
	
	// Reset instead of starting if one of the processors did not initialize
	if (isInitialized == false)
	{
		Reset();
	}
	else
	{
		// call base class start at last to align the output channels
		SPNode::Start(elapsed);
	}
}


// update all processors
void ProcessorNode::Update(const Time& elapsed, const Time& delta)
{
	// update base class
	SPNode::Update(elapsed, delta);

	if (mIsInitialized == true)
	{
		// update all processors
		uint32 numProcessors = mProcessors.Size();
		for (uint32 i = 0; i < numProcessors; ++i)
			mProcessors[i]->Update(elapsed, delta);
	}
	else
	{
		// advance sample reader, because they are not read otherwise and would overflow
		mInputReader.Flush();
	}

}


// check and verify all connections
bool ProcessorNode::ValidateConnections()
{
	const uint32 numInputs		= GetNumInputPorts();
	const uint32 numOutputs		= GetNumOutputPorts();
	const uint32 numProcessors	= mProcessors.Size();

	// check inputs
	for (uint32 i=0; i<numInputs; ++i)
	{
		MultiChannel* channels = GetInputPort(i).GetChannels();
		
		// check input size
		if (channels != NULL)
			if (channels->GetNumChannels() != numProcessors)
				return false;

		// check individual connections
		ChannelBase* inputPortChannel;
		for (uint32 p=0; p<numProcessors; ++p)
		{
			inputPortChannel = (channels == NULL) ? NULL : channels->GetChannel(p);
		
			if (mProcessors[p]->GetInput(i) != inputPortChannel)
				return false;
		}
	}

	// check outputs
	for (uint32 i=0; i<numOutputs; ++i)
	{	
		MultiChannel* channels = GetOutputPort(i).GetChannels();
		
		// check output size
		if (channels != NULL)
			if (channels->GetNumChannels() != numProcessors)
				return false;

		// check individual connections
		ChannelBase* outputPortChannel;
		for (uint32 p=0; p<numProcessors; ++p)
		{
			outputPortChannel = (channels == NULL) ? NULL : channels->GetChannel(p);
		
			if (mProcessors[p]->GetOutput(i) != outputPortChannel)
				return false;
		}
	}

	// everything OK
	return true;
}


// configure all SP node processors
void ProcessorNode::SetupProcessors()
{
	LogTrace("SetupProcessors");

	const ChannelProcessor::Settings& settings = GetSettings();
	const uint32 numProcessors = mProcessors.Size();
	for (uint32 i=0; i<numProcessors; ++i)
		mProcessors[i]->Setup(settings);
}


// ReInit all SP node processors
void ProcessorNode::ReInitProcessors()
{ 
	LogTrace("ReInitProcessors");

	const uint32 numProcessors = mProcessors.Size();
	for (uint32 i=0; i<numProcessors; ++i)
		mProcessors[i]->ReInit();
}

double ProcessorNode::GetDelay(uint32 inputPortIndex, uint32 outputPortIndex) const
{
	if (mProcessors.Size() == 0)
		return 0.0;

	ChannelBase* channel = mProcessors[0]->GetInput(inputPortIndex);
	if (channel == NULL)
		return 0.0;

	const double sampleRate = channel->GetSampleRate();

	if (sampleRate <= 0.0)
		return 0.0;

	const uint32 delayInNumInputSamples = mProcessors[0]->GetDelay(inputPortIndex, outputPortIndex);
	const double delayInSeconds = delayInNumInputSamples / sampleRate;
	return delayInSeconds;
}


double ProcessorNode::GetLatency(uint32 inputPortIndex, uint32 outputPortIndex) const
{
	if (mProcessors.Size() == 0)
		return 0.0;

	return mProcessors[0]->GetLatency(inputPortIndex, outputPortIndex);
}


double ProcessorNode::GetSampleRatio (uint32 inputPortIndex, uint32 outputPortIndex) const
{
	if (mProcessors.Size() == 0)
		return 0.0;

	return mProcessors[0]->GetSampleRatio(inputPortIndex, outputPortIndex);
}


		// TODO deprecate this
uint32 ProcessorNode::GetNumStartupSamples (uint32 inputPortIndex) const
{
	if (mProcessors.Size() == 0)
		return 0;

	return mProcessors[0]->GetNumStartupSamples(inputPortIndex);
}


uint32 ProcessorNode::GetNumEpochSamples(uint32 inputPortIndex) const
{
	if (mProcessors.Size() == 0)
		return 0;

	return mProcessors[0]->GetNumEpochSamples(inputPortIndex);
}


String& ProcessorNode::GetDebugString(String& inout)
{
	SPNode::GetDebugString(inout);

	mTempString.Format("Processors: %i", mProcessors.Size());
	inout += mTempString;

	return inout;
}
