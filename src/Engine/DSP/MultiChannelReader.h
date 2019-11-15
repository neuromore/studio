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

#ifndef __NEUROMORE_MULTICHANNELREADER_H
#define __NEUROMORE_MULTICHANNELREADER_H

// include required headers
#include "../Config.h"
#include "MultiChannel.h"
#include "ChannelReader.h"


// helper class for reading from multichannels and detecting changes in the input
class ENGINE_API MultiChannelReader
{
	CORE_LOGTRACE_DISABLE("MultiChannelReader", "", this);
	CORE_LOGDEBUG_DISABLE("MultiChannelReader", "", this);

	public:
		// constructors & destructor
		MultiChannelReader(MultiChannel* inputChannels = NULL);
		virtual ~MultiChannelReader();

		// reinit and update
		void Reset();
		void Update();

		// input channel to read from
		void SetInput(MultiChannel* inputChannel);
		MultiChannel* GetChannels()											{ return mInputChannels; }
		
		// size of the channel set and reader
		uint32 GetNumChannels()	const										{ return mInputChannels->GetNumChannels(); }
		
		// access channel reader by index or by channel
		ChannelBase* GetChannel(uint32 index);
		const ChannelBase* GetChannel(uint32 index) const;
		ChannelReader* GetReader(uint32 index);

		// find reader for a given channel
		ChannelReader* FindReader(const ChannelBase* channel);

		// start reading synchronized, beginning the given time
		void Start(const Core::Time& time);

		// get min/max tims of last sample across all channels
		Core::Time FindMinLastSampleTime(); 
		Core::Time FindMaxLastSampleTime();

		// calculate the timing difference of the channel readers
		Core::Time CalcReaderSyncOffset();

		// check if sample rates of all channels are identical
		bool HasUniformSampleRate();
		double GetSampleRate();

		// get number of samples that can be read from all channels
		uint32 GetMinNumNewSamples() const;

		// advance the channel readers (separately or all together)
		void Advance(uint32 numSamples);
		void Flush(bool independent = false);

		// detection of changes in the input channel
		void DetectInputChanges();
		bool HasInputChanged(ChannelReader::EChangeType type = ChannelReader::ANY) const;

	protected:									
		MultiChannel*				mInputChannels;
		bool						mInputChangeDetected;						
		Core::Array<ChannelReader>	mChannelReaders;
};

#endif
