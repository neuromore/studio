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

// include required files
#include "EngineManager.h"
#include "Core/LogManager.h"
#include "Devices/DeviceInventory.h"
#include "Core/EventHandler.h"
#include "Version.h"

using namespace Core;

// the global neuromore Engine manager
ENGINE_API EngineManager* gEngineManager = NULL;

//--------------------------------------------------

// constructor
EngineManager::EngineManager()
{
	mLogManager			= NULL;
	mCounter			= NULL;
	mEventManager		= NULL;
	mAttributeFactory	= NULL;

	// set version
	mVersion = Version( NEUROMORE_ENGINE_VERSION_MAJOR, NEUROMORE_ENGINE_VERSION_MINOR, NEUROMORE_ENGINE_VERSION_PATCH );
}


// destructor
EngineManager::~EngineManager()
{
	LogDetailedInfo("Destructing engine manager");

	// get rid of the loaded experience
	delete mActiveExperience;

	// get rid of Device Manager (includes all device systems and devices)
	delete mDeviceManager;

	// get rid of the EEG 10-20 system electrodes
	delete mEEGElectrodes;

	// get rid of the session
	delete mSession;

	// get rid of the graph object factory
	delete mGraphObjectFactory;

	// get rid of the graph manager
	delete mGraphManager;

	// get rid of the spectrum analyzer settings
	delete mSpectrumAnalyzerSettings;

	// get rid of the callback
	delete mCallback;

	// networking
	delete mOscMessageRouter;

	// serial port manager
	delete mSerialPortManager;


	// destruct core systems

	// shutdown the log manager
	delete mLogManager;
	mLogManager = NULL;

	// delete the counter
	delete mCounter;
	mCounter = NULL;

	// delete the attribute factory
	delete mAttributeFactory;
	mAttributeFactory = NULL;

	// get rid of the event manager
	delete mEventManager;
	mEventManager = NULL;
}


// initialize the core manager
bool EngineManager::Init()
{
	// initialize core systems
	mLogManager			= new LogManager();
	LogDetailedInfo("Initializing engine manager ...");

	mCounter			= new Counter();
	mAttributeFactory	= new AttributeFactory();
	mEventManager		= new EventManager();


	// state
	mActiveBci				= NULL;
	mActiveClassifier		= NULL;
	mActiveStateMachine		= NULL;
	mActiveExperience		= NULL;
	mIsRunning				= true;
	mIsSoftPaused			= false;
	mAllowAssetStreaming	= false;
	mElapsedTime			= 0;

	// general config
	mPowerLineFrequencyType	= POWERLINEFREQ_AUTO;
	mAutoSyncEnabled		= true;
	mAutoDetectionEnabled	= Branding::DefaultAutoDetectionEnabled;

	// drift correction settings
	mDriftCorrectionSettings.mIsEnabled = true;
	mDriftCorrectionSettings.mMaxDriftUntilSync	= 2;
	mDriftCorrectionSettings.mMaxForwardDrift	= 0.2;
	mDriftCorrectionSettings.mMaxBackwardDrift	= 1.0;

	// callbacks
	mCallback				= NULL;

	// add the EEG 10-20 system electrodes
	mEEGElectrodes			= new EEGElectrodes();

	// create the spectrum analyzer settings
	mSpectrumAnalyzerSettings = new SpectrumAnalyzerSettings();

	// create the osc router (must be created before device manager!)
	mOscMessageRouter		= new OscMessageRouter();

	// construct and initialize device manager
	mDeviceManager = new DeviceManager();
	if (mDeviceManager->Init() == false)
		return false;

	// create the default session
	mSession				= new Session();

	// create the graph manager
	mGraphManager			= new GraphManager();

	// create the graph object factory
	mGraphObjectFactory		= new GraphObjectFactory();

	// create the serial port manager
	mSerialPortManager		= new SerialPortManager();


	LogDetailedInfo("Engine manager initialized");

	return true;
}



