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

#ifndef __NEUROMORE_ENGINEMANAGER_H
#define __NEUROMORE_ENGINEMANAGER_H

// include required headers
#include "Config.h"
#include "Core/StandardHeaders.h"
#include "Core/String.h"
#include "Core/Array.h"
#include "Core/Time.h"
#include "Core/Timer.h"
#include "Core/Version.h"
#include "Core/FpsCounter.h"
#include "Core/EventManager.h"
#include "Core/EventSource.h"
#include "Core/Counter.h"
#include "BciDevice.h"
#include "EEGElectrodes.h"
#include "Session.h"
#include "Experience.h"
#include "User.h"
#include "DSP/SpectrumAnalyzerSettings.h"
#include "Graph/GraphManager.h"
#include "Graph/GraphObjectFactory.h"
#include "Graph/Classifier.h"
#include "Graph/StateMachine.h"
#include "Networking/OscMessageRouter.h"
#include "DeviceManager.h"
#include "SerialPortManager.h"
#include "Core/AttributeFactory.h"

// forward declarations


// the neuromore Engine manager class
class ENGINE_API EngineManager : public Core::EventSource
{
	friend class EngineInitializer;

	public:
		class Callback
		{
			public:
            Callback()                                                                                      {}
            virtual ~Callback()                                                                             {}
				virtual Core::String GenerateRandomUUID() = 0;
				Core::String ConstructRandomFilename(const char* folderPath, const char* fileExtension)		{ Core::String result = folderPath; result += GenerateRandomUUID(); result += fileExtension; return result; }
				virtual Core::Time Now() = 0;
		};

		// callback
		void SetCallback(Callback* callback)									{ mCallback = callback; }
		Callback* GetCallback()													{ return mCallback; }


		//
		// Engine Control
		//

		// main update function
		void Update(Core::Time delta);
		
		// TODO add another type of reset  (unload everything, so its like brand new; Reset() doesn't do that for us
		// engine reset
		void Reset();

		// get elapsed engine time since engine start
		Core::Time GetElapsedTime()												{ return mElapsedTime; }

		// TODO use Start() / Stop() and move the state in the engine
		//		
		//      add CanStart() [move the logic from neuromoreEngine here]
		// start stop the engine
		bool IsRunning() const													{ return mIsRunning; }

		// TODO get rid of this, never controll the running flag externally (if you don't want to update the engine, don't call the update function). Only use start/stop to start the engine.
		void SetIsRunning(bool enabled = true)									{ mIsRunning = enabled; }

		// soft-pause feature
		void SoftPause();
		void SoftContinue();
		bool IsSoftPaused() const												{ return mIsSoftPaused; }

		// FIXME make these private, they should never be called from outside
		// asynchronous executed sample-level synchronization accross all sensors
		void Sync();
		void SyncAsync();

		//
		// Loading 
		//

		// load graphs (classifier/statemachines)
		bool LoadStateMachine(StateMachine* stateMachine);
		bool LoadClassifier(Classifier* classifier);
		
		// load any graph (also nested graphs)
		bool LoadGraph(Graph* graph);
		bool UnloadGraph(Graph* graph);

		// load/unload an experience
		bool LoadExperience(Experience* experience);

		bool LoadCloudParameters(const CloudParameters& parameters);
		
		// TODO loader for other ressources? Like edf files?

		// get active classifier/statemachine/experience
		Classifier* GetActiveClassifier()										{ return mActiveClassifier; }
		StateMachine* GetActiveStateMachine()									{ return mActiveStateMachine; }
		Experience* GetActiveExperience()										{ return mActiveExperience; }
	
		// unload everything that was loaded 
		void Unload();
		void UnloadClassifiers();
		void UnloadStateMachines();


		//
		// User and Session
		//

		// the active session
		Session* GetSession()													{ return mSession; }

		// main user
		void SetUser(const User& user)											{ mUser = user; SetSessionUser(mUser); }
		User* GetUser()															{ return &mUser; }

		// session user (defaults to mUser)
		void RemoveSessionUser()												{ SetSessionUser(mUser);  }
		void SetSessionUser(const User& user);
		User* GetSessionUser()													{ return &mSessionUser; }

		
		//
		// CORE SYSTEMS
		//

		inline Core::LogManager& GetLogManager()								{ return *mLogManager; }
		inline Core::Counter& GetCounter()										{ return *mCounter; }
		inline Core::EventManager& GetEventManager()							{ return *mEventManager; }
		inline Core::AttributeFactory& GetAttributeFactory()					{ return *mAttributeFactory; }


		//
		// DEVICES
		//

		// device manager
		DeviceManager* GetDeviceManager() const									{ return mDeviceManager; }
		
		// serial port management
		SerialPortManager* GetSerialPortManager() const							{ return mSerialPortManager; }

		// Note: this is only used to select which BCI should be displayed in the studio. This should not be here.
		// neuro headset management
		void SetActiveBci(BciDevice* device);
		BciDevice* GetActiveBci() const											{ return mActiveBci; }

		// 10-20 system (EEG)
		EEGElectrodes* GetEEGElectrodes()										{ return mEEGElectrodes; } 
		
		//
		// Networking 
		//

		// networking
		OscMessageRouter* GetOscMessageRouter() const							{ return mOscMessageRouter; }

		// graph helpers
		GraphObjectFactory* GetGraphObjectFactory()								{ return mGraphObjectFactory; }
		GraphManager* GetGraphManager()											{ return mGraphManager; }

	
		//
		// Engine Misc
		// 

		// performance statistics
		const Core::FpsCounter& GetFpsCounter() const							{ return mFpsCounter; }
		
		// version
		Core::Version GetVersion() const										{ return mVersion; }

