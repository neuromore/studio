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

#ifndef __NEUROMORE_BODYFEEDBACKNODE_H
#define __NEUROMORE_BODYFEEDBACKNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "FeedbackNode.h"


class ENGINE_API BodyFeedbackNode : public FeedbackNode
{
	public:
		enum { TYPE_ID				= 0x0104 };
		static const char* Uuid () { return "8efd1d32-e8e1-11e4-b02c-1681e6b88ec1"; }
		
		enum
		{
			INPUTPORT_HEARTRATE		= 0,
			INPUTPORT_HRV			= 1,
			INPUTPORT_BREATHINGRATE	= 2
		};
		
		enum
		{
			ATTRIB_USERID = NUM_FEEDBACKBASEATTRIBUTES
		};

		BodyFeedbackNode(Graph* parentGraph);
		~BodyFeedbackNode();

		// initialize & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Reset() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		Core::Color GetColor() const override									{ return Core::RGBA(45,224,0); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Body Feedback"; }
		const char* GetRuleName() const override final							{ return "NODE_BodyFeedback"; }
		GraphObject* Clone(Graph* graph) override								{ BodyFeedbackNode* clone = new BodyFeedbackNode(graph); return clone; }
		bool IsDeprecated() const override final								{ return true; }

		void WriteOscMessage(OscPacketParser::OutStream* outStream) override;

		double GetHeartRate() const												{ return mChannels[INPUTPORT_HEARTRATE]->GetLastSample(); }
		double GetHRV() const													{ return mChannels[INPUTPORT_HRV]->GetLastSample(); }
		double GetBreathingRate() const											{ return mChannels[INPUTPORT_BREATHINGRATE]->GetLastSample(); }
};

#endif
