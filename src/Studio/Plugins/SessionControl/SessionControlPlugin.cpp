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

// include required headers
#include "SessionControlPlugin.h"
#include <SessionExporter.h>
#include "../../Windows/ReportWindow.h"
#include "../../Windows/VisualizationSelectWindow.h"
#include <Backend/DataChunksCreateRequest.h>
#include <Backend/DataChunksCreateResponse.h>

using namespace Core;

// constructor
SessionControlPlugin::SessionControlPlugin() : Plugin(GetStaticTypeUuid())
{
	LogDetailedInfo("Constructing session control plugin ...");

	mSessionInfoWidget		 = NULL;
	mClientInfoWidget		 = NULL;
	mPreSessionWidget		 = NULL;
	mWhileSessionWidget		 = NULL;
	mStageControlWidget		 = NULL;
	mCanStartSession		 = false;
	mLostClientError		 = false;
	mLostDeviceError		 = false;
}


// destructor
SessionControlPlugin::~SessionControlPlugin()
{
	LogDetailedInfo("Destructing session control plugin ...");

	CORE_EVENTMANAGER.RemoveEventHandler(this);
}


// init after the parent dock window has been created
bool SessionControlPlugin::Init()
{
	LogDetailedInfo("Initializing session control plugin ...");

	QWidget* mainWidget = new QWidget();
	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainWidget->setLayout( mainLayout );

	const int32 buttonSize = IMAGEBUTTONSIZE_BIG;
	const int32 startButtonSize = IMAGEBUTTONSIZE_BIG;

	// create the dummy widget and a layout
	QWidget* dummyWidget = new QWidget();
	QVBoxLayout* vLayout = new QVBoxLayout();
	QHBoxLayout* hLayout = new QHBoxLayout();
	vLayout->setMargin(0);
	hLayout->setMargin(0);
	vLayout->addLayout(hLayout);
	dummyWidget->setLayout(vLayout);

	// pre-session widget
	mPreSessionWidget = new PreSessionWidget(this, NULL, startButtonSize);
	mPreSessionWidget->ReInit();
	hLayout->addWidget(mPreSessionWidget);
	connect(mPreSessionWidget->GetStartButton(), &QPushButton::clicked, this, &SessionControlPlugin::OnStart);

	// show user button only if user is allowed to select a client
	if (GetUser()->FindRule("STUDIO_SETTING_SelectUser") == NULL)
		mPreSessionWidget->ShowSelectUserButton(false);

	// while-session widget
	mWhileSessionWidget = new WhileSessionWidget(NULL, buttonSize);
	hLayout->addWidget(mWhileSessionWidget);
	connect( mWhileSessionWidget->GetStopButton(), SIGNAL(clicked()), this, SLOT(OnStop()) );
	connect( mWhileSessionWidget->GetPauseButton(), SIGNAL(clicked()), this, SLOT(OnPause()) );
	connect( mWhileSessionWidget->GetContinueButton(), SIGNAL(clicked()), this, SLOT(OnContinue()) );

	// session-info widget
	mSessionInfoWidget = new SessionInfoWidget();
	mSessionInfoWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	vLayout->addWidget(mSessionInfoWidget);

	// add session widget
	dummyWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	mainLayout->addWidget(dummyWidget);

	// add stage widget
	mStageControlWidget = new StageControlWidget(mainWidget);
	mStageControlWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	mainLayout->addWidget(mStageControlWidget);
	
	// add client info widget
#ifndef PRODUCTION_BUILD
	mClientInfoWidget = new ClientInfoWidget(mainWidget);
	mClientInfoWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	mainLayout->addWidget(mClientInfoWidget);
#endif

	// add spacer widget
	QWidget* spacerWidget = new QWidget(mainWidget);
	spacerWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
	mainLayout->addWidget(spacerWidget);

	// connect to server signals so the interface gets updated
	NetworkServer* networkServer = GetNetworkServer();
	connect( networkServer, SIGNAL( ClientAdded  ( NetworkServerClient* ) ), this, SLOT( OnClientChanged( NetworkServerClient* ) ) );
	connect( networkServer, SIGNAL( ClientChanged( NetworkServerClient* ) ), this, SLOT( OnClientChanged( NetworkServerClient* ) ) );
	connect( networkServer, SIGNAL( ClientRemoved( NetworkServerClient* ) ), this, SLOT( OnClientRemoved( NetworkServerClient* ) ) );
	connect( networkServer, SIGNAL( MessageReceived( NetworkServerClient*, NetworkMessage* ) ), this, SLOT( OnMessageReceived ( NetworkServerClient*, NetworkMessage* ) ) );

	// other connections
	connect( GetBackendInterface()->GetUploader(), SIGNAL(UploadFinished()), this, SLOT(UpdateShowReportButton()) );
	connect( GetBackendInterface()->GetParameters(), SIGNAL(SaveFinished(bool)), this, SLOT(OnParametersSaved(bool)) );
	connect( GetBackendInterface()->GetParameters(), SIGNAL(LoadFinished(bool)), this, SLOT(OnParametersLoaded(bool)) );

	// prepare the dock window
	UpdateInterface();
	UpdateWidgets();
	UpdateStartButton();
	mDock->SetContents(mainWidget);
	mDock->update();

	// register with event handler
	CORE_EVENTMANAGER.AddEventHandler(this);

	LogDetailedInfo("Session control plugin successfully initialized");

	return true;
}