// update the core manager
void EngineManager::Update(Time delta)
{
	// skip the update in case the whole system is paused
	if (mIsRunning == false)
		return;

	// important: during syncing we ignore the timer value and perform an update call with 0 time
	if (mDoSync == true)
		delta = 0;

	mFpsCounter.BeginTiming();

	// increase current elapsed time
	mElapsedTime += delta;

	// FIXME change other update routines to use the new time format
	const double timeDelta = delta.InSeconds();

	// 1) update the session first (because of session state and time elapsed)
	mSession->Update(mElapsedTime, timeDelta);

	// 2) update the osc message router, so the newest messages are pushed into the devices
	// this internally passes the asynch received messages over to the receiver objects and let them process the data
	mOscMessageRouter->ProcessData();
	mOscMessageRouter->ScrubPacketPool();

	// 3) update the devices, so the newest messages will be processed
	mDeviceManager->Update(mElapsedTime, delta);

	// TODO get rid of engine sync here (update loop should never reset the engine, it has to be called from outside by the owner that controls it (studio, app, etc)

	// 4) sync engine sensors after device update, so the list of sensors is up to date
	if (mDoSync == true)
	{
		Reset();

		//// explicit reinit before sync (the reinit would not be called directly after loading a classifier)
		//if (mActiveClassifier != NULL)
		//{
		//	mActiveClassifier->Reset();
		//	mActiveClassifier->Finalize(0.0, 0.0);
		//}

		//Sync();

		mDoSync = false;
		return;
	}

	// 5) update classifier (if we did not sync)
	// update and output the state machine
	if (mActiveStateMachine != NULL)
		mActiveStateMachine->Update(mElapsedTime, delta);

	// update and output the classifier
	if (mActiveClassifier != NULL)
		mActiveClassifier->Update(mElapsedTime, delta);

	mFpsCounter.StopTiming();
}


void EngineManager::Reset()
{
	mElapsedTime = 0;
	mDoSync = false;
	mIsSoftPaused = false;

	// reset session
	mSession->Reset();

	// reset classifier
	Classifier* activeClassifier = GetEngine()->GetActiveClassifier();
	if (activeClassifier != NULL)
		activeClassifier->Reset();

	// reset statemachine
	StateMachine* activeStateMachine = GetEngine()->GetActiveStateMachine();
	if (activeStateMachine != NULL)
		activeStateMachine->Reset();

	// reset device (data)
	GetDeviceManager()->ResetDevices();

	// clean network packet queues
	mOscMessageRouter->ScrubPacketPool();

	// sync engine
	Sync();
}


void EngineManager::SoftPause()
{
	// already paused
	if (mIsSoftPaused == true)
		return;

	mIsSoftPaused = true;

	if (mActiveClassifier != NULL)
		mActiveClassifier->Pause();

	if (mActiveStateMachine != NULL)
		mActiveStateMachine->Pause();
}


void EngineManager::SoftContinue()
{
	// already unpaused
	if (mIsSoftPaused == false)
		return;

	mIsSoftPaused = false;

	if (mActiveClassifier != NULL)
		mActiveClassifier->Continue();

	if (mActiveStateMachine != NULL)
		mActiveStateMachine->Continue();
}


// sample-level synchronization accross all sensors inside the engine
void EngineManager::Sync()
{
	LogInfo ("Syncing Engine Sensors ... ");

	// reset elapsed time to 0
	mElapsedTime = 0;

	// find the highest input delay across all sensors of all devices
	double maxLatency = GetDeviceManager()->FindMaximumLatency();

	// find the highest input delay across all sensors of the input nodes inside the classifier (if any)
	Classifier* classifier = GetActiveClassifier();
	if (classifier != NULL)
		maxLatency = Max(maxLatency, classifier->FindMaximumInputLatency());

	// sync all device sensors
	GetDeviceManager()->SyncDevices(maxLatency);

	// call update and run 
	if (classifier != NULL)
		classifier->Update(maxLatency, maxLatency);
}


void EngineManager::SyncAsync()
{
	// do not sync if the session is running!
	if (mSession->IsRunning() == false)
	{ 
		mDoSync = true;
	}
}


