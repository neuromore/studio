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

#ifndef __NEUROMORE_SPNODE_H
#define __NEUROMORE_SPNODE_H

// include required headers
#include "../Config.h"
#include "Node.h"
#include "../DSP/MultiChannelReader.h"

class EngineManager;

// signal processing node
class ENGINE_API SPNode : public Node
{
	CORE_LOGTRACE_DISABLE("SPNode", GetReadableType(), this);
	CORE_LOGDEBUG_DISABLE("SPNode", GetReadableType(), this);

	public:

		enum { NODE_TYPE = 0x002 };

	
		// constructors & destructor
		SPNode(Graph* parent);
		virtual ~SPNode();
		
		// type management
		virtual uint32 GetNodeType() const override					{ return NODE_TYPE; }

		enum ECategory
		{
			CATEGORY_SENSORS = 0,
			CATEGORY_INPUT,
			CATEGORY_OUTPUT,
			CATEGORY_MATH,
			CATEGORY_DSP,
			CATEGORY_BIO,
			CATEGORY_UTILS,
			NUM_CATEGORIES,
		};

		enum EError
		{
			ERROR_INPUT_CONSTANT_SAMPLERATE			= GraphObjectError::ERROR_CONFIGURATION | 0x0101,
			ERROR_INPUT_MATCHING_SAMPLERATES		= GraphObjectError::ERROR_CONFIGURATION | 0x0102,
			ERROR_INPUT_SYNCHRONIZED				= GraphObjectError::ERROR_CONFIGURATION | 0x0103,
			ERROR_INPUT_INCOMPATIBLE				= GraphObjectError::ERROR_CONFIGURATION | 0x0104,
			ERROR_INPUT_INCOMPATIBLE_MULTICHANNELS	= GraphObjectError::ERROR_CONFIGURATION | 0x0105,
		};


		enum EWarning
		{
			WARNING_INPUT_TIMING_MISSMATCH			= GraphObjectWarning::WARNING_RUNTIME | 0x01,
		};

		static const char* GetCategoryName(ECategory category);
		static const char* GetCategoryDescription(ECategory category);

		// reset everything
		virtual void Reset() override;

		// Try to reinitialize the node. This is called recursively (see node implementations). Use this method to check if node is able to execute, and set/reset mIsInitialized accordingly.
		virtual void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;

		// Call this at the end of ReInit() in the derived class
		void PostReInit(const Core::Time& elapsed, const Core::Time& delta);

		// Called by PostReInit when the node transitions from inactive to active state. Place stuff like startup allocations here.
		virtual void Start(const Core::Time& elapsed);

		// Main update function for signal processing
		virtual void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		virtual bool IsTimeIndependent() const						{ return true; }

		// node configuration options (use this in the contructor of the derived node class)
		void UseMultiChannelMultiplication(bool enable = true)		{ mUseMultiChannelMultiplication = enable; }
		void UseChannelNamePropagation(bool enable = true)			{ mUseChannelNamePropagation = enable; }
		void UseChannelMetadataPropagation(bool enable = true)		{ mUseChannelMetadataPropagation = enable; }
		void UseChannelColoring(bool enable = true)					{ mUseChannelColoring = enable; }
		void RequireInputConnection(bool enable = true)				{ mRequireInputConnection = enable; }
		void RequireAllInputConnections(bool enable = true)			{ mRequireAllInputConnections = enable; }
		void RequireConstantSampleRate(bool enable = true)			{ mRequireConstantSampleRate = enable; }
		void RequireMatchingSampleRates(bool enable = true)			{ mRequireMatchingSampleRates = enable; }
		void RequireSyncedInput(bool enable = true)					{ mRequireSyncedInput = enable; }

		// get the size of the largest multichannel on the inputs
		uint32 FindMaxInputMultiChannelSize();

		// list containing all input channels (some may are multiplied if multichannel multiplication is used)
		virtual void CollectInputChannels();
		MultiChannelReader* GetInputReader()						{ return &mInputReader; }

