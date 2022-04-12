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
#include "EngineStatusPlugin.h"
#include <Core/Config.h>
#include <Core/LogManager.h>
#include <Networking/NetworkClient.h>
#include <EngineManager.h>
#include "../../MainWindow.h"
#include "../../AppManager.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QIcon>


using namespace Core;

// constructor
EngineStatusPlugin::EngineStatusPlugin() : Plugin(GetStaticTypeUuid())
{
	LogDetailedInfo("Constructing engine status plugin ...");

	// Init stuff
	mLogOutput = NULL;
	mClientInfoWidget = NULL;

	// reset last values
	mLastMemoryUsed = 0;
	mLastNumTotalAllocs = 0;
	mLastNumAllocs = 0;
	mLastNumReallocs = 0;
	mLastNumFrees = 0;
	mLastNumOscPacketsReceived = 0;
	mLastNumOscPacketsSent = 0;

	// Log Callback
	mLogCallback = new EngineStatusLogCallback(this);
	CORE_LOGMANAGER.AddLogCallback(mLogCallback);
}


// destructor
EngineStatusPlugin::~EngineStatusPlugin()
{
	LogDetailedInfo("Destructing engine status plugin ...");

	CORE_LOGMANAGER.RemoveLogCallback( mLogCallback, false );
	delete mLogCallback;
}


// spacer helper
QWidget* EngineStatusPlugin::GetHSpacer()
{
	QWidget* spacer = new QWidget();
	spacer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	return spacer;
}

