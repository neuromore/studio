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
#include <Studio/Precompiled.h>

#include "TobiiEyeXDriver.h"

using namespace Core;

#ifdef INCLUDE_DEVICE_TOBIIEYEX

// TODO: Move that somehow as static member into *.h file
TX_HANDLE mGlobalStaticHandle;
Core::Array<double> mEyeTrackerValues;

// constructor
TobiiEyeXDriver::TobiiEyeXDriver() : DeviceDriver(false)
{
	LogDetailedInfo("Constructing Tobii EyeX driver ...");
	mTobiiEyeXDevice = new TobiiEyeXDevice(this);
	mTobiiEyeXDevice->Init();
	GetDeviceManager()->AddDeviceAsync(mTobiiEyeXDevice);

	AddSupportedDevice(TobiiEyeXDevice::TYPE_ID);

	// init array
	const uint32 numSensors = mTobiiEyeXDevice->GetNumSensors();
	mEyeTrackerValues.Reserve(numSensors);
	for (uint32 i=0;i<numSensors;++i)
	{
		mEyeTrackerValues[i] = 0.0;
	}
}


// destructor
TobiiEyeXDriver::~TobiiEyeXDriver()
{

	LogDetailedInfo("Destructing Tobii EyeX driver ...");

	// disable and delete the context
	txDisableConnection(mContext);
	txReleaseObject(&mGlobalStaticHandle);
	mSuccess = txShutdownContext(mContext, TX_CLEANUPTIMEOUT_DEFAULT, TX_FALSE) == TX_RESULT_OK;
	mSuccess &= txReleaseContext(&mContext) == TX_RESULT_OK;
	mSuccess &= txUninitializeEyeX() == TX_RESULT_OK;
	if (!mSuccess) {
		LogDebug("EyeX could not be shut down cleanly. Did you remember to release all handles?\n");
	}
	mEyeTrackerValues.Clear();
}


void TX_CALLCONVENTION TobiiEyeXDriver::HandleEvent(TX_CONSTHANDLE asyncData, TX_USERPARAM userParam)
{
	TX_HANDLE event = TX_EMPTY_HANDLE;
	TX_HANDLE behaviorEyePositionData = TX_EMPTY_HANDLE;
	TX_HANDLE behaviorEyeGazeData     = TX_EMPTY_HANDLE;

	txGetAsyncDataContent(asyncData, &event);

	if (txGetEventBehavior(event, &behaviorEyePositionData, TX_BEHAVIORTYPE_EYEPOSITIONDATA) == TX_RESULT_OK) {
		OnEyePositionDataEvent(behaviorEyePositionData);
		txReleaseObject(&behaviorEyePositionData);
	}

	if (txGetEventBehavior(event, &behaviorEyeGazeData, TX_BEHAVIORTYPE_GAZEPOINTDATA) == TX_RESULT_OK)
	{
		OnGazeDataEvent(behaviorEyeGazeData);
		txReleaseObject(&behaviorEyeGazeData);
	}
	txReleaseObject(&event);
}


void TobiiEyeXDriver::OnEyePositionDataEvent(TX_HANDLE eyePositionDataBehavior)
{
	COORD position = { 0, 8 };
	TX_EYEPOSITIONDATAEVENTPARAMS eventParams;
	if (txGetEyePositionDataEventParams(eyePositionDataBehavior, &eventParams) == TX_RESULT_OK) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);

		// update variables
		mEyeTrackerValues[0] = eventParams.LeftEyeX;
		mEyeTrackerValues[1] = eventParams.LeftEyeY;
		mEyeTrackerValues[2] = eventParams.LeftEyeZ;
		mEyeTrackerValues[3] = eventParams.LeftEyeXNormalized;
		mEyeTrackerValues[4] = eventParams.LeftEyeYNormalized;
		mEyeTrackerValues[5] = eventParams.LeftEyeZNormalized;
		mEyeTrackerValues[6] = eventParams.RightEyeX;
		mEyeTrackerValues[7] = eventParams.RightEyeY;
		mEyeTrackerValues[8] = eventParams.RightEyeZ;
		mEyeTrackerValues[9] = eventParams.RightEyeXNormalized;
		mEyeTrackerValues[10] = eventParams.RightEyeYNormalized;
		mEyeTrackerValues[11] = eventParams.RightEyeZNormalized;

		//LogDebug("\n");
		//LogDebug("The 3D position consists of X,Y,Z coordinates expressed in millimeters \n");
		//LogDebug("in relation to the center of the screen where the eye tracker is mounted. \n");
		//LogDebug("\n");
		//LogDebug("The normalized coordinates are expressed in relation to the track box, \n");
		//LogDebug("i.e. the volume in which the eye tracker is theoretically able to track eyes. \n");
		//LogDebug("- (0,0,0) represents the upper, right corner closest to the eye tracker. \n");
		//LogDebug("- (1,1,1) represents the lower, left corner furthest away from the eye tracker. \n");
	}
	else {
		LogDebug("Failed to interpret eye position data event packet.\n");
	}
}


void TobiiEyeXDriver::OnGazeDataEvent(TX_HANDLE gazePositionDataBehavior)
{
	TX_GAZEPOINTDATAEVENTPARAMS eventParams;
	if (txGetGazePointDataEventParams(gazePositionDataBehavior, &eventParams) == TX_RESULT_OK)
	{
		mEyeTrackerValues[12] = eventParams.X;
		mEyeTrackerValues[13] = eventParams.Y;
	}
	else
	{
		LogDebug("Failed to interpret gaze data event packet.\n");
	}
}


