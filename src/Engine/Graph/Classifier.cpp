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
#include "Classifier.h"
#include "GraphImporter.h"
#include "GraphExporter.h"
#include "../EngineManager.h"
#include "FileWriterNode.h"


using namespace Core;

// default constructor
Classifier::Classifier(Graph* parentNode) : Graph(parentNode)
{
	SetName("Classifier");
	
	mIsDirty		= false;
	mIsRunning		= false;
	mIsFinalized	= false;
	mBufferDuration	= 10.0;
}


// destructor
Classifier::~Classifier()
{
}


// update the graph
void Classifier::Update(const Time& elapsed, const Time& delta)
{
	// start performance timing
	mFpsCounter.BeginTiming();
		
	/////////////////////////////////////////////////////////////
	// Phase 1: Finalize  (only required if graph can change between updates)
	//       note: as unituitive as it sounds, the reinit must happen _after_ the update!
	// finalize classifier, if not already
	//if (mIsFinalized == false)
	// FIXME execute finalize only if something changed (not working correctly, ReinitAsync() is not called at every required action which leaves the classifier partially uninitialized)
	Finalize(elapsed, delta);

	if (mIsRunning == true && mCreud.Execute() == true)
	{
		/////////////////////////////////////////////////////////////
		// Phase 2: Update
		
		// reset update ready flags for all nodes
		ResetUpdateReadyFlags();

		// update all nodes
		const uint32 numEndNodes = mEndNodes.Size();
		for (uint32 i = 0; i<numEndNodes; ++i)
			mEndNodes[i]->Update(elapsed, delta);
	}

	// always update channel activity (but only required for rendering) 
	const uint32 numNodes = mNodes.Size();
	for (uint32 i = 0; i<numNodes; ++i)
	{
		if (mNodes[i]->GetNodeType() == Node::NODE_TYPE)
			continue;

		SPNode* node = static_cast<SPNode*>(mNodes[i]);
		node->UpdateChannelActivity(delta.InSeconds());
	}
	
	// stop performance timing
	mFpsCounter.StopTiming();
}


void Classifier::Reset()
{
	// reset all nodes
	Graph::Reset();
}


// force reinit during next update
void Classifier::ReInitAsync()
{
	mIsFinalized = false;
}


// recursive reinit of all nodes, in top down order
void Classifier::ReInit(const Time& elapsed, const Time& delta)
{
	// reset reinit ready flags for all nodes
	ResetReInitReadyFlags();

	// recursively reinit all nodes, beginning with the endnodes
	const uint32 numEndNodes = mEndNodes.Size();
	for (uint32 i = 0; i < numEndNodes; ++i) {
		if (!mIsRunning && dynamic_cast<FileWriterNode*>(mEndNodes[i]) != nullptr) {
			continue;
		}
		mEndNodes[i]->ReInit(elapsed, delta);
	}

	// resize buffers
	ResizeBuffers(mBufferDuration);
}


// prepare classifier for update() 
void Classifier::Finalize(const Time& elapsed, const Time& delta)
{
	// collect all nodes into lists
	CollectNodes();

	// reinit nodes
	ReInit(elapsed, delta);

	// collect all channels (must happen after reinit)
	CollectViewChannels();

	// collect all sensors
	CollectUsedSensors();

	// remember that finalize was called
	mIsFinalized = true;

	// TODO finalize all child classifiers
}


void Classifier::Pause()
{
	mIsPaused = true;

	// TODO pause all child classifiers

}


void Classifier::Continue()
{
	mIsPaused = false;
	// TODO continue all child classifiers
}

void Classifier::Stop()
{
	// iterate over all endNodes and call FileWriterNode::closeFile to close the file.

	const uint32 numEndNodes = mEndNodes.Size();

	for (uint32 i = 0; i < numEndNodes; ++i) {
		FileWriterNode* fileWriterInstance = dynamic_cast<FileWriterNode*>(mEndNodes[i]);
		if (fileWriterInstance != nullptr) {
			if (!fileWriterInstance->closeFile()) {
				SetError(GraphObjectError::EError::ERROR_RUNTIME, "Cannot close the file.");
			}
		}
	}
	mIsRunning = false;
}