void SessionControlPlugin::ReInit()
{
	// reinit widgets
	mPreSessionWidget->ReInit();
	mStageControlWidget->ReInit();
#ifndef PRODUCTION_BUILD
	mClientInfoWidget->ReInit();
#endif

	UpdateInterface();
	UpdateWidgets();
	UpdateStartButton();
}


// update interface information
void SessionControlPlugin::UpdateInterface()
{
	// propagate interface update to widgets
	mWhileSessionWidget->UpdateInterface();
	mStageControlWidget->UpdateInterface();
#ifndef PRODUCTION_BUILD
	mClientInfoWidget->UpdateInterface();
#endif

	// NOTE we probably shouldn't do this here; maybe use a Stop() and Abort() event?

	// automatically stop the session in case the time ran out
	// Note: feature is currently removed with comments
	/*if ( GetSession()->IsRunning() == true )
	{
		const Time totalTime = GetEngine()->GetSession()->GetTotalTime();
		const Time timeRemaining = GetEngine()->GetSession()->GetRemainingTime();
		if (totalTime > 0 && timeRemaining == 0.0)
			OnStop();
	}*/

	bool stateMachineStopped = false;
	StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
	if (stateMachine != NULL && stateMachine->ExitStateReached() == true)
		stateMachineStopped = true;

	// abort session if necessary
	if (GetSession()->IsRunning() == true)
		if (mLostDeviceError || mLostClientError || stateMachineStopped)
			OnStop();
	
	CheckStartRequirements();
	UpdateStartButton();
}


// update the buttons
void SessionControlPlugin::UpdateWidgets()
{
	// no starter and no vis -> show error widget
	if (GetNetworkServer()->GetNumClients() == 0)
	{
		mStageControlWidget->hide();
#ifndef PRODUCTION_BUILD
		mClientInfoWidget->hide();
#endif
	}
	else
	{
		mStageControlWidget->show();
#ifndef PRODUCTION_BUILD
		mClientInfoWidget->show();
#endif
	}
	
	// show different widgets, depending on running state
	const bool isPreparing = GetEngine()->GetSession()->IsPreparing();
	const bool isRunning = GetEngine()->GetSession()->IsRunning();
	if (isRunning || isPreparing)
	{
		mWhileSessionWidget->show();
		mPreSessionWidget->hide();
	}
	else
	{
		mWhileSessionWidget->hide();
		mPreSessionWidget->show();
	}
}


void SessionControlPlugin::OnPreparedSession()
{
	if (Classifier* c = GetEngine()->GetActiveClassifier())
	{
		if (StateMachine* activeStateMachine = GetEngine()->GetActiveStateMachine())
			activeStateMachine->Continue();

		Start();
	}
	else
		Reset();
}


void SessionControlPlugin::OnSessionUserChanged(const User& user)
{
	String name = user.CreateDisplayableName();
	
	// append space due to stylesheet problems (no margins!)
	name = name + " ";

	QPushButton* userButton = mPreSessionWidget->GetSelectUserButton();
	userButton->setText(name.AsChar());
}

