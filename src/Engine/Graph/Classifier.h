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

#ifndef __NEUROMORE_CLASSIFIER_H
#define __NEUROMORE_CLASSIFIER_H

// include the required headers
#include "../Config.h"
#include "../Core/Array.h"
#include "../Core/EventHandler.h"
#include "Graph.h"
#include "CustomFeedbackNode.h"
#include "BodyFeedbackNode.h"
#include "PointsNode.h"
#include "OutputNode.h"
#include "InputNode.h"
#include "ViewNode.h"
#include "LoretaNode.h"
#include "ThresholdNode.h"
#include "ParameterNode.h"
#include "CloudInputNode.h"
#include "CloudOutputNode.h"
#include "AnnotationNode.h"
#include "DeviceInputNode.h"
#include "ToneGeneratorNode.h"


class ENGINE_API Classifier : public Graph, public Core::EventHandler
{
	public:
		enum { TYPE_ID = 0x1001 };

		Classifier(Graph* parentNode=NULL);
		virtual ~Classifier();

		// main functions
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		void Reset() override;
		void ResetOnSessionStart();

		// overloads
		uint32 GetType() const override							{ return TYPE_ID; }
		const char* GetTypeUuid() const override				{ return "ab16b7ee-bb6e-11e4-8dfc-aa07a5b093db"; }
		const char* GetReadableType() const override			{ return "Classifier"; }
		GraphObject* Clone(Graph* parentGraph) override			{ Classifier* clone = new Classifier(parentGraph); return clone; }

		bool OnAttributeChanged(Core::Attribute* attribute) override final;


		//
		// Nodes
		//
		
		virtual void CollectObjects() override;

		void CollectNodes();

		// access input nodes
		uint32 GetNumInputNodes()											{ return mInputNodes.Size(); }
		InputNode* GetInputNode(uint32 index)								{ return mInputNodes[index]; }

		// access output nodes
		uint32 GetNumOutputNodes()											{ return mOutputNodes.Size(); }
		OutputNode* GetOutputNode(uint32 index)								{ return mOutputNodes[index]; }

		// input device nodes
		uint32 GetNumDeviceInputNodes()										{ return mDeviceInputNodes.Size(); }
		DeviceInputNode* GetDeviceInputNode(uint32 index)					{ return mDeviceInputNodes[index]; }

		// Feedback nodes
		FeedbackNode* GetFeedbackNode(uint32 index) const					{ return mFeedbackNodes[index]; }
		uint32 GetNumFeedbackNodes() const									{ return mFeedbackNodes.Size(); }

		// Custom feedback nodes
		CustomFeedbackNode* GetCustomFeedbackNode(uint32 index)				{ return mCustomFeedbackNodes[index]; }
		uint32 FindCustomFeedbackNodeIndex(CustomFeedbackNode* node) const	{ return mCustomFeedbackNodes.Find(node); }
		uint32 GetNumCustomFeedbackNodes() const							{ return mCustomFeedbackNodes.Size(); }

		// Points
		PointsNode* GetPointsNode(uint32 index)								{ return mPointsNodes[index]; }
		uint32 FindPointsNodeIndex(PointsNode* node) const					{ return mPointsNodes.Find(node); }
		uint32 GetNumPointsNodes() const									{ return mPointsNodes.Size(); }

		// View nodes
		ViewNode* GetViewNode(uint32 index) const							{ return mViewNodes[index]; }
		uint32 GetNumViewNodes() const										{ return mViewNodes.Size(); }

		// LORETA nodes
		LoretaNode* GetLoretaNode(uint32 index) const						{ return mLoretaNodes[index]; }
		uint32 GetNumLoretaNodes() const									{ return mLoretaNodes.Size(); }

		// Annotation nodes
		AnnotationNode* GetAnnotationNode(uint32 index) const				{ return mAnnotationNodes[index]; }
		uint32 GetNumAnnotationNodes() const								{ return mAnnotationNodes.Size(); }

		// ToneGenerator nodes
		ToneGeneratorNode* GetToneGeneratorNode(uint32_t index) const		{ return mToneGeneratorNodes[index]; }
		uint32_t GetNumToneGeneratorNodes() const							{ return mToneGeneratorNodes.Size(); }

		//
		// Parameters
		//

		// Parameter nodes
		ParameterNode* GetParameterNode(uint32 index) const					{ return mParameterNodes[index]; }
		uint32 GetNumParameterNodes() const									{ return mParameterNodes.Size(); }
		
		//
		// Cloud Parameters
		//

		CloudInputNode* GetCloudInputNode(uint32 index) const				{ return mCloudInputNodes[index]; }
		uint32 GetNumCloudInputNodes() const								{ return mCloudInputNodes.Size(); }

		CloudOutputNode* GetCloudOutputNode(uint32 index) const				{ return mCloudOutputNodes[index]; }
		uint32 GetNumCloudOutputNodes() const								{ return mCloudOutputNodes.Size(); }

