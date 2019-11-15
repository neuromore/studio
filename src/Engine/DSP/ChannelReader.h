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

#ifndef __NEUROMORE_CHANNELREADER_H
#define __NEUROMORE_CHANNELREADER_H

// include required headers
#include "../Config.h"
#include "ChannelBase.h"
#include "Epoch.h"
#include "../Core/LogManager.h"

// A Helper class for detecting new samples in channels and accessing them
class ENGINE_API ChannelReader
{
	friend class MultiChannelReader;

	CORE_LOGTRACE_DISABLE("ChannelReader", "", this);
	CORE_LOGDEBUG_DISABLE("ChannelReader", "", this);

	public:
		// constructors & destructor
		ChannelReader(ChannelBase*  inputChannel = NULL);
		virtual ~ChannelReader();

		// reset
		void Reset();				// also resets detection flags
		void ResetCounters();		// does not reset flags

		// start reading at the given point in time
		void Start(const Core::Time& time);

		// call this regulary 
		void Update();

		// set/get the input channel
		void SetChannel(ChannelBase* inputChannel);
		bool HasChannel() const													{ return mChannel != NULL; }
		ChannelBase* GetChannel()												{ return mChannel; }
		const ChannelBase* GetChannel() const									{ return mChannel; }
		
		// accessors for incoming samples
		uint32 GetNumNewSamples() const											{ return mNumNewSamples; }
		void   Flush()															{ Advance(mNumNewSamples); }
		void   Advance(uint32 numSamples);

		template <class T> const T& GetSample(uint32 index);
		template <class T> const T& GetOldestSample();			
		template <class T> const T& PopOldestSample();		
		template <class T> const T& GetNewestSample();			


		// returns the index of the sample inside the channel
		uint64		GetSampleIndex(uint32 index);
		uint64		GetOldestSampleIndex();
		Core::Time	GetOldestSampleTime();

		// configure epoch reading
		void SetEpochLength(uint32 length)										{ mEpochLength = length;}
		void SetEpochShift(uint32 shift)										{ mEpochShift = shift; }
		void SetEpochZeroPadding(bool enable)									{ mEpochZeroPadding = enable; }

		// accessors for the epochs (you need to config the reader first)
		uint32 GetNumEpochs() const;						// total number of epochs we can read ..
		Epoch GetEpoch(uint32 index) const;					// get a specific epoch where 0 is the oldest one
		Epoch PopOldestEpoch();								// get oldest epoch an mark it as processed
		void  ClearNewEpochs();								// mark all epochs as processed

		// detection of changes in the input channel
		enum EChangeType { RESET, REFERENCE, SAMPLERATE, ANY, NUM_CHANGETYPES };
		void DetectInputChanges();
		bool HasInputChanged(EChangeType type = ANY) const						{ return mInputChangeDetected[type]; }

		// access to sample counters
		uint64 GetNumSamplesProcessed() const									{ return mNumSamplesProcessed; }
		uint64 GetNumSamplesReceived() const									{ return mNumSamplesReceived; }

		// check if there are samples that cannot be read anymore because the input buffer has advanced too far
		bool IsOutOfBounds() const												{ return mNumNewSamples > mChannel->GetNumSamples(); }

	private:									
		ChannelBase*	mChannel;
		Core::Time		mStartTime;
		bool			mHasStarted;

		// change detection
		bool			mInputConnectionChanged;
		bool			mInputChangeDetected[NUM_CHANGETYPES];			
		uint64			mLastSampleCounter;							
		double			mLastSampleRate;		
		uint32			mLastBufferSize;
											
		// number of new samples on the input between the last two Update() calls
		uint64			mNumNewSamples;

		// number of samples received and processed since reset
		uint64			mNumSamplesProcessed;
		uint64			mNumSamplesReceived;

		// epoch reading configuration
		uint64			mEpochLength;
		uint64			mEpochShift;
		uint64			mEpochZeroPadding;

		// Epoch index calc helper: calc position of an epoch
		uint64 CalcEpochPosition(uint32 index) const;

};

#endif