void SessionControlPlugin::OnRemoveDevice(Device* device)
{ 
	if (GetSession()->IsRunning() == true) 
		mLostDeviceError = true;
}


void SessionControlPlugin::UpdateStartButton()
{
	if (mPreSessionWidget == NULL)
		return;

	mPreSessionWidget->GetStartButton()->setEnabled(mCanStartSession);
}


// a client was added or modified
void SessionControlPlugin::OnClientChanged( NetworkServerClient* client)
{
	ReInit();
}


// a client was removed
void SessionControlPlugin::OnClientRemoved( NetworkServerClient* client)
{
	// early exit
	if (mLostClientError == true || GetSession()->IsRunning() == false)
		return;

	// stop session if all visualization clients are now disconnected
	NetworkServer* networkServer = GetNetworkServer();
	if (networkServer->GetNumClients(NetworkClient::Visualization) == 0)
		if (GetSession()->IsRunning() == true)
			mLostClientError = true;

	ReInit();
}

// TODO this is disable until we have meaningfull messages from the clients other than the client config message
// react on event messages from studio
void SessionControlPlugin::OnMessageReceived( NetworkServerClient* client, NetworkMessage* message )
{
	CORE_ASSERT( client != NULL );
	//if (client == mVisualizationClient || client == mStarterClient)
	//{

	//	// parse json event
	//	Json jsonEvent;
	//	jsonEvent.Parse( message->GetString() );
	//	Json::Item command;

	//	// visualization autoswitched to another stage
	//	command = jsonEvent.FindItem( "doSwitchStage" );
	//	if (command != NULL)
	//	{
	//		int stageIndex = command->AsInt();

	//		mStageControlWidget->SwitchStage( stageIndex );
	//	}

	//	//// add more events like this:
	//	//command = jsonEvent.FindItem( "doSomething" );
	//	//if (command != NULL)
	//	//{
	//	//	// get command argument here
	//	//}

	//}

	// message must be deleted after is was processed
	delete message;
}


