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

// include the required headers
#include "AppManager.h"
#include "MainWindow.h"
#include "Version.h"
#include "VisualizationManager.h"

#ifdef NEUROMORE_PLATFORM_WINDOWS
#include <windows.h>
#include <winuser.h>
#endif

using namespace Core;

String brandingName;
bool includeDeviceTest = false;
bool includeDeviceEEMagine = false;
bool includeDeviceNeurosityNotion = false;
bool includeDeviceBrainmaster = false;
bool includeDeviceInteraxonMuse = false;
bool includeDeviceSenselabsVersus = false;
bool includeDeviceOpenBCI = false;
bool includeDeviceEsenseskinResponse = false;
bool includeDeviceMitsar = false;
bool includeDeviceBrainflow = false;

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

	// set version
	mVersion = Version( NEUROMORE_STUDIO_VERSION_MAJOR, NEUROMORE_STUDIO_VERSION_MINOR, NEUROMORE_STUDIO_VERSION_PATCH );

	mOpenGLManager	= NULL;
	gAppManager		= this;

#ifdef NEUROMORE_PLATFORM_OSX
    // needed for mac package
    QDir dir(argv[0]);
    if(dir.cd("../../PlugIns"))
    {
        QCoreApplication::setLibraryPaths(QStringList(dir.absolutePath()));
    }
#endif

	// enable high-dpi support
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // create a single or normal (multiple) instance(s) application
	mApp = new SingleApplication(argc, argv);

	QCommandLineParser parser;
    QCommandLineOption brandOption(QStringList() << "branding",
            QCoreApplication::translate("main", "Choose branding version."),
            QCoreApplication::translate("main", "branding"));
    parser.addOption(brandOption);
    parser.process(*mApp);

	if (!parser.values(brandOption).isEmpty()) {
		brandingName = parser.values(brandOption).first().toLocal8Bit().data();
	}

	updateAllowedDevices();

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
	CORE_LOGMANAGER.CreateLogFile( logFilename.AsChar() );

	// log header
	LogInfo();
	LogDetailedInfo("Log file '%s' created ...", logFilename.AsChar());

	// show splash screen
	LogDetailedInfo("Initializing splash screen ...");

	QString splashImagePath = ":/Images/SplashScreen-neuromore.png";

	if (brandingName == AntBrandingName) {
		splashImagePath = ":/Images/SplashScreen-ANT.png";
	} else if (brandingName == StarrbaseBrandingName) {
		splashImagePath = ":/Images/SplashScreen-Starrbase.png";
	}

	QPixmap pixmap(splashImagePath);
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
	connect( mApp, &SingleApplication::instanceStarted, mMainWindow, &MainWindow::OnRaise );

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
	String name;

	User* user = GetUser();
	if (user != NULL)
	{
		if (brandingName == AntBrandingName) {
			if (user->FindRule("ROLE_ResellerAdmin") != NULL) name = "eego perform studio - Reseller Admin";
			else if (user->FindRule("ROLE_ClinicAdmin") != NULL) name = "eego perform studio - Clinic Admin";
			else if (user->FindRule("ROLE_ClinicClinician") != NULL) name = "eego perform studio - Clinician";
			else if (user->FindRule("ROLE_ClinicOperator") != NULL) name = "eego perform studio - Operator";
			else if (user->FindRule("ROLE_ClinicPatient") != NULL) name = "eego perform studio - Patient";
			else name = "eego perform studio";
		} else if (brandingName == StarrbaseBrandingName) {
			if (user->FindRule("ROLE_Admin") != NULL) name = "Starrbase - Admin";
			else if (user->FindRule("ROLE_ClinicAdmin") != NULL) name = "Starrbase - Clinic Admin";
			else if (user->FindRule("ROLE_ClinicClinician") != NULL) name = "Starrbase - Clinician";
			else if (user->FindRule("ROLE_ClinicOperator") != NULL) name = "Starrbase - Operator";
			else if (user->FindRule("ROLE_ClinicPatient") != NULL) name = "Starrbase - Patient";
			else name = "Starrbase";
		} else {
			if (user->FindRule("ROLE_Admin") != NULL)					name = "neuromore Studio Administrator";
			else if (user->FindRule("ROLE_Ultimate") != NULL)				name = "neuromore Studio Ultimate";
			else if (user->FindRule("ROLE_Professional") != NULL)			name = "neuromore Studio Professional";
			else if (user->FindRule("ROLE_Community") != NULL)				name = "neuromore Studio Community";
			else if (user->FindRule("ROLE_BiofeedbackProvider") != NULL)	name = "neuromore Studio";
			else if (user->FindRule("ROLE_BiofeedbackUser") != NULL)		name = "neuromore Studio";
			else															name = "neuromore Studio";
		}
	}

	return name;
}