// init after the parent dock window has been created
bool EngineStatusPlugin::Init()
{
	LogDetailedInfo("Initializing engine status plugin ...");

	QWidget* mainWidget = new QWidget();
	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainWidget->setLayout(mainLayout);

	uint32 row = 0;

	//////////////////////////////////////////////////////////////////////////
	// 0) main status
	mEngineStatusLabel = new QLabel("N/A");
	QGridLayout* engineLayout = new QGridLayout(mDock);
	engineLayout->setMargin(0);
	engineLayout->addWidget(new QLabel("Engine State"), row, 0);
	engineLayout->addWidget(mEngineStatusLabel, row, 1);
	row++;

	// TODO more engine status values

	mainLayout->addWidget( new QLabel("Status") );
	mainLayout->addLayout( engineLayout );

	//////////////////////////////////////////////////////////////////////////
	// 1) performance

	mNeuroPerformanceLabel = new QLabel("N/A");
	mInterfacePerformanceLabel = new QLabel("N/A");
	mClassifierPerformanceLabel = new QLabel("N/A");
	mDeviceManagerPerformanceLabel = new QLabel("N/A");
	mOscRouterPerformanceLabel = new QLabel("N/A");
	mMemoryUsedLabel = new QLabel("N/A");
	mMemoryCurrentAllocsLabel = new QLabel("N/A");
	mMemoryAllocsLabel = new QLabel("N/A");
	mMemoryReallocsLabel = new QLabel("N/A");
	mMemoryFreesLabel = new QLabel("N/A");
	QGridLayout* perfLayout = new QGridLayout(mDock);
	perfLayout->setMargin(0);
	row = 0;

	perfLayout->addWidget(new QLabel("OpenGL Widgets"), row, 0);
	perfLayout->addWidget(mInterfacePerformanceLabel, row, 1);
	row++;

	perfLayout->addWidget(new QLabel("Engine"), row, 0);
	perfLayout->addWidget(mNeuroPerformanceLabel, row, 1);
	row++;

	perfLayout->addWidget(new QLabel("Device Manager"), row, 0);
	perfLayout->addWidget(mDeviceManagerPerformanceLabel, row, 1);
	row++;

	perfLayout->addWidget(new QLabel("Osc Message Router"), row, 0);
	perfLayout->addWidget(mOscRouterPerformanceLabel, row, 1);
	row++;

	perfLayout->addWidget(new QLabel("Classifier"), row, 0);
	perfLayout->addWidget(mClassifierPerformanceLabel, row, 1);
	row++;
	perfLayout->addWidget(new QLabel("Memory Used"), row, 0);
	perfLayout->addWidget(mMemoryUsedLabel, row, 1);
	row++;
	perfLayout->addWidget(new QLabel("Current Allocations"), row, 0);
	perfLayout->addWidget(mMemoryCurrentAllocsLabel, row, 1);
	row++;
	perfLayout->addWidget(new QLabel("Total Allocations"), row, 0);
	perfLayout->addWidget(mMemoryAllocsLabel, row, 1);
	row++;
	perfLayout->addWidget(new QLabel("Total Reallocations"), row, 0);
	perfLayout->addWidget(mMemoryReallocsLabel, row, 1);
	row++;
	perfLayout->addWidget(new QLabel("Total Frees"), row, 0);
	perfLayout->addWidget(mMemoryFreesLabel, row, 1);

	mainLayout->addWidget( new QLabel("") );
	mainLayout->addWidget( new QLabel("Performance") );
	mainLayout->addLayout( perfLayout );


	//////////////////////////////////////////////////////////////////////////
	// 2) network server information 
	mServerStatusLabel = new QLabel("-");
	mNumMessagesLabel = new QLabel("-");
	mNumBytesLabel = new QLabel("-");
	mNumClientsLabel = new QLabel("-");
	mClientInfoWidget = new ClientInfoWidget();
	
	QGridLayout* serverLayout = new QGridLayout(mDock);
	serverLayout->setMargin(0);
	row = 0;
	
	serverLayout->addWidget(new QLabel("TCP Server status:"), row, 0);
	serverLayout->addWidget(mServerStatusLabel, row, 1);
	row++;
	
	serverLayout->addWidget(new QLabel("Messages RX/TX:"), row, 0);
	serverLayout->addWidget(mNumMessagesLabel, row, 1);
	row++;
	
	serverLayout->addWidget(new QLabel("kBytes RX/TX:"), row, 0);
	serverLayout->addWidget(mNumBytesLabel, row, 1);
	row++;
	
	serverLayout->addWidget(new QLabel("Active Clients:"), row, 0);
	serverLayout->addWidget(mNumClientsLabel, row, 1);
	row++;

	serverLayout->addWidget(mClientInfoWidget, row, 0, 1, 2);
	row++;
	
	mainLayout->addWidget( new QLabel("") );
	mainLayout->addWidget( new QLabel("Networking") );
	mainLayout->addLayout( serverLayout );

	//////////////////////////////////////////////////////////////////////////
	// 2) osc listener information 
	mOscListenerStatusLabel = new QLabel("-");
	mNumOscReceiversLabel = new QLabel("-");
	mNumOscPacketsReceivedLabel = new QLabel("-");
	mNumOscPacketPoolRXStatusLabel = new QLabel("-");
	mNumOscMessagesRouterStatusLabel = new QLabel("-");
	mNumOscPacketsSentLabel = new QLabel("-");
	mNumOscPacketPoolTXStatusLabel = new QLabel("-");

	QGridLayout* oscListenerLayout = new QGridLayout(mDock);
	oscListenerLayout->setMargin(0);
	row = 0;
	
	oscListenerLayout->addWidget(new QLabel("Status:"), row, 0);
	oscListenerLayout->addWidget(mOscListenerStatusLabel, row, 1);
	row++;

	oscListenerLayout->addWidget(new QLabel("Registered receivers:"), row, 0);
	oscListenerLayout->addWidget(mNumOscReceiversLabel, row, 1);
	row++;
	
	oscListenerLayout->addWidget(new QLabel("Packets received:"), row, 0);
	oscListenerLayout->addWidget(mNumOscPacketsReceivedLabel, row, 1);
	row++;
	
	oscListenerLayout->addWidget(new QLabel("RX Packet Pool size/used/free:"), row, 0);
	oscListenerLayout->addWidget(mNumOscPacketPoolRXStatusLabel, row, 1);
	row++;
	
	oscListenerLayout->addWidget(new QLabel("Messages routed/failed"), row, 0);
	oscListenerLayout->addWidget(mNumOscMessagesRouterStatusLabel, row, 1);
	row++;

	oscListenerLayout->addWidget(new QLabel("Packets sent:"), row, 0);
	oscListenerLayout->addWidget(mNumOscPacketsSentLabel, row, 1);
	row++;

	oscListenerLayout->addWidget(new QLabel("TX Packet Pool size/used/free:"), row, 0);
	oscListenerLayout->addWidget(mNumOscPacketPoolTXStatusLabel, row, 1);
	row++;
	
	mainLayout->addWidget( new QLabel("") );
	mainLayout->addWidget( new QLabel("OSC Server") );
	mainLayout->addLayout( oscListenerLayout );

	//////////////////////////////////////////////////////////////////////////
	// 4) debug log text field
	mLogOutput = new QTextEdit();
	mainLayout->addWidget( new QLabel("") );
	mainLayout->addWidget( new QLabel("Debug Log") );
	mainLayout->addWidget( mLogOutput );

	// reset and update labels
	UpdateInterface();
	
	mDock->SetContents(mainWidget);
	mDock->update();

	// interface update timer
	connect(&mUpdateTimer, SIGNAL(timeout()), this, SLOT(OnTimerTimeout()));
	mUpdateTimer.start(2000);


	// connect to network server
	NetworkServer* server = GetNetworkServer();
	connect( server, SIGNAL( ClientAdded  ( NetworkServerClient* ) ), this, SLOT( OnClientListChanged( ) ) );
	connect( server, SIGNAL( ClientChanged( NetworkServerClient* ) ), this, SLOT( OnClientListChanged( ) ) );
	connect( server, SIGNAL( ClientRemoved( NetworkServerClient* ) ), this, SLOT( OnClientListChanged( ) ) );

	LogDetailedInfo("Engine status plugin successfully initialized");

	return true;
}

	
// update interface labels 
void EngineStatusPlugin::OnTimerTimeout()
{
	UpdateInterface();
}