void SessionControlPlugin::CheckStartRequirements()
{
	Experience* experience = GetEngine()->GetActiveExperience();
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();

	mCanStartSession = true;

	// classifier is always required
	const bool haveClassifier = (classifier != NULL);

	// permissions
	const bool noClassifierPermissions = (classifier != NULL && classifier->GetCreud().Execute() == false);
	const bool noStatemachinePermissions = (stateMachine != NULL && stateMachine->GetCreud().Execute() == false);
	const bool noExperiencePermissions = (experience != NULL && experience->GetCreud().Execute() == false);
	const bool noPermissions = noClassifierPermissions || noStatemachinePermissions  || noExperiencePermissions;
	
	// a device is in test mode or has low battery
	const bool deviceTestRunning = GetDeviceManager()->IsDeviceTestRunning();
	const bool deviceBatteryLow = (GetDeviceManager()->IsDevicePowerOk() == false);
	const bool deviceMissing = (classifier != NULL && classifier->HasError (DeviceInputNode::ERROR_DEVICE_NOT_FOUND) == true);

	// classifier or statemachine has error
	const bool designError = (classifier != NULL && classifier->HasError() == true) || (stateMachine != NULL && stateMachine->HasError() == true);
	
	if (haveClassifier == false || noPermissions) /// || deviceTestRunning || deviceBatteryLow || deviceMissing)
		mCanStartSession = false;

	// now show the special notifications directly
	const char* noClassifierInfo = "No design loaded";
	if (haveClassifier == false)
		mSessionInfoWidget->ShowInfo( SessionInfoWidget::TYPE_INFO, noClassifierInfo, "Please load design before starting a session.");
	else
		mSessionInfoWidget->RemoveInfo( noClassifierInfo );

	// permissions
	const char* noPermissionInfo = "Insufficient permissions";
	if (noPermissions == true)
		mSessionInfoWidget->ShowInfo( SessionInfoWidget::TYPE_ERROR, noPermissionInfo, "You don't have permission to execute the loaded design.");
	else
		mSessionInfoWidget->RemoveInfo( noPermissionInfo );

	// device test
	const char* testModeInfo = "Device is in test mode";
	if (deviceTestRunning == true)
		mSessionInfoWidget->ShowInfo( SessionInfoWidget::TYPE_INFO, testModeInfo, "Please stop the device test first.");
	else
		mSessionInfoWidget->RemoveInfo( testModeInfo );

	// device not connected
	const char* noDevicePower = "Device battery is almost empty";
	if (deviceBatteryLow == true)
		mSessionInfoWidget->ShowInfo( SessionInfoWidget::TYPE_WARNING, noDevicePower, "The device battery is in a critical state.");
	else
		mSessionInfoWidget->RemoveInfo( noDevicePower );

	// device not connected
	const char* noDeviceInfo = "Device not connected";
	if (deviceMissing == true)
		mSessionInfoWidget->ShowInfo( SessionInfoWidget::TYPE_ERROR, noDeviceInfo, "The device used by the design is not connected");
	else
		mSessionInfoWidget->RemoveInfo( noDeviceInfo );

	
	// design errors : enable only for the professional users that edit graphs
	if (GetUser()->FindRule("STUDIO_SETTING_EasyWorkflow") == NULL)
	{	
		const char* designErrorInfo = "A graph contains errors";
		if (designError == true)
		{
			mCanStartSession = false;
			if (mSessionInfoWidget->HasInfo(designErrorInfo) == false)
			{
				// TODO: errors for statemachines
				Array<String> classifierErrors;
				String classifierString;

				if (classifier != NULL)
				{
					classifier->CreateErrorList(classifierErrors);
					String::Join(classifierErrors, "<br>", classifierString);
				}

				Array<String> stateMachineErrors;
				String stateMachineString;

				if (stateMachine != NULL)
				{
					stateMachine->CreateErrorList(stateMachineErrors);
					String::Join(stateMachineErrors, "<br>", stateMachineString);
				}

				
				String designErrorDescription;
				if (classifierErrors.Size() > 0 && stateMachineErrors.Size() > 0)
					designErrorDescription.Format("<p style=\"color:red\"><b>Classifier has %i Errors:</b><br>%s<b>Statemachine has %i Errors:</b><br>%s</p>", classifierErrors.Size(), classifierString.AsChar(), stateMachineErrors.Size(), stateMachineString.AsChar());
				else if (classifierErrors.Size() > 0)
					designErrorDescription.Format("<p style=\"color:red\"><b>Classifier has %i Errors:</b><br>%s</p>", classifierErrors.Size(), classifierString.AsChar());
				else if (stateMachineErrors.Size() > 0)
					designErrorDescription.Format("<p style=\"color:red\"><b>Statemachine has %i Errors:</b><br>%s</p>", stateMachineErrors.Size(), stateMachineString.AsChar());


				mSessionInfoWidget->ShowInfo( SessionInfoWidget::TYPE_ERROR, designErrorInfo, designErrorDescription.AsChar());
				
			}
		}
		else
			mSessionInfoWidget->RemoveInfo( designErrorInfo );
	}
}

// start session
void SessionControlPlugin::OnStart()
{
	Classifier* activeClassifier = GetEngine()->GetActiveClassifier();
	StateMachine* activeStateMachine = GetEngine()->GetActiveStateMachine();
	Experience* activeExperience = GetEngine()->GetActiveExperience();

	// gather all errors an compose them into a string
/*	if (activeClassifier != NULL && activeClassifier->HasError() == true)
	{
		Array<String> errorMessages;
		activeClassifier->CreateErrorList(errorMessages);

		// show error message, if any
		if (errorMessages.Size() > 0)
		{
			String allErrors;
			String::Join(errorMessages, "<br>", allErrors);
			
			String message;
			message.Format("Cannot start session because the design has errors:<br><p style=\"color:red\"><b>%s </b></p>", allErrors.AsChar());
			QMessageBox::critical(GetMainWindow(),"Design has errors", message.AsChar());
			return;
		}
	}*/

	if (mCanStartSession == false)
		return;

	mLostClientError = false;
	mLostDeviceError = false;

	// BEGIN

	// switch to first stage
	EMIT_EVENT( OnSwitchStage(0) );

	// TODO do checks on statemachine / experience?
	mPreSessionWidget->setEnabled(false);

	// make sure no session is running
	CORE_ASSERT( GetSession()->IsRunning() == false );

	GetEngine()->Reset();     // reset engine (also resets session)
	GetEngine()->SoftPause(); // but keep it paused

	// load parameters
	GetBackendInterface()->GetParameters()->Load(true, *GetSessionUser(), activeExperience, activeClassifier);
}


