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
#include "Config.h"
#include "neuromoreEngine.h"
#include "Core/StandardHeaders.h"
#include "Core/String.h"
#include "Core/Timer.h"
#include "Core/Json.h"
#include "Core/FpsCounter.h"
#include "Core/LogManager.h"
#include "EngineManager.h"
#include "CloudParameters.h"
#include "SessionExporter.h"
#include "Graph/GraphImporter.h"
#include "Graph/StateTransitionButtonCondition.h"
#include "Graph/StateTransitionAudioCondition.h"
#include "Graph/StateTransitionVideoCondition.h"

// devices
#include "Devices/DeviceInventory.h"
#include "Devices/Generic/GenericDevices.h"
#include "Devices/Generic/GenericDeviceNodes.h"
#include "Devices/Muse/MuseDevice.h"
#include "Devices/Muse/MuseNode.h"
#include "Devices/Emotiv/EmotivEPOCDevice.h"
#include "Devices/Emotiv/EmotivEPOCNode.h"
#include "Devices/Emotiv/EmotivInsightDevice.h"
#include "Devices/Emotiv/EmotivInsightNode.h"
#include "Devices/NeuroSky/NeuroSkyDevice.h"
#include "Devices/NeuroSky/NeuroSkyNode.h"
#include "Devices/Versus/VersusDevice.h"
#include "Devices/Versus/VersusNode.h"
#include "Devices/eSense/eSenseSkinResponseDevice.h"
#include "Devices/eSense/eSenseSkinResponseNode.h"

using namespace Core;

namespace neuromoreEngine
{

Callback* gCallback = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// neuromore Engine Data Storage
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// forward declaration
class EngineThreadHandler;

struct FeedbackData
{
	Core::String mName;
	double mValue;
	double mMinValue;
	double mMaxValue;

	void Reset()
	{
		mValue		= 0.0;
		mMinValue	= 0.0;
		mMaxValue	= 0.0;
	}
};


class FeedbacksData
{
	public:
		void Resize(uint32 newNumFeedbacks)
		{
			mLock.Lock();

			const uint32 numFeedbacks = mData.Size();
			if (numFeedbacks == newNumFeedbacks)
			{
				mLock.Unlock();
				return;
			}

			mData.Resize(newNumFeedbacks);
			for (uint32 i=0; i<newNumFeedbacks; ++i)
			{
				mData[i].Reset();
			}

			mLock.Unlock();
		}

		void SetFeedbackData(uint32 index, const char* name, double value, double minValue, double maxValue)
		{
			mLock.Lock();
			const uint32 numFeedbacks = mData.Size();
			if(index < numFeedbacks)
			{
				mData[index].mValue		= value;
				mData[index].mMinValue	= minValue;
				mData[index].mMaxValue	= maxValue;

				// only adjust name in case it differs
				if (mData[index].mName.IsEqual(name) == false)
					mData[index].mName = name;
			}
			mLock.Unlock();
		}

		uint32 GetNumFeedbacks()									{ uint32 result = 0; mLock.Lock(); result = mData.Size(); mLock.Unlock(); return result; }
		double GetFeedbackValue(uint32 index)						{ double result = 0.0; mLock.Lock(); const uint32 numFeedbacks = mData.Size(); if(index < numFeedbacks) result = mData[index].mValue; mLock.Unlock(); return result; }
		double GetFeedbackMinValue(uint32 index)					{ double result = 0.0; mLock.Lock(); const uint32 numFeedbacks = mData.Size(); if(index < numFeedbacks) result = mData[index].mMinValue; mLock.Unlock(); return result; }
		double GetFeedbackMaxValue(uint32 index)					{ double result = 0.0; mLock.Lock(); const uint32 numFeedbacks = mData.Size(); if(index < numFeedbacks) result = mData[index].mMaxValue; mLock.Unlock(); return result; }
		const char* GetFeedbackName(uint32 index)					{ const char* result = NULL; mLock.Lock(); const uint32 numFeedbacks = mData.Size(); if(index < numFeedbacks) result = mData[index].mName.AsChar(); mLock.Unlock(); return result; }

	private:
		Mutex						mLock;
		Core::Array<FeedbackData>	mData;
};

class NMEngineData
{
	public:
		// constructor & destructor
		NMEngineData()						{} 
		~NMEngineData()						{}

		Array<StateMachine::Asset> mStateMachineAssets;

		MemoryFile		mTempMemoryFile;
		Timer			mUpdateTimer;

		// engine update thread
		Core::Thread*					mThread;
		EngineThreadHandler*			mThreadHandler;

		// thread safe feedback data
		FeedbacksData					mFeedbackData;

		// for creating jsons strings
		String mTempJsonString;

		// performance statistics (thread safe operation)
		PerformanceStatistics GetPerformanceStatistics()																{ mPerformanceStatisticsLock.Lock(); PerformanceStatistics result = mPerformanceStatistics; mPerformanceStatisticsLock.Unlock(); return result; }
		void SetPerformanceStatistics(PerformanceStatistics stats)														{ mPerformanceStatisticsLock.Lock(); mPerformanceStatistics = stats; mPerformanceStatisticsLock.Unlock(); }

