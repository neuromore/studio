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

#ifndef __NEUROMORE_SERIALPORTMANAGER_H
#define __NEUROMORE_SERIALPORTMANAGER_H

// include required headers
#include "Config.h"
#include "Core/Mutex.h"
#include "Core/Array.h"
#include "Core/String.h"

// the SerialPortManager class
class ENGINE_API SerialPortManager
{
	public:
		
		// constructors & destructor
		SerialPortManager();
		virtual ~SerialPortManager();
		
		// request the exclusive access to a serial port - don't forget to release it afterwards!
		// if true is returned, you are allowed to use the serial port
		bool AcquireSerialPort(const char* portName);

		// free the serial port again
		bool ReleaseSerialPort(const char* portName);

		// if a port is contained in the lock list
		bool IsLocked(const char* portName);

		// lock/unlock for portlisting used in 
		void LockListing()								{ mListingLock.Lock(); }
		void UnlockListing()							{ mListingLock.Unlock(); }

	private:


		// the list of locked serial ports
		Core::Array<Core::String>	mAcquiredSerialPorts;

		// lock for threadsafe Acquire/release
		Core::Mutex					mLock;

		Core::Mutex					mListingLock;
};

#endif
