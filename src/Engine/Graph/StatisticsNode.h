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

#ifndef __NEUROMORE_STATISTICSNODE_H
#define __NEUROMORE_STATISTICSNODE_H

// include required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../Core/Array.h"
#include "../DSP/Channel.h"
#include "../DSP/StatisticsProcessor.h"
#include "ProcessorNode.h"


class ENGINE_API StatisticsNode : public ProcessorNode
{
	public:
		enum { TYPE_ID				= 0x0020 };
		static const char* Uuid () { return "ad13b064-bb6a-11e4-8dfc-aa07a5b093db"; }

		enum
		{
			ATTRIB_INTERVAL_TYPE = 0,
			ATTRIB_INTERVAL_DURATION,
			ATTRIB_INTERVAL_SIZE,
			ATTRIB_TYPE,
			ATTRIB_PERCENTILE,
			ATTRIB_EPOCHMODE,
			ATTRIB_ZEROPADDING
		};

		enum
		{
			INPUTPORT_CHANNEL		= 0,
			OUTPUTPORT_CHANNEL		= 0,
		};

		enum 
		{
			PORTID_INPUT_CHANNEL	= 0,
			PORTID_OUTPUT_CHANNEL	= 1,
		};

		enum
		{
			INTERVALTYPE_DURATION = 0,
			INTERVALTYPE_NUMSAMPLES = 1,
		};

		enum
		{
			EPOCHMODE_ON = 0,
			EPOCHMODE_OFF = 1,
		};

		// constructor & destructor
		StatisticsNode(Graph* graph);
		~StatisticsNode();

		// initialize & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;

		Core::Color GetColor() const override							{ return Core::RGBA(200,255,0); }
		uint32 GetType() const override										{ return TYPE_ID; }
		const char* GetTypeUuid() const override final						{ return Uuid(); }
		const char* GetReadableType() const override						{ return "Statistics"; }
		const char* GetRuleName() const override final						{ return "NODE_Statistics"; }
		uint32 GetPaletteCategory() const override							{ return CATEGORY_MATH; }
		GraphObject* Clone(Graph* graph) override							{ StatisticsNode* clone = new StatisticsNode(graph); return clone; }
		
		const ChannelProcessor::Settings& GetSettings() override			{ return mSettings; }

	protected:
		StatisticsProcessor::StatisticsSettings		mSettings;
};


#endif
