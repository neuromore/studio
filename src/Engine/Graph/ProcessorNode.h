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

#ifndef __NEUROMORE_PROCESSORNODE_H
#define __NEUROMORE_PROCESSORNODE_H

// include required headers
#include "../Config.h"
#include "SPNode.h"
#include "../DSP/ChannelProcessor.h"


// signal processing node
class ENGINE_API ProcessorNode : public SPNode
{
	public:

		enum { NODE_TYPE = 0x005 };

		// constructors & destructor
		ProcessorNode(Graph* parent, ChannelProcessor* processor);
		virtual ~ProcessorNode();

		// type management
		virtual uint32 GetNodeType() const override						{ return NODE_TYPE; }

		// init, reset, reinit and update
		virtual void Init() override;
		virtual void Reset() override;
		virtual void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		virtual void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		virtual const ChannelProcessor::Settings& GetSettings() = 0;	

		// DSP properties
		
		double GetDelay(uint32 inputPortIndex, uint32 outputPortIndex) const override;
		double GetLatency(uint32 inputPortIndex, uint32 outputPortIndex) const override;
		double GetSampleRatio (uint32 inputPortIndex, uint32 outputPortIndex) const override;
		// TODO deprecate this
		uint32 GetNumStartupSamples (uint32 inputPortIndex) const override;
		uint32 GetNumEpochSamples (uint32 inputPortIndex) const override;

		virtual Core::String& GetDebugString(Core::String& inout) override;

	protected:
		void Start(const Core::Time& elapsed) override;

		void SetupProcessors();
		void ReInitProcessors();
		bool ValidateConnections();

		Core::Array<ChannelProcessor*>	mProcessors;

	private:
		ChannelProcessor*				mProcessorPrototype;

};


#endif