// reset the resize buffer ready flag of all nodes
void Classifier::ResetResizeBuffersReadyFlags()
{
	// get the number of nodes, iterate through them and reset their update ready flag
	const uint32 numNodes = mNodes.Size();
	for (uint32 i=0; i<numNodes; ++i)
	{
		Node* node = mNodes[i];
		const uint32 nodeType = node->GetNodeType();

		// update only SPNodes
		if (nodeType != Node::NODE_TYPE)
			static_cast<SPNode*>(node)->SetResizeBuffersReady(false);
	}
}


// resize all buffers in the graph to hold at least this many seconds of samples
void Classifier::ResizeBuffers(double seconds)
{
	// reset resize buffer flags of all nodes
	ResetResizeBuffersReadyFlags();

	// call recursive buffer resize, beginning with the end nodes
	const uint32 numEndNodes = mEndNodes.Size();
	for (uint32 i = 0; i<numEndNodes; ++i)
		mEndNodes[i]->ResizeBuffers(seconds);
}


// clear all buffers in the graph to hold at least this many seconds of samples
void Classifier::ResetBuffers()
{
	// get the number of nodes, iterate through them and reset their update ready flag
	const uint32 numNodes = mNodes.Size();
	for (uint32 i=0; i<numNodes; ++i)
	{
		if (mNodes[i]->GetType() == Node::NODE_TYPE)
			continue;
		else
		{
			SPNode* node = static_cast<SPNode*>(mNodes[i]);
			node->ResetBuffers();
		}
	}
}


// sort function for Array class to sort node pointer arrays based on their visual y position
template <class T>
static int32 CORE_CDECL NodeVisualYCompare(const T& itemA, const T& itemB)
{
	if (itemA->GetVisualPosY() < itemB->GetVisualPosY()) return -1;
	else if (itemA->GetVisualPosY() == itemB->GetVisualPosY()) return 0;
	else return 1;
}


void Classifier::CollectObjects()
{
	Graph::CollectObjects();

	// types of nodess
	CollectNodes();

	// sensors
	CollectUsedSensors();

	// view channels
	CollectViewChannels();
}


