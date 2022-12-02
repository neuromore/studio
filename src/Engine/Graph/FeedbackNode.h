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

#ifndef __NEUROMORE_FEEDBACKNODE_H
#define __NEUROMORE_FEEDBACKNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "OutputNode.h"


class ENGINE_API FeedbackNode : public OutputNode
{
	public:
		enum { OUTPUTNODE_TYPE = 0x001 };
		enum
		{
			ATTRIB_SENDOSCNETWORKMESSAGES	= NUM_BASEATTRIBUTES + 0,
			ATTRIB_OSCADDRESS				= NUM_BASEATTRIBUTES + 1,
			NUM_FEEDBACKBASEATTRIBUTES		= NUM_BASEATTRIBUTES + 2
		};

		FeedbackNode(Graph* parentGraph);
		virtual ~FeedbackNode();

		virtual uint32 GetOutputNodeType() const override									{ return OUTPUTNODE_TYPE; }
		
		virtual void Init() override;
		virtual void OnAttributesChanged() override;

		// OSC
		virtual void WriteOscMessage(OscPacketParser::OutStream* outStream)					{}
		bool GetSendOscNetworkMessages() const												{ return GetBoolAttribute(ATTRIB_SENDOSCNETWORKMESSAGES); }

		virtual double GetRangeMin() const                                              	{ return 0.0; }
		virtual double GetRangeMax() const                                              	{ return 0.0; }

	private:
};

#endif
