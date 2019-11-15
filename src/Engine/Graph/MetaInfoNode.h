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

#ifndef __NEUROMORE_METAINFONODE_H
#define __NEUROMORE_METAINFONODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../DSP/ClockGenerator.h"
#include "InputNode.h"


class ENGINE_API MetaInfoNode : public InputNode
{
	public:
		enum { TYPE_ID = 0x0045 };
		static const char* Uuid () { return "76d647ce-96dd-11e5-8994-feff819cdc9f"; }
		
		enum 
		{
			INPUTPORT_VALUE	 = 0,
			OUTPUTPORT_VALUE = 0 
		};

		enum
		{
			ATTRIB_INFOTYPE = NUM_INPUTNODEATTRIBUTES,
			ATTRIB_SAMPLERATE
		};

		// constructor & destructor
		MetaInfoNode(Graph* graph);
		~MetaInfoNode();

		// initialize & update
		void Init() override; 
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		
		void OnAttributesChanged() override;

		// node information & helpers
		Core::Color GetColor() const override									{ return Core::RGBA(0,159,227); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Meta Info"; }
		const char* GetRuleName() const override final							{ return "NODE_MetaInfo"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_UTILS; }
		GraphObject* Clone(Graph* graph) override								{ MetaInfoNode* clone = new MetaInfoNode(graph); return clone; }

		uint32 GetNumSensors() const override									{ return 1; }
		Sensor* GetSensor(uint32 index)  override								{ return &mSensor; }

		void GenerateSamples() override;

	private:
		Sensor			mSensor;			// holds output channel		
		ClockGenerator	mClock;				// for creating aequidistant samples

		void UpdateSensorName();

		enum EInfoType
		{
			INFOTYPE_MULTICHANNEL_NUMCHANNELS,

			INFOTYPE_CHANNEL_SAMPLERATE,
			INFOTYPE_CHANNEL_SAMPLECOUNTER,
			INFOTYPE_CHANNEL_NUMSAMPLES,
			INFOTYPE_CHANNEL_NUMNEWSAMPLES,
			INFOTYPE_CHANNEL_BUFFERSIZE,
			INFOTYPE_CHANNEL_ELAPSEDTIME,
			INFOTYPE_CHANNEL_LASTSAMPLETIME,
			INFOTYPE_CHANNEL_STARTTIME,
			INFOTYPE_CHANNEL_ISINDEPENDENT,
			INFOTYPE_CHANNEL_LATENCY,

			INFOTYPE_CLASSIFIER_BUFFERUSAGE,
			INFOTYPE_CLASSIFIER_MAXLATENCY,

			INFOTYPE_ENGINE_UPDATE_MS,
			INFOTYPE_ENGINE_CLASSIFIER_MS,

			// TODO add more values during debugging 

			NUM_INFOTYPES
		};

		const char* GetInfoTypeString(EInfoType type);
		const char* GetShortInfoTypeString(EInfoType type);

		EInfoType mInfoType;

		double GetInfoValue(EInfoType type);
};


#endif
