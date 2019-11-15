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

#ifndef __NEUROMORE_SESSIONINFONODE_H
#define __NEUROMORE_SESSIONINFONODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../DSP/ClockGenerator.h"
#include "InputNode.h"


class ENGINE_API SessionInfoNode : public InputNode
{
	public:
		enum { TYPE_ID = 0x0054 };
		static const char* Uuid () { return "29da805e-f44b-11e5-9ce9-5e5517507c66"; }
		
		enum 
		{ 
			OUTPUTPORT_TIME = 0,
			OUTPUTPORT_PAUSED,
			OUTPUTPORT_POINTS,
		};
		
		enum 
		{ 
			ATTRIB_TIMEUNIT = NUM_INPUTNODEATTRIBUTES,
			ATTRIB_SAMPLERATE
		};

		
		// constructor & destructor
		SessionInfoNode(Graph* graph);
		~SessionInfoNode();

		// initialize & update
		void Init() override;
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;

		// node information & helpers
		Core::Color GetColor() const override									{ return Core::RGBA(25,125, 53); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Session Info"; }
		const char* GetRuleName() const override final							{ return "NODE_SessionInfo"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_INPUT; }
		GraphObject* Clone(Graph* graph) override								{ SessionInfoNode* clone = new SessionInfoNode(graph); return clone; }
		
		uint32 GetNumSensors() const override									{ return 3; }
		Sensor* GetSensor(uint32 index)  override								{ return mSensors[index]; }
	
		void GenerateSamples() override;

		// available time units
		enum ETimeUnit
		{
			UNIT_MILLISECONDS		= 0,
			UNIT_SECONDS			= 1,
			UNIT_MINUTES			= 2,
			UNIT_HOURS				= 3,
			UNIT_NUM				= 4
		};
		static const char* GetTimeUnitString(ETimeUnit unit);
		static const char* GetTimeUnitStringShort(ETimeUnit unit);

	private:
		Core::Array<Sensor*> mSensors;
		Sensor				 mTimeSensor;
		Sensor				 mPauseSensor;
		Sensor				 mPointsSensor;
		ClockGenerator		 mClock;		// for creating aequidistant samples
};


#endif