const char* AppManager::GetCompanyName() const
{
	const char* companyName = "neuromore";
	if (brandingName == AntBrandingName) {
		companyName = "eemagine";
	} else if (brandingName == StarrbaseBrandingName) {
		companyName = "myneurva";
	}
	return companyName;
}
const char* AppManager::GetWebsite() const
{
	const char* website = "https://www.neuromore.com";
	if (brandingName == AntBrandingName) {
		website = "https://eego-perform.com";
	} else if (brandingName == StarrbaseBrandingName) {
		website = "https://myneurva.com";
	}
	return website;
}
const char* AppManager::GetAccountUrl() const
{
	const char* accountUrl = "https://account.neuromore.com";
	if (brandingName == AntBrandingName) {
		accountUrl = "https://account.eego-perform.com";
	} else if (brandingName == StarrbaseBrandingName) {
		accountUrl = "https://starrbase.myneurva.com";
	}
	return accountUrl;
}
const char* AppManager::GetStoreUrl() const
{
	const char* storeUrl = "https://www.neuromore.com";
	if (brandingName == AntBrandingName) {
		storeUrl = "https://eego-perform.com";
	} else if (brandingName == StarrbaseBrandingName) {
		storeUrl = "https://myneurva.com";
	}
	return storeUrl;
}
const char* AppManager::GetForgotPasswordUrl() const
{
	const char* forgotPasswordUrl = "https://account.neuromore.com/#/resetrequest";
	if (brandingName == AntBrandingName) {
		forgotPasswordUrl = "https://account.eego-perform.com/resetrequest";
	} else if (brandingName == StarrbaseBrandingName) {
		forgotPasswordUrl = "https://starrbase.myneurva.com/resetrequest";
	}
	return forgotPasswordUrl;
}
const char* AppManager::GetSupportEMail() const
{
	const char* supportEMail = "support@neuromore.com";
	if (brandingName == AntBrandingName) {
		supportEMail = "support@eemagine.com";
	} else if (brandingName == StarrbaseBrandingName) {
		supportEMail = "support@myneurva.com";
	}
	return supportEMail;
}
const char* AppManager::GetAppShortName() const
{
	const char* appShortName = "NMStudio";
	if (brandingName == AntBrandingName) {
		appShortName = "eego-perform-studio";
	} else if (brandingName == StarrbaseBrandingName) {
		appShortName = "Starrbase";
	}
	return appShortName;
}
const char* AppManager::GetMenuStudioName() const
{
	const char* appShortName = "NMStudio";
	if (brandingName == AntBrandingName) {
		appShortName = "EPStudio";
	} else if (brandingName == StarrbaseBrandingName) {
		appShortName = "Starrbase";
	}
	return appShortName;
}
const char* AppManager::GetLicenseUrl() const
{
	const char* licenseUrl = "https://raw.githubusercontent.com/neuromore/studio/master/neuromore-licensing-info.md";
	if (brandingName == AntBrandingName) {
		licenseUrl = "https://assets.eego-perform.com/license/license.txt";
	} else if (brandingName == StarrbaseBrandingName) {
		licenseUrl = "https://assets.starrbase.myneurva.com/license/license.txt";
	}
	return licenseUrl;
}
const char* AppManager::GetCloudTermsUrl() const
{
	const char* cloudTermsUrl = "https://raw.githubusercontent.com/neuromore/legal/master/neuromore-general-terms.md";
	if (brandingName == AntBrandingName) {
		cloudTermsUrl = "https://assets.eego-perform.com/license/terms-and-conditions.txt";
	} else if (brandingName == StarrbaseBrandingName) {
		cloudTermsUrl = "https://assets.starrbase.myneurva.com/license/terms-and-conditions.txt";
	}
	return cloudTermsUrl;
}
const char* AppManager::GetPrivacyPolicyUrl() const
{
	const char* privacyPolicyUrl = "https://raw.githubusercontent.com/neuromore/legal/master/neuromore-privacy.md";
	if (brandingName == AntBrandingName) {
		privacyPolicyUrl = "https://assets.eego-perform.com/license/privacy-policy.txt";
	} else if (brandingName == StarrbaseBrandingName) {
		privacyPolicyUrl = "https://assets.starrbase.myneurva.com/license/privacy-policy.txt";
	}
	return privacyPolicyUrl;
}
const bool AppManager::IsLoginRemberMePrechecked() const
{
	bool isLoginRemberMePrechecked = true;
	if (brandingName == AntBrandingName) {
		isLoginRemberMePrechecked = false;
	}
	return isLoginRemberMePrechecked;
}
const char* AppManager::GetLoginImageName() const
{
	const char* loginImageName = ":/Images/Login-neuromore.png";
	if (brandingName == AntBrandingName) {
		loginImageName = ":/Images/Login-ANT.png";
	} else if (brandingName == StarrbaseBrandingName) {
		loginImageName = ":/Images/Login-Starrbase.png";
	}
	return loginImageName;
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

void AppManager::updateAllowedDevices()
{
	if (brandingName == AntBrandingName) {
		includeDeviceTest = true;
#if !defined(NEUROMORE_PLATFORM_OSX)
		includeDeviceEEMagine = true;
#endif
	} else if (brandingName == StarrbaseBrandingName) {
		includeDeviceTest = true;
		includeDeviceNeurosityNotion = true;
		#ifndef _M_X64
			includeDeviceBrainmaster = true;
		#endif
	} else {
		includeDeviceTest = true;
		includeDeviceInteraxonMuse = true;
		includeDeviceSenselabsVersus = true;
		includeDeviceOpenBCI = true;
		includeDeviceEsenseskinResponse = true;
		includeDeviceNeurosityNotion = true;
		includeDeviceBrainflow = true;
#if !defined(NEUROMORE_PLATFORM_OSX)
		includeDeviceEEMagine = true;
#endif
#if defined(NEUROMORE_PLATFORM_WINDOWS)
		includeDeviceEEMagine = true;
		includeDeviceMitsar = true;
		#ifndef _M_X64
			includeDeviceBrainmaster = true;
		#endif
#endif
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