// collect the different types of nodes
void Classifier::CollectNodes()
{
	// TODO replace everything is this method with new CollectGraphObjects() methods
	//Graph::CollectGraphObjectsOfType<CustomFeedbackNode>(mCustomFeedbackNodes);

	// get the number of nodes inside the classifier
	const uint32 numNodes = mNodes.Size();

	// calculate the number of output nodes
	uint32 numCustomFeedbackNodes	= 0;
	uint32 numFeedbackNodes			= 0;
	uint32 numPointNodes			= 0;
	uint32 numViewNodes				= 0;
	uint32 numLoretaNodes			= 0;
	uint32 numParameterNodes		= 0;
	uint32 numCloudInputNodes		= 0;
	uint32 numCloudOutputNodes		= 0;
	uint32 numAnnotationNodes		= 0;
	uint32 numOutputNodes			= 0;
	uint32 numInputNodes			= 0;
	uint32 numEndNodes				= 0;
	uint32 numDeviceInputNodes		= 0;

	for (uint32 i=0; i<numNodes; ++i)
	{
		Node* node = mNodes[i];

		//// BEGIN elseif-block node-GetType()
		// is the given node a custom feedback node?
		if (node->GetType() == CustomFeedbackNode::TYPE_ID)
		{
			numCustomFeedbackNodes++;
			numFeedbackNodes++;
		}

		// is the given node a point node?
		else if (node->GetType() == PointsNode::TYPE_ID)
		{
			numPointNodes++;
			//numFeedbackNodes++;
		}

		// is the given node a body feedback node?
		else if (node->GetType() == BodyFeedbackNode::TYPE_ID)
			numFeedbackNodes++;

		// is the given node a view node?
		else if (node->GetType() == ViewNode::TYPE_ID)
			numViewNodes++;

		// is the given node a LORETA node?
		else if (node->GetType() == LoretaNode::TYPE_ID)
			numLoretaNodes++;

		// is the node an input of the graph?
		else if (node->GetType() == ParameterNode::TYPE_ID)
			numParameterNodes++;

		else if (node->GetType() == CloudInputNode::TYPE_ID)
			numCloudInputNodes++;

		else if (node->GetType() == CloudOutputNode::TYPE_ID)
			numCloudOutputNodes++;

		else if (node->GetType() == AnnotationNode::TYPE_ID)
			numAnnotationNodes++;

		
		//// END elseif-block node-GetType()

		//// BEGIN elseif-block node-GetNodeType()
		// is the node an output of the graph?
		if (node->GetNodeType() == OutputNode::NODE_TYPE)
			numOutputNodes++;

		// is the node an input of the graph?
		else if (node->GetNodeType() == InputNode::NODE_TYPE)
			numInputNodes++;

		else if (node->GetNodeType() == DeviceInputNode::NODE_TYPE)
			numDeviceInputNodes++;

		//// END elseif-block node-GetNodeType()
		
		
		// is the node an end node? // check if there is an outgoing connection
		bool haveOutConnection = false;
		const uint32 numOutPorts = node->GetNumOutputPorts();
		for (uint32 p=0; p<numOutPorts && haveOutConnection == false; ++p)
			if (CalcNumOutputConnections(node) > 0)
				haveOutConnection = true;

		if (haveOutConnection == false && node->GetNodeType() != Node::NODE_TYPE)
			numEndNodes++;

	}

	// make sure our arrays have the correct size
	if (mCustomFeedbackNodes.Size() != numCustomFeedbackNodes)	mCustomFeedbackNodes.Resize( numCustomFeedbackNodes );
	if (mFeedbackNodes.Size() != numFeedbackNodes)				mFeedbackNodes.Resize( numFeedbackNodes );
	if (mPointsNodes.Size() != numPointNodes)					mPointsNodes.Resize( numPointNodes );
	if (mViewNodes.Size() != numViewNodes)						mViewNodes.Resize(numViewNodes);
	if (mLoretaNodes.Size() != numLoretaNodes)					mLoretaNodes.Resize(numLoretaNodes);
	if (mParameterNodes.Size() != numParameterNodes)			mParameterNodes.Resize(numParameterNodes);
	if (mCloudInputNodes.Size() != numCloudInputNodes)			mCloudInputNodes.Resize(numCloudInputNodes);
	if (mCloudOutputNodes.Size() != numCloudOutputNodes)		mCloudOutputNodes.Resize(numCloudOutputNodes);
	if (mAnnotationNodes.Size() != numAnnotationNodes)			mAnnotationNodes.Resize(numAnnotationNodes);
	if (mOutputNodes.Size() != numOutputNodes)					mOutputNodes.Resize(numOutputNodes);
	if (mInputNodes.Size() != numInputNodes)					mInputNodes.Resize( numInputNodes );
	if (mDeviceInputNodes.Size() != numDeviceInputNodes)		mDeviceInputNodes.Resize( numDeviceInputNodes );
	if (mEndNodes.Size() != numEndNodes)						mEndNodes.Resize(numEndNodes);

	uint32 customFeedbackNodeIndex = 0;
	uint32 feedbackNodeIndex = 0;
	uint32 pointNodeIndex = 0;
	uint32 viewNodeIndex = 0;
	uint32 loretaNodeIndex = 0;
	uint32 parameterNodeIndex = 0;
	uint32 cloudInputNodeIndex = 0;
	uint32 cloudOutputNodeIndex = 0;
	uint32 annotationNodeIndex = 0;
	uint32 outputNodeIndex = 0;
	uint32 inputNodeIndex = 0;
	uint32 deviceInputNodeIndex = 0;
	uint32 endNodeIndex = 0;
	
	for (uint32 i=0; i<numNodes; ++i)
	{
		Node* node = mNodes[i];
		
		//// BEGIN elseif-block (node->GetType() = ...)
		// is the given node a custom feedback node?
		if (node->GetType() == CustomFeedbackNode::TYPE_ID)
		{
			mCustomFeedbackNodes[customFeedbackNodeIndex] = static_cast<CustomFeedbackNode*>(node);
			customFeedbackNodeIndex++;

			mFeedbackNodes[feedbackNodeIndex] = static_cast<CustomFeedbackNode*>(node);
			feedbackNodeIndex++;
		}

		// is the given node a point node?
		else if (node->GetType() == PointsNode::TYPE_ID)
		{
			mPointsNodes[pointNodeIndex] = static_cast<PointsNode*>(node);
			pointNodeIndex++;

			// points node do not belong to feedbacknodes, do they?
			//mFeedbackNodes[feedbackNodeIndex] = static_cast<CustomFeedbackNode*>(node);
			//feedbackNodeIndex++;
		}

		// is the given node a body feedback node?
		else if (node->GetType() == BodyFeedbackNode::TYPE_ID)
		{
			mFeedbackNodes[feedbackNodeIndex] = static_cast<CustomFeedbackNode*>(node);
			feedbackNodeIndex++;
		}

		// is the given node a view node?
		else if (node->GetType() == ViewNode::TYPE_ID)
		{
			mViewNodes[viewNodeIndex] = static_cast<ViewNode*>(node);
			viewNodeIndex++;
		}

		// is the given node a LORETA node?
		else if (node->GetType() == LoretaNode::TYPE_ID)
		{
			mLoretaNodes[loretaNodeIndex] = static_cast<LoretaNode*>(node);
			loretaNodeIndex++;
		}

		// is the given node a parameter node?
		else if (node->GetType() == ParameterNode::TYPE_ID)
		{
			mParameterNodes[parameterNodeIndex] = static_cast<ParameterNode*>(node);
			parameterNodeIndex++;
		}

		// is the given node a parameter node?
		else if (node->GetType() == CloudInputNode::TYPE_ID)
		{
			mCloudInputNodes[cloudInputNodeIndex] = static_cast<CloudInputNode*>(node);
			cloudInputNodeIndex++;
		}

		// is the given node a parameter node?
		else if (node->GetType() == CloudOutputNode::TYPE_ID)
		{
			mCloudOutputNodes[cloudOutputNodeIndex] = static_cast<CloudOutputNode*>(node);
			cloudOutputNodeIndex++;
		}

		// is the given node an Annotation node?
		else if (node->GetType() == AnnotationNode::TYPE_ID)
		{
			mAnnotationNodes[annotationNodeIndex] = static_cast<AnnotationNode*>(node);
			annotationNodeIndex++;
		}
		//// END elseif-block (node->GetType() = ...)

		//// BEGIN elseif-block (node->GetNodeType() = ...)
		// collect all output nodes (note: output nodes are nodes without output ports)
		if (node->GetNodeType() == OutputNode::NODE_TYPE)
		{
			mOutputNodes[outputNodeIndex] = static_cast<OutputNode*>(node);
			outputNodeIndex++;
		}

		// is the node an input of the graph?
		else if (node->GetNodeType() == InputNode::NODE_TYPE)
		{
			mInputNodes[inputNodeIndex] = static_cast<InputNode*>(node);
			inputNodeIndex++;
		}
		// is device input node?
		else if (node->GetNodeType() == DeviceInputNode::NODE_TYPE)
		{
			mDeviceInputNodes[deviceInputNodeIndex] = static_cast<DeviceInputNode*>(node);
			deviceInputNodeIndex++;

		}
		//// END elseif-block (node->GetNodeType() = ...)

		// is the node an end node? // check if there is an outgoing connection
		bool haveOutConnection = false;
		const uint32 numOutPorts = node->GetNumOutputPorts();
		for (uint32 p=0; p<numOutPorts && haveOutConnection == false; ++p)
			if (CalcNumOutputConnections(node) > 0)
				haveOutConnection = true;

		if (haveOutConnection == false && node->GetNodeType() != Node::NODE_TYPE)
		{
			mEndNodes[endNodeIndex] = static_cast<SPNode*>(node);
			endNodeIndex++;
		}
	}

	// sort nodes by visual position
	mFeedbackNodes.Sort(NodeVisualYCompare);
	mCustomFeedbackNodes.Sort(NodeVisualYCompare);
	mViewNodes.Sort(NodeVisualYCompare);
	mLoretaNodes.Sort(NodeVisualYCompare);
	mParameterNodes.Sort(NodeVisualYCompare);
	mCloudInputNodes.Sort(NodeVisualYCompare);
	mCloudOutputNodes.Sort(NodeVisualYCompare);
}

