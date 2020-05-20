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
#include "MitsarDriver.h"
#include <Core/LogManager.h>
#include <Core/ThreadHandler.h>
#include <QMessageBox>
#include <QtBaseConfig.h>
#include <QtBaseManager.h>

#ifdef INCLUDE_DEVICE_MITSAR

using namespace Core;

// constructor
MitsarDriver::MitsarDriver() : DeviceDriver(false)
{
	LogDetailedInfo("Constructing Mitsar Driver ...");

	mThreadHandler = NULL;
	mDevice = NULL;

	AddSupportedDevice(Mitsar201Device::TYPE_ID);
}


// destructor
MitsarDriver::~MitsarDriver()
{
	LogDetailedInfo("Destructing Mitsar Driver ...");

	if (mThread != NULL)
		mThread->Stop();

	delete mThread;

	// remove event handler
	CORE_EVENTMANAGER.RemoveEventHandler(this);
}


// initialization
bool MitsarDriver::Init()
{
	LogInfo("Initializing Mitsar Driver... ");

	// register event handler for reacting to device removal
	CORE_EVENTMANAGER.AddEventHandler(this);

	// init thread
	mThreadHandler = new MitsarConnectorHandler(this);
	mThread = new Thread(mThreadHandler, "Mitsar Driver Thread");

	LogDetailedInfo("Mitsar Driver initialized.");
	return true;
}


// update the emotiv manager
void MitsarDriver::Update(const Time& elapsed, const Time& delta)
{
	// do not detect devices if system is disabled
	if (IsEnabled() == false)
		return;
}


void MitsarDriver::SetAutoDetectionEnabled(bool enable)
{
	DeviceDriver::SetAutoDetectionEnabled(enable);
	mThreadHandler->SetAutoDetectEnabled(enable);

	// start thread if not already running
	if (enable == true && mIsEnabled)
		mThread->Start();
	else
		mThread->Stop();
}


bool MitsarDriver::IsDetectionRunning() const
{
	return mThreadHandler->IsSearching();
}


// detect once
void MitsarDriver::DetectDevices()
{
	if (mIsEnabled == false)
		return;

	mThreadHandler->DetectOnce();

	// start thread if not already running
	mThread->Start();
}


Device* MitsarDriver::CreateDevice(uint32 deviceTypeID)
{
	CORE_ASSERT (IsDeviceSupported(deviceTypeID) == true);
	if (IsDeviceSupported(deviceTypeID) == false)
		return NULL;

	switch (deviceTypeID)
	{
		case Mitsar201Device::TYPE_ID: 	return new Mitsar201Device(this);
		// TODO more devices
		default: return NULL;
	}
}

// mitsar should be removed -> thread has to be stopped
void MitsarDriver::OnRemoveDevice(Device* device) 
{
	// terminate thread if our device was removed
	if (device == mDevice && mUseAutoDetection == true)
		mThread->Stop();
}


// mitsar device was added
void MitsarDriver::OnDeviceAdded(Device* device)
{
	if (IsDeviceSupported(device->GetType()) == false)
		return;

	mDevice = device;
}


void MitsarDriver::StartTest(Device* device)
{
	if (device != mDevice)
		return;

	mThreadHandler->StartImpedanceTest();
}


void MitsarDriver::StopTest(Device* device)
{
	if (device != mDevice)
		return;

	mThreadHandler->StopImpedanceTest();
}


bool MitsarDriver::IsTestRunning(Device* device)
{
	if (device != mDevice)
		return false;

	return mThreadHandler->mMode == MitsarConnectorHandler::MODE_IMPEDANCE;
}



//
// Mitsar Connector Thread Handler
//

MitsarConnectorHandler::MitsarConnectorHandler(MitsarDriver* driver)
{
	mDriver		= driver;
	mMode		= EMode::MODE_STREAM;
	mState		= EState::STATE_IDLE;
	mBreak		= false;
	mForceStart = false;
	mProcessInformation.hProcess = 0;
}