// reinit client list on changes
void EngineStatusPlugin::OnClientListChanged()
{
	mClientInfoWidget->ReInit();
}


// update interface information
void EngineStatusPlugin::UpdateInterface()
{
	// measure time delta
	const double timeElapsed = mUpdateDeltaTimer.GetTimeDelta().InSeconds();

	// ignore t=0, otherwise we would divide by zero later
	if (timeElapsed == 0)
		return;

	// 0) Engine Status

	const char* engineStatus = "-";
	if (GetEngine()->IsRunning())
	{
		//if (GetEngine()->IsPaused())
		//	engineStatus = "Paused";
		//else
		//{
		//	if (GetEngine()->IsSoftPaused())
		//		engineStatus = "Paused (Soft)";
		//	else
		//		engineStatus = "Running";
		//}
	}
	else
	{
		engineStatus = "Stopped";
	}

	mEngineStatusLabel->setText(engineStatus);

	// 1) studio and engine performance

#ifdef CORE_DEBUG

	// get values from memory manager
	const uint32 numAllocs = 0;//CORE_MEMORYINFORMATION->mNumAllocs;			// current number of allocations
	const uint32 totalNumAllocs = 0;//CORE_MEMORYINFORMATION->mTotalAllocs;		// total number of allocations since start
	const uint32 totalNumReallocs = 0;//CORE_MEMORYINFORMATION->mTotalReallocs;	// total number of reallocs since start
	const uint32 totalNumFrees = 0;//CORE_MEMORYINFORMATION->mTotalFrees;		// total number of frees since start

	// total memory used
	uint32 totalNumBytes = 0;
	for (uint32 i=0; i<numAllocs; ++i) 
		totalNumBytes += 0;//CORE_MEMORYINFORMATION->mAllocs[i].mNumBytes;
	
	// differences and rates (per second)

	// rate of memory change in bytes / second
	const int32 numBytesDiff = totalNumBytes - (int32)mLastMemoryUsed;
	mLastMemoryUsed = totalNumBytes;
	const double memoryRate = numBytesDiff / timeElapsed;
	
	// current alloc rate
	const int32 numNewAllocs = numAllocs - mLastNumAllocs;
	mLastNumAllocs = numAllocs;
	const double allocRate = numNewAllocs / timeElapsed;

	// total alloc rate
	const uint32 numNewTotalAllocs = totalNumAllocs - mLastNumTotalAllocs;
	mLastNumTotalAllocs = totalNumAllocs;
	const double totalAllocRate = numNewTotalAllocs / timeElapsed;

	// total realloc rate
	const uint32 numNewReallocs = totalNumReallocs - mLastNumReallocs;
	mLastNumReallocs = totalNumReallocs;
	const double reallocRate = numNewReallocs / timeElapsed;

	// total free rate
	const uint32 numNewFrees = totalNumFrees - mLastNumFrees;
	mLastNumFrees = totalNumFrees;
	const double freeRate = numNewFrees / timeElapsed;

	// update ui elements
	mTempString.Format("%i \t(%.1f/s)", numAllocs, allocRate);
	mMemoryCurrentAllocsLabel->setText(mTempString.AsChar());

	mTempString.Format("%.1fMB \t(%.2fKB/s)", (double)totalNumBytes / (1024.0*1024.0), memoryRate / (1024.0));
	mMemoryUsedLabel->setText(mTempString.AsChar());

	// total allocs/reallocs/frees
	mTempString.Format("%i \t(%.1f/s)", totalNumAllocs, totalAllocRate);
	mMemoryAllocsLabel->setText(mTempString.AsChar());
	mTempString.Format("%i \t(%.1f/s)", totalNumReallocs, reallocRate);
	mMemoryReallocsLabel->setText(mTempString.AsChar());
	mTempString.Format("%i \t(%.1f/s)", totalNumFrees, freeRate);
	mMemoryFreesLabel->setText(mTempString.AsChar());
#endif

	// interface performance timing
	mTempString.Format("%.0f (%.0f) FPS - %.2f ms", GetMainWindow()->GetOpenGLFpsCounter().GetFps(), GetMainWindow()->GetOpenGLFpsCounter().GetTheoreticalFps(), GetMainWindow()->GetOpenGLFpsCounter().GetAveragedTimeDelta()*1000.0);
	mInterfacePerformanceLabel->setText( mTempString.AsChar() );
	
	// neuro performance timing
	mNeuroPerformanceLabel->setText( GetEngine()->GetFpsCounter().GetText() );

	// classifier performance timing
	Classifier* activeClassifier = GetEngine()->GetActiveClassifier();
	if (activeClassifier != NULL)
		mClassifierPerformanceLabel->setText( activeClassifier->GetFpsCounter().GetText() );
	else
		mClassifierPerformanceLabel->setText( "N/A" );

	mDeviceManagerPerformanceLabel->setText( GetDeviceManager()->GetFpsCounter().GetText() );
	mOscRouterPerformanceLabel->setText( GetOscMessageRouter()->GetFpsCounter().GetText() );

	// 2) network server
	NetworkServer* server = GetNetworkServer();
	
	// server status
	mTempString.Format("%s", (server->IsOnline() ? "Listening" : "Offline"));
	mServerStatusLabel->setText(mTempString.AsChar());

	// rx/tx stats
	mTempString.Format("%i / %i", server->GetNumMessagesReceived(), server->GetNumMessagesTransmitted());
	mNumMessagesLabel->setText(mTempString.AsChar());
	mTempString.Format("%.2f / %.1f", (double)server->GetNumBytesReceived() / 1024.0, (double)server->GetNumBytesTransmitted() / 1024.0 );
	mNumBytesLabel->setText(mTempString.AsChar());

	// num clients
	mTempString.Format("%i", server->GetNumClients());
	mNumClientsLabel->setText(mTempString.AsChar());

	// update client list
	mClientInfoWidget->UpdateInterface();


	// 3) osc listener
	OscServer* oscServer = GetOscServer();

	// status
	if (oscServer == NULL)
	{
		mOscListenerStatusLabel->setText("Offline");
	}
	else
	{
		mTempString.Format("Listening on UDP port %i", oscServer->GetListenPort());
		mOscListenerStatusLabel->setText(mTempString.AsChar());
		
		// receive

		const uint32 numPacketsReceived = oscServer->GetNumPacketsReceived();
		const uint32 numNewPacketsReceived = numPacketsReceived - mLastNumOscPacketsReceived;
		mLastNumOscPacketsReceived = numPacketsReceived;
		const double inputRate = numNewPacketsReceived / timeElapsed;

		mTempString.Format("%i (%.2f/s)", numPacketsReceived, inputRate);
		mNumOscPacketsReceivedLabel->setText(mTempString.AsChar());

		OscMessageRouter* router = GetOscMessageRouter();

		mTempString.Format("%i", router->GetNumRegisteredReceivers());
		mNumOscReceiversLabel->setText(mTempString.AsChar());
		
		const uint32 rxPacketsFree = oscServer->GetNumPooledPacketsFree();
		const uint32 rxPacketsUsed = oscServer->GetNumPooledPacketsUsed();
		mTempString.Format("%i / %i / %i", rxPacketsFree + rxPacketsUsed, rxPacketsUsed, rxPacketsFree);
		mNumOscPacketPoolRXStatusLabel->setText(mTempString.AsChar());

		mTempString.Format("%i / %i", router->GetNumMessagesReceived(), router->GetNumMessageUnroutable());
		mNumOscMessagesRouterStatusLabel->setText(mTempString.AsChar());

		// transmit
		const uint32 numPacketsSent = oscServer->GetNumPacketsTransmitted();
		const uint32 numNewPacketsSent = numPacketsSent - mLastNumOscPacketsSent;
		mLastNumOscPacketsSent = numPacketsSent;
		const double outputRate = numNewPacketsSent / timeElapsed;

		mTempString.Format("%i (%.2f/s)", numPacketsSent, outputRate);
		mNumOscPacketsSentLabel->setText(mTempString.AsChar());

		const uint32 txPacketsFree = router->GetNumPooledPacketsFree();
		const uint32 txPacketsUsed = router->GetNumPooledPacketsUsed();
		mTempString.Format("%i / %i / %i", txPacketsFree + txPacketsUsed, txPacketsUsed, txPacketsFree);
		mNumOscPacketPoolTXStatusLabel->setText(mTempString.AsChar());
	}
}