		// data file management
		const Core::String& GetAppDataFolder() const							{ return mAppDataFolder; }
		void SetAppDataFolder(const Core::String& appDataFolder)				{ mAppDataFolder = appDataFolder; }

	
		//
		// Settings
		//

		// spectrum analyzer settings
		SpectrumAnalyzerSettings* GetSpectrumAnalyzerSettings()					{ return mSpectrumAnalyzerSettings; }

		// power line frequency
		enum EPowerLineFrequencyType
		{
			POWERLINEFREQ_AUTO = 0,
			POWERLINEFREQ_50HZ = 1,
			POWERLINEFREQ_60HZ = 2
		};


		double GetPowerLineFrequency(User* user) const;
		void SetPowerLineFrequencyType(EPowerLineFrequencyType type)			{ mPowerLineFrequencyType = type; }
		EPowerLineFrequencyType GetPowerLineFrequencyType() const				{ return mPowerLineFrequencyType; }

		// TODO get rid of this
		// auto sync
		bool GetAutoSyncSetting() const											{ return mAutoSyncEnabled; }
		void SetAutoSyncSetting(bool enable)									{ mAutoSyncEnabled = enable; }

		// auto detection
		bool GetAutoDetectionSetting() const									{ return mAutoDetectionEnabled; }
		void SetAutoDetectionSetting(bool enable);

		// asset streaming
		bool IsAssetStreamingAllowed() const									{ return mAllowAssetStreaming; }
		void SetAllowAssetStreaming(bool allow)									{ mAllowAssetStreaming = allow; }

		// drift correction
		struct DriftCorrectionSettings
		{
			bool mIsEnabled;
			double mMaxDriftUntilSync;
			double mMaxForwardDrift;		// early signal
			double mMaxBackwardDrift;		// late signal
		};

		DriftCorrectionSettings& GetDriftCorrectionSettings()					{ return mDriftCorrectionSettings; }

	protected:

		// constructor & destructor
		EngineManager();
		virtual ~EngineManager();
		bool Init();

	private:

		// main user
		User							mUser;
		
		// optional session user
		User							mSessionUser;

		// version
		Core::Version					mVersion;

		// data file management
		Core::String					mAppDataFolder;

		// session
		Session*						mSession;
		bool							mIsRunning;
		bool							mIsSoftPaused;
		Core::Time						mElapsedTime;
		bool							mAllowAssetStreaming;

		// async sensor syncing
		bool							mDoSync;

		// core systems
		Core::LogManager*				mLogManager;
		Core::Counter*					mCounter;
		Core::AttributeFactory*			mAttributeFactory;
		Core::EventManager*				mEventManager;
		
		// devices
		DeviceManager*					mDeviceManager;
		BciDevice*						mActiveBci;
		EEGElectrodes*					mEEGElectrodes;
		SerialPortManager*				mSerialPortManager;

		// networking
		OscMessageRouter*				mOscMessageRouter;

		// signal processing
		SpectrumAnalyzerSettings*		mSpectrumAnalyzerSettings;

		// power line frequency
		EPowerLineFrequencyType			mPowerLineFrequencyType;

		// other settings
		bool							mAutoSyncEnabled;
		bool							mAutoDetectionEnabled;
		DriftCorrectionSettings			mDriftCorrectionSettings;

		// graphs and graphbjects
		GraphManager*					mGraphManager;
		GraphObjectFactory*				mGraphObjectFactory;

			
		// reference to active classifier/statemachine owned by graph manager
		void SetActiveClassifier(Classifier* classifier);
		void SetActiveStateMachine(StateMachine* classifier);
		Classifier*						mActiveClassifier;
		StateMachine*					mActiveStateMachine;

		// experience (owned by engine)
		Experience*						mActiveExperience;

		// callbacks
		Callback*						mCallback;

		// performance timing
		Core::FpsCounter				mFpsCounter;

};


// the neuromore Engine initializer
class ENGINE_API EngineInitializer
{
	public:
		static bool Init();
		static void Shutdown();
};


// the global
extern ENGINE_API EngineManager* gEngineManager;

// core shortcuts
#define CORE_LOGMANAGER			(gEngineManager->GetLogManager())
#define CORE_COUNTER			(gEngineManager->GetCounter())
#define CORE_STRINGIDGENERATOR	(gEngineManager->GetStringIdGenerator())
#define CORE_ATTRIBUTEFACTORY	(gEngineManager->GetAttributeFactory())
#define CORE_EVENTMANAGER		(gEngineManager->GetEventManager())

// shortcuts
inline EngineManager*		GetEngine()						{ return gEngineManager; }
inline User*				GetUser()						{ return gEngineManager->GetUser(); }
inline User*				GetSessionUser()				{ return gEngineManager->GetSessionUser(); }
inline DeviceManager*		GetDeviceManager()				{ return gEngineManager->GetDeviceManager(); }
inline EEGElectrodes*		GetEEGElectrodes()				{ return gEngineManager->GetEEGElectrodes(); }
inline Session*				GetSession()					{ return gEngineManager->GetSession(); }
inline GraphObjectFactory*	GetGraphObjectFactory()			{ return gEngineManager->GetGraphObjectFactory(); }
inline GraphManager*		GetGraphManager()				{ return gEngineManager->GetGraphManager(); }
inline OscMessageRouter*	GetOscMessageRouter()			{ return gEngineManager->GetOscMessageRouter(); }
inline SerialPortManager*	GetSerialPortManager()			{ return gEngineManager->GetSerialPortManager(); }

inline Core::String		GenerateRandomUuid()			{ EngineManager::Callback* callback = gEngineManager->GetCallback(); if (callback == NULL) return ""; return callback->GenerateRandomUUID(); }

#endif