void MitsarConnectorHandler::Execute()
{
	mIsFinished = false;
	mBreak = false;

	while (mBreak == false)
	{
		switch (mState)
		{
			// do nothing
			case STATE_IDLE:
				if (mUseAutoDetection == true || mForceStart == true)
					mState = STATE_SEARCHING;

				mForceStart = false;
				break;

			// search device and stream data
			case STATE_SEARCHING: 
			{
				mModeLock.Lock();
				const bool success = StartMitsarConnector(mMode);
				mModeLock.Unlock();

				// stall this thread until the process finishes (waits here until device search / data streaming ends)
				if (success == true)
				{
					mState = STATE_STREAMING;
					WaitForSingleObject(mProcessInformation.hProcess, INFINITE);
				}
				mState = STATE_IDLE;
			}	break;
		}
		
		// limit update rate
		if (mBreak == false || mDriver->IsEnabled() == false)
			Thread::Sleep(1000);
	}

	mIsFinished = true;
}


void MitsarConnectorHandler::Terminate()
{
	StopMitsarConnector();
	mBreak = true;
}


// enable autodetection
void MitsarConnectorHandler::SetAutoDetectEnabled(bool enable)
{
	mUseAutoDetection = enable;

	// start search
	if (enable == true)
		if (mState == STATE_IDLE)
			mState = STATE_SEARCHING;
}

// detect only once
void MitsarConnectorHandler::DetectOnce()
{
	if (mState != STATE_IDLE)
		return;
	
	mModeLock.Lock();
	mMode = GetDefaultStreamMode();
	LogDebug("Mitsar Driver: Changed mode to Streaming ");

	mModeLock.Unlock();


	mForceStart = true;
}

void MitsarConnectorHandler::StartImpedanceTest()
{
	mModeLock.Lock();

	// already in impedance mode
	if (mMode != MODE_IMPEDANCE)
	{
		// change mode and terminate current connector
		mMode = MODE_IMPEDANCE;
		LogDebug("Mitsar Driver: Changed mode to Impedance ");

		mForceStart = true;
		StopMitsarConnector();
	}

	mModeLock.Unlock();
}


// stop impedance test
void MitsarConnectorHandler::StopImpedanceTest()
{
	mModeLock.Lock();

	if (mMode == MODE_IMPEDANCE)
	{
		// switch back to streaming mode
		mMode = GetDefaultStreamMode();
		LogDebug("Mitsar Driver: Changed mode to Streaming ");

		mForceStart = true;
		StopMitsarConnector();
	}

	mModeLock.Unlock();
}


// (re)start MitsarConnector in the specified mode
bool MitsarConnectorHandler::StartMitsarConnector(EMode mode)
{
	// terminate mitsar connector if already running
	StopMitsarConnector();

	LogDebug("Mitsar Driver: Starting Mitsar Connector in mode %i", (int)mode);

	String commandLine;

	commandLine.Format("%s%s ", FromQtString(GetAppDir()).AsChar(), "Mitsar/MitsarConnector.exe");

	commandLine += "--no-retry ";
	 
	// mode arguments
	if (mode == MODE_STREAM_RAW)
		commandLine += "--raw ";
	else if (mode == MODE_IMPEDANCE)
		commandLine += "--impedance 0 ";

	//commandLine += "--silent ";
	//commandLine += "--osc-host 127.0.0.1 ";
	//commandLine += "--osc-port 4545 ";

	QString qCommandLine = commandLine.AsChar();
	wchar_t wCommandLine[4096];
	qCommandLine.toWCharArray(wCommandLine);
	wCommandLine[qCommandLine.size()] = 0; // set end byte required?! doesn't work without it

	// reset process id first
	mProcessInformation.hProcess = 0;
	
	STARTUPINFO si = { 0 }; si.cb = sizeof(si);

	// enable this for debug
	const bool showWindow = true; 
	const DWORD creationFlag = (showWindow ? 0 : CREATE_NO_WINDOW);
	
	// start process
	const bool success = CreateProcess(NULL, wCommandLine, NULL, NULL, FALSE, creationFlag, NULL, NULL, &si, &mProcessInformation);

	return success;
}


void MitsarConnectorHandler::StopMitsarConnector()
{
	// is not running
	if (mProcessInformation.hProcess == 0)
		return;

	LogDebug("Mitsar Driver: Stopping Mitsar Connector");
	
	TerminateProcess(mProcessInformation.hProcess, 0);
	mProcessInformation.hProcess = 0;
}

#endif
