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

#ifndef __NEUROMORE_APPMANAGER_H
#define __NEUROMORE_APPMANAGER_H

// include required headers
#include "Config.h"
#include <Core/StandardHeaders.h>
#include <Core/Timer.h>
#include "Rendering/OpenGLManager.h"
#include <Core/String.h>
#include <QtBaseManager.h>
#include <PluginSystem/PluginManager.h>
#include "Windows/LoginWindow.h"
#include <singleapplication/singleapplication.h>

// include Qt
#include <QObject>
#include <QString>
#include <QWidget>
#include <QSplashScreen>

// forward declarations
QT_FORWARD_DECLARE_CLASS(QSplashScreen)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QLabel)


// forward declarations
class MainWindow;
class VisualizationManager;

/**
 *
 *  What is the responsibility of this class? 
 *
 */
class AppManager : public QObject
{
	Q_OBJECT

	public:
		// constructor and destructor
		AppManager(int argc, char* argv[]);
		virtual ~AppManager();

		// access
		inline MainWindow* GetMainWindow()								{ return mMainWindow; }
		inline PluginManager* GetPluginManager()						{ return mPluginManager; }
		inline FileManager* GetFileManager()							{ return mFileManager; }
		inline OpenGLManager* GetOpenGLManager() const					{ return mOpenGLManager; }
		inline VisualizationManager* GetVisualizationManager()			{ return mVisualizationManager; }

		// version
		Core::Version GetVersion() const								{ return mVersion; }
		
		// information
		const char* GetCompanyName() const; // do not put Inc. behind this as this is also used as folder name
		const char* GetDeveloperName() const							{ return "neuromore Inc."; }
		const char* GetWebsite() const;
		const char* GetDocumentationUrl() const							{ return "https://doc.neuromore.com"; }
		const char* GetAccountUrl() const;
		const char* GetStoreUrl() const;
		const char* GetForgotPasswordUrl() const;
		const char* GetSupportEMail() const;
		const char* GetAppShortName() const;
		const char* GetMenuStudioName() const;
		const char* GetLicenseUrl() const;
		const char* GetCloudTermsUrl() const;
		const char* GetPrivacyPolicyUrl() const;

		const bool IsLoginRemberMePrechecked() const;
		const char* GetLoginImageName() const;

		Core::String GetAppName() const;
		const char* GetBackendSystemName() const;

		// authenticate and start the app in case of success
		bool ExecuteApp();

		void LogInfo();

		Core::String GetLogFilename() const								{ return QtBaseManager::GetAppDataFolder() + GetAppShortName() + "_Log.txt"; }

		// splash screen
		void SetSplashScreenMessage(const char* text)					{ mSplashScreen->showMessage(text, Qt::AlignBottom | Qt::AlignHCenter, Qt::white); }
		void CloseSplashScreen(QWidget* mainWindow)						{ mSplashScreen->finish(mainWindow); }

		void ProcessCommandLine();

	signals:
		void AppStartPrepared();

	private:
		void updateAllowedDevices();

	private:
		Core::Array<Core::String>			mCommandLineArguments;
		QSplashScreen*						mSplashScreen;
		MainWindow*							mMainWindow;
		SingleApplication*					mApp;
		PluginManager*						mPluginManager;
		FileManager*						mFileManager;
		OpenGLManager*						mOpenGLManager;
		VisualizationManager*				mVisualizationManager;
		Core::Version						mVersion;
};


class AppInitializer
{
	public:
		static bool CORE_CDECL Init(int argc, char* argv[]);
		static void CORE_CDECL Shutdown();
};


// the global manager
extern AppManager* gAppManager;

// shortcuts
inline AppManager*				GetManager()				{ return gAppManager; }
inline MainWindow*				GetMainWindow()				{ return gAppManager->GetMainWindow(); }
inline OpenGLManager*			GetOpenGLManager()			{ return gAppManager->GetOpenGLManager(); }

#endif