// load any graph (also nested graphs)
bool EngineManager::LoadGraph(Graph* graph)
{
	switch (graph->GetType())
	{
		case Classifier::TYPE_ID:	return LoadClassifier(static_cast<Classifier*>(graph));
		case StateMachine::TYPE_ID:	return LoadStateMachine(static_cast<StateMachine*>(graph));
		
			// TODO: child graphs

		default: return false;
	}
}


// unload graph
bool EngineManager::UnloadGraph(Graph* graph)
{
	if (mGraphManager->FindGraphIndex(graph) == CORE_INVALIDINDEX32)
		return false;

	Classifier* classifier = mActiveClassifier;
	if (graph == classifier)
		SetActiveClassifier(NULL);

	StateMachine* stateMachine = mActiveStateMachine;
	if (graph == stateMachine)
		SetActiveStateMachine(NULL);
	
	// copy name before deleting graph
	String graphName = graph->GetName();

	const bool success = mGraphManager->RemoveGraph(graph);
	if (success)
		LogDetailedInfo("Unloaded graph '%s'", graphName.AsChar());
	else
		LogError("Failed unloading graph '%s': not found in engine", graphName.AsChar());

	return success;
}



bool EngineManager::LoadClassifier(Classifier* classifier)
{
	// there can only be one graph with his ID
	UnloadGraph(classifier);

	// HACK there can only be one classifier right now -> purge array before loading the new one
	UnloadClassifiers();

	// TODO: check here if all  nested graphs are loaded, and tell the classifier about them; abort with error otherwise

	// add to graph pool
	mGraphManager->AddGraph(classifier);

	// set as active
	SetActiveClassifier(classifier);

	return true;
}


// load graphs (classifier/statemachines)
bool EngineManager::LoadStateMachine(StateMachine* stateMachine)
{
	// there can only be one graph with his ID
	UnloadGraph(stateMachine);

	// HACK there can only be one statemachine right now -> purge array before loading the new one
	UnloadStateMachines();


	// TODO: check here if all  nested graphs are loaded, and tell the statemachine about them; abort with error otherwise

	// add to graph pool
	mGraphManager->AddGraph(stateMachine);

	// set as active
	SetActiveStateMachine(stateMachine);

	return true;
}


// load an experience
bool EngineManager::LoadExperience(Experience* experience)
{
	// enforce that we only load into a clean engine
	if (mActiveExperience != NULL)
	{
		LogError("Cannot load a second experience. Clear() and Reset() the engine first.");
		CORE_ASSERT(false);
		return false;
	}

	mActiveExperience = experience;

	//// load attached classifier
	//Classifier* classifier = experience->GetAttachedClassifier();
	//if (classifier != NULL)
	//	if (LoadClassifier(classifier) == false)
	//		return false;

	//// load attached statemachine
	//StateMachine* stateMachine = experience->GetAttachedStateMachine();
	//if (stateMachine != NULL)
	//	if (LoadStateMachine(stateMachine) == false)
	//		return false;

	// load graph settings from experience
	if (experience != NULL)
	{
		if (mActiveClassifier != NULL && experience->GetClassifierSettings().Size() > 0)
		{
			mActiveClassifier->ApplySettings(experience->GetClassifierSettings());
			mActiveClassifier->SetIsDirty(false);
		}

		if (mActiveStateMachine != NULL && experience->GetStateMachineSettings().Size() > 0)
		{
			mActiveStateMachine->ApplySettings(experience->GetStateMachineSettings());
			mActiveStateMachine->SetIsDirty(false);
		}
	}

	EMIT_EVENT( OnActiveExperienceChanged(mActiveExperience) );

	return true;
}


bool EngineManager::LoadCloudParameters(const CloudParameters& parameters)
{
	uint32 numApplied = 0;

	if (mActiveExperience != NULL)
		numApplied += mActiveExperience->LoadCloudParameters(parameters);
	
	// load only if this classifier does not belong to engine
	if (mActiveClassifier != NULL)
		numApplied += mActiveClassifier->LoadCloudParameters(parameters);

	// Note: no cloud parameters for state machine

	if (numApplied != parameters.GetParameters().Size())
	{
		LogWarning("Warning: the engine did not use every cloud parameter that was supplied");
		return false;
	}
	
	LogInfo("Loaded %i Cloud Parameters.", numApplied);
	return true;
}


