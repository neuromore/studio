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
#include <Core/String.h>
#include <Engine/Branding.h>
#include <QtBase/QtBaseManager.h>
#include <PluginSystem/PluginManager.h>
#include <singleapplication/singleapplication.h>
#include "Rendering/OpenGLManager.h"
#include "Windows/LoginWindow.h"
#include "OnboardingAction.h"

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
class TourManager;

/**
 *
 *  What is the responsibility of this class? 
 *
 */
class AppManager : public QObject
{
	Q_OBJECT

#if defined(NEUROMORE_PLATFORM_OSX)
		using Application = QApplication;
#else
		using Application = SingleApplication;
#endif

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

		const char* GetCompanyName() const       { return Branding::CompanyName; }
		const char* GetDeveloperName() const     { return Branding::DeveloperName; }
		const char* GetWebsite() const           { return Branding::Website; }
		const char* GetDocumentationUrl() const  { return Branding::DocumentationUrl; }
		const char* GetAccountUrl() const        { return Branding::AccountUrl; }
		const char* GetStoreUrl() const          { return Branding::StoreUrl; }
		const char* GetForgotPasswordUrl() const { return Branding::ForgotPasswordUrl; }
		const char* GetSupportEMail() const      { return Branding::SupportEMail; }
		const char* GetAppShortName() const      { return Branding::AppShortName; }
		const char* GetMenuStudioName() const    { return Branding::MenuStudioName; }
		const char* GetLicenseUrl() const        { return Branding::LicenseUrl; }
		const char* GetCloudTermsUrl() const     { return Branding::CloudTermsUrl; }
		const char* GetPrivacyPolicyUrl() const  { return Branding::PrivacyPolicyUrl; }
		const char* GetLoginImageName() const    { return Branding::LoginImageName; }
		const bool  IsLoginRemberMePrechecked() const { return Branding::LoginRemberMePrechecked; }

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

		void SetPluginTabVisible(int activePluginIdx);

	public slots:
		void LoadTourManager();

		void CloseTour();

	signals:
		void AppStartPrepared();

	private:
		Core::Array<Core::String>			mCommandLineArguments;
		QSplashScreen*						mSplashScreen;
		MainWindow*							mMainWindow;
		Application*						mApp;
		PluginManager*						mPluginManager;
		FileManager*						mFileManager;
		OpenGLManager*						mOpenGLManager;
		VisualizationManager*				mVisualizationManager;
		TourManager*						mTourManager;
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
