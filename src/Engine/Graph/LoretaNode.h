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

#ifndef __NEUROMORE_LORETANODE_H
#define __NEUROMORE_LORETANODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "SPNode.h"
#include "../EEGElectrodes.h"


class ENGINE_API LoretaNode : public SPNode
{
	public:
		enum { TYPE_ID = 0x0034 };
		static const char* Uuid () { return "5e75fae8-345f-11e5-a151-feff819cdc9f"; }
		
		//
		enum
		{
			INPUTPORT	= 0,
		};

		/*enum
		{
			ATTRIB_FOOBAR	= 0,
		};*/

		enum EError
		{
			ERROR_ELECTRODE_NAMES	= GraphObjectError::ERROR_CONFIGURATION | 0x01,
		};

		// constructor & destructor
		LoretaNode(Graph* graph);
		~LoretaNode();

		// initialize & update
		void Init() override;
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;

		void CollectElectrodeChannels();

		Core::Color GetColor() const override								{ return Core::RGBA(20, 110, 105); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "sLORETA"; }
		const char* GetRuleName() const override final							{ return "NODE_Loreta"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_MATH; }
		GraphObject* Clone(Graph* graph) override								{ LoretaNode* clone = new LoretaNode(graph); return clone; }

		// get the selected channels and the electrodes they belong to
		uint32 GetNumChannels() const											{ return mChannels.GetNumChannels(); }
		Channel<double>* GetChannel(uint32 index)								{ return static_cast<Channel<double>*>(mChannels.GetChannel(index)); /* cast is OK because channel is under control of the node itself */ }
		const EEGElectrodes::Electrode& GetElectrode(uint32 index) const		{ return mElectrodes[index]; }

		Core::String& GetDebugString(Core::String& inout) override;

	private:

		// the channels the loreta algorithm will use
		MultiChannel							mChannels;

		// one electrode per channel
		Core::Array<EEGElectrodes::Electrode>	mElectrodes;

};


#endif
