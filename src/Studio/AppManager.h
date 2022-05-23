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
#ifdef NEUROMORE_BRANDING_ANT
		const char* GetCompanyName() const								{ return "eemagine"; } // do not put Inc. behind this as this is also used as folder name
		const char* GetDeveloperName() const							{ return "neuromore Inc."; }
		const char* GetWebsite() const									{ return "https://eego-perform.com"; }
		const char* GetDocumentationUrl() const							{ return "https://doc.neuromore.com"; }
		const char* GetAccountUrl() const								{ return "https://account.eego-perform.com"; }
		const char* GetStoreUrl() const									{ return "https://eego-perform.com"; }
		const char* GetForgotPasswordUrl() const						{ return "https://account.eego-perform.com/resetrequest"; }
		const char* GetSupportEMail() const								{ return "support@eemagine.com"; }
		const char* GetAppShortName() const								{ return "eego-perform-studio"; }
		const char* GetMenuStudioName() const							{ return "EPStudio"; }
		const char* GetLicenseUrl() const								{ return "https://assets.eego-perform.com/license/license.txt"; }
		const char* GetCloudTermsUrl() const							{ return "https://assets.eego-perform.com/license/terms-and-conditions.txt"; }
		const char* GetPrivacyPolicyUrl() const							{ return "https://assets.eego-perform.com/license/privacy-policy.txt"; }

		const bool IsLoginRemberMePrechecked() const { return false; }
		const char* GetLoginImageName() const							{ return ":/Images/Login-ANT.png"; }
#elif NEUROMORE_BRANDING_STARRBASE
		const char* GetCompanyName() const								{ return "myneurva"; } // do not put Inc. behind this as this is also used as folder name
		const char* GetDeveloperName() const							{ return "neuromore Inc."; }
		const char* GetWebsite() const									{ return "https://myneurva.com"; }
		const char* GetDocumentationUrl() const						{ return "https://doc.neuromore.com"; }
		const char* GetAccountUrl() const								{ return "https://starrbase.myneurva.com"; }
		const char* GetStoreUrl() const									{ return "https://myneurva.com"; }
		const char* GetForgotPasswordUrl() const						{ return "https://starrbase.myneurva.com/resetrequest"; }
		const char* GetSupportEMail() const								{ return "support@myneurva.com"; }
		const char* GetAppShortName() const								{ return "Starrbase"; }
		const char* GetMenuStudioName() const							{ return "Starrbase"; }
		const char* GetLicenseUrl() const								{ return "https://assets.starrbase.myneurva.com/license/license.txt"; }
		const char* GetCloudTermsUrl() const							{ return "https://assets.starrbase.myneurva.com/license/terms-and-conditions.txt"; }
		const char* GetPrivacyPolicyUrl() const						{ return "https://assets.starrbase.myneurva.com/license/privacy-policy.txt"; }
		const bool  IsLoginRemberMePrechecked() const				{ return true; }
		const char* GetLoginImageName() const							{ return ":/Images/Login-Starrbase.png"; }
#else
		const char* GetCompanyName() const								{ return "neuromore"; } // do not put Inc. behind this as this is also used as folder name
		const char* GetDeveloperName() const							{ return "neuromore Inc."; }
		const char* GetWebsite() const									{ return "https://www.neuromore.com"; }
		const char* GetDocumentationUrl() const							{ return "https://doc.neuromore.com"; }
		const char* GetAccountUrl() const								{ return "https://account.neuromore.com"; }
		const char* GetStoreUrl() const									{ return "https://www.neuromore.com"; }
		const char* GetForgotPasswordUrl() const						{ return "https://account.neuromore.com/#/resetrequest"; }
		const char* GetSupportEMail() const								{ return "support@neuromore.com"; }
		const char* GetAppShortName() const								{ return "NMStudio"; }
		const char* GetMenuStudioName() const							{ return "NMStudio"; }
		const char* GetLicenseUrl() const								{ return "https://raw.githubusercontent.com/neuromore/studio/master/neuromore-licensing-info.md"; }
		const char* GetCloudTermsUrl() const							{ return "https://raw.githubusercontent.com/neuromore/legal/master/neuromore-general-terms.md"; }
		const char* GetPrivacyPolicyUrl() const							{ return "https://raw.githubusercontent.com/neuromore/legal/master/neuromore-privacy.md"; }

		const bool IsLoginRemberMePrechecked() const					{ return true; }
		const char* GetLoginImageName() const							{ return ":/Images/Login-neuromore.png"; }

#endif

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
		SingleApplication*					mApp;
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