//
//// collect all channels (buffers, storage and viewed channels) inside the classifier
//void Classifier::CollectChannels()
//{
//	uint32 numBufferSpectrumChannels = 0;
//	uint32 numBufferChannels = 0;
//
//	uint32 numStorageChannels = 0;
//	uint32 numStorageSpectrumChannels = 0;
//
//	// count number of storage and buffer channels by looking at the output ports of all SPNodes 
//	// Note: this counts EEG channels twice! but we don't care that the arrays are larger than required, it's still faster because of less allocations
//	const uint32 numNodes = mNodes.Size();
//	for (uint32 i = 0; i < numNodes; ++i)
//	{
//		Node* node = mNodes[i];
//
//		if (node->GetBaseType() != SPNode::BASE_TYPE)
//			continue;
//
//		SPNode* spnode = static_cast<SPNode*>(node);
//
//		// iterate over all output ports
//		const uint32 numOutputPorts = spnode->GetNumOutputPorts();
//		for (uint32 p = 0; p < numOutputPorts; ++p)
//		{
//			Port& outPort = spnode->GetOutputPort(p);
//
//			MultiChannel* channels = outPort.GetChannels();
//			if (channels == NULL)
//				continue;
//
//			// iterate over all channels in multichannel and count them, depending on their type and buffer configuration
//			const uint32 numChannels = channels->GetNumChannels();
//			for (uint32 c = 0; c < numChannels; c++)
//			{
//				ChannelBase* channel = channels->GetChannel(c);
//				if (channel->GetType() == Channel<double>::TYPE_ID)
//				{
//					if (channel->IsBuffer() == true)
//						numBufferChannels++;
//					else
//						numStorageChannels++;
//				}
//				else if(channel->GetType() == Channel<Spectrum>::TYPE_ID)
//				{
//					if (channel->IsBuffer() == true)
//						numBufferSpectrumChannels++;
//					else
//						numStorageSpectrumChannels++;
//				}
//					
//			} // end for all channels
//		} // end for all output ports
//	} // end for all nodes
//
//	//////////////////////////////////////////////////////////////////////////////////////////
//
//	// clear array
//	mBufferChannels.Clear();
//	mBufferSpectrumChannels.Clear();
//
//	mStorageChannels.Clear();
//	mStorageSpectrumChannels.Clear();
//
//	// grow the arrays, if neccessary
//	mBufferChannels.Reserve(numBufferChannels);
//	mBufferSpectrumChannels.Reserve(numBufferSpectrumChannels);
//
//	mStorageChannels.Reserve(numStorageChannels);
//	mStorageSpectrumChannels.Reserve(numStorageSpectrumChannels);
//
//	//////////////////////////////////////////////////////////////////////////////////////////
//
//	// add all storage and buffer channels to the arrays
//	for (uint32 i = 0; i < numNodes; ++i)
//	{
//		Node* node = mNodes[i];
//
//		if (node->GetBaseType() != SPNode::BASE_TYPE)
//			continue;
//
//		SPNode* spnode = static_cast<SPNode*>(node);
//
//		// iterate over all output ports
//		const uint32 numOutputPorts = spnode->GetNumOutputPorts();
//		for (uint32 p = 0; p < numOutputPorts; ++p)
//		{
//			Port& outPort = spnode->GetOutputPort(p);
//
//			MultiChannel* channels = outPort.GetChannels();
//			if (channels == NULL)
//				continue;
//
//			// iterate over all channels in multichannel
//			const uint32 numChannels = channels->GetNumChannels();
//			for (uint32 c = 0; c < numChannels; c++)
//			{
//				// put it in either the buffer- , or the storage channel list
//				// make sure we store only one reference to the channel (e.g. device nodes have the same channel reference on multiple ports)
//				
//				ChannelBase* channelbase = channels->GetChannel(c);
//				if (channelbase->GetType() == Channel<double>::TYPE_ID)
//				{
//					Channel<double>* channel = static_cast<Channel<double>*>(channelbase);
//					if (channel->IsBuffer() == true)
//					{
//						if (mBufferChannels.Contains(channel) == false)
//							mBufferChannels.Add(channel);
//					}
//					else
//					{
//						if (mStorageChannels.Contains(channel) == false)
//							mStorageChannels.Add(channel);
//					}
//				}
//				else   // Spectrum Channels
//				{
//					Channel<Spectrum>* channel = static_cast<Channel<Spectrum>*>(channelbase);
//					if (channel->IsBuffer() == true)
//					{
//						if (mBufferSpectrumChannels.Contains(channel) == false)
//							mBufferSpectrumChannels.Add(channel);
//					}
//					else
//					{
//						if (mStorageSpectrumChannels.Contains(channel) == false)
//							mStorageSpectrumChannels.Add(channel);
//					}
//				}
//
//			} // end for all channels
//		} // end for all ports
//	} // end for all nodes
//}


