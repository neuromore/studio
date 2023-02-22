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

#ifndef __NEUROMORE_MULTICHANNEL_H
#define __NEUROMORE_MULTICHANNEL_H

// include required headers
#include "../Config.h"
#include "Channel.h"


// the MultiChannel class
class ENGINE_API MultiChannel
{
	public:
		// constructors & destructor
		MultiChannel()										 	{}
		virtual ~MultiChannel()									{}
		
		virtual uint32 GetNumChannels() const					{ return mChannels.Size(); }

		uint32 GetType() const;
		
		// add/get single channels
		void AddChannel(ChannelBase* channel);
		ChannelBase* GetChannel(uint32 index) const			 	{ return mChannels[index]; }
		const Core::Array<ChannelBase*>& GetChannels()			{ return mChannels; }
		
		// add / set another channel set
		void AddMultiChannel(MultiChannel* channels);
		void SetMultiChannel(MultiChannel* channels);

		// remove all channels from the set
		void Clear()											{ mChannels.Clear(); }

		// reset all channels in the set
		void Reset();

		// get sample rate
		double GetSampleRate() const;

		// get range accross all channels
		double GetMinValue() const;
		double GetMaxValue() const;

		// returns true if at least one channel in the set is active
		bool IsActive() const;
		
		bool IsHighlighted() const;

		// get/set buffer size of all channels
		void SetBufferSize(uint32 numSamples, bool discard = true);
		inline void SetBufferSizeInSeconds(double seconds, bool discard = true) { 
			SetBufferSize((uint32)::ceil(GetSampleRate() * seconds), discard); 
		}

		uint32 GetMinBufferSize() const;
		bool IsBuffer() const;
		uint32 CalculateMemoryAllocated(bool countBuffersOnly = false);
		uint32 CalculateMemoryUsed(bool countBuffersOnly = false);

		// if a channel is compatible with the set
		bool IsCompatible(ChannelBase* otherChannel);

	private:
		Core::Array<ChannelBase*>  mChannels;

		// check set for constistency
		bool Validate();
};

#endif
