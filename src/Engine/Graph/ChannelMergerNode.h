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

#ifndef __NEUROMORE_CHANNELMERGERNODE_H
#define __NEUROMORE_CHANNELMERGERNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"
#include "../DSP/ChannelProcessor.h"


class ENGINE_API ChannelMergerNode : public SPNode
{
	public:
		enum { TYPE_ID = 0x0032 };
		static const char* Uuid () { return "fd1692e8-047b-11e5-8418-1697f925ec7b"; }

		enum
		{
			OUTPUTPORT_MERGED	= 0,
		};

		enum 
		{
			ATTRIB_NUMINPUTPORTS	= 0,
		};

		// constructor & destructor
		ChannelMergerNode(Graph* graph);
		~ChannelMergerNode();

		// initialize & update
		void Init() override;
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;

		void OnAttributesChanged() override;

		Core::Color GetColor() const override								{ return Core::RGBA(250,223,22); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Merge Channels"; }
		const char* GetRuleName() const override final							{ return "NODE_ChannelMerger"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_UTILS; }
		GraphObject* Clone(Graph* graph) override								{ ChannelMergerNode* clone = new ChannelMergerNode(graph); return clone; }

	private:
		void EnsureFreeInputPort();
		void UpdateInputPorts();
		void DeleteOutputChannels();


};


#endif
