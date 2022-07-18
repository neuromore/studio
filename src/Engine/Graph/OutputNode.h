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

#ifndef __NEUROMORE_OUTPUTNODE_H
#define __NEUROMORE_OUTPUTNODE_H

// include required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../Core/Array.h"
#include "../DSP/Channel.h"
#include "../DSP/ResampleProcessor.h"
#include "../Networking/OscPacketParser.h"
#include "SPNode.h"


class ENGINE_API OutputNode : public SPNode
{
	public:
		enum { NODE_TYPE			= 0x004 };

		enum
		{
			ATTRIB_SIGNALRESOLUTION = 0,
			ATTRIB_UPLOAD			= 1,
			NUM_BASEATTRIBUTES		= 2
		};

		enum SignalResolution
		{
			RESOLUTION_RAW	= 0,
			RESOLUTION_HIGH	= 1,
			RESOLUTION_MID	= 2,
			RESOLUTION_LOW	= 3,
			RESOLUTION_NUM	= 4
		};
		
		OutputNode(Graph* parentGraph);
		virtual ~OutputNode();
		
		// type management
		virtual uint32 GetNodeType() const override											{ return NODE_TYPE; }
		virtual uint32 GetOutputNodeType() const = 0;
		uint32 GetPaletteCategory() const override final									{ return CATEGORY_OUTPUT; }

		void Init() override;
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;

		// synchronization
		void Sync(double syncTime) override;
		
		// get signal resolution
		SignalResolution GetSignalResolution() const										{ return (SignalResolution)GetInt32Attribute(ATTRIB_SIGNALRESOLUTION); }

		// get upload flag
		bool IsUploadEnabled() const														{ return GetBoolAttribute(ATTRIB_UPLOAD); }

		// update output resamplers
		void UpdateResamplers(const Core::Time& elapsed, const Core::Time& delta);

		// channels
		void AddOutput();
		uint32 GetNumOutputChannels() const													{ return mChannels.Size(); }
		Channel<double>* GetOutputChannel(uint32 index) 									{ return mChannels[index]; }
		
		// access output values
		// TODO: merge this with GetNumOutputChannels() when adding full multi-channel output support to the engine
		uint32 GetNumCurrentValues() const;
		double GetCurrentValue(uint32 channelIndex = 0) const;
		bool IsEmpty(uint32 channelIndex = 0) const;

		// helpers
		bool IsValidInput(uint32 portIndex);

		// burst size (number of samples added during the last update)
		uint32 GetLastBurstSize(uint32 portIndex) const										{ return mLastBurstSizes[portIndex]; }

		// overriden, so we can include the node's internal processor delays
		virtual double FindMaximumDelayForInput(uint32 inputPortIndex) override;
		virtual double FindMaximumLatencyForInput(uint32 inputPortIndex) override;
		
		// DSP properties (from resampling)
		double GetDelay(uint32 inputPortIndex, uint32 outputPortIndex) const override;
		double GetLatency(uint32 inputPortIndex, uint32 outputPortIndex) const override;

		// memory usage
		uint32 CalculateOutputMemoryUsed();

		virtual Core::String& GetDebugString(Core::String& inout) override;

		double GetSampleRate(SignalResolution resolution, ChannelBase* reference);

	protected:
		// channel for storing all input samples
		Core::Array< Channel<double>* > mChannels;

		// get the channel that should be resampled (can be overridden to implement processing in the output node)
		virtual Channel<double>* GetRawInputChannel(uint32 portIndex);


	private:
		Core::Array<ResampleProcessor>  mResamplers;
		Core::Array<uint32>				mLastBurstSizes;
		
		// signal resolution 
		const char* GetResolutionString(SignalResolution resolution);
		SignalResolution mCurrentResolution;

};


#endif
