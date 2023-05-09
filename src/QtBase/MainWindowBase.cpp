/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

 // include precompiled header
#include <QtBase/Precompiled.h>

// include the required headers
#include "MainWindowBase.h"
#include "QtBaseManager.h"
#include <EngineManager.h>
#include <QGuiApplication>
#include <QScreen>
#include <QPushButton>
#include <QMessageBox>
#include "PluginSystem/PluginManager.h"
/*#include "AES.h"
#include <QFile>
#include <QIODevice>*/


using namespace Core;

// constructor
MainWindowBase::MainWindowBase(QWidget* parent, Qt::WindowFlags flags) : QMainWindow(parent, flags)
{
/*	AES aes;

	QString keyString = "1234567891234567";
	QByteArray keyData = keyString.toLatin1();

	QString originalString = "Steve du Gott bitte decrypte mich!!!";
	QByteArray originalData = originalString.toLatin1();

	QByteArray encrypted = aes.Encrypt( originalData, keyData );
	QString encryptedBase64String = encrypted.toBase64();
	LogInfo("Encrypted Base64 String: %s", encryptedBase64String.toLatin1().data());

	QByteArray decrypted = aes.Decrypt( encrypted, keyData );
	QString decryptedString = decrypted;
	LogInfo("Decrypted File: %s", decryptedString.toLatin1().data());

	// decrypt base64 string
	QByteArray base64String ="RJoI3+16/E80Pl1QYVNYQbxvZ0rwsH6Cr+D6R3jl/WU=";
	QByteArray unbase64 = QByteArray::fromBase64( base64String );
	QByteArray decryptedBase64 = aes.Decrypt( unbase64, keyData );
	QString decryptedBase64String = decryptedBase64;
	LogInfo("Decrypted Base64: %s", decryptedBase64String.toLatin1().data());

	QFile file("D:\\AES128_CBC_TestFile.aes");
	file.open(QIODevice::WriteOnly);
	file.write(encrypted);
	file.close();
	*/
	mEngineTimer			= NULL;
	mRealtimeUITimer		= NULL;
	mInterfaceTimer			= NULL;
	mShowFPS				= false;
	mShowPerformanceInfo	= false;

	mEngineUpdateRate		= 100.0;
	mRealtimeUIUpdateRate	= 60.0;
	mInterfaceUpdateRate	= 10.0;
}


// destructor
MainWindowBase::~MainWindowBase()
{
	mEngineTimer->stop();
	mRealtimeUITimer->stop();
	mInterfaceTimer->stop();
}


void MainWindowBase::Init()
{
	// create engine timer
	LogDetailedInfo("Starting neuromore Engine timer ...");
	mEngineTimer = new QTimer(this);
	connect( mEngineTimer, SIGNAL(timeout()), this, SLOT(OnEngineUpdate()) );
	SetEngineTimerEnabled(false);

	// create the realtime timer
	LogDetailedInfo("Starting real-time interface timer ...");
	mRealtimeUITimer = new QTimer(this);
	connect( mRealtimeUITimer, SIGNAL(timeout()), this, SLOT(OnRealtimeUIUpdate()) );
	SetRealtimeUITimerEnabled(false);

	// create the interface timer
	LogDetailedInfo("Starting interface timer ...");
	mInterfaceTimer = new QTimer(this);
	connect( mInterfaceTimer, SIGNAL(timeout()), this, SLOT(OnUpdateInterface()) );
	SetInterfaceTimerEnabled(false);

	// log the platform name
	LogInfo( "Platform Name: %s", FromQtString(qApp->platformName()).AsChar() );

	// log the available screens
	QList<QScreen*> screens = qApp->screens();
	const int numScreens = screens.count();
	LogInfo( "Screens (%i)", numScreens );
	for (int i=0; i<numScreens; ++i)
	{
		QScreen* screen = screens[i];
		LogInfo( "   Screen #%i: Resolution=(%i, %i), DPI=%.2f", i, screen->size().width(), screen->size().height(), screen->logicalDotsPerInch() );
	}

	// set default font size
	QFont defaultFont = QFont();
    defaultFont.setPointSize( (float)defaultFont.pointSizeF() * 0.9f );
	QFontMetrics defaultFontMetrics(defaultFont);
	mDefaultFontSize = defaultFontMetrics.height();
}


void MainWindowBase::SetEngineTimerEnabled(bool isEnabled)
{
	if (isEnabled == true)
	{
		mEngineTimer->setTimerType( Qt::PreciseTimer );
		mEngineTimer->start(1000 / mEngineUpdateRate);
		mEngineUpdateTimer.GetTimeDelta();
	}
	else
	{
		mEngineTimer->stop();
	}
}


void MainWindowBase::SetRealtimeUITimerEnabled(bool isEnabled)
{
	if (isEnabled == true)
	{
		mRealtimeUITimer->setTimerType( Qt::PreciseTimer );
		mRealtimeUITimer->start(1000 / mRealtimeUIUpdateRate);
		mRealtimeUIUpdateTimer.GetTimeDelta();
	}
	else
	{
		mRealtimeUITimer->stop();
	}
}


void MainWindowBase::SetInterfaceTimerEnabled(bool isEnabled)
{
	if (isEnabled == true)
	{
		mInterfaceTimer->setTimerType( Qt::PreciseTimer );
		mInterfaceTimer->start(1000 / mInterfaceUpdateRate);
	}
	else
	{
		mInterfaceTimer->stop();
	}
}


