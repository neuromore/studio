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

#ifndef __NEUROMORE_BIQUADFILTERNODE_H
#define __NEUROMORE_BIQUADFILTERNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"
#include "../DSP/LinearFilterProcessor.h"


class ENGINE_API BiquadFilterNode : public ProcessorNode
{
	public:
		enum { TYPE_ID = 0x0025 };
		static const char* Uuid () { return "e50bc5c6-c333-11e4-8dfc-aa07a5b093db"; }

		enum
		{
			ATTRIB_B0				= 0,
			ATTRIB_B1				= 1,
			ATTRIB_B2				= 2,
			ATTRIB_A1				= 3,
			ATTRIB_A2				= 4,
			ATTRIB_GAIN				= 5,
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

		enum EError
		{
			ERROR_UNSTABLE_FILTER	= GraphObjectError::ERROR_RUNTIME | 0x01,
		};
		
		// constructor & destructor
		BiquadFilterNode(Graph* graph);
		~BiquadFilterNode();

		// initialize & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		
		void OnAttributesChanged() override;

		Core::Color GetColor() const override								{ return Core::RGBA(22,128,255); }
		uint32 GetType() const override										{ return TYPE_ID; }
		const char* GetTypeUuid() const override final						{ return Uuid(); }
		const char* GetReadableType() const override						{ return "Biquad Filter"; }
		const char* GetRuleName() const override final						{ return "NODE_BiquadFilter"; }
		uint32 GetPaletteCategory() const override							{ return CATEGORY_DSP; }
		GraphObject* Clone(Graph* graph) override							{ BiquadFilterNode* clone = new BiquadFilterNode(graph); return clone; }

		const ChannelProcessor::Settings& GetSettings() override			{ return mSettings; }


	private:
		LinearFilterProcessor::LinearFilterSettings		mSettings;

};


#endif
