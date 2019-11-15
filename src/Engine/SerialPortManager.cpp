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

// include required files
#include "SerialPortManager.h"
#include "EngineManager.h"

using namespace Core;

// constructor
SerialPortManager::SerialPortManager()
{
	LogDetailedInfo("Constructing serial port manager ...");
}


// destructor
SerialPortManager::~SerialPortManager()
{
	LogDetailedInfo("Destructing serial port manager ...");
}


// request the exclusive access to a serial port - don't forget to release it afterwards!
// if true is returned, you are allowed to use the serial port
bool SerialPortManager::AcquireSerialPort(const char* portName)
{
	// port is already in use -> return false immediately
	if (IsLocked(portName) == true)
		return false;
	
	mLock.Lock();

	mAcquiredSerialPorts.Add(portName);

	mLock.Unlock();

	return true;
}


// free the Acquired serial port again
bool SerialPortManager::ReleaseSerialPort(const char* portName)
{
	mLock.Lock();

	const uint32 numPorts = mAcquiredSerialPorts.Size();
	for (uint32 i=0; i<numPorts; ++i)
	{
		// delete port from list if found
		if (mAcquiredSerialPorts[i].Compare(portName) == 0)
		{
			mAcquiredSerialPorts.Remove(i);
			mLock.Unlock();
			return true;	
		}
	}

	mLock.Unlock();
	
	return false;
}


// check the list if a port is already Acquired
bool SerialPortManager::IsLocked(const char* portName)
{
	mLock.Lock();

	const uint32 numPorts = mAcquiredSerialPorts.Size();
	for (uint32 i=0; i<numPorts; ++i)
	{
		// compare port names
		if (mAcquiredSerialPorts[i].Compare(portName) == 0)
		{
			mLock.Unlock();
			return true;	
		}
	}

	mLock.Unlock();

	return false;
}
