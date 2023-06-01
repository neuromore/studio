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

#ifndef __NEUROMORE_CHANNELSELECTORNODE_H
#define __NEUROMORE_CHANNELSELECTORNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "SPNode.h"


class ENGINE_API ChannelSelectorNode : public SPNode
{
	public:
		static constexpr const uint32 NUMPORTSDEFAULT = 16;

		enum { TYPE_ID = 0x0031 };
		static const char* Uuid () { return "fd168f5a-047b-11e5-8418-1697f925ec7b"; }

		enum
		{
			INPUTPORT_SET     = 0,
		};

		enum
		{
			ATTRIB_NUMOUTPUTPORTS = 0,
			ATTRIB_CHANNELNAMES   = 1,
			ATTRIB_SINGLE_OUTPUT  = 2,
			ATTRIB_QUICK_CONFIG   = 3
		};

		enum EError
		{
			ERROR_CHANNEL_NOT_FOUND = GraphObjectError::ERROR_CONFIGURATION | 0x01,
		};

		struct Mapping
		{
			Channel<double>* in;
			Channel<double>* out;
			Core::String     name;
			Mapping(Channel<double>* in = 0, Channel<double>* out = 0, const Core::String& name = "") : 
				in(in), out(out), name(name) { }
		};

		// constructor & destructor
		ChannelSelectorNode(Graph* graph);
		~ChannelSelectorNode();

		// initialize & update
		void Init() override;
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
	
		void OnAttributesChanged() override;

		Core::Color GetColor() const override							{ return Core::RGBA(89,123,138); }
		uint32 GetType() const override									{ return TYPE_ID; }
		const char* GetTypeUuid() const override final					{ return Uuid(); }
		const char* GetReadableType() const override					{ return "Select Channels"; }
		const char* GetRuleName() const override final					{ return "NODE_ChannelSelector"; }
		uint32 GetPaletteCategory() const override						{ return CATEGORY_UTILS; }
		GraphObject* Clone(Graph* graph) override						{ ChannelSelectorNode* clone = new ChannelSelectorNode(graph); return clone; }

		// check if input channels are selected (forwarded) to the output
		bool IsChannelSelected(const ChannelBase* channel) const;

	private:
		void CollectSelectedInputChannels();				// collect all channels that match the provided selection into mSelectedInputChannels array

		void ReInitOutputPorts();							// create numPorts output ports and connect the 
		void ReInitOutputChannels();						// create the output channels (one for each selected input channel) and connect them to the outputs
		void DeleteOutputChannels();

		Core::Array<Mapping> mMapping;						// mapping between input and output channels
};


#endif