void MainWindowBase::SetEngineUpdateRate(double updateFPS)
{
	mEngineUpdateRate = updateFPS;

	if (mEngineTimer != NULL)
		mEngineTimer->setInterval(1000.0 / updateFPS);
}


void MainWindowBase::SetRealtimeUIUpdateRate(double updateFPS)
{
	mRealtimeUIUpdateRate = updateFPS;

	if (mRealtimeUITimer != NULL)
		mRealtimeUITimer->setInterval(1000.0 / updateFPS);
}


void MainWindowBase::SetInterfaceUpdateRate(double updateFPS)
{
	mInterfaceUpdateRate = updateFPS;

	if (mInterfaceTimer != NULL)
		mInterfaceTimer->setInterval(1000.0 / updateFPS);
}


// update interfaces
void MainWindowBase::OnUpdateInterface()
{
	// skip the interface update in case the interfaces are disabled
	if (GetQtBaseManager()->IsInterfaceEnabled() == false)
		return;

	if (GetQtBaseManager()->IsInterfacePaused() == true)
	{
#ifdef CORE_DEBUG
		LogWarning("MainWindow::OnRealtimeUpdate(): Real-time update requested while interface is paused.");
#endif
		return;
	}

	// iterate over the plugins and update the interfaces
	const uint32 numPlugins = GetPluginManager()->GetNumActivePlugins();
	for (uint32 i=0; i<numPlugins; ++i)
	{
		// check if the given plugin needs a regular interface update, if yes update interface, if no skip directly
		Plugin* plugin = GetPluginManager()->GetActivePlugin(i);
		if (plugin->NeedRegularUpdateInterface() == true)
			plugin->UpdateInterface();
	}
}


// engine update
void MainWindowBase::OnEngineUpdate()
{
	if (GetQtBaseManager()->IsInterfacePaused() == true)
	{
#ifdef CORE_DEBUG
		LogWarning("MainWindow::OnRealtimeUpdate(): Real-time update requested while interface is paused.");
#endif
		return;
	}

	// get the time delta between the last engine update and the current time
	Time timeDelta = mEngineUpdateTimer.GetTimeDelta();

	// a large timeDelta indicates that either a severe lag has happened, or that the main thread was halted for some time (e.g. PC was put in standby)
	// -> we clamp it to 5 seconds and report an error
	const double maxAllowedLag = 10.0;
	if (timeDelta.operator>(maxAllowedLag))		// FIXME resolve ambiguity of > when comparing time with double
	{

#ifndef PRODUCTION_BUILD
		const int pressedButton = QMessageBox::critical( this, "ERROR", "Real-time capability lost: Application wasn't able to update within 5 seconds. Press OK to stop execution and cancel any running session.", QMessageBox::Ok, QMessageBox::Cancel);
		
		// pause classifier (and stop session) if user clicked OK
		if (pressedButton == QMessageBox::Ok)
		{
			Classifier* classifier = GetEngine()->GetActiveClassifier();
			if (classifier != NULL)
				classifier->Pause();

			Session* session = GetEngine()->GetSession();
			if (session != NULL)
				if (session->IsRunning() == true)
					session->Stop();

			// TODO this needs to trigger the usual session abort mechanism used in SessionControlPlugin -> add error flag or something
		}
		else
		{
			// schedule sync
			GetEngine()->SyncAsync();
		}
#else
		// schedule sync
		GetEngine()->SyncAsync();
#endif

		LogError("MainWindow::OnRealtimeUpdate(): Detected a large jump in the realtime timer (%.2f s).", timeDelta.InSeconds());
	}
	else
	{
		GetEngine()->Update(timeDelta);
	}
}


// real-time user interface update
void MainWindowBase::OnRealtimeUIUpdate()
{
	if (GetQtBaseManager()->IsInterfacePaused() == true)
	{
#ifdef CORE_DEBUG
		LogWarning("MainWindow::OnRealtimeUpdate(): Real-time update requested while interface is paused.");
#endif
		return;
	}

	// get the time delta between the last interface update and the current time
	double timeDelta = mRealtimeUIUpdateTimer.GetTimeDelta().InSeconds();

	// update all realtime plugins in case the interfaces are enabled
	if (GetQtBaseManager()->IsInterfaceEnabled() == true)
		UpdateRealtimePlugins(timeDelta);
}


void MainWindowBase::UpdateRealtimePlugins(double timeDelta)
{
	if (GetQtBaseManager()->IsInterfacePaused() == true)
		return;

	PluginManager* pluginManager = GetPluginManager();

	// setup async performance timing
	mOpenGLWidgetFpsCounter.UnPause();
	mOpenGLWidgetFpsCounter.StopTiming();
	mOpenGLWidgetFpsCounter.BeginTiming();
	mOpenGLWidgetFpsCounter.Pause();

	// get the number of active plugins and iterate through them
	const uint32 numPlugins = pluginManager->GetNumActivePlugins();
	for (uint32 i=0; i<numPlugins; ++i)
	{
		// get the plugin and call the realtime update function
		Plugin* plugin = pluginManager->GetActivePlugin(i);
		plugin->SetShowPerformanceInfo( mShowPerformanceInfo );
		plugin->RealtimeUpdate();
	}
}