// collect all view node channels (and, in the future, all other channels marked as 'viewed')
void Classifier::CollectViewChannels()
{
	uint32 numDoubleViews = 0;
	uint32 numSpectrumViews = 0;

	// 1) count the number of view node channels
	const uint32 numViewNodes = mViewNodes.Size();
	for (uint32 i = 0; i < numViewNodes; ++i)
	{
		ViewNode* node = mViewNodes[i];

		// node is disabled
		if (node->GetEnableValue() == false || node->IsInitialized() == false)
			continue;

		// double channels
		if (node->GetNumDoubleChannels() > 0)
		{
			if (node->CombinedView() == true)
				numDoubleViews++;
			else
				numDoubleViews += node->GetNumDoubleChannels();
		}

		// spectrum channels
		if (node->GetNumSpectrumChannels() > 0)
		{
			if (node->CombinedView() == true)
				numSpectrumViews++;
			else
				numSpectrumViews += node->GetNumSpectrumChannels();
		}
	}

	// grow the arrays, if neccessary
	mViewChannels.Resize(numDoubleViews);
	mViewNodeMap.Resize(numDoubleViews);
	mViewSpectrumChannels.Resize(numSpectrumViews);
	mViewNodeSpectrumMap.Resize(numSpectrumViews);

	// add view channels of each node to the respective multichannel in the array
	uint32 doubleViewIndex = 0;
	uint32 spectrumViewIndex = 0;
	for (uint32 i = 0; i < numViewNodes; ++i)
	{
		ViewNode* node = mViewNodes[i];

		// node is disabled
		if (node->GetEnableValue() == false || node->IsInitialized() == false)
			continue;

		// double channels
		const uint32 numDoubleChannels = node->GetNumDoubleChannels();
		if (numDoubleChannels > 0)
		{
			// all channels in one chart
			if (node->CombinedView() == true)
			{
				mViewChannels[doubleViewIndex].Clear();
				
				for (uint32 c = 0; c < numDoubleChannels; c++)
					mViewChannels[doubleViewIndex].AddChannel(node->GetDoubleChannel(c));

				mViewNodeMap[doubleViewIndex] = node;

				doubleViewIndex++;
			}
			else // one chart per channel
			{
				for (uint32 c = 0; c < numDoubleChannels; c++)
				{
					mViewChannels[doubleViewIndex].Clear();
					mViewChannels[doubleViewIndex].AddChannel(node->GetDoubleChannel(c));
					mViewNodeMap[doubleViewIndex] = node;

					doubleViewIndex++;
				}
			}

		}

		// spectrum channel
		const uint32 numSpectrumChannels = node->GetNumSpectrumChannels();
		if (numSpectrumChannels > 0)
		{
			// all channels in one chart
			if (node->CombinedView() == true)
			{
				mViewSpectrumChannels[spectrumViewIndex].Clear();
			
				for (uint32 c = 0; c < numSpectrumChannels; c++)
					mViewSpectrumChannels[spectrumViewIndex].AddChannel(node->GetSpectrumChannel(c));
				
				mViewNodeSpectrumMap[spectrumViewIndex] = node;
				
				spectrumViewIndex++;

			}
			else // one chart per channel
			{
				for (uint32 c = 0; c < numSpectrumChannels; c++)
				{
					mViewSpectrumChannels[spectrumViewIndex].Clear();
					mViewSpectrumChannels[spectrumViewIndex].AddChannel(node->GetSpectrumChannel(c));
					mViewNodeSpectrumMap[spectrumViewIndex] = node;

					spectrumViewIndex++;
				}
				
			}

		}
	}
	
}


