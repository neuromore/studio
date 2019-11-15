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

// include the required headers
#include "Thread.h"
#include "LogManager.h"


namespace Core
{

// constructor
Thread::Thread(ThreadHandler* threadHandler, const char* name)
{
	mThread		= NULL;
	mHandler	= threadHandler;
	mName		= name;
}


// destructor
Thread::~Thread()
{
	Stop();
	
	// delete the thread handler
	delete mHandler;
	mHandler = NULL;
}


// global thread execution function, which executes the handler execute
#ifndef NEUROMORE_PLATFORM_WINDOWS
	static void ThreadGlobalFunction(void* argument)
#else
	static void __stdcall ThreadGlobalFunction(void* argument)
#endif
{
	ThreadHandler* threadHandler = (ThreadHandler*)argument;
	threadHandler->Execute();
}


// start the thread
void Thread::Start()
{
	if (mThread != NULL)
		return;

	mThread = new std::thread( &ThreadGlobalFunction, mHandler );
	LogInfo("Thread started (%s)", mName.AsChar());
}


// stop the thread
void Thread::Stop()
{
	// we have to wait for the thread to finish its execute function, else we'll get a runtime error
	mHandler->Terminate();
	WaitForThreadFinish();

	// delete the thread
	delete mThread;
	mThread = NULL;

	LogInfo("Thread closed (%s)", mName.AsChar());
}


// blocks the current thread until this thread finishes its execution
void Thread::WaitForThreadFinish()
{
	if (mThread != NULL)
		mThread->join();
}


// blocks the execution of the current thread for at least the specified time
// a steady clock is used to measure the duration. this function may block for longer than the given time due to scheduling or resource contention delays
void Thread::Sleep(double milliseconds)
{
	const unsigned long microseconds = milliseconds*1000.0;
	std::chrono::microseconds sleepDuration( microseconds );
	std::this_thread::sleep_for(sleepDuration);
}

} // namespace Core
