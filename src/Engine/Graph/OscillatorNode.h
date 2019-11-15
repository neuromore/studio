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

#ifndef __NEUROMORE_OscillatorNode_H
#define __NEUROMORE_OscillatorNode_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"
#include "../DSP/ResampleProcessor.h"


class ENGINE_API OscillatorNode : public ProcessorNode
{
	private:
		class OscillatorProcessor;

	public:
		enum { TYPE_ID = 0x0048 };
		static const char* Uuid () { return "94b134ee-a834-11e5-bf7f-feff819cdc9f"; }
		
		enum
		{
			ATTRIB_SAMPLERATE = 0,
			ATTRIB_WAVEFORM,
			ATTRIB_DEFAULT_AMPLITUDE,
			ATTRIB_DEFAULT_FREQUENCY,
			ATTRIB_DEFAULT_PHASE,
		};
		
		enum
		{
			INPUTPORT_AMPLITUDE		= 0,
			INPUTPORT_FREQUENCY		= 1,
			INPUTPORT_PHASE			= 2,
			OUTPUTPORT_SIGNAL		= 0
		};

		// constructor & destructor
		OscillatorNode(Graph* graph);
		~OscillatorNode();

		// init, reinit & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		
		void OnAttributesChanged() override;

		Core::Color GetColor() const override							{ return Core::RGBA(51,204,255); }
		uint32 GetType() const override										{ return TYPE_ID; }
		const char* GetTypeUuid() const override final						{ return Uuid(); }
		const char* GetReadableType() const override						{ return "Oscillator"; }
		const char* GetRuleName() const override final						{ return "NODE_Oscillator"; }
		uint32 GetPaletteCategory() const override							{ return CATEGORY_DSP; }
		GraphObject* Clone(Graph* graph) override							{ OscillatorNode* clone = new OscillatorNode(graph); return clone; }

		const ChannelProcessor::Settings& GetSettings() override			{ return mSettings; }

	private:

		class OscillatorProcessor : public ChannelProcessor
		{
			public:
				enum EWaveformType
				{
					WAVEFORM_SINE,
					WAVEFORM_SQUARE,
					WAVEFORM_TRIANGLE,
					WAVEFORM_SAWTOOTH,
					NUM_WAVEFORMS
				};
			
				// constructors & destructor
				OscillatorProcessor();
				virtual ~OscillatorProcessor();

				uint32 GetType() const override											{ return TYPE_ID; }
				ChannelProcessor* Clone() override										{ OscillatorProcessor* clone = new OscillatorProcessor(); clone->Setup(mSettings); return clone; }

				void Init() override;
				void ReInit() override;
				void Update() override;
				void Update(const Core::Time& elapsed, const Core::Time& delta) override;

				// settings
				class Settings : public ChannelProcessor::Settings
				{
					public:
						enum { TYPE_ID = 0x0048 };

						Settings()									{}
						virtual ~Settings()							{}

						uint32 GetType() const override				{ return TYPE_ID; }
						
						double				mSampleRate;	// specified output sample rate
						EWaveformType		mWaveformType;	// waveform type
						double				mDefaultAmplitude;
						double				mDefaultFrequency;
						double				mDefaultPhase;
						Core::Time			mStartTime;		// clock start time
				};

				void Setup(const ChannelProcessor::Settings& settings) override			{ mSettings = static_cast<const Settings&>(settings); }
				const Settings& GetSettings() const	override							{ return mSettings; }

				// configure processor
				void SetSampleRate(double sampleRate)									{ mSettings.mSampleRate = sampleRate; }
				void SetWaveform(EWaveformType waveform)								{ mSettings.mWaveformType = waveform; }
				void SetStartTime(Core::Time startTime)									{ mSettings.mStartTime = startTime; }

			private:
				Settings			mSettings;
				ClockGenerator		mOutputClock;
				
				double				mPhaseAccumulator;		// required for perfect smooth swept wave generation

				// waveform function pointer
				typedef double (CORE_CDECL OscillatorProcessor::*WaveformFunction)(double phi);

				WaveformFunction GetWaveformFunction(EWaveformType waveform);

				double CORE_CDECL SineWaveFunction(double phi);
				double CORE_CDECL SquareWaveFunction(double phi);
				double CORE_CDECL TriangleWaveFunction(double phi);
				double CORE_CDECL SawtoothWaveFunction(double phi);
		};

		OscillatorProcessor::Settings		mSettings;

};


#endif
