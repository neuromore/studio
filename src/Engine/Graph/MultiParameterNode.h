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

#ifndef __NEUROMORE_MULTIPARAMETERNODE_H
#define __NEUROMORE_MULTIPARAMETERNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../DSP/ClockGenerator.h"
#include "InputNode.h"


class ENGINE_API MultiParameterNode : public InputNode
{
	public:
		enum { TYPE_ID = 0x0049 };
		static const char* Uuid () { return "2c814b90-acab-11e5-bf7f-feff819cdc9f"; }
		
		enum { OUTPUTPORT_VALUE = 0 };
		enum
		{
			ATTRIB_SAMPLERATE = NUM_INPUTNODEATTRIBUTES,
			ATTRIB_VALUES,
		};

		// constructor & destructor
		MultiParameterNode(Graph* graph);
		~MultiParameterNode();

		// initialize & update
		void Init() override; 
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		
		void OnAttributesChanged() override;

		// node information & helpers
		Core::Color GetColor() const override									{ return Core::RGBA(211,122,255); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Multi Parameter"; }
		const char* GetRuleName() const override final							{ return "NODE_MultiParameter"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_INPUT; }
		GraphObject* Clone(Graph* graph) override								{ MultiParameterNode* clone = new MultiParameterNode(graph); return clone; }

		void CreateSensors();
		uint32 GetNumSensors() const override									{ return mSensors.Size(); }
		Sensor* GetSensor(uint32 index)  override								{ return &mSensors[index]; }

		void GenerateSamples() override;

	private:
		Core::Array<Sensor>			mSensors;		// output channels
		Core::Array<double>			mValues;		// current output values
		ClockGenerator				mClock;			// output clock
		double						mSampleRate;

};


#endif