// activate another neuro headset
void EngineManager::SetActiveBci(BciDevice* device)
{
	LogDetailedInfo("Setting active headset");

	mActiveBci = device;

	EMIT_EVENT(OnActiveBciChanged(device));
}


// set the given graph as the new classifier
void EngineManager::SetActiveClassifier(Classifier* classifier)
{
	// TODO check if classifier is complete and runnable, dont set it as active otherwise
	LogDetailedInfo("Setting active classifier");

	mActiveClassifier = classifier;

	EMIT_EVENT(OnActiveClassifierChanged(mActiveClassifier));
}


// set the given graph as the new state machine
void EngineManager::SetActiveStateMachine(StateMachine* stateMachine)
{
	mActiveStateMachine = stateMachine;

	LogDetailedInfo("Setting active state machine");

	EMIT_EVENT(OnActiveStateMachineChanged(mActiveStateMachine));
}


// unload the experience again // TODO get rid of this
void EngineManager::Unload()
{
	// always unload all graphs first (incl. active state machine and classifier)
	const uint32 numGraphs = mGraphManager->GetNumGraphs();
	for (uint32 i=0; i<numGraphs; ++i)
		UnloadGraph(mGraphManager->GetGraph(i));

	if (mActiveExperience != NULL)
	{
		// unload experience (after SM+classifier was unloaded!)
		LogDetailedInfo("Unloading experience '%s'", mActiveExperience->GetName());

		EMIT_EVENT( OnActiveExperienceChanged(NULL) );

		delete mActiveExperience;
		mActiveExperience = NULL;
	}

	// TODO unload other ressources here (like ChannelFile data)
}


void EngineManager::UnloadClassifiers()
{
	// always unload all graphs first (incl. active state machine and classifier)
	const uint32 numGraphs = mGraphManager->GetNumGraphs();
	for (uint32 i=0; i<numGraphs; ++i)
	{
		Graph* graph = mGraphManager->GetGraph(i);
		if (graph->GetType() == Classifier::TYPE_ID)
			UnloadGraph(graph);
	}
}


void EngineManager::UnloadStateMachines()
{
	// always unload all graphs first (incl. active state machine and classifier)
	const uint32 numGraphs = mGraphManager->GetNumGraphs();
	for (uint32 i=0; i<numGraphs; ++i)
	{
		Graph* graph = mGraphManager->GetGraph(i);
		if (graph->GetType() == StateMachine::TYPE_ID)
			UnloadGraph(graph);
	}
}


// get the power line frequency
double EngineManager::GetPowerLineFrequency(User* user) const
{
	switch (mPowerLineFrequencyType)
	{
		case POWERLINEFREQ_50HZ: return 50.0;
		case POWERLINEFREQ_60HZ: return 60.0;
        default:                 break;
	}

	if (user == NULL)
		return 0.0;

	return user->GetAutoPowerLineFrequency();
}


// enable autodetection
void EngineManager::SetAutoDetectionSetting(bool enable)
{
	mAutoDetectionEnabled = enable; 

	// enable auto detection in all drivers
	GetDeviceManager()->SetAutoDetectionEnabled(enable);
}


void EngineManager::SetSessionUser(const User& user)
{
	mSessionUser = user; 
	EMIT_EVENT(OnSessionUserChanged(mSessionUser));
}

//--------------------------------------------------

// initialize the core manager
bool EngineInitializer::Init()
{
	// create the manager object
	if (gEngineManager != NULL)
		return true;

	gEngineManager = new EngineManager();
	return gEngineManager->Init();
}


// shutdown the neuromore Engine manager
void EngineInitializer::Shutdown()
{
	LogInfo("Shutting down biofeedback engine ...");

	// delete the manager
	delete gEngineManager;
	gEngineManager = NULL;
}
