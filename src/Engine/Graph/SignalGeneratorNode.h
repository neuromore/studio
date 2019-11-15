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

#ifndef __NEUROMORE_SIGNALGENERATORNODE_H
#define __NEUROMORE_SIGNALGENERATORNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../DSP/ClockGenerator.h"
#include "InputNode.h"

class ENGINE_API SignalGeneratorNode : public InputNode
{
	public:
		enum { TYPE_ID = 0x0023 };
		static const char* Uuid () { return "4107b810-bb6b-11e4-8dfc-aa07a5b093db"; }

		enum
		{
			OUTPUTPORT_CHANNEL  = 0,
		};

		enum
		{
			ATTRIB_SIGNALTYPE = NUM_INPUTNODEATTRIBUTES,
			ATTRIB_SAMPLERATE,
			ATTRIB_FREQUENCY,
			ATTRIB_AMPLITUDE,	
			ATTRIB_DCOFFSET,	
			ATTRIB_DUTYCYCLE,	
		};

		enum ESignalType
		{
			SIGNALTYPE_SINE, 
			SIGNALTYPE_SQUARE, 
			SIGNALTYPE_NOISE, 
			SIGNALTYPE_RAMP, 
			SIGNALTYPE_SAWTOOTH, 
			SIGNALTYPE_TRIANGLE, 
			SIGNALTYPE_BRAINWAVE, 
			NUM_SIGNALTYPES
		};

		static const char* GetSignalTypeName (ESignalType type);

		// constructor & destructor
		SignalGeneratorNode(Graph* graph);
		~SignalGeneratorNode();

		// main functions
		void Init() override;
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;
		
		// node information & helpers
		Core::Color GetColor() const override								{ return Core::RGBA(231, 19, 142); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Signal Generator"; }
		const char* GetRuleName() const override final							{ return "NODE_SignalGenerator"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_INPUT; }
		GraphObject* Clone(Graph* graph) override								{ SignalGeneratorNode* clone = new SignalGeneratorNode(graph); return clone; }

		uint32 GetNumSensors() const override									{ return 1; }
		Sensor* GetSensor(uint32 index)  override								{ return &mSensor; }

		void GenerateSamples() override;
		void UpdateSensorName();

	private:
		Sensor			mSensor;	// holds output channel
		ClockGenerator	mClock;		// for creating aequidistant samples

		void ShowAttributesForSignalType(ESignalType type);

	/////////////////////////////////////////////

		// rectangle function with duty cycle (value normalized to 0 .. 1)
		inline double RectangleWaveFunction (double time, double frequency, double dutycycle)
		{
			// relative position in time within one wave period
			const double t = Core::Math::FModD(time, 1.0 / frequency);
			
			// within one period, the signal is high first, then becomes low after dutycycle seconds
			if (t < dutycycle / frequency)
				return 1.0;
			else
				return 0.0;
		}

		// sawtooth function
		inline double SawtoothWaveFunction (double time, double frequency)
		{
			if (frequency == 0)
				return 0.0;

			// relative position in time within one wave period
			const double periodDuration = 1.0 / frequency;
			const double t = Core::Math::FModD(time, periodDuration);
			
			// within one period, the signal ramps up from 0 to 1
			return t / periodDuration;
		}


		// triangle function
		inline double TriangleWaveFunction (double time, double frequency)
		{
			if (frequency == 0)
				return 0.0;

			// relative position in time within one wave period
			const double periodDuration = 1.0 / frequency;
			const double t = Core::Math::FModD(time, periodDuration);
			
			// within one period, the signal ramps up from 0 to 1, then back down to 0
			const double halfPeriod = periodDuration / 2.0;
			double value = 0.0;
			value =  t / halfPeriod;

			// flip second half 
			if (t > halfPeriod)
				value = 2.0 - value;

			return value;
		}
};


#endif
