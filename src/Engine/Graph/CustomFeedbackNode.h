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

#ifndef __NEUROMORE_CUSTOMFEEDBACKNODE_H
#define __NEUROMORE_CUSTOMFEEDBACKNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "FeedbackNode.h"


// forward declaration
class Classifier;

class ENGINE_API CustomFeedbackNode : public FeedbackNode
{
	public:
		enum { TYPE_ID				= 0x0004 };
		static const char* Uuid () { return "27679c9e-bb6c-11e4-8dfc-aa07a5b093db"; }
		
		enum { INPUTPORT_VALUE		= 0 };
		
		enum
		{
			ATTRIB_USERID = NUM_FEEDBACKBASEATTRIBUTES,
			ATTRIB_ISRANGED,
			ATTRIB_RANGEMIN,
			ATTRIB_RANGEMAX,
		};
		enum EError
		{
			ERROR_DUPLICATE_OSC_ADDRESS	= GraphObjectError::ERROR_CONFIGURATION | 0x01,
			ERROR_VALUE_RANGE			= GraphObjectError::ERROR_RUNTIME		| 0x02,
		};

		enum EWarning
		{
			WARNING_DEPRECATED_NODE_NAME			= GraphObjectWarning::WARNING_CUSTOM	| 0x01,
		};

		CustomFeedbackNode(Graph* parentGraph);
		virtual ~CustomFeedbackNode();

		// initialize & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Reset() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		void OnAttributesChanged() override;

		Core::Color GetColor() const override;
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override								{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Custom Feedback"; }
		const char* GetRuleName() const override 								{ return "NODE_Feedback"; }
		GraphObject* Clone(Graph* graph) override								{ CustomFeedbackNode* clone = new CustomFeedbackNode(graph); return clone; }

		// accessors
		bool IsRanged() const													{ return GetBoolAttribute(ATTRIB_ISRANGED); }
		double GetRangeMin() const override										{ return GetFloatAttribute(ATTRIB_RANGEMIN); }
		double GetRangeMax() const override										{ return GetFloatAttribute(ATTRIB_RANGEMAX); }

		// OSC
		const char* GetOscAddress() const										{ return GetStringAttribute(ATTRIB_OSCADDRESS); }
		bool IsOscAddressUnique(const char* address);

		void WriteOscMessage(OscPacketParser::OutStream* outStream) override;

		static Core::Color GetColor(uint32 index);

	private:
		Core::String GenerateUniqueOscAddress();
};

#endif