		// reset everything to t=0 but nothing else
		virtual void Sync(double syncTime)							{ }

		// update channel activity timer
		void UpdateChannelActivity(double timePassedInSeconds);

		// reset output channels
		void ResetBuffers();

		// resize buffers and that of and all parent nodes recursively
		void ResizeBuffers(double seconds);
		bool IsResizeBuffersReady() const							{ return mIsResizeBuffersReady; }
		void SetResizeBuffersReady(bool isReady)					{ mIsResizeBuffersReady = isReady; }

		// recursively find maxmimum delay of this node
		virtual double FindMaximumDelay();
		virtual double FindMaximumDelayForInput(uint32 inputPortIndex);
		virtual double FindMaximumDelayForOutput(uint32 outputPortIndex);

		// recursively find maxmimum latency of this node
		virtual double FindMaximumLatency();
		virtual double FindMaximumLatencyForInput(uint32 inputPortIndex);
		virtual double FindMaximumLatencyForOutput(uint32 outputPortIndex);
		
		// check if node has at least one incoming connection with a non-empty channel set
		bool HasIncomingChannel();

		// TODO deprecate
		// recursively calculate the startup delay
		virtual double FindStartupDelay();

		// the node delay: the output channel starts this many seconds after the start of the input channel
		virtual double GetDelay(uint32 inputPortIndex, uint32 outputPortIndex) const				{ return 0; }

		// the node latency: a stimulus on the input produces an effect on the output after this many second 
		virtual double GetLatency(uint32 inputPortIndex, uint32 outputPortIndex) const				{ return 0.0; }

		// the number of samples we read for each produced output sample. May be a fraction.
		virtual double GetSampleRatio (uint32 inputPortIndex, uint32 outputPortIndex) const			{ return 1.0; }		// unused right now

		// number of samples the input has to read before it can produce the _first_ output sample 
		// TODO deprecate this
		virtual uint32 GetNumStartupSamples (uint32 inputPortIndex) const							{ return 1; }

		// number of samples the input will read at once
		virtual uint32 GetNumEpochSamples(uint32 inputPortIndex) const								{ return 1; }
		
		// get buffer stats
		uint32 CalculateBufferMemoryAllocated() const;
		uint32 CalculateBufferMemoryUsed() const;

		virtual Core::String& GetDebugString(Core::String& inout) override;

	protected:
		MultiChannel		mInputChannels;
		MultiChannelReader	mInputReader;

		// will store mIsInitialized right the beginning of SPNode::ReInit() so we can detect a change in state
		bool		mLastActivityState;

		// marker flag for recursive buffer reize; true if this node's buffers were already resized
		bool		mIsResizeBuffersReady;

		// helper list for storing the mapping : input port multichannel index after multiplication <-> channel reader indices
		Core::Array<uint32> mChannelReaderMap;		// array of all multichannel indices of all port (for all ports: for all [multiplied] channels of port)
			
		// for formating strings locally
		Core::String mTempString;

	private:
		bool mUseMultiChannelMultiplication;
		bool mUseChannelNamePropagation;
		bool mUseChannelMetadataPropagation;
		bool mUseChannelColoring;
		bool mRequireInputConnection;
		bool mRequireAllInputConnections;
		bool mRequireMatchingSampleRates;
		bool mRequireConstantSampleRate;
		bool mRequireSyncedInput;

		// TODO get rid of this
		// returns true if input connections have changed
		//bool ValidateConnections();

		// check that the sample timing accuracy of multi input nodes is valid
		bool ValidateInputTiming();

		// Propagate channel names, colores etc from input to output (you may override this)
		virtual void PropagateChannelMetadata();
		
		// set output channel source names (you may override this)
		virtual void UpdateOutputChannelSourceNames();
	
		// update the output channel color (you may override this)
		virtual void UpdateOutputChannelColors();

};

#endif


