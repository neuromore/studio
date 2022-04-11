/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "RequestQueue.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
RequestQueue::RequestQueue()
{
}


// destructor
RequestQueue::~RequestQueue()
{
	Clear();
}


// remove all requests
void RequestQueue::Clear()
{
	// iterate through all requests in the queue and delete them
	const uint32 numRequests = mQueue.Size();
	for (uint32 i=0; i<numRequests; ++i)
		delete mQueue[i];

	// remove all elements from the queue array
	mQueue.Clear();
}


// get the number of elements in the queue
uint32 RequestQueue::Size()
{
	return mQueue.Size();
}


// push a new element to the queue
void RequestQueue::Push(Request* request)
{
	mQueue.Add( request );
}


// pop the oldest element from the queue
void RequestQueue::Pop()
{
	mQueue.RemoveFirst();
}


// log the queue
void RequestQueue::Log()
{
	const uint32 size = Size();
	LogInfo("RequestQueue: (%i)", size);
	for (uint32 i=0; i<size; ++i)
		LogInfo( "#%i: %s", i, mQueue[i]->GetUrl() );

}