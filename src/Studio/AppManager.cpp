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
#include <qsettings.h>
#include <QDir>
#include <QFileInfo>

// include the required headers
#include "AppManager.h"
#include "MainWindow.h"
#include "Version.h"
#include "VisualizationManager.h"
#include "TourManager.h"

#ifdef NEUROMORE_PLATFORM_WINDOWS
#include <windows.h>
#include <winuser.h>
#endif

using namespace Core;

//--------------------------------------------------------------------------
// globals
//--------------------------------------------------------------------------
AppManager* gAppManager = NULL;

//--------------------------------------------------------------------------
// class AppManager
//--------------------------------------------------------------------------

// constructor
AppManager::AppManager(int argc, char* argv[])
{
	LogDetailedInfo("Constructing application manager object ...");

	// init tourmanager
	mTourManager = NULL;

	// set version
	mVersion = Version( NEUROMORE_STUDIO_VERSION_MAJOR, NEUROMORE_STUDIO_VERSION_MINOR, NEUROMORE_STUDIO_VERSION_PATCH );

	mOpenGLManager	= NULL;
	gAppManager		= this;

	// enable high-dpi support
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // create a single or normal (multiple) instance(s) application
	mApp = new Application(argc, argv);

	// get the application directory
	QString appDir = qApp->applicationDirPath();
	appDir	+= '/';
	GetQtBaseManager()->SetAppDir( appDir );

	LogDetailedInfo("Setting application information ...");
	QCoreApplication::setOrganizationName( GetCompanyName() );
	QCoreApplication::setOrganizationDomain( GetWebsite() );
	QCoreApplication::setApplicationName( GetAppShortName() );

	// create the log file callback
	String logFilename = GetLogFilename();
	if (QDir().mkpath(QFileInfo(logFilename.AsChar()).absolutePath()))
		CORE_LOGMANAGER.CreateLogFile( logFilename.AsChar() );

	// log header
	LogInfo();
	LogDetailedInfo("Log file '%s' created ...", logFilename.AsChar());

	// show splash screen
	LogDetailedInfo("Initializing splash screen ...");
	QPixmap pixmap(Branding::SplashImageName);
	mSplashScreen = new QSplashScreen(pixmap);

	// force taskbar entry for splashscreen on windows (otherwise it can get stuck in a state where the user cannot reach it anymore.. happened multiple times)
#ifdef NEUROMORE_PLATFORM_WINDOWS
	int exstyle = GetWindowLong( (HWND)mSplashScreen->winId(), GWL_EXSTYLE );
    SetWindowLong( (HWND)mSplashScreen->winId(), GWL_EXSTYLE, exstyle & ~WS_EX_TOOLWINDOW );
#endif

	// disable close-on-click
	mSplashScreen->setEnabled(false);

	// set font and text
	QFont splashFont;
	splashFont.setPixelSize(12);
	mSplashScreen->setFont(splashFont);
	SetSplashScreenMessage("Starting ...");

	LogDetailedInfo("Showing splash screen ...");
	
	// make sure this window is on top
	mSplashScreen->show();
	mSplashScreen->activateWindow();
	mSplashScreen->raise();

	mApp->processEvents();

	// parse the command line arguments and store them
	LogDetailedInfo("Parsing command line arguments ...");
	for (int32 i=0; i<argc; ++i)
		mCommandLineArguments.Add( argv[i] );

	// enable this if you want to share OpenGL contexts between all QOpenGLWidgets
	//QCoreApplication::setAttribute( Qt::AA_ShareOpenGLContexts );

	// pass the app data folder to the neuro manager
	GetEngine()->SetAppDataFolder( QtBaseManager::GetAppDataFolder() );

	// set the window icon
	LogDetailedInfo("Setting application icon ...");
	QIcon icon( ":/Images/Icons/WindowIcon.png" );
	mApp->setWindowIcon(icon);

	// set the color palette
	LogDetailedInfo("Setting color palette ...");
	QPalette palette;
	qApp->setPalette(palette);
}


// destructor
AppManager::~AppManager()
{
	delete mFileManager;
	delete mPluginManager;
	delete mMainWindow;
	delete mOpenGLManager;
	delete mVisualizationManager;

	delete mSplashScreen;

	// NOTE: this crashed on application shutdown on some computers for older versions
	delete mApp;
}


// log info
void AppManager::LogInfo()
{
	Core::LogInfo( "======================================================================================================" );
	Core::LogInfo( "%s - Log", GetAppName().AsChar() );
	Core::LogInfo( "Compilation date: %s", GetQtBaseManager()->GetCompileDate());
	Core::LogInfo( "======================================================================================================" );
	Core::LogInfo( "Build CPU Architecture: %s", QSysInfo::buildCpuArchitecture().toUtf8().data() );
	Core::LogInfo( "CPU Architecture: %s", QSysInfo::currentCpuArchitecture().toUtf8().data() );
	Core::LogInfo( "Endianness: %s", QSysInfo::ByteOrder == QSysInfo::LittleEndian ? "Little Endian" : "Big Endian");
	Core::LogInfo( "Machine Host Name: %s", QSysInfo::machineHostName().toUtf8().data() );
	Core::LogInfo( "Operating System: %s", QSysInfo::prettyProductName().toUtf8().data() );
	Core::LogInfo( "Kernel Type: %s", QSysInfo::kernelType().toUtf8().data() );
	Core::LogInfo( "Kernel Version: %s", QSysInfo::kernelVersion().toUtf8().data() );
	//Core::LogInfo( "Product Type: %s", QSysInfo::productType().toUtf8().data() );
	//Core::LogInfo( "Product Version: %s", QSysInfo::productVersion().toUtf8().data() );
	Core::LogInfo( "======================================================================================================" );
}


// execute the application
bool AppManager::ExecuteApp()
{
	CORE_ASSERT( mApp );

	LogDetailedInfo("Constructing main window ...");
	mMainWindow = new MainWindow();

#if defined(NEUROMORE_PLATFORM_OSX)
	mMainWindow->OnRaise();
#else
	connect(mApp, &SingleApplication::instanceStarted, mMainWindow, &MainWindow::OnRaise);
#endif

	connect( mMainWindow, &MainWindow::postAuthenticationInitSucceed, this, &AppManager::LoadTourManager);

	SetSplashScreenMessage("Initializing windows ...");

	// initializing the OpenGL manager
	mOpenGLManager = new OpenGLManager();

	// initializing the plugin manager
	mPluginManager = new PluginManager();
	GetQtBaseManager()->SetPluginManager(mPluginManager);

	// file manager
	mFileManager = new FileManager();
	GetQtBaseManager()->SetFileManager(mFileManager);

	// visualization manager
	mVisualizationManager = new VisualizationManager();

	// initialize main window
	mMainWindow->Init();

	// load the custom font
	/*QString fontFilename = GetQtBaseManager()->GetAppDir() + "/Fonts/Apercu/Apercu Medium.otf";
	int fontResult = QFontDatabase::addApplicationFont(fontFilename);
	if (fontResult != -1)
	{
		QFontDatabase fontDatabase;
		QFont newFont = fontDatabase.font("Apercu Medium", "", QApplication::font().pointSize());
		QApplication::setFont(newFont);
	}*/

	// update the main window create window item with, so that it shows all loaded plugins
	GetPluginManager()->ReInitMenu();

	// try to load the default style sheet
	SetSplashScreenMessage("Loading style sheet ...");
	QString stylePath = ":/Styles/Default.style";
	if (QtBaseManager::LoadStyleSheet( stylePath, mMainWindow) == false)
		Core::LogError("Failed to load default style sheet '%s' from resource path ", FromQtString(stylePath).AsChar());
    
	// process events and the command line
	LogDetailedInfo( "Processing events ..." );
	mApp->processEvents();
	ProcessCommandLine();
	
	// reload the settings a second time after all windows have been initialized
	LogDetailedInfo( "Load application settings (after all plugins successfully loaded) ..." );
	mMainWindow->OnLoadSettings();

	// tell everything that we fully loaded everything and that we are about to start the event loop
	emit AppStartPrepared();

	// authenticate the user
	GetAuthenticationCenter()->Start();

	// execute the application
	LogDetailedInfo( "Execute application ..." );
	return mApp->exec();
}


// parse command line arguments
void AppManager::ProcessCommandLine()
{
	// parse the command line arguments and load the corresponding files
	/*const uint32 numArguments = mCommandLineArguments.Size();
	if (numArguments > 1)
		mMainWindow->LoadFiles( mCommandLineArguments );*/
}


String AppManager::GetAppName() const
{
   String name = Branding::AppName;
   if (User* user = GetUser())
   {
      // default ones
      if      (user->FindRule("ROLE_Admin"))        name += " - Administrator";
      else if (user->FindRule("ROLE_Ultimate"))     name += " - Ultimate";
      else if (user->FindRule("ROLE_Professional")) name += " - Professional";
      else if (user->FindRule("ROLE_Community"))    name += " - Community";

      // clinic
      else if (user->FindRule("ROLE_ResellerAdmin"))   name += " - Reseller Admin";
      else if (user->FindRule("ROLE_ClinicAdmin"))     name += " - Clinic Admin";
      else if (user->FindRule("ROLE_ClinicClinician")) name += " - Clinician";
      else if (user->FindRule("ROLE_ClinicOperator"))  name += " - Operator";
      else if (user->FindRule("ROLE_ClinicPatient"))   name += " - Patient";
   }
   return name;
}


const char* AppManager::GetBackendSystemName() const
{
#ifdef NEUROMORE_PLATFORM_WINDOWS
	return "win_studio";
#endif
#ifdef NEUROMORE_PLATFORM_OSX
	return "osx_studio";
#endif
#ifdef NEUROMORE_PLATFORM_LINUX
	return "win_studio"; // TODO linux backend system name
#endif
}

void AppManager::SetPluginTabVisible(int activePluginIdx)
{
	if (activePluginIdx < 0)
	{
		return;
	}

	auto activePlugin = GetQtBaseManager()->GetPluginManager()
					  ->GetActivePlugin(activePluginIdx);
	if (nullptr == activePlugin)
	{
		return;
	}
	QDockWidget* dockWidget = activePlugin->GetDockWidget();
	Q_FOREACH(QTabBar * tabBar, mMainWindow->findChildren<QTabBar*>()) {
		for (int i = 0; i < tabBar->count(); ++i) {
			if (dockWidget == (QDockWidget*)tabBar->tabData(i).toULongLong())
			{
				tabBar->setCurrentIndex(i);
				return;
			}
		}
	}
}

void AppManager::LoadTourManager()
{
	this->mTourManager = new TourManager();
	QTimer::singleShot(1000, this, [this] {
		if (this->mTourManager->InitOnboardingActions()) {
			this->mTourManager->startTour();
		} else {
			Core::LogError("Could not find some widgets or tabs for the tour.");
		}
	});
}

void AppManager::CloseTour()
{
	if (mTourManager != nullptr)
	{
		delete mTourManager;
		mTourManager = nullptr;
	}
}

//--------------------------------------------------------------------------
// class Initializer
//--------------------------------------------------------------------------
// initialize application
bool AppInitializer::Init(int argc, char* argv[])
{
	// do nothing if we already have initialized
	if (gAppManager)
		return true;

	// create the new application object
	gAppManager = new AppManager(argc, argv);

	// return success
	return true;
}


// the shutdown function
void AppInitializer::Shutdown()
{
	LogInfo("Shutting down application ...");

	delete gAppManager;
	gAppManager = NULL;
}
