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

#ifndef __NEUROMORE_CHANNELPROCESSOR_H
#define __NEUROMORE_CHANNELPROCESSOR_H

// include required headers
#include "../Config.h"
#include "Channel.h"
#include "ChannelReader.h"
#include "../Core/Time.h"

class ENGINE_API ChannelProcessor
{
	CORE_LOGTRACE_DISABLE("ChannelProcessor", "", this)
	CORE_LOGDEBUG_DISABLE("ChannelProcessor", "", this)

	public:
		class Settings
		{
		  public:
			virtual ~Settings() {};
			virtual uint32 GetType() const	{ return 0; }
		};

		// constructors & destructor
		ChannelProcessor();
		virtual ~ChannelProcessor();

		virtual uint32 GetType() const = 0;
		virtual ChannelProcessor* Clone() = 0;

		virtual void Init() = 0;
		virtual void ReInit();

		// update processor
		virtual void Update();
		virtual void Update(const Core::Time& elapsed, const Core::Time& delta)					{ Update(); }

		// setup processor
		virtual void Setup(const Settings& settings) = 0;
		virtual const Settings& GetSettings() const	= 0;

		// if the processor is in working condition
		bool IsInitialized() const																{ return mIsInitialized;  }

		//
		// Inputs	
		//

		uint32 GetNumInputs() const																{ return mInputs.Size();	}
		
		// connect inputs
		void SetInput(ChannelBase* channel, uint32 index = 0)									{ CORE_ASSERT(mIsOwnInputReader[index] == true); mInputs[index]->SetChannel(channel); }
		ChannelBase* GetInput(uint32 index = 0) const;
		ChannelReader* GetInputReader(uint32 index = 0) const									{ return mInputs[index];	}

		// replace the reader with something externally controlled 
		void SetDelegateInputReader(uint32 index, ChannelReader* reader);
		void RemoveDelegateInputReader(uint32 index);

		//
		// Outputs
		//

		uint32 GetNumOutputs() const															{ return mOutputs.Size();	}
		ChannelBase* GetOutput(uint32 index = 0) const											{ return mOutputs[index];		}

		//
		// DSP related properties
		//
		
		// the delay for sample alignment): the processor needs to consume this many additional input samples until it can begin outputting the first valid sample
		virtual uint32 GetDelay(uint32 inputPortIndex, uint32 outputPortIndex) const			{ return 0; }
		
		// the latency that is imposed on the signal: a stimulus on the input produces an effect on the output after this many second
		virtual double GetLatency(uint32 inputPortIndex, uint32 outputPortIndex) const			{ return 0; }

		// the number of samples we read for each produced output sample. May be a fraction.
		virtual double GetSampleRatio (uint32 inputPortIndex, uint32 outputPortIndex) const		{ return 1.0; }			// unused right now

		// TODO deprecate this
		// number of samples the input has to read before it can produce the _first_ output sample 
		virtual uint32 GetNumStartupSamples (uint32 inputPortIndex) const						{ return 1; }

		// number of samples the input will read at once
		virtual uint32 GetNumEpochSamples(uint32 inputPortIndex) const							{ return 1; }


	protected: 
		template <class T> void AddInput()														{ mInputs.Add(new ChannelReader()); mIsOwnInputReader.Add(true);}
		template <class T> void AddOutput()														{ mOutputs.Add(new Channel<T>(0,123)); }
		
		bool						mIsInitialized;
		
	private:
		Settings					mProcessorSettings;

		// Inputs and owner flags
		Core::Array<ChannelReader*>	mInputs;
		Core::Array<bool> mIsOwnInputReader;
		
		// Outputs
		Core::Array<ChannelBase*>	mOutputs;
		
};


#endif