// collect all used sensors
void Classifier::CollectUsedSensors()
{
	mUsedSensors.Clear();

	const uint32 numNodes = mNodes.Size();
	for (uint32 i = 0; i < numNodes; ++i)
	{
		Node* node = mNodes[i];
		
		// TODO collect sensors from other nodes, right now we only have them in device input nodes
		if (node->GetNodeType() == DeviceInputNode::NODE_TYPE)
		{
			DeviceInputNode* deviceNode = static_cast<DeviceInputNode*>(node);
			const uint32 numSensors = deviceNode->GetNumSensors();
			for (uint32 j = 0; j < numSensors; ++j)
			{
				if (deviceNode->IsSensorUsed(j))
					mUsedSensors.Add(deviceNode->GetSensor(j));
			}
		}
	}
}


// highest latency of all classifier inputs
double Classifier::FindMaximumInputLatency()
{
	double maxLatency = 0.0;

	const uint32 numInputs = mInputNodes.Size();
	for (uint32 i=0; i<numInputs; ++i)
		maxLatency = Max(maxLatency, mInputNodes[i]->FindMaximumInputLatency());
		
	return maxLatency;
}


// highest path delay accross all outputs
double Classifier::FindMaximumPathDelay()
{
	double maxDelay = 0.0;

	const uint32 numOutputs = mOutputNodes.Size();
	for (uint32 i=0; i<numOutputs; ++i)
		maxDelay = Max(maxDelay, mOutputNodes[i]->FindMaximumDelay());

	return maxDelay;
}


