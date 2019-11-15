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

#ifndef __NEUROMORE_SWITCHNODE_H
#define __NEUROMORE_SWITCHNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"
#include "../DSP/ChannelProcessor.h"


class ENGINE_API SwitchNode : public SPNode
{
	public:
		enum { TYPE_ID = 0x0058 };
		static const char* Uuid () { return "d5a5272e-6569-11e6-8b77-86f30ca893d3"; }

		enum
		{
			INPUTPORT_CONTROL = 0,
			INPUTPORT_SWITCHPORT0 = 1,
			INPUTPORT_SWITCHPORT1 = 2,
			OUTPUTPORT = 0,
		};

		enum 
		{
			ATTRIB_NUMINPUTPORTS	= 0,
		};

		enum EError
		{
			ERROR_CONTROLPORT_MISSMATCH = GraphObjectError::ERROR_CONFIGURATION | 0x01,
			ERROR_SWITCHPORT_MISSMATCH = GraphObjectError::ERROR_CONFIGURATION | 0x02
		};


		// constructor & destructor
		SwitchNode(Graph* graph);
		~SwitchNode();

		// initialize & update
		void Init() override;
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;

		void OnAttributesChanged() override;

		Core::Color GetColor() const override									{ return Core::RGBA(147,97,176); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Switch"; }
		const char* GetRuleName() const override final							{ return "NODE_Switch"; } // NODE_Switch
		uint32 GetPaletteCategory() const override								{ return CATEGORY_UTILS; }
		GraphObject* Clone(Graph* graph) override								{ SwitchNode* clone = new SwitchNode(graph); return clone; }

	private:
		void UpdateInputPorts();
		void DeleteOutputChannels();


};


#endif