		uint32 LoadCloudParameters(const CloudParameters& parameter);
		uint32 SaveCloudParameters(CloudParameters& parameter) const;

		//
		// Channels
		//

		// ViewNode channels
		uint32 GetNumViewMultiChannels() const								{ return mViewChannels.Size(); }
		const MultiChannel& GetViewMultiChannel(uint32 index) const			{ return mViewChannels[index]; }
		const ViewNode& GetViewNodeForMultiChannel(uint32 index) const		{ return *mViewNodeMap[index]; }

		uint32 GetNumViewSpectrumMultiChannels() const							{ return mViewSpectrumChannels.Size(); }
		const MultiChannel& GetViewSpectrumMultiChannel(uint32 index) const		{ return mViewSpectrumChannels[index]; }
		const ViewNode& GetViewNodeForSpectrumMultiChannel(uint32 index) const	{ return *mViewNodeSpectrumMap[index]; }
		
		//
		//  Device
		//

		void CollectUsedSensors();
		bool IsSensorUsed (Sensor* sensor) const							{ return mUsedSensors.Contains(sensor); }

		//
		//  Classifier Control
		//

		// ReInitalize nodes (in correct order)
		void ReInit(const Core::Time& elapsed, const Core::Time& delta);
		void ReInitAsync();
	
		// finalize the graph (locks it and prepares it, for faster updating)
		void Finalize(const Core::Time& elapsed, const Core::Time& delta);

		// resume/pause classifier execution
		void Pause();
		void Continue();
		void Stop();

		//
		//  Buffers
		//
		
		// the the number of seconds the buffers can hold at maximum
		void SetBufferDuration(double seconds)								{ mBufferDuration = seconds; }

		// reset buffers
		void ResetBuffers();

		// recursive resize of all buffers
		void ResizeBuffers(double seconds);
		void ResetResizeBuffersReadyFlags();

		//
		// Sync and Timing
		//

		// Synchronization of nodes
		void Sync(double syncTime);

		//
		// Graph Calculations
		//

		// highest input latency of all classifier inputs
		double FindMaximumInputLatency();

		// maximum latency accros all output paths
		double FindMaximumLatency();

		// highest delay of all outputs paths
		double FindMaximumPathDelay();

		// deprecated
		double FindMaximumStartupDelay();

		// buffer memory statistics; returnvalues are in bytes
		uint32 CalculateBufferMemoryAllocated() const;
		uint32 CalculateBufferMemoryUsed() const;

		// calculate input/output memory
		uint32 CalculateInputMemoryUsed() const;
		uint32 CalculateOutputMemoryUsed() const;

		// total number of bytes that must be uploaded
		uint32 CalculateUploadData(double seconds) const;

		// number of buffers
		uint32 CalcNumBufferChannelsUsed() const;

	
	protected:
		// graph internal callback
		void OnGraphModified(Graph* graph, GraphObject* object) override;


		// collected nodes for quick access
		Core::Array<CustomFeedbackNode*>		mCustomFeedbackNodes;	// custom feedback nodes
		Core::Array<FeedbackNode*>				mFeedbackNodes;			// all feedback nodes
		Core::Array<PointsNode*>				mPointsNodes;			// point output nodes
		Core::Array<ViewNode*>					mViewNodes;				// all instances of ViewNode
		Core::Array<LoretaNode*>				mLoretaNodes;			// all instances of LoretaNode
		Core::Array<ParameterNode*>				mParameterNodes;		// all instances of ParameterNode
		Core::Array<CloudInputNode*>			mCloudInputNodes;		// all instances of CloudInputNode
		Core::Array<CloudOutputNode*>			mCloudOutputNodes;		// all instances of CloudOutputNode
		Core::Array<AnnotationNode*>			mAnnotationNodes;		// all instances of AnnotationNode
		Core::Array<ToneGeneratorNode*>			mToneGeneratorNodes;	// all instances of ToneGeneratorNode

		Core::Array<Sensor*>					mUsedSensors;			// all instances of Sensors (mainly: devices) // TODO add read/write differentiation
		Core::Array<InputNode*>					mInputNodes;			// all instances of InputNode
		Core::Array<OutputNode*>				mOutputNodes;			// all instances of OutputNode
		Core::Array<DeviceInputNode*>			mDeviceInputNodes;		// all instances of InputNode

		Core::Array<SPNode*>					mEndNodes;				// all instances of nodes that have no children

		void CollectViewChannels();
		Core::Array<MultiChannel>				mViewChannels;			// all view channels (double)
		Core::Array<ViewNode*>					mViewNodeMap;			// all the nodes that provide the view channels
		Core::Array<MultiChannel>				mViewSpectrumChannels;	// all view channels (Spectrum)
		Core::Array<ViewNode*>					mViewNodeSpectrumMap;	// all the nodes that provide the spectrum channels

		bool	mIsPaused;				
		bool	mIsFinalized;			// true, after finalize() was called, until something is changed
		double  mBufferDuration;		// number of seconds the buffers can take (also defines the absolute minimum update frequency)
};


#endif