	private:
		PerformanceStatistics	mPerformanceStatistics;
		Mutex					mPerformanceStatisticsLock;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Event Handler
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class NMEngineEventHandler : public Core::EventHandler
{
	public:
		NMEngineEventHandler() : EventHandler()																			{}
		virtual ~NMEngineEventHandler()																						{}

		void OnPlayAudio(const char* url, int32 numLoops, double beginAt, double volume, bool allowStream) override final	{ if (gCallback) gCallback->OnPlayAudio(url, numLoops, beginAt, volume); }
		void OnStopAudio(const char* url) override final																						{ if (gCallback) gCallback->OnStopAudio(url); }
		void OnPauseAudio(const char* url, bool unPause) override final																	{ if (gCallback) gCallback->OnPauseAudio(url, unPause); }
		void OnSetAudioVolume(const char* url, double volume) override final																{ if (gCallback) gCallback->OnSetAudioVolume(url, volume); }
		void OnSeekAudio(const char* url, uint32 millisecs) override final																{ if (gCallback) gCallback->OnSeekAudio(url, millisecs); }

		void OnPlayVideo(const char* url, int32 numLoops, double beginAt, double volume, bool allowStream) override final	{ if (gCallback) gCallback->OnPlayVideo(url, numLoops, beginAt, volume); }
		void OnStopVideo() override final																											{ if (gCallback) gCallback->OnStopVideo(); }
		void OnPauseVideo(const char* url, bool unPause) override final																	{ if (gCallback) gCallback->OnPauseVideo(url, unPause); }
		void OnSetVideoVolume(const char* url, double volume) override final																{ if (gCallback) gCallback->OnSetVideoVolume(url, volume); }
		void OnSeekVideo(const char* url, uint32 millisecs) override final																{ if (gCallback) gCallback->OnSeekVideo(url, millisecs); }

		void OnShowImage(const char* url) override final																						{ if (gCallback) gCallback->OnShowImage(url); }
		void OnHideImage() override final																											{ if (gCallback) gCallback->OnHideImage(); }

		void OnShowText(const char* text, const Core::Color& color) override final														{ if (gCallback) gCallback->OnShowText(text, color.r, color.g, color.b, color.a); }
		void OnHideText() override final																												{ if (gCallback) gCallback->OnHideText(); }

		void OnSetFourZoneAVEColors(const float* red, const float* green, const float* blue, const float* alpha) override final	{ if (gCallback) gCallback->OnSetFourZoneAVEColors(red, green, blue, alpha); }
		void OnHideFourZoneAVE() override final																											{ if (gCallback) gCallback->OnHideFourZoneAVE(); }

		void OnShowButton(const char* text, uint32 buttonId) override final																		{ if (gCallback) gCallback->OnShowButton(text, buttonId); }
		void OnClearButtons() override final																												{ if (gCallback) gCallback->OnClearButtons(); }

		void OnCommand(const char* command) override final																								{ if (gCallback) gCallback->OnCommand(command); }
		void OnExitStateReached(uint32 exitStatus) override final																					{ if (gCallback) gCallback->OnStop((EStatus)exitStatus); }
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NMEngineData*			gNMEngineData	= NULL;
NMEngineEventHandler*	gEventHandler	= NULL;


// get the time delta since the last call (returns 0.0 in case session is not running)
// TODO: REMOVE THIS ONCE WE SWITCHED OVER TO THE THREADED WAY!!!
Time GetTimeDelta()
{
	if (gNMEngineData == NULL)
		return 0.0;

	const Time timeDelta = gNMEngineData->mUpdateTimer.GetTimeDelta();

	if (IsRunning() == false)
		return 0.0;

	return timeDelta;
}


void UpdateFeedbackData()
{
	EngineManager* engine = GetEngine();
	if (engine == NULL)
		return;

	// get the active classifier
	Classifier* classifier = engine->GetActiveClassifier();
	if (classifier != NULL)
	{
		// resize the feedback data
		const uint32 numFeedbacks = classifier->GetNumCustomFeedbackNodes();
		gNMEngineData->mFeedbackData.Resize( numFeedbacks );

		// update feedback data
		for (uint32 i=0; i<numFeedbacks; ++i)
		{
			CustomFeedbackNode* node = classifier->GetCustomFeedbackNode(i);

			// output values
			const double minValue = node->GetFloatAttribute(CustomFeedbackNode::ATTRIB_RANGEMIN);
			const double maxValue = node->GetFloatAttribute(CustomFeedbackNode::ATTRIB_RANGEMAX);
			gNMEngineData->mFeedbackData.SetFeedbackData(i, node->GetName(), node->GetCurrentValue(), minValue, maxValue );
		}
	}
}


// update the engine
BOOL Update(const Time& timeDelta)
{
	EngineManager* engine = GetEngine();
	if (engine == NULL)
		return false;

	// update engine
	engine->Update( timeDelta );

	// update feedback data
	UpdateFeedbackData();

	return TRUE;
}


BOOL Update()
{
	return Update( GetTimeDelta() );
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// neuromore Engine Thread Handler
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EngineThreadHandler : public Core::ThreadHandler
{
	public:
		// constructor & destructor
		EngineThreadHandler() : ThreadHandler()			{ mBreak = false;}
		virtual ~EngineThreadHandler()					{}

		// start thread execution
		void Execute() override
		{
			mIsFinished = false;
			mBreak = false;

			// get access to the engine
			EngineManager* engine = GetEngine();
			if (engine == NULL)
			{
				LogError("Cannot start engine thread. Engine manager not valid.");
				mBreak = true;
				mIsFinished = true;
			}

			// get the active classifier
			Classifier* classifier = engine->GetActiveClassifier();
			if (classifier == NULL)
			{
				LogError("Cannot start engine thread. No active classifier.");
				mBreak = true;
				mIsFinished = true;
			}

			// reset the timer
			mRealTimer.GetTimeDelta();

			mDesiredFps = 60;
			const double desiredFpsDeltaTime = 1.0 / mDesiredFps;

			// real-time loop
			while (mBreak == false)
			{
				// get the time delta since the last iteration
				const Time timeDelta = mRealTimer.GetTimeDelta();

				// update engine
				mFpsCounter.BeginTiming();
				mUpdateTimer.GetTimeDelta();

				// update engine
				Update( timeDelta );

				const double updateTime = mUpdateTimer.GetTimeDelta().InSeconds();
				mFpsCounter.StopTiming();

				// update the fps statistics of the engine data (thread safe operation)
				PerformanceStatistics perfStats( mFpsCounter.GetFps(), mFpsCounter.GetTheoreticalFps(), mFpsCounter.GetAveragedTimeDelta(), mFpsCounter.GetBestCaseTiming(), mFpsCounter.GetWorstCaseTiming() );
				gNMEngineData->SetPerformanceStatistics( perfStats );

				// update rate control
				const double sleepTime = desiredFpsDeltaTime - updateTime;
				if (mBreak == false && sleepTime > 0.0)
					Thread::Sleep( sleepTime * 1000.0 );
			}

			mIsFinished = true;
		}

		// stop and terminate thread
		void Terminate() override
		{
			mBreak = true;
		}

	private:
		FpsCounter				mFpsCounter;
		Timer					mRealTimer;			// times the differences between full real-time loop iterations (real time!)
		Timer					mUpdateTimer;		// times how long the engine->Update() call takes
		uint32					mDesiredFps;
		bool					mBreak;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Log Callback
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class neuromoreEngineLogCallback : public LogCallback
{
	public:
		enum { TYPE_ID = 0x5b407 };
		neuromoreEngineLogCallback() : LogCallback()				{}
		virtual ~neuromoreEngineLogCallback()						{}
		uint32 GetType() const override                             { return TYPE_ID; }

		void Log(const char* text, Core::ELogLevel logLevel) override final
		{
            // make sure the callback is present
            if (gCallback == NULL)
                return;
            
            // make sure the logged text is valid and meaningful
            if (text == NULL || strlen(text) == 0)
                return;

			// add the log level parameter
			switch (logLevel)
			{
				case LOGLEVEL_CRITICAL:			{ String errorMessage;  errorMessage.Format( "[CRITICAL]: %s", text );   gCallback->OnLog( errorMessage.AsChar() ); break; }
				case LOGLEVEL_ERROR:			{ String errorMessage;  errorMessage.Format( "[ERROR]: %s", text );      gCallback->OnLog( errorMessage.AsChar() ); break; }
				case LOGLEVEL_WARNING:			{ String errorMessage;  errorMessage.Format( "[WARNING]: %s", text );    gCallback->OnLog( errorMessage.AsChar() ); break; }
				default:						{ gCallback->OnLog( text );break; }
			}
		}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initialization, cleanup and update
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialization
BOOL Init()
{
	// initialize core helper system
	if (EngineInitializer::Init() == false)
	{
		LogCritical("Failed to initialize the neuromore Engine.");
		return false;
	}

	// construct data object
	gNMEngineData = new NMEngineData();

	// start engine by default
	GetEngine()->SetIsRunning(true);

	// disable auto sync setting
	GetEngine()->SetAutoSyncSetting(false);

	// create and register our log callback
	neuromoreEngineLogCallback* logCallback = new neuromoreEngineLogCallback();
	CORE_LOGMANAGER.AddLogCallback( logCallback );

	// register all core devices and their nodes (force disabling of CRUD check, we don't have that here)
	DeviceInventory::RegisterDevices(true);

	// disable device autoremoval
	GetDeviceManager()->SetRemoveInactiveDevicesEnabled(false);

	// create the event handler
	gEventHandler = new NMEngineEventHandler();
	CORE_EVENTMANAGER.AddEventHandler( gEventHandler );

	return TRUE;
}


// is initialized?
BOOL IsInitialized()
{
	EngineManager* engine = GetEngine();
	return (engine != NULL);
}


void SetSessionLength(double seconds)
{
	// zero or negativ means infinite session length
	if (seconds <= 0)
		seconds = DBL_MAX;

	GetSession()->SetTotalTime(seconds);
}


// set classifier buffer size
BOOL SetBufferLength(double seconds)
{
	if (IsRunning())
		return FALSE;
	
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	if (!classifier)
		return FALSE;

	// resize classifier buffers
	classifier->SetBufferDuration(seconds);

	return TRUE;
}


// set the power line frequency type
BOOL SetPowerLineFrequencyType(EPowerLineFrequencyType powerLineFrequencyType)
{
	if (IsRunning() == TRUE)
		return false;

	GetEngine()->SetPowerLineFrequencyType( (EngineManager::EPowerLineFrequencyType)powerLineFrequencyType );
	return TRUE;
}


double GetPowerLineFrequency()
{
	if (GetEngine() == NULL)
		return 0.0;

	return GetEngine()->GetPowerLineFrequency( GetEngine()->GetUser() );
}


// Check if the engine is ready to start
BOOL IsReady()
{
	// an running engine is not ready to start
	if (IsRunning())
		return FALSE;

	// no classifier loaded
	if (!HasClassifier())
		return FALSE;

	//Classifier* classifier = GetEngine()->GetActiveClassifier();

	// TODO is there a case where we have a statemachine but no classifier? this could be a very simple static statemachines with no inputs

	// TODO if there is a statemachine, check if its compatible with the classifier

	// classifier has an error
	// FIXME FIXME FIXME
	// FIXME this does not work, the device nodes are not in the error state after the classifier was loaded
	//       -> the studio has no issues with this because the engine update loop is _always_ running. 
	//		 -> We have to sort out when we should call Reinit, Sync, Update (do we do it after loading the classifier? before start? Here in IsReady()?)
	// NOTE: DO NOT ENABLE THIS!!!!
	// imagine this case Start() -> Stop() -> Reset() -> now no HR sensor classifier with HR sensor is in error state but can't get out of that as classifier isn't updated anymore -> Can't restart as classifier stays in error state!
	//if (classifier->HasError() == true)
	//	return false;
		
	return TRUE;
}


// Begin processing data.
BOOL Start()
{
	// do nothing if not ready
	if (!IsReady())
	{
		LogError("neuromoreEngine::Start(): Cannot start as engine is not ready.");
		return FALSE;
	}

	// Note: don't reset classifier here, as it would reset the parameters

	// reset start state machine
	if (HasStateMachine())
	{
		GetEngine()->GetActiveStateMachine()->Start();
		GetEngine()->GetActiveStateMachine()->Reset();
	}

	// reset the timer
	GetTimeDelta();

	// start session
	GetSession()->Start();

	LogInfo("neuromoreEngine::Start(): Starting engine.");
	return TRUE;
}


// Stop the engine if it is currently running.
BOOL Stop()
{
	if (!IsRunning())
	{
		LogWarning("neuromoreEngine::Stop(): Can't stop engine. Engine was not running.");
		return FALSE;
	}

	// stop session first and then stop the engine
	GetSession()->Stop();

	// pause the start state machine
	if (HasStateMachine())
	{
		// FIXME this will be obsolete
		// NOTE: this is needed so that Reset() calls don't fire actions from the entry states
		GetEngine()->GetActiveStateMachine()->Stop();
	}

	// reset the timer
	GetTimeDelta();

	LogInfo("neuromoreEngine::Stop(): Stopping engine.");
	return TRUE;
}


// Begin processing data.
BOOL StartThreaded()
{
	// start engine, skip post processing if starting engine fails already
	if (!Start())
		return FALSE;

	// init thread
	gNMEngineData->mThreadHandler = new EngineThreadHandler();
	gNMEngineData->mThread = new Thread(gNMEngineData->mThreadHandler, "neuromore Engine Thread");

	// start thread if not already running
	gNMEngineData->mThread->Start();

	return TRUE;
}


// Stop the engine if it is currently running.
BOOL StopThreaded()
{
	if (!IsRunning())
	{
		LogWarning("neuromoreEngine::Stop(): Can't stop engine. Engine was not running.");
		return false;
	}

	Timer stopThreadTimer;

	if (gNMEngineData->mThread != NULL)
		gNMEngineData->mThread->Stop();

	delete gNMEngineData->mThread;

	const double stopThreadTiming = stopThreadTimer.GetTime().InMilliseconds();
	LogInfo( "Stopping engine thread took: %.1f ms.", stopThreadTiming );

	Stop();
	return TRUE;
}


void Reset()
{
	LogInfo("neuromoreEngine::Reset(): Resetting engine.");
	GetEngine()->Reset();
	GetEngine()->Update(0.0);
}


// Check if the engine is currently running.
BOOL IsRunning()
{
	return GetSession()->IsRunning();
}


// cleanup
void Shutdown()
{
	// destroy engine data
	LogInfo("Destroying neuromore Engine data ...");
	delete gNMEngineData;
	LogDetailedInfo("neuromore Engine data destroyed");

	// destroy the event handler
	LogInfo("Removing event handler ...");
	CORE_EVENTMANAGER.RemoveEventHandler( gEventHandler );
	LogDetailedInfo("Event handler removed");
	LogInfo("Destructing event handler ...");
	delete gEventHandler;
	gEventHandler = NULL;
	LogDetailedInfo("Event handler destructed");

	// destroy the callback
	LogInfo("Destructing callback ...");
	// TODO: change this once the CPP one is gone
	// this is also bad coding, should not release foreign allocated mem here
#if defined(NEUROMORE_ENGINE_CPP_CALLBACK)
	delete gCallback;
#else
	free(gCallback);
#endif
	gCallback = NULL;
	LogDetailedInfo("Callback destructed");

	// shutdown the example
	Core::LogInfo( "Shutting down neuromore Engine ..." );
	EngineInitializer::Shutdown();
}


// gather performance statistics from engine thread
BOOL GetPerformanceStatistics(double* outFps, double* outTheoreticalFps, double* outAveragedTiming, double* outBestCaseTiming, double* outWorstCaseTiming)
{
	*outFps				= 0.0;
	*outTheoreticalFps	= 0.0;
	*outAveragedTiming	= 0.0;
	*outBestCaseTiming	= 0.0;
	*outWorstCaseTiming	= 0.0;

	if (!IsRunning())
	{
		LogWarning("Can't get performance statistics. Engine is not running.");
		return FALSE;
	}

	if (gNMEngineData == NULL)
		return FALSE;

	PerformanceStatistics perfStats = gNMEngineData->GetPerformanceStatistics();

	*outFps				= perfStats.mFps;
	*outTheoreticalFps	= perfStats.mTheoreticalFps;
	*outAveragedTiming	= perfStats.mAveragedAlgorithmTime;
	*outBestCaseTiming	= perfStats.mBestCaseTiming;
	*outWorstCaseTiming	= perfStats.mWorstCaseTiming;

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helpers
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void EnableDebugLogging()
{
	EngineManager* engine = GetEngine();
	if (engine != NULL)
		CORE_LOGMANAGER.SetActiveLogLevelPreset( "Debug" );
}


void SetAllowAssetStreaming(BOOL allow)
{
	EngineManager* engine = GetEngine();
	if (engine != NULL)
		engine->SetAllowAssetStreaming(allow);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Devices
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// check if there is a device with the given type
BOOL HasDevice(EDevice type)
{
	uint32 internalType = CORE_INVALIDINDEX32;

	switch (type)
	{
		case EDevice::DEVICE_GENERIC_HEARTRATE:		internalType = HeartRateDevice::TYPE_ID;		break;
		case EDevice::DEVICE_GENERIC_ACCELEROMETER:	internalType = AccelerometerDevice::TYPE_ID;	break;
		case EDevice::DEVICE_GENERIC_GYROSCOPE:		internalType = GyroscopeDevice::TYPE_ID;		break;

#ifdef INCLUDE_DEVICE_INTERAXON_MUSE
		case EDevice::DEVICE_INTERAXON_MUSE:		internalType = MuseDevice::TYPE_ID;				break;
#endif

#ifdef INCLUDE_DEVICE_EMOTIV
        // include emotiv devices
		case EDevice::DEVICE_EMOTIV_EPOC:			internalType = EmotivEPOCDevice::TYPE_ID;		break;
		case EDevice::DEVICE_EMOTIV_INSIGHT:		internalType = EmotivInsightDevice::TYPE_ID;	break;
#endif

#ifdef INCLUDE_DEVICE_NEUROSKY_MINDWAVE
		case EDevice::DEVICE_NEUROSKY_MINDWAVE:		internalType = NeuroSkyDevice::TYPE_ID;			break;
#endif

#ifdef INCLUDE_DEVICE_SENSELABS_VERSUS
		case EDevice::DEVICE_SENSELABS_VESUS:		internalType = VersusDevice::TYPE_ID;			break;
#endif

#ifdef INCLUDE_DEVICE_ESENSESKINRESPONSE
		case EDevice::DEVICE_ESENSE_SKINRESPONSE:	internalType = eSenseSkinResponseDevice::TYPE_ID; break;
#endif

		default:
			return FALSE;
	}

	// find device by internal type id
	Array<Device*> devices = GetDeviceManager()->FindDevicesByType(internalType);
	return (devices.Size() > 0);
}


// add a device of a certain type
int AddDevice(EDevice type)
{
	// not available while running
	if (IsRunning())
	{
		LogError("neuromoreEngine::AddDevice(): Cannot add new device. Engine is running.");
		return -1;
	}

	Device* device = NULL;

	// map devices types of interface to the internal classes
	switch (type)
	{
		case EDevice::DEVICE_GENERIC_HEARTRATE:		device = new HeartRateDevice();			break;
		case EDevice::DEVICE_GENERIC_ACCELEROMETER:	device = new AccelerometerDevice();		break;
		case EDevice::DEVICE_GENERIC_GYROSCOPE:		device = new GyroscopeDevice();			break;

#ifdef INCLUDE_DEVICE_INTERAXON_MUSE
		case EDevice::DEVICE_INTERAXON_MUSE:		device = new MuseDevice();				break;
#endif

#ifdef INCLUDE_DEVICE_EMOTIV
        // include emotiv devices
		case EDevice::DEVICE_EMOTIV_EPOC:			device = new EmotivEPOCDevice();		break;
		case EDevice::DEVICE_EMOTIV_INSIGHT:		device = new EmotivInsightDevice();		break;
#endif

#ifdef INCLUDE_DEVICE_NEUROSKY_MINDWAVE
		case EDevice::DEVICE_NEUROSKY_MINDWAVE:		device = new NeuroSkyDevice();			break;
#endif

#ifdef INCLUDE_DEVICE_SENSELABS_VERSUS
		case EDevice::DEVICE_SENSELABS_VESUS:		device = new VersusDevice();			break;
#endif

#ifdef INCLUDE_DEVICE_ESENSESKINRESPONSE
		case EDevice::DEVICE_ESENSE_SKINRESPONSE:	device = new eSenseSkinResponseDevice(); break;
#endif

		default:
		{
			LogError("neuromoreEngine::AddDevice(): Cannot add new device. Specified device type unknown.");
			return -1;
		}
	}

	// add device
	GetDeviceManager()->AddDevice(device);

	// get the device index
	const uint32 deviceIndex = GetDeviceManager()->FindDeviceIndex(device);
	if (deviceIndex == CORE_INVALIDINDEX32)
	{
		LogError( "neuromoreEngine::AddDevice(): Something went wrong with adding a new device of type %i. Couldn't find the device in the internal device manager after adding it.", type );
		return -1;
	}

	LogInfo( "neuromoreEngine::AddDevice(): New device of type %i successfully added at position %i.", type, deviceIndex );
	return deviceIndex;
}


// Get the number of active devices.
int GetNumDevices()
{
	return GetDeviceManager()->GetNumDevices();
}


// remove a device 
BOOL RemoveDevice(int deviceIndex)
{
	// not available while running
	if (IsRunning())
	{
		LogError( "neuromoreEngine::RemoveDevice(): Cannot remove device at position %i. Engine is running.", deviceIndex );
		return FALSE;
	}

	// invalid device index
	if (deviceIndex < 0 || deviceIndex >= (int)GetDeviceManager()->GetNumDevices())
	{
		LogError( "neuromoreEngine::RemoveDevice(): Cannot remove device at position %i. Device is not in range of valid devices. Current number of devices is %i.", deviceIndex, GetDeviceManager()->GetNumDevices() );
		return FALSE;
	}

	// remove device
	Device* device = GetDeviceManager()->GetDevice(deviceIndex);
	GetDeviceManager()->RemoveDevice(device);
	LogInfo( "neuromoreEngine::RemoveDevice(): Device at position %i successfully removed.", deviceIndex );

	return TRUE;
}


EDevice GetDevice(int deviceIndex)
{
	if (deviceIndex < 0 || deviceIndex >= (int)GetDeviceManager()->GetNumDevices())
		return (EDevice)-1;

	Device* device = GetDeviceManager()->GetDevice(deviceIndex);

	// map devices types of interface to the internal classes
	switch (device->GetType())
	{
		case HeartRateDevice::TYPE_ID:				return EDevice::DEVICE_GENERIC_HEARTRATE;
		case AccelerometerDevice::TYPE_ID:			return EDevice::DEVICE_GENERIC_ACCELEROMETER;
		case GyroscopeDevice::TYPE_ID:				return EDevice::DEVICE_GENERIC_GYROSCOPE;

#ifdef INCLUDE_DEVICE_INTERAXON_MUSE
		case MuseDevice::TYPE_ID:					return EDevice::DEVICE_INTERAXON_MUSE;
#endif

#ifdef INCLUDE_DEVICE_EMOTIV
		// include emotiv devices
		case EmotivEPOCDevice::TYPE_ID:				return EDevice::DEVICE_EMOTIV_EPOC; 
		case EmotivInsightDevice::TYPE_ID:			return EDevice::DEVICE_EMOTIV_INSIGHT;
#endif

#ifdef INCLUDE_DEVICE_NEUROSKY_MINDWAVE
		case NeuroSkyDevice::TYPE_ID:				return EDevice::DEVICE_NEUROSKY_MINDWAVE;
#endif

#ifdef INCLUDE_DEVICE_SENSELABS_VERSUS
		case VersusDevice::TYPE_ID:					return EDevice::DEVICE_SENSELABS_VESUS;
#endif

#ifdef INCLUDE_DEVICE_ESENSESKINRESPONSE
		case eSenseSkinResponseDevice::TYPE_ID:		return EDevice::DEVICE_ESENSE_SKINRESPONSE;
#endif

		default:
			return (EDevice)-1;
	}

}


BOOL ConnectDevice(int deviceIndex)
{
    // invalid device index
    if (deviceIndex < 0 || deviceIndex >= (int)GetDeviceManager()->GetNumDevices())
        return FALSE;
    
    Device* device = GetDeviceManager()->GetDevice(deviceIndex);
    return device->Connect();
}


BOOL DisonnectDevice(int deviceIndex)
{
    // invalid device index
    if (deviceIndex < 0 || deviceIndex >= (int)GetDeviceManager()->GetNumDevices())
        return FALSE;
    
    Device* device = GetDeviceManager()->GetDevice(deviceIndex);
    return device->Disconnect();
}


// Get the number of inputs of a device.
int GetNumInputs(int deviceIndex)
{
	// invalid device index
	if (deviceIndex < 0 || deviceIndex >= (int)GetDeviceManager()->GetNumDevices())
		return -1;

	Device* device = GetDeviceManager()->GetDevice(deviceIndex);

	return device->GetNumSensors();
}


// Push a value into a device input.
BOOL AddInputSample(int deviceIndex, int inputIndex, double sampleValue)
{
	// return directly in case the engine is not running
	if (IsRunning() == false)
		return FALSE;

	// invalid device index
	if (deviceIndex < 0 || deviceIndex >= (int)GetDeviceManager()->GetNumDevices())
		return FALSE;

	Device* device = GetDeviceManager()->GetDevice(deviceIndex);

	// invalid input index
	if (inputIndex < 0 || inputIndex >= (int)device->GetNumSensors())
		return FALSE;

	// add input sample
	device->GetSensor(inputIndex)->AddQueuedSample(sampleValue);

	return TRUE;
}


// Set the battery charge level of a device.
BOOL SetBatteryChargeLevel(int deviceIndex, double normalizedCharge)
{
	// invalid device index
	if (deviceIndex < 0 || deviceIndex >= (int)GetDeviceManager()->GetNumDevices())
		return FALSE;

	Device* device = GetDeviceManager()->GetDevice(deviceIndex);

	// update device battery charge
	device->SetBatteryChargeLevel(normalizedCharge);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Classifier
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// load the given classifier from json
BOOL LoadClassifier(const char* jsonContent, const char* uuid, int revision)
{
	// make sure the engine got initialized and is not running
	if (GetEngine() == NULL)
		return FALSE;
	if (IsRunning())
		return FALSE;

	// try to load new classifier
	Classifier* classifier = new Classifier();
	if (GraphImporter::LoadFromString(jsonContent, classifier) == false)
	{
		delete classifier;
		classifier = NULL;
		return FALSE;
	}

	classifier->SetUuid( uuid );
	classifier->SetRevision( revision );
	classifier->CollectNodes();

	GetEngine()->LoadGraph(classifier);

	Reset();
	UpdateFeedbackData();

	return TRUE;
}


BOOL HasClassifier()
{
	// make sure the engine got initialized
	if (GetEngine() == NULL)
		return FALSE;

	Classifier* classifier = GetEngine()->GetActiveClassifier();
	return (classifier != NULL);
}


// check if the given device type is required by the classifier
BOOL IsDeviceRequiredByClassifier(EDevice deviceType)
{
	if (!HasClassifier())
		return FALSE;

	Classifier* classifier = GetEngine()->GetActiveClassifier();

	Array<Node*> nodes;
	switch (deviceType)
	{
		case EDevice::DEVICE_GENERIC_HEARTRATE:		{ classifier->CollectNodesOfType( HeartRateNode::TYPE_ID, &nodes );			break; }
		case EDevice::DEVICE_GENERIC_ACCELEROMETER:	{ classifier->CollectNodesOfType( AccelerometerNode::TYPE_ID, &nodes );		break; }
		case EDevice::DEVICE_GENERIC_GYROSCOPE:		{ classifier->CollectNodesOfType( GyroscopeNode::TYPE_ID, &nodes );			break; }

#ifdef INCLUDE_DEVICE_INTERAXON_MUSE
		case EDevice::DEVICE_INTERAXON_MUSE:		{ classifier->CollectNodesOfType( MuseNode::TYPE_ID, &nodes );				break; }
#endif

#ifdef INCLUDE_DEVICE_EMOTIV
        // include emotiv devices
		case EDevice::DEVICE_EMOTIV_EPOC:			{ classifier->CollectNodesOfType( EmotivEPOCNode::TYPE_ID, &nodes );		break; }
		case EDevice::DEVICE_EMOTIV_INSIGHT:		{ classifier->CollectNodesOfType( EmotivInsightNode::TYPE_ID, &nodes );		break; }
#endif

#ifdef INCLUDE_DEVICE_NEUROSKY_MINDWAVE
		case EDevice::DEVICE_NEUROSKY_MINDWAVE:		{ classifier->CollectNodesOfType( NeuroSkyNode::TYPE_ID, &nodes );			break; }
#endif

#ifdef INCLUDE_DEVICE_SENSELABS_VERSUS
		case EDevice::DEVICE_SENSELABS_VESUS:		{ classifier->CollectNodesOfType(VersusNode::TYPE_ID, &nodes);				break; }
#endif

#ifdef INCLUDE_DEVICE_ESENSESKINRESPONSE
		case EDevice::DEVICE_ESENSE_SKINRESPONSE:		{ classifier->CollectNodesOfType(eSenseSkinResponseNode::TYPE_ID, &nodes); break; }
#endif

		default:
			return FALSE;
	}

	if (nodes.IsEmpty() == true)
		return FALSE;
	else
		return TRUE;
}


// get the number of custom feedback nodes
int GetNumFeedbacks()
{
	if (gNMEngineData == NULL)
	{
		LogError("GetNumFeedbacks(): No engine data present.");
		return -1;
	}

	return gNMEngineData->mFeedbackData.GetNumFeedbacks();
}


// get the node name of a custom feedback node
const char* GetFeedbackName(int index)
{
	if (gNMEngineData == NULL)
		return "";

	if (index >= (int)gNMEngineData->mFeedbackData.GetNumFeedbacks())
		return "";

	const char* result = gNMEngineData->mFeedbackData.GetFeedbackName(index);
	if (result == NULL)
		return "";

	return result;
}


// value range of this feedback
void GetFeedbackRange(int index, double* outMinValue, double* outMaxValue)
{
	// zero outputs first
	*outMinValue = 0.0;
	*outMaxValue = 0.0;

	if (gNMEngineData == NULL)
		return;

	if (index >= (int)gNMEngineData->mFeedbackData.GetNumFeedbacks())
		return;

	// output values
	*outMinValue = gNMEngineData->mFeedbackData.GetFeedbackMinValue(index);
	*outMaxValue = gNMEngineData->mFeedbackData.GetFeedbackMaxValue(index);
}


// get the current feedback values
double GetCurrentFeedbackValue(int index)
{
	if (gNMEngineData == NULL)
		return 0.0;

	if (index >= (int)gNMEngineData->mFeedbackData.GetNumFeedbacks())
		return 0.0;

	return gNMEngineData->mFeedbackData.GetFeedbackValue(index);
}


// find the feedback index by name
int FindFeedbackIndexByName(const char* name)
{
	if (gNMEngineData == NULL)
	{
		LogError("GetFeedbackIndexByName(): No engine data present.");
		return -1;
	}

	// get the number of feedback nodes and iterate through them
	const uint32 numFeedbackNodes = gNMEngineData->mFeedbackData.GetNumFeedbacks();
	for (uint32 i=0; i<numFeedbackNodes; ++i)
	{
		// compare node names and return index in case they are equal
		const char* currentName = gNMEngineData->mFeedbackData.GetFeedbackName(i);
		if (strcmp(currentName, name) == 0)
			return i;
	}

	return -1;
}


// construct json request string for retreiving cloud input parameters
const char* CreateJSONRequestFindParameters(const char* userId)
{
	if (!HasClassifier())
		return "";

	Classifier* classifier = GetEngine()->GetActiveClassifier();
	
	// create request content
	Json jsonParser;
	Json::Item rootItem = jsonParser.GetRootItem();
	Json::Item paramArray = rootItem.AddArray("parameters");

	const uint32 numParameters = CloudParameters::CreateFindParametersJson(*classifier, paramArray);

	// nothing to do
	if (numParameters == 0)
		return "";

	// create json string and return it
	jsonParser.WriteToString(gNMEngineData->mTempJsonString, false);
	return gNMEngineData->mTempJsonString.AsChar();
}


// construct json request string for setting cloud output parameters
const char* CreateJSONRequestSetParameters()
{
	// no classifier: return NULL 
	if (!HasClassifier())
		return 0;

	Classifier* classifier = GetEngine()->GetActiveClassifier();

	CloudParameters parameters;
	classifier->SaveCloudParameters(parameters);

	const uint32 numParameters = parameters.GetParameters().Size();

	// no parameters -> empty json
	if (numParameters == 0)
		return "";

	// create request content
	Json jsonParser;
	Json::Item rootItem = jsonParser.GetRootItem();
	parameters.CreateSetRequestJson(rootItem);

	// create json string and return it
	jsonParser.WriteToString(gNMEngineData->mTempJsonString, false);
	return gNMEngineData->mTempJsonString.AsChar();
}


BOOL HandleJSONReplyFindParameters(const char* jsonString, const char* userId)
{
	if (!HasClassifier())
		return FALSE;

	Json jsonParser;
	if (!jsonParser.Parse(jsonString))
	{
		LogError("HandleJSONReplyFindParameters: Cannot parse JSON.");
		return FALSE;
	}

	// get the data
	CloudParameters parameters;

	Json::Item dataItem = jsonParser.Find("data");
	if (dataItem.IsNull() == false)
	{
		// get the system
		Json::Item parametersItem = dataItem.Find("parameters");
		if (parametersItem.IsArray() == true)
		{
			if (parameters.Load(parametersItem) == false)
			{
				LogError("HandleJSONReplyFindParameters::Error parsing FindParameter JSON Reply");
				return FALSE;
			}

		} 
		else
		{
			LogError("HandleJSONReplyFindParameters: Cannot find 'parameters' item in json.");
			return FALSE;
		}

	}
	else
	{
		LogError("HandleJSONReplyFindParameters: Cannot find 'data' item in json.");
		return FALSE;
	}

	// load parameters
	if (GetEngine()->LoadCloudParameters(parameters) == false)
	{
		LogWarning("HandleJSONReplyFindParameters: Engine did not process all parameters. Incompatible parameters?");
		// return false; // not sure if returning false here is good or bad ..
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cloud Data Serialization
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// calculate the number of data chunk channels
int GetNumDataChunkChannels()
{
	// make sure the engine got initialized
	if (GetEngine() == NULL)
	{
		LogError( "GetNumDataChunkChannels: Engine is not initialized." );
		return 0;
	}

	// get the active classifier and make sure it is valid
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	if (classifier == NULL)
	{
		LogError( "GetNumDataChunkChannels: No active classifier." );
		return 0;
	}

	uint32 result = 0;

	// get the number of feedback nodes and iterate through them
	const uint32 numFeedbackNodes = classifier->GetNumFeedbackNodes();
	for (uint32 i=0; i<numFeedbackNodes; ++i)
	{
		FeedbackNode* feedbackNode = classifier->GetFeedbackNode(i);
		
		// skip node if data upload is disabled in node
		if (feedbackNode->IsUploadEnabled() == false)
			continue;

		// add the number of output channels to our result
		result += feedbackNode->GetNumOutputChannels();
	}

	return result;
}


// get the correct feedback node and channel for the given data chunk channel index
BOOL GetFeedbackNodeChannel(uint32 channelIndex, FeedbackNode** outFeedbackNode, Channel<double>** outChannel)
{
	*outFeedbackNode = NULL;
	*outChannel = NULL;

	// get the active classifier
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	if (classifier == NULL)
		return FALSE;

	uint32 currentIndex = 0;

	// get the number of feedback nodes and iterate through them
	const uint32 numFeedbackNodes = classifier->GetNumFeedbackNodes();
	for (uint32 i=0; i<numFeedbackNodes; ++i)
	{
		FeedbackNode* feedbackNode = classifier->GetFeedbackNode(i);
		
		// skip node if data upload is disabled in node
		if (feedbackNode->IsUploadEnabled() == false)
			continue;

		// get the number of output channels and iterate through them
		const uint32 numChannels = feedbackNode->GetNumOutputChannels();
		for (uint32 j=0; j<numChannels; ++j)
		{
			Channel<double>* channel = feedbackNode->GetOutputChannel(j);

			// check if the given data chunk channel is the current one
			if (channelIndex == currentIndex)
			{
				*outFeedbackNode = feedbackNode;
				*outChannel = channel;
				return TRUE;
			}

			// increase our current index
			currentIndex++;
		}
	}

	return FALSE;
}


BOOL CheckClassifierPrerequisites(const char* functionName, BOOL shallEngineRun)
{
	// make sure the engine got initialized
	if (GetEngine() == NULL)
	{
		LogError( "%s: Engine is not initialized.", functionName );
		return FALSE;
	}

	// make sure the engine is in the right state
	if (IsRunning() != shallEngineRun)
	{
		LogError( "%s: Engine is still running.", functionName );
		return FALSE;
	}

	// get the active classifier and make sure it is valid
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	if (classifier == NULL)
	{
		LogError( "%s: No active classifier.", functionName );
		return FALSE;
	}

	return TRUE;
}


// get the create data chunk json for POST /api/datachunks/create
const char* GetCreateDataChunkJson(const char* userId, const char* experienceUuid, int experienceRevision)
{
	// check if the engine is in the correct state, if the classifier is valid etc.
	if (CheckClassifierPrerequisites("GetCreateDataChunkJson", false) == false)
		return "";

	// get the active state machine and make sure it is valid
	StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
	if (stateMachine == NULL)
	{
		LogError( "GetCreateDataChunkJson: No active state machine." );
		return 0;
	}

	// get access to the active classifier
	Session* session = GetSession();
	Classifier* classifier = GetEngine()->GetActiveClassifier();

	// generate json
	Json json;
	Json::Item rootItem = json.GetRootItem();
	bool result = SessionExporter::GenerateDataChunkJson( json, rootItem, userId, userId, userId, classifier->GetUuid(), classifier->GetRevision(), stateMachine->GetUuid(), stateMachine->GetRevision(), experienceUuid, experienceRevision, session->GetStartTime().AsUtcString(), session->GetStopTime().AsUtcString() );
	if (result == false)
	{
		LogError( "GetCreateDataChunkJson: Something went wrong while generating the json." );
		return "";
	}

	// write json object to string and return it
	json.WriteToString( gNMEngineData->mTempJsonString );
	return gNMEngineData->mTempJsonString.AsChar();
}


// get the data chunk channel json for POST /api/datachunks/upload
const char* GetDataChunkChannelJson(const char* userId, const char* dataChunkUuid, int channelIndex)
{
	// check if the engine is in the correct state, if the classifier is valid etc.
	if (CheckClassifierPrerequisites("GetDataChunkChannelJson", false) == false)
		return "";

	Classifier* classifier = GetEngine()->GetActiveClassifier();

	// find the feedback node and the given channel
	FeedbackNode* feedbackNode;
	Channel<double>* channel;
	bool foundChannel = GetFeedbackNodeChannel( channelIndex, &feedbackNode, &channel );
	if (foundChannel == false || feedbackNode == NULL || channel == NULL)
	{
		LogError( "GetDataChunkChannelJson: Cannot find node or channel for data chunk channel index %i.", channelIndex );
		return "";
	}

	// generate channel json
	Json json;
	Json::Item rootItem = json.GetRootItem();
	if (SessionExporter::GenerateChannelJson(json, rootItem, userId, dataChunkUuid, classifier->GetUuid(), feedbackNode->GetUuid(), channel) == false)
	{
		LogError( "GetDataChunkChannelJson: Something went wrong while generating the json." );
		return "";
	}

	// write json object to string and return it
	json.WriteToString( gNMEngineData->mTempJsonString );
	return gNMEngineData->mTempJsonString.AsChar();
}


// export the data chunk channel 
BOOL GenerateDataChunkChannelData(int channelIndex)
{
	// check if the engine is in the correct state, if the classifier is valid etc.
	if (CheckClassifierPrerequisites("GenerateDataChunkChannelData", false) == false)
		return FALSE;

	// find the feedback node and the given channel
	FeedbackNode* feedbackNode;
	Channel<double>* channel;
	bool foundChannel = GetFeedbackNodeChannel( channelIndex, &feedbackNode, &channel );
	if (foundChannel == false || feedbackNode == NULL || channel == NULL)
	{
		LogError( "GenerateDataChunkChannelData: Cannot find node or channel for data chunk channel index %i.", channelIndex );
		return FALSE;
	}

	// save samples to memory file
	if (SessionExporter::SaveSamplesToMemoryFile(&gNMEngineData->mTempMemoryFile, channel) == false)
	{
		LogError( "GenerateDataChunkChannelData: Something went wrong with serializing the channel." );
		return FALSE;
	}

	return TRUE;
}


const char* GetDataChunkChannelData(int channelIndex)
{
	if (gNMEngineData == NULL)
		return NULL;

	return (const char*)gNMEngineData->mTempMemoryFile.GetData();
}


int GetDataChunkChannelDataSize(int channelIndex)
{
	if (gNMEngineData == NULL)
		return 0;

	return gNMEngineData->mTempMemoryFile.GetSize();
}


void ClearDataChunkChannelData()
{
	if (gNMEngineData == NULL)
		return;

	return gNMEngineData->mTempMemoryFile.Close();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// State Machine
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// load the given state machine from json
BOOL LoadStateMachine(const char* jsonContent, const char* uuid, int revision)
{
	// make sure the engine got initialized and is not already running
	if (GetEngine() == NULL)
		return FALSE;
	if (IsRunning())
		return FALSE;

	// load statemachine
	StateMachine* stateMachine = new StateMachine();
	if (GraphImporter::LoadFromString(jsonContent, stateMachine) == false)
	{
		delete stateMachine;
		stateMachine = NULL;
		return FALSE;
	}

	stateMachine->SetUuid( uuid );
	stateMachine->SetRevision( revision );
	
	// collect all assets the state machine uses
	stateMachine->CollectStates();
	stateMachine->CollectAssets();
	gNMEngineData->mStateMachineAssets = stateMachine->GetAssets();

	// load the statemachine into the engine
	GetEngine()->LoadGraph(stateMachine);

	return TRUE;
}


BOOL HasStateMachine()
{
	// make sure the engine got initialized
	if (GetEngine() == NULL)
		return FALSE;

	StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
	return (stateMachine != NULL);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Multimedia Assets
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// convert asset type from exposed API type to internal
Action::AssetType ConvertAssetType(AssetType type)
{
	switch (type)
	{
		case ASSET_AUDIO: { return Action::ASSET_AUDIO; }
		case ASSET_VIDEO: { return Action::ASSET_VIDEO; }
		case ASSET_IMAGE: { return Action::ASSET_IMAGE; }
        default:          { return Action::ASSET_NONE; }
	}
}


int GetNumAssetsOfType(AssetType type)
{
	// make sure the engine got initialized
	if (GetEngine() == NULL)
		return 0;

	// get the active state machine and make sure it is valid
	StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
	if (stateMachine == NULL)
		return 0;

	return stateMachine->GetNumAssetsOfType( ConvertAssetType(type) );
}


const char* GetAssetLocationOfType(AssetType type, int index)
{
	// make sure the engine got initialized
	if (GetEngine() == NULL)
		return 0;

	// get the active state machine and make sure it is valid
	StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
	if (stateMachine == NULL)
		return 0;

	return stateMachine->GetAssetLocationOfType( ConvertAssetType(type), index );
}


BOOL GetAssetAllowStreamingOfType(AssetType type, int index)
{
	// make sure the engine got initialized
	if (GetEngine() == NULL)
		return FALSE;

	// get the active state machine and make sure it is valid
	StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
	if (stateMachine == NULL)
		return FALSE;

	return stateMachine->GetAssetAllowStreamingOfType( ConvertAssetType(type), index );
}


int GetNumAssets()
{
	// make sure the engine got initialized
	if (GetEngine() == NULL)
		return 0;

	// get the active state machine and make sure it is valid
	StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
	if (stateMachine == NULL)
		return 0;

	return stateMachine->GetAssets().Size();
}


const char* GetAssetLocation(int index)
{
	// make sure the engine got initialized
	if (GetEngine() == NULL)
		return 0;

	// get the active state machine and make sure it is valid
	StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
	if (stateMachine == NULL)
		return 0;

	return stateMachine->GetAssets()[index].mLocation;
}


BOOL GetAssetAllowStreaming(int index)
{
	// make sure the engine got initialized
	if (GetEngine() == NULL)
		return FALSE;

	// get the active state machine and make sure it is valid
	StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
	if (stateMachine == NULL)
		return FALSE;

	return stateMachine->GetAssets()[index].mAllowStreaming;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Expeprience
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// load the given classifier from json
BOOL LoadExperience(const char* jsonContent, const char* uuid, int revision)
{
	// make sure the engine got initialized and is not running
	if (GetEngine() == NULL)
		return FALSE;
	if (IsRunning())
		return FALSE;

	// try to load new classifier
	Json jsonParser;
	jsonParser.Parse(jsonContent);

	Experience* experience = new Experience();
	if (experience->Load(jsonParser, jsonParser.GetRootItem()) == false)
	{
		delete experience;
		experience = NULL;
		return FALSE;
	}

	experience->SetUuid( uuid );
	experience->SetRevision( revision );
	
	GetEngine()->LoadExperience(experience);

	Reset();
	UpdateFeedbackData();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SetCallback(Callback* callback)
{
	// check if there already is a callback assigned
	if (gCallback != NULL)
	{
		// TODO: change this once the CPP one is gone
		// this is also bad coding, should not release foreign allocated mem here
#if defined(NEUROMORE_ENGINE_CPP_CALLBACK)
		delete gCallback;
#else
		free(gCallback);
#endif
		gCallback = NULL;
	}

	gCallback = callback;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Input Events
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// called when a button got clicked
BOOL ButtonClicked(int buttonId)
{
	// skip directly in case the engine is not running
	if (IsRunning())
		return FALSE;

	// no state machine loaded
	if (!HasStateMachine())
		return FALSE;

	// get the active state machine
	StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
	if (stateMachine == NULL)
		return FALSE;

	// get the number of transitions and iterate through them
	const uint32 numTransitions = stateMachine->GetNumConnections();
	for (uint32 i=0; i<numTransitions; ++i)
	{
		StateTransition* transition = stateMachine->GetTransition(i);

		// get the number of conditions and iterate through them
		const uint32 numConditions = transition->GetNumConditions();
		for (uint32 j=0; j<numConditions; ++j)
		{
			StateTransitionCondition* condition = transition->GetCondition(j);

			// check if the current condition is a button condition
			if (condition->GetType() == StateTransitionButtonCondition::TYPE_ID)
			{
				StateTransitionButtonCondition* buttonCondition = static_cast<StateTransitionButtonCondition*>(condition);

				// check if the condition id equals the button id
				if (buttonCondition->GetID() == (uint32)buttonId)
				{
					// click the button
					buttonCondition->ButtonClicked();

					//LogInfo( "Button '%s' clicked.", button->text().toLatin1().data() );
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}


// call this when an audio file looped
BOOL AudioLooped(const char* url)
{
	// skip directly in case the engine is not running
	if (!IsRunning())
		return FALSE;

	// no state machine loaded
	if (!HasStateMachine())
		return FALSE;

	// get the active state machine
	StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
	if (stateMachine == NULL)
		return FALSE;

	String urlString = url;

	// get the number of transitions and iterate through them
	const uint32 numTransitions = stateMachine->GetNumConnections();
	for (uint32 i=0; i<numTransitions; ++i)
	{
		StateTransition* transition = stateMachine->GetTransition(i);

		// get the number of conditions and iterate through them
		const uint32 numConditions = transition->GetNumConditions();
		for (uint32 j=0; j<numConditions; ++j)
		{
			StateTransitionCondition* condition = transition->GetCondition(j);

			// check if the current condition is a audio condition
			if (condition->GetType() == StateTransitionAudioCondition::TYPE_ID)
			{
				StateTransitionAudioCondition* audioCondition = static_cast<StateTransitionAudioCondition*>(condition);

				// check if the condition id equals the button id
				if (urlString.IsEqual(audioCondition->GetUrl()) == true)
				{
					// increase the loop counter
					audioCondition->Looped();

					//LogInfo( "AudioCondition increased loop counter '%i'.", audioCondition->GetLoops() );
					return TRUE;
				}
			}
		}
	}

	return TRUE; // TODO: Why is this true, should be false?
}


// call this when an video file looped
BOOL VideoLooped(const char* url)
{
	// skip directly in case the engine is not running
	if (!IsRunning())
		return FALSE;

	// no state machine loaded
	if (!HasStateMachine())
		return FALSE;

	// get the active state machine
	StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
	if (stateMachine == NULL)
		return FALSE;

	String urlString = url;

	// get the number of transitions and iterate through them
	const uint32 numTransitions = stateMachine->GetNumConnections();
	for (uint32 i=0; i<numTransitions; ++i)
	{
		StateTransition* transition = stateMachine->GetTransition(i);

		// get the number of conditions and iterate through them
		const uint32 numConditions = transition->GetNumConditions();
		for (uint32 j=0; j<numConditions; ++j)
		{
			StateTransitionCondition* condition = transition->GetCondition(j);

			// check if the current condition is a audio condition
			if (condition->GetType() == StateTransitionVideoCondition::TYPE_ID)
			{
				StateTransitionVideoCondition* videoCondition = static_cast<StateTransitionVideoCondition*>(condition);

				// check if the condition id equals the button id
				if (urlString.IsEqual(videoCondition->GetUrl()) == true)
				{
					// increase the loop counter
					videoCondition->Looped();

					//LogInfo( "VideoCondition increased loop counter '%i'.", videoCondition->GetLoops() );
					return TRUE;
				}
			}
		}
	}

	return TRUE; // TODO: Why is this true, should be false?
}

}; // namespace neuromoreEngine
