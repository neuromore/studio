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

#ifndef __NEUROMORE_HRVNODE_H
#define __NEUROMORE_HRVNODE_H

// include the required headers
#include "../Config.h"
#include "../DSP/HrvProcessor.h"
#include "ProcessorNode.h"


// heart rate variability node
class ENGINE_API HrvNode : public ProcessorNode
{
	public:
		enum { TYPE_ID = 0x0039 };
		static const char* Uuid () { return "b56e59fc-65d9-11e5-9d70-feff819cdc9f"; }
		
		enum
		{
			ATTRIB_METHOD			= 0,
			ATTRIB_NUM_RR_INTERVALS = 1
		};
		
		enum
		{
			INPUTPORT_CHANNEL		= 0,
			OUTPUTPORT_CHANNEL		= 0
		};

		enum 
		{
			PORTID_INPUT			= 0,
			PORTID_OUTPUT			= 1
		};
		
		// constructor & destructor
		HrvNode(Graph* graph);
		~HrvNode();

		// init, reinit & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		
		void OnAttributesChanged() override;

		Core::Color GetColor() const override								{ return Core::RGBA(255, 115, 42); }
		uint32 GetType() const override										{ return TYPE_ID; }
		const char* GetTypeUuid() const override final						{ return Uuid(); }
		const char* GetReadableType() const override						{ return "HRV"; }
		const char* GetRuleName() const override final						{ return "NODE_HrvAnalysis"; }
		uint32 GetPaletteCategory() const override							{ return CATEGORY_BIO; }
		GraphObject* Clone(Graph* graph) override							{ HrvNode* clone = new HrvNode(graph); return clone; }

		const ChannelProcessor::Settings& GetSettings() override			{ return mSettings; }

	public:
		HrvProcessor::Settings mSettings;
};


#endif