void SessionControlPlugin::OnParametersLoaded(bool success)
{
	// error occured while loading parameters
	if (success == false)
	{
		mSessionInfoWidget->ShowInfo( SessionInfoWidget::TYPE_ERROR, "Session start failed", "There was an error retrieving the design parameters.", 10.0);
		
		// skip directly to fail code path
		Reset();
		return;
	}

   Classifier* c = GetEngine()->GetActiveClassifier();

   // must have a classifier
   if (!c) {
      Reset();
      return;
   }

	GetSession()->Prepare();     // prepare the session
	UpdateWidgets();             // update ui
	GetEngine()->SoftContinue(); // continue engine

	// but keep the statemachine paused until session really starts
	if (StateMachine* sm = GetEngine()->GetActiveStateMachine())
		sm->Pause();
}


void SessionControlPlugin::Start()
{
	GetSession()->Reset();
	GetSession()->Start();

	// update the interface
	UpdateWidgets();

	mPreSessionWidget->setEnabled(true);
	mPreSessionWidget->GetShowReportButton()->setVisible(false);
}


void SessionControlPlugin::Reset()
{
	GetSession()->Stop();
	GetSession()->Reset();

	// update the interface
	UpdateWidgets();

	mPreSessionWidget->setEnabled(true);
	
	mLostClientError = false;
	mLostDeviceError= false;
}


// stop session
void SessionControlPlugin::OnStop()
{
	mWhileSessionWidget->setEnabled(false);

	// make sure the session is running
	CORE_ASSERT( GetSession()->IsRunning() == true );

	// pause the whole system until we have saved the physiological data
	GetEngine()->SetIsRunning(false);

	// stop the current session
	GetSession()->Stop();

	if (mLostDeviceError == true)
	{
		mSessionInfoWidget->ShowInfo( SessionInfoWidget::TYPE_ERROR, "Session aborted", "The session was stopped automatically because a device disappeared.", 10.0 );
		LogError("The session was stopped automatically because a device disappeared.");
	}
	else if (mLostClientError == true)
	{
		mSessionInfoWidget->ShowInfo( SessionInfoWidget::TYPE_ERROR, "Session aborted", "The session was stopped automatically because the visualization stopped.", 10.0 );
		LogError("The session was stopped automatically because the visualization stopped.");
	}
	else
	{
		mSessionInfoWidget->ShowInfo( SessionInfoWidget::TYPE_INFO, "Session ended", "The session has ended.", 10.0 );
	}

	// update the interface
	UpdateWidgets();

	Classifier* activeClassifier = GetEngine()->GetActiveClassifier();
	StateMachine* activeStateMachine = GetEngine()->GetActiveStateMachine();
	Experience* activeExperience = GetEngine()->GetActiveExperience();

	////////////////////////////////////////
	// start backend parameter upload
	////////////////////////////////////////
	GetBackendInterface()->GetParameters()->Save(*GetSessionUser(), *GetSession(), activeExperience, activeClassifier);

	////////////////////////////////////////
	// create data chunk
	////////////////////////////////////////

	User* user = GetUser();
	String token				= user->GetToken();
	String userId				= GetSessionUser()->GetId();
	String supervisorId			= user->GetId();
	String paymentRecipientId	= supervisorId;

	// if there is a single company assigned, then the company is expected to pay
	if (user->GetNumParentCompanyIds() == 1)
		paymentRecipientId = user->GetParentCompanyId(0);

	const char* classifierUUID = activeClassifier->GetUuid();
	const int classifierRevision = activeClassifier->GetRevision();
	const char* statemachineUUID = (activeStateMachine != NULL ? activeStateMachine->GetUuid() : "");
	const int statemachineRevision = (activeStateMachine != NULL ? activeStateMachine->GetRevision() : 0); 
	const char* experienceUUID = (activeExperience != NULL ? activeExperience->GetUuid() : "");
	const int experienceRevision = (activeExperience != NULL ? activeExperience->GetRevision() : 0);

	const Time& startTime = GetSession()->GetStartTime();
	const Time& stopTime  = GetSession()->GetStopTime();
	const bool isValid = true;

	// disable active classifier data updating.
	activeClassifier->Stop();

	// 1. construct /datachunks/create request
	DataChunksCreateRequest request( token.AsChar(), userId.AsChar(), supervisorId.AsChar(), paymentRecipientId.AsChar(), classifierUUID, classifierRevision, statemachineUUID, statemachineRevision, experienceUUID, experienceRevision, startTime, stopTime, isValid );

	// 2. process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request );
	connect(reply, &QNetworkReply::finished, this, [reply, this]()
	{
		QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );

		// 3. construct and parse response
		DataChunksCreateResponse response(networkReply);

		// 4. handle response
		if (response.HasError() == true)
		{
			// clear the data chunk id and hide the show report button
			mReportSessionId.Clear();
			UpdateShowReportButton();
		}
		else
		{
			// remember the session id for the report generation
			mReportSessionId = response.GetDataChunkId();

			if (GetBackendInterface()->GetUploader()->IsEnabled() == true)
			{
				// get the physiological data folder
				Core::String path = GetQtBaseManager()->GetPhysiologicalDataFolder();

				// save the physiological data
				SessionExporter::Save( path.AsChar(), GetSessionUser()->GetId(), response.GetDataChunkId() );

#ifndef NEUROMORE_BRANDING_ANT
				if (GetUser()->FindRule("STUDIO_SETTING_EasyWorkflow") != NULL)
				{
					// show report window
					ReportWindow reportWindow( mReportSessionId, GetMainWindow() );
					reportWindow.exec();
				}
#endif
			}
		}
	});
}