// highest graph startup delay of all outputs
double Classifier::FindMaximumStartupDelay()
{
	double maxDelay = 0.0;

	const uint32 numOutputs = mOutputNodes.Size();
	for (uint32 i=0; i<numOutputs; ++i)
		maxDelay = Max(maxDelay, mOutputNodes[i]->FindStartupDelay());

	return maxDelay;
}


// highest graph latency delay accross all outputs (input nodes will include the input latency!)
double Classifier::FindMaximumLatency()
{
	double maxLatency = 0.0;

	const uint32 numOutputs = mOutputNodes.Size();
	for (uint32 i=0; i<numOutputs; ++i)
		maxLatency = Max(maxLatency, mOutputNodes[i]->FindMaximumLatency());

	return maxLatency;
}


// synchronize all sensors of all input nodes so the next sample that is received falls on this timestamp
void Classifier::Sync(double syncTime)
{
	// call sync of all nodes
	const uint32 numInputs = mNodes.Size();
	for (uint32 i=0; i<numInputs; ++i)
	{
		// only sync SPNodes
		if (mNodes[i]->GetNodeType() == Node::NODE_TYPE)
			continue;
		
		// call sync
		SPNode* node = static_cast<SPNode*>(mNodes[i]);
		node->Sync(syncTime);
	}
}


// calculate the total number of bytes allocated in channel buffers
uint32 Classifier::CalcNumBufferChannelsUsed() const
{
	uint32 num = 0;

	// iterate over all nodes and sum up the total memory usage of all outgoing channels
	const uint32 numNodes = mNodes.Size();
	for (uint32 i=0; i<numNodes; ++i)
	{
		Node* node = mNodes[i];
		
		if (node->GetType() == Node::NODE_TYPE)
			continue;

		SPNode* spNode = static_cast<SPNode*>(node);
		
		const uint32 numPorts = spNode->GetNumOutputPorts();
		for (uint32 j=0; j<numPorts; ++j)
		{
			MultiChannel* channels = spNode->GetOutputPort(j).GetChannels();
			num += channels->GetNumChannels();
		}
	}

	return num;
}


