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

// include precompiled header
#include <Engine/Precompiled.h>

// include the required headers
#include "Mutex.h"


namespace Core
{

// constructor
Mutex::Mutex()
{
}


// destructor
Mutex::~Mutex()
{
}


// locks the mutex, blocks if the mutex is not available
// if another thread has already locked the mutex, a call to lock will block execution until the lock is acquired
// if lock is called by a thread that already owns the mutex, the behavior is undefined: the program may deadlock
void Mutex::Lock()
{
	mMutex.lock();
}


// unlocks the mutex
void Mutex::Unlock()
{
	mMutex.unlock();
}


// tries to lock the mutex, returns if the mutex is not available
bool Mutex::TryLock()
{
	return mMutex.try_lock();
}

} // namespace Core