void TX_CALLCONVENTION TobiiEyeXDriver::OnSnapshotCommitted(TX_CONSTHANDLE asyncData, TX_USERPARAM param)
{
	TX_RESULT result = TX_RESULT_UNKNOWN;
	txGetAsyncDataResultCode(asyncData, &result);
	assert(result == TX_RESULT_OK || result == TX_RESULT_CANCELLED);
}


void TX_CALLCONVENTION TobiiEyeXDriver::OnEngineConnectionStateChanged(TX_CONNECTIONSTATE connectionState, TX_USERPARAM userParam)
{
	switch (connectionState) {
	case TX_CONNECTIONSTATE_CONNECTED: {
		BOOL success;
		// commit the snapshot with the global interactor as soon as the connection to the engine is established.
		// (it cannot be done earlier because committing means "send to the engine".)
		success = txCommitSnapshotAsync(mGlobalStaticHandle, &TobiiEyeXDriver::OnSnapshotCommitted, NULL) == TX_RESULT_OK;
		if (!success) {
			LogDebug("Failed to initialize the data stream.\n");
		}
		else {
			LogDebug("Waiting for eye position data to start streaming...\n");
		}
		
	}
		break;

	case TX_CONNECTIONSTATE_DISCONNECTED:
		LogDebug("The connection state is now DISCONNECTED (We are disconnected from the EyeX Engine)\n");
		break;

	case TX_CONNECTIONSTATE_TRYINGTOCONNECT:
		LogDebug("The connection state is now TRYINGTOCONNECT (We are trying to connect to the EyeX Engine)\n");
		break;

	case TX_CONNECTIONSTATE_SERVERVERSIONTOOLOW:
		LogDebug("The connection state is now SERVER_VERSION_TOO_LOW: this application requires a more recent version of the EyeX Engine to run.\n");
		break;

	case TX_CONNECTIONSTATE_SERVERVERSIONTOOHIGH:
		LogDebug("The connection state is now SERVER_VERSION_TOO_HIGH: this application requires an older version of the EyeX Engine to run.\n");
		break;
	}
}


bool TobiiEyeXDriver::InitDevice(TX_CONTEXTHANDLE context)
{
	TX_HANDLE interactor = TX_EMPTY_HANDLE;
	TX_HANDLE behavior   = TX_EMPTY_HANDLE;
	TX_GAZEPOINTDATAPARAMS params = { TX_GAZEPOINTDATAMODE_LIGHTLYFILTERED };
	bool success;

	success = txCreateGlobalInteractorSnapshot(
		context,
		"interactorId",
		&mGlobalStaticHandle,
		&interactor) == TX_RESULT_OK;
	success &= txCreateInteractorBehavior(interactor, &behavior, TX_BEHAVIORTYPE_EYEPOSITIONDATA) == TX_RESULT_OK;
	success &= txCreateGazePointDataBehavior(interactor, &params) == TX_RESULT_OK;

	txReleaseObject(&interactor);

	return success;
}


// overriden methods
bool TobiiEyeXDriver::Init()
{
	LogDetailedInfo("Init Tobii EyeX...");
	mGlobalStaticHandle = TX_EMPTY_HANDLE;
	mContext = TX_EMPTY_HANDLE;
	TX_TICKET connectionStateChangedTicket = TX_INVALID_TICKET;
	TX_TICKET eventHandlerTicket = TX_INVALID_TICKET;

	// initialize and enable the context that is our link to the EyeX Engine
	mSuccess  = txInitializeEyeX(TX_EYEXCOMPONENTOVERRIDEFLAG_NONE, NULL, NULL, NULL, NULL) == TX_RESULT_OK;
	mSuccess &= txCreateContext(&mContext, TX_FALSE) == TX_RESULT_OK;
	mSuccess &= InitDevice(mContext);
	mSuccess &= txRegisterConnectionStateChangedHandler(mContext, &connectionStateChangedTicket, &TobiiEyeXDriver::OnEngineConnectionStateChanged, NULL) == TX_RESULT_OK;
	mSuccess &= txRegisterEventHandler(mContext, &eventHandlerTicket, &TobiiEyeXDriver::HandleEvent, NULL) == TX_RESULT_OK;
	mSuccess &= txEnableConnection(mContext) == TX_RESULT_OK;

	// register event handler for reacting to device removal
	CORE_EVENTMANAGER.AddEventHandler(this);

	return mSuccess;
}


void TobiiEyeXDriver::Update(const Core::Time& elapsed, const Core::Time& delta)
{
	if (IsEnabled() == false)
		return;

	// force data update
	if (mTobiiEyeXDevice->GetType() == TobiiEyeXDevice::TYPE_ID)
	{
		DoDataUpdateEyeX(mTobiiEyeXDevice, 0);
	}
}


void TobiiEyeXDriver::DoDataUpdateEyeX(TobiiEyeXDevice* tobiiEyeXDevice, int32 userid)
{
	const uint32 numSensors = tobiiEyeXDevice->GetNumSensors();
	for (uint32 i=0;i<numSensors;++i)
	{
		tobiiEyeXDevice->GetSensor(i)->AddQueuedSample(mEyeTrackerValues[i]);
	}
}

#endif