// calculate the total number of bytes actually used in the channel buffers
uint32 Classifier::CalculateBufferMemoryAllocated() const
{
	uint32 numBytes = 0;

	// iterate over all nodes and sum up the total memory usage of all outgoing channels
	const uint32 numNodes = mNodes.Size();
	for (uint32 i=0; i<numNodes; ++i)
	{
		Node* node = mNodes[i];
		
		if (node->GetType() == Node::NODE_TYPE)
			continue;

		SPNode* spNode = static_cast<SPNode*>(node);
		numBytes += spNode->CalculateBufferMemoryAllocated();
	}

	return numBytes;
}



// calculate the total number of bytes actually used in the channel buffers
uint32 Classifier::CalculateBufferMemoryUsed() const
{
	uint32 numBytes = 0;

	// iterate over all nodes and sum up the total memory usage of all outgoing channels
	const uint32 numNodes = mNodes.Size();
	for (uint32 i=0; i<numNodes; ++i)
	{
		Node* node = mNodes[i];
		
		if (node->GetType() == Node::NODE_TYPE)
			continue;

		SPNode* spNode = static_cast<SPNode*>(node);
		numBytes += spNode->CalculateBufferMemoryUsed();
	}

	return numBytes;
}


// sum up the bytes used in the channels of all input sensors
uint32 Classifier::CalculateInputMemoryUsed() const
{
	uint32 numBytes = 0;

	const uint32 numInputs = mInputNodes.Size();
	for (uint32 i=0; i<numInputs; ++i)
		numBytes += mInputNodes[i]->CalculateInputMemoryUsed();

	return numBytes;
}


// total number of bytes used by the classifier outputs
uint32 Classifier::CalculateOutputMemoryUsed() const
{
	uint32 numBytes = 0;

	const uint32 numOutputs = mOutputNodes.Size();
	for (uint32 i=0; i<numOutputs; ++i)
		numBytes += mOutputNodes[i]->CalculateOutputMemoryUsed();

	return numBytes;
}


// total number of bytes that must be uploaded
uint32 Classifier::CalculateUploadData(double seconds) const
{
	uint32 numBytesTotal = 0;

	const uint32 numOutputs = mOutputNodes.Size();
	for (uint32 i = 0; i<numOutputs; ++i)
	{
		if (mOutputNodes[i]->IsUploadEnabled() == false || mOutputNodes[i]->GetNumOutputChannels() == 0)
			continue;

		const double sampleRate = mOutputNodes[i]->GetOutputChannel(0)->GetSampleRate();

		// Note: TODO add sample size calculation; right now nmd files consists of 32 bit floats
		const uint32 numBytes = (uint32)(seconds * sampleRate * 4.0);
		numBytesTotal += numBytes;
	}

	return numBytesTotal;
}


// react if the graph was changed (graph-internal callback, not via event system, because this has must happen first)
void Classifier::OnGraphModified(Graph* graph, GraphObject* object)
{
	// return if the modified graph is not this one
	// TODO check if its a subgraph (for hierarchical classifiers)
	if (graph != this)
		return;

	// immediately update nodes lists
	CollectObjects();
}



bool Classifier::OnAttributeChanged(Core::Attribute* attribute) 
{
	if (Graph::OnAttributeChanged(attribute))
	{
		// set IsFinalized to false
		ReInitAsync();

		return true;
	}

	return false;
}


// cloud input nodes
uint32 Classifier::LoadCloudParameters(const CloudParameters& parameter)
{
	uint32 numLoaded = 0;

	// cloud input nodes
	const uint32 numCloudInputNodes = GetNumCloudInputNodes();
	for (uint32 i = 0; i < numCloudInputNodes; ++i)
		numLoaded += GetCloudInputNode(i)->LoadCloudParameters(parameter);

	return numLoaded;
}


// cloud output nodes
uint32 Classifier::SaveCloudParameters(CloudParameters& parameter) const
{
	uint32 numSaved = 0;

	// cloud output nodes
	const uint32 numCloudOutputNodes = GetNumCloudOutputNodes();
	for (uint32 i = 0; i < numCloudOutputNodes; ++i)
		numSaved += GetCloudOutputNode(i)->SaveCloudParameters(parameter);

	return numSaved;
}
