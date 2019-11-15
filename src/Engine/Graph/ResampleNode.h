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

#ifndef __NEUROMORE_RESAMPLENODE_H
#define __NEUROMORE_RESAMPLENODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"
#include "../DSP/ResampleProcessor.h"


class ENGINE_API ResampleNode : public ProcessorNode
{
	public:
		enum { TYPE_ID = 0x0028 };
		static const char* Uuid () { return "1b964922-ed96-11e4-90ec-1681e6b88ec1"; }
		
		enum
		{
			ATTRIB_SAMPLERATE		= 0,
			ATTRIB_MODE			 	= 1,
			ATTRIB_ALGORITHM		= 2,
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
		ResampleNode(Graph* graph);
		~ResampleNode();

		// init, reinit & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		
		void OnAttributesChanged() override;

		Core::Color GetColor() const override								{ return Core::RGBA(51,204,255); }
		uint32 GetType() const override										{ return TYPE_ID; }
		const char* GetTypeUuid() const override final						{ return Uuid(); }
		const char* GetReadableType() const override						{ return "Resample"; }
		const char* GetRuleName() const override final						{ return "NODE_Resample"; }
		uint32 GetPaletteCategory() const override							{ return CATEGORY_UTILS; }
		GraphObject* Clone(Graph* graph) override							{ ResampleNode* clone = new ResampleNode(graph); return clone; }

		const ChannelProcessor::Settings& GetSettings() override			{ return mSettings; }

	private:
		ResampleProcessor::Settings		mSettings;
};


#endif
