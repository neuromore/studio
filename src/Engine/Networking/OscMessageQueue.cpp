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

// include required headers
#include "OscMessageQueue.h"
#include "../Core/LogManager.h"


// constructor
OscMessageQueue::OscMessageQueue()
{
	// reserve memory
	mQueue.Reserve(1024);
}


// destructor
OscMessageQueue::~OscMessageQueue()
{
	ClearQueue();
}


// flush the queue (mark all messages as read)
void OscMessageQueue::ClearQueue()
{
	mLock.Lock();

	const uint32 numMsg = mQueue.Size();
	for (uint32 i=0; i<numMsg; ++i)
	{
		OscMessageParser* message =	 mQueue.GetFirst();
		message->mIsReady = true;
		mQueue.RemoveFirst();
	}

	mLock.Unlock();
}


// push new object
void OscMessageQueue::Push(OscMessageParser* message)
{
	mLock.Lock();
	mQueue.Add(message);
	mLock.Unlock();
}


// pop oldest object
OscMessageParser* OscMessageQueue::Pop()
{
	mLock.Lock();

	// if there is no object to pop anymore, return NULL
	if (IsEmpty() == true)
	{
		mLock.Unlock();
		return NULL;
	}

	OscMessageParser* result = mQueue.GetFirst();
	mQueue.RemoveFirst();

	mLock.Unlock();

	return result;
}
