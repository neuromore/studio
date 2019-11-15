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

#ifndef __NEUROMORE_WAVEFORMNODE_H
#define __NEUROMORE_WAVEFORMNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"
#include "../DSP/ClockGenerator.h"


class ENGINE_API WaveformNode : public ProcessorNode
{
	private:
		class WaveformProcessor;

	public:
		enum { TYPE_ID = 0x0051 };
		static const char* Uuid () { return "6a24b1d6-b4e3-11e5-9f22-ba0be0483c18"; }
		
		enum
		{
			ATTRIB_SAMPLERATE = 0,
			ATTRIB_WAVEFORM,
			ATTRIB_WAVEFORM_TYPE,
			ATTRIB_PLAYMODE,
			ATTRIB_CONTINOUSOUTPUT,
		};
		
		enum
		{
			INPUTPORT_ACTIVATE = 0,
			OUTPUTPORT_SIGNAL  = 0
		};

		// constructor & destructor
		WaveformNode(Graph* graph);
		~WaveformNode();

		// init, reinit & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		
		void OnAttributesChanged() override;

		Core::Color GetColor() const override								{ return Core::RGBA(251,94,55); }
		uint32 GetType() const override										{ return TYPE_ID; }
		const char* GetTypeUuid() const override final						{ return Uuid(); }
		const char* GetReadableType() const override						{ return "Custom Waveform"; }
		const char* GetRuleName() const override final						{ return "NODE_CustomWaveform"; }
		uint32 GetPaletteCategory() const override							{ return CATEGORY_DSP; }
		GraphObject* Clone(Graph* graph) override							{ WaveformNode* clone = new WaveformNode(graph); return clone; }

		const ChannelProcessor::Settings& GetSettings() override			{ return mSettings; }

	private:

		class WaveformProcessor : public ChannelProcessor
		{
			public:
				enum { TYPE_ID = 0x0051 };

				enum EPlayMode
				{
					PLAY_ALWAYS,
					PLAY_ONCE_IF_SAMPLE,
					PLAY_ONCE_IF_TRUE,
					PLAY_WHILE_TRUE,
				};

				enum EWaveformType
				{
					WAVEFORM_STEPS,
					WAVEFORM_LINEAR,
					//WAVEFORM_BICUBIC,
					//WAVEFORM_BSPLINE
				};
			
				// constructors & destructor
				WaveformProcessor();
				virtual ~WaveformProcessor();

				uint32 GetType() const override											{ return TYPE_ID; }
				ChannelProcessor* Clone() override										{ WaveformProcessor* clone = new WaveformProcessor(); clone->Setup(mSettings); return clone; }

				void Init() override;
				void ReInit() override;
				void Update(const Core::Time& elapsed, const Core::Time& delta) override;


				// settings
				class Settings : public ChannelProcessor::Settings
				{
					public:
						enum { TYPE_ID = 0x0051 };

						Settings()									{}
						virtual ~Settings()							{}

						uint32 GetType() const override				{ return TYPE_ID; }
						
						double				mSampleRate;		// specified output sample rate
						Core::String		mWaveformDefinition;// waveform in text form
						EWaveformType		mWaveformType;		// waveform type
						EPlayMode			mPlayMode;			// play type
						bool				mContinuousOutput;	// continously output samples
						Core::Time			mStartTime;			// clock start time
				};

				void Setup(const ChannelProcessor::Settings& settings) override			{ mSettings = static_cast<const Settings&>(settings); }
				const Settings& GetSettings() const	override							{ return mSettings; }

				// configure processor
				void SetSampleRate(double sampleRate)									{ mSettings.mSampleRate = sampleRate; }
				void SetWaveformDefinition(const char* definition)						{ mSettings.mWaveformDefinition = definition; }
				void SetWaveformType(EWaveformType type)								{ mSettings.mWaveformType = type; }
				void SetPlayMode(EPlayMode playMode)									{ mSettings.mPlayMode = playMode; }
				void SetContinousOutput(bool enable)									{ mSettings.mContinuousOutput = enable; }
				void SetStartTime(Core::Time startTime)									{ mSettings.mStartTime = startTime; }

			private:
				Settings			mSettings;
				ClockGenerator		mOutputClock;

				bool				mIsPlaying;						// if waveform is played back (in sync with output clock)
				bool				mPlayedOnce;					// if waveform was already played once

				void ParseWaveformString(const Core::String& waveformString, Core::Array<double>& outValues, Core::Array<double>& outDurations);
				void RecalculateWaveform();

				uint32					mNumElements;				// number of points in the waveform
				Core::Array<double>		mValues;					// all values
				Core::Array<double>		mDurations;					// all durations
				Core::Array<double>		mTimestamps;				// summed up durations
				double					mWaveformLength;			// total length in seconds

		};

		WaveformProcessor::Settings		mSettings;

};


#endif
