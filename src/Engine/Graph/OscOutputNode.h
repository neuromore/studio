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

#ifndef __NEUROMORE_OSCOUTPUTNODE_H
#define __NEUROMORE_OSCOUTPUTNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "OutputNode.h"


// forward declaration
class Classifier;

class ENGINE_API OscOutputNode: public OutputNode
{
	public:
		enum { TYPE_ID = 0x0035 };
		enum { OUTPUTNODE_TYPE = 0x0002 };
		static const char* Uuid() { return "15fee7ea-2a35-11e5-b345-feff819cdc9f"; }
		
		enum { INPUTPORT_VALUE		= 0 };
		
		enum
		{
			ATTRIB_OSCADDRESS		= NUM_BASEATTRIBUTES + 0
		};

		enum EError
		{
			ERROR_INVALID_OSC_ADDRESS	= GraphObjectError::ERROR_CONFIGURATION | 0x01,
			ERROR_DUPLICATE_OSC_ADDRESS = GraphObjectError::ERROR_CONFIGURATION | 0x02,
		};

		OscOutputNode(Graph* parentGraph);
		virtual ~OscOutputNode();

		// initialize & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Reset() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		void OnAttributesChanged() override;

		Core::Color GetColor() const override									{ return Core::RGBA(71,112,193); }
		uint32 GetType() const override											{ return TYPE_ID; }
		uint32 GetOutputNodeType() const override								{ return OUTPUTNODE_TYPE; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "OSC Output"; }
		const char* GetRuleName() const override final							{ return "NODE_OscOutput"; }
		GraphObject* Clone(Graph* graph) override								{ OscOutputNode* clone = new OscOutputNode(graph); return clone; }

		void SetName(const char* name) override;

		// OSC
		const char* GetOscAddress() const										{ return GetStringAttribute(ATTRIB_OSCADDRESS); }

		double GetCurrentValue() const											{ return mChannels[INPUTPORT_VALUE]->GetLastSample(); }
		bool IsEmpty() const													{ return mChannels[INPUTPORT_VALUE]->GetNumSamples() == 0; }

		void WriteOscMessage(OscPacketParser::OutStream* outStream);

		static Core::Color GetColor(uint32 index);

	private:
		Core::String GenerateUniqueOscAddress();
		bool IsOscAddressUnique(const char* address);

};

#endif
