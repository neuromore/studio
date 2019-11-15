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

#ifndef __NEUROMORE_GRAPHLINEARFILTERNODE_H
#define __NEUROMORE_GRAPHLINEARFILTERNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"
#include "../DSP/LinearFilterProcessor.h"


class ENGINE_API LinearFilterNode : public ProcessorNode
{
	public:
		enum { TYPE_ID = 0x0021 };
		static const char* Uuid () { return "70144f84-bb6a-11e4-8dfc-aa07a5b093db"; }
		
		enum
		{
			ATTRIB_FILTERTYPE		= 0,
			ATTRIB_FILTERMETHOD 	= 1,
			ATTRIB_FILTERORDER		= 2,
			ATTRIB_CASCADELENGTH	= 3,
			ATTRIB_LOWCUTFREQ		= 4,
			ATTRIB_HIGHCUTFREQ		= 5,
			ATTRIB_CHEBYSHEVRIPPLE  = 6
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
			ERROR_UNSTABLE_FILTER = GraphObjectError::ERROR_RUNTIME | 0x01,
		};

		enum EWarning
		{
			WARNING_FILTER_NOT_IMPLEMENTED = GraphObjectWarning::WARNING_CONFIGURATION & 0x01,
		};
		
		// constructor & destructor
		LinearFilterNode(Graph* graph);
		~LinearFilterNode();

		// init, reinit & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		
		void OnAttributesChanged() override;

		Core::Color GetColor() const override								{ return Core::RGBA(128,22,255); }
		uint32 GetType() const override										{ return TYPE_ID; }
		const char* GetTypeUuid() const override final						{ return Uuid(); }
		const char* GetReadableType() const override						{ return "IIR/FIR Filter"; }
		const char* GetRuleName() const override final						{ return "NODE_LinearFilter"; }
		uint32 GetPaletteCategory() const override							{ return CATEGORY_DSP; }
		GraphObject* Clone(Graph* graph) override							{ LinearFilterNode* clone = new LinearFilterNode(graph); return clone; }

		const ChannelProcessor::Settings& GetSettings() override			{ return mSettings; }

	private:
		LinearFilterProcessor::LinearFilterSettings		mSettings;
		
		void ShowAttributesForFilterType(Filter::EFilterType type);
};


#endif
