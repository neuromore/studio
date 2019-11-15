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

#ifndef __NEUROMORE_OSCINPUTNODE_H
#define __NEUROMORE_OSCINPUTNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../Networking/OscReceiver.h"
#include "InputNode.h"
#include "../Sensor.h"

class ENGINE_API OscInputNode : public InputNode, public OscReceiver
{
	public:
		enum { TYPE_ID = 0x0022 };
		static const char* Uuid () { return "15fd9b62-bb6b-11e4-8dfc-aa07a5b093db"; }

		enum 
		{ 
			ATTRIB_OSCADDRESS = NUM_INPUTNODEATTRIBUTES,
			ATTRIB_SAMPLERATE
		};

		enum EError
		{
			ERROR_INVALID_OSC_ADDRESS	= GraphObjectError::ERROR_CONFIGURATION | 0x01,
			ERROR_DUPLICATE_OSC_ADDRESS = GraphObjectError::ERROR_CONFIGURATION | 0x02,
		};
		
		enum { OUTPUTPORT = 0 };

		// constructor & destructor
		OscInputNode(Graph* graph);
		~OscInputNode();

		// initialize & update
		void Init() override;
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;

		// node information & helpers
		Core::Color GetColor() const override									{ return Core::RGBA(71,112,193); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "OSC Input"; }
		const char* GetRuleName() const override final							{ return "NODE_OscInput"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_INPUT; }
		GraphObject* Clone(Graph* graph) override								{ OscInputNode* clone = new OscInputNode(graph); return clone; }

		uint32 GetNumSensors() const override									{ return 1; }
		Sensor* GetSensor(uint32 index)  override								{ return mSensor; }

		// osc stuff
		virtual void ProcessMessage(OscMessageParser* message) override;

	private:
		Core::String GenerateUniqueOscAddress();
		bool IsOscAddressUnique(const char* address);
		void UpdateChannelAndPortNames();

		Sensor*			mSensor;

		Core::String	mTempString;
};


#endif