// pause session: soft-pause the engine
void SessionControlPlugin::OnPause()
{
	// pause engine
	GetEngine()->SoftPause();

	// pause the current session?
	GetSession()->Pause();

	// update the interface
	UpdateWidgets();
}


// pause session: soft-pause the engine
void SessionControlPlugin::OnContinue()
{
	// pause engine
	GetEngine()->SoftContinue();

	// continue the session
	GetSession()->Continue();

	// update the interface
	UpdateWidgets();
}


void SessionControlPlugin::OnParametersSaved(bool success)
{
	// something didn't work
	if (success == false)
	{
		//QMessageBox::critical(GetMainWindow(), "Cannot save parameters", "Something went wrong during parameter upload. The parameters may be lost.");
		mSessionInfoWidget->ShowInfo( SessionInfoWidget::TYPE_ERROR, "Parameter upload failed", "Something went wrong during parameter upload.", 10.0 );
		return;
	}

	UpdateWidgets();

	// unpause the whole system as we have saved the physiological data from the last session
	GetEngine()->SetIsRunning(true);

	// enable widget again
	mWhileSessionWidget->setEnabled(true);
}


void SessionControlPlugin::UpdateShowReportButton()
{
	QPushButton* showReportButton = mPreSessionWidget->GetShowReportButton();
	
	const bool isVisible = (mReportSessionId.IsEmpty() == false);
	showReportButton->setVisible(isVisible);
}


void SessionControlPlugin::ShowReport()
{
	// this is a duplicate from ReportWindow

	if (mReportSessionId.IsEmpty() == true)
		return;

	String finalUrl = GetBackendInterface()->GetNetworkAccessManager()->GetActiveServerPreset().mWebserverUrl;
	finalUrl += "/#/report?dataChunkId=";
	finalUrl += mReportSessionId;
	finalUrl += "&token=";
	finalUrl += GetUser()->GetToken();
	finalUrl += "&clamping=false";

	// start-up the online statistics in the default browser
	QDesktopServices::openUrl(QUrl(finalUrl.AsChar()));
}

// called after switching layout
void SessionControlPlugin::OnAfterLoadLayout()
{
	if (GetUser()->FindRule("STUDIO_SETTING_EasyWorkflow") != NULL)
	{
		// open visualization select window if one is available and none is running
		VisualizationManager* vizManager = GetManager()->GetVisualizationManager();
		if (vizManager->GetNumVisualizations() > 0 && !vizManager->IsRunning())
		{
			VisualizationSelectWindow selectVizWindow(GetMainWindow());
			selectVizWindow.exec();
		}
	}
}
