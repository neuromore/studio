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

// configuration of AppManager (private for brandings)
#ifdef NEUROMORE_BRANDING
#include <Studio/AppManagerConfig.h>
#else
class AppManagerConfig
{
private:
   inline AppManagerConfig() { }
public:
   static constexpr const char* CompanyName             = "neuromore";      // do not put Inc. behind this as this is also used as folder name
   static constexpr const char* DeveloperName           = "neuromore Inc.";
   static constexpr const char* Website                 = "https://www.neuromore.com";
   static constexpr const char* DocumentationUrl        = "https://doc.neuromore.com";
   static constexpr const char* AccountUrl              = "https://account.neuromore.com";
   static constexpr const char* StoreUrl                = "https://www.neuromore.com";
   static constexpr const char* ForgotPasswordUrl       = "https://account.neuromore.com/#/resetrequest";
   static constexpr const char* SupportEMail            = "support@neuromore.com";
   static constexpr const char* AppShortName            = "NMStudio";
   static constexpr const char* MenuStudioName          = "NMStudio";
   static constexpr const char* LicenseUrl              = "https://raw.githubusercontent.com/neuromore/studio/master/neuromore-licensing-info.md";
   static constexpr const char* CloudTermsUrl           = "https://raw.githubusercontent.com/neuromore/legal/master/neuromore-general-terms.md";
   static constexpr const char* PrivacyPolicyUrl        = "https://raw.githubusercontent.com/neuromore/legal/master/neuromore-privacy.md";
   static constexpr const char* LoginImageName          = ":/Images/Login-neuromore.png";
   static constexpr const bool  LoginRemberMePrechecked = true;
};
#endif

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

		const char* GetCompanyName() const       { return AppManagerConfig::CompanyName; }
		const char* GetDeveloperName() const     { return AppManagerConfig::DeveloperName; }
		const char* GetWebsite() const           { return AppManagerConfig::Website; }
		const char* GetDocumentationUrl() const  { return AppManagerConfig::DocumentationUrl; }
		const char* GetAccountUrl() const        { return AppManagerConfig::AccountUrl; }
		const char* GetStoreUrl() const          { return AppManagerConfig::StoreUrl; }
		const char* GetForgotPasswordUrl() const { return AppManagerConfig::ForgotPasswordUrl; }
		const char* GetSupportEMail() const      { return AppManagerConfig::SupportEMail; }
		const char* GetAppShortName() const      { return AppManagerConfig::AppShortName; }
		const char* GetMenuStudioName() const    { return AppManagerConfig::MenuStudioName; }
		const char* GetLicenseUrl() const        { return AppManagerConfig::LicenseUrl; }
		const char* GetCloudTermsUrl() const     { return AppManagerConfig::CloudTermsUrl; }
		const char* GetPrivacyPolicyUrl() const  { return AppManagerConfig::PrivacyPolicyUrl; }
		const char* GetLoginImageName() const    { return AppManagerConfig::LoginImageName; }
      const bool  IsLoginRemberMePrechecked() const { return true; }

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
