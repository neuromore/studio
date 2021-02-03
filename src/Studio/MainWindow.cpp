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

// include required headers
#include "MainWindow.h"
#include <Core/LogManager.h>
#include <License.h>
#include "AppManager.h"
#include <PluginSystem/PluginManager.h>
#include <Graph/GraphImporter.h>
#include "Windows/AboutWindow.h"
#include "Windows/UpgradeWindow.h"
#include "Windows/SettingsWindow.h"
#include "Windows/LicenseAgreementWindow.h"
#include "Windows/EnterLabelWindow.h"
#include "Windows/SelectUserWindow.h"
#include <EngineManager.h>
#include <AutoUpdate/AutoUpdate.h>
#include <LayoutManager.h>
#include <LayoutComboBox.h>
#include <LayoutMenu.h>
#include "Experience.h"
#include "VisualizationManager.h"
#include "DocumentationExporter.h"
#include <Backend/FilesGetRequest.h>
#include <Backend/FilesGetResponse.h>

#include <Devices/DeviceInventory.h>
#include "Devices/DriverInventory.h"
#include <Devices/Test/TestDeviceDriver.h>
#include <Devices/Muse/MuseDevice.h>
#include "Devices/Audio/AudioDriver.h"

#include <System/SerialPort.h>
#include <System/BluetoothHelpers.h>

// include the plugin headers
#include "Plugins/SessionControl/SessionControlPlugin.h"
#include "Plugins/Graph/ClassifierPlugin.h"
#include "Plugins/Graph/StateMachinePlugin.h"
#include "Plugins/Feedback/FeedbackPlugin.h"
#include "Plugins/Parameter/ParameterControlPlugin.h"
#include "Plugins/RawWaveform/RawWaveformPlugin.h"
#include "Plugins/Spectrogram2D/Spectrogram2DPlugin.h"
#include "Plugins/Spectrogram/SpectrogramSurfacePlugin.h"
#include "Plugins/Spectrogram/SpectrogramBandsPlugin.h"
#include "Plugins/Spectrogram/SpectrogramPluginCollection.h"
#include "Plugins/SpectrumAnalyzerSettings/SpectrumAnalyzerSettingsPlugin.h"
#include "Plugins/BackendFileSystem/BackendFileSystemPlugin.h"
#include "Plugins/Networking/NetworkTestClientPlugin.h"
#include "Plugins/EngineStatus/EngineStatusPlugin.h"
#include "Plugins/Devices/DevicesPlugin.h"
#include "Plugins/View/ViewPlugin.h"
#include "Plugins/View/ViewSpectrumPlugin.h"
#include "Plugins/Experience/ExperiencePlugin.h"
#include "Plugins/ExperienceSelection/ExperienceSelectionPlugin.h"

// example plugins
#include "Plugins/Examples/SimpleOpenGLPlugin/ExampleOpenGLPlugin.h"

// development plugins
#include "Plugins/Development/LatencyTestPlugin/LatencyTestPlugin.h"
#include "Plugins/Development/EEGElectrodePlacementPlugin/EEGElectrodePlacementPlugin.h"
#include "Plugins/Development/HeatmapPlugin/HeatmapPlugin.h"

#ifdef OPENCV_SUPPORT
//#include "Plugins/Development/LORETA/LoretaPlugin.h"
#endif

// include Qt related
#include <QMenu>
#include <QMenuBar>
#include <QVariant>
#include <QSignalMapper>
#include <QTextEdit>
#include <QDir>
#include <QMessageBox>
#include <QToolBar>
#include <QLineEdit>
#include <QLabel>
#include <QFileDialog>
#include <QApplication>
#include <QDesktopServices>
#include <QComboBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QTimer>
#include <QSettings>
#include <QProcess>


using namespace Core;

// constructor
MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags) : MainWindowBase(parent, flags)
{
	mAboutWindow				= NULL;
	mActiveBciCombo				= NULL;
	mSettingsWindow				= NULL;
	mSessionUserSelectionWindow = NULL;
	mExperienceWizardWindow    = NULL;
	mVisualizationMenu			= NULL;

	LogDetailedInfo("Adding main window event handler ...");
	CORE_EVENTMANAGER.AddEventHandler(this);

	// setup some properties
    setObjectName("MainWindow");
}


// destructor
MainWindow::~MainWindow()
{
	CORE_EVENTMANAGER.RemoveEventHandler(this);

	// save the settings
	OnSaveSettings();

	delete mAboutWindow;
	delete mNetworkServer;
	delete mOscServer;
	delete mAuthenticationCenter;
	delete mSessionUserSelectionWindow;
}


// init the main window
void MainWindow::Init()
{
	// tell Qt base about the main window
	GetQtBaseManager()->SetMainWindow(this);

	// initialize the qt base manager
	LogDetailedInfo("Initializing Qt base manager ...");
	GetQtBaseManager()->Init();

	// create the network server
	LogDetailedInfo("Initializing network components...");
	mNetworkServer = new NetworkServer(STUDIO_NETWORKSERVER_TCP_PORT, STUDIO_NETWORKSERVER_UDP_PORT, STUDIO_NETWORKCLIENT_UDP_PORT);
	mNetworkServer->SetClientTimeout( STUDIO_NETWORKSERVER_CLIENT_TIMEOUT );
	mNetworkServer->SetRealtimeUpdateRate( STUDIO_NETWORKSERVER_REALTIME_UPDATE_FREQUENCY );
	mNetworkServer->SetAnnounceInterval(STUDIO_NETWORKSERVER_ANNOUNCE_INTERVAL);
	mNetworkServer->SetBroadcastServerAnnounce( false ); // default value: disable server announce
	mNetworkServer->SetListenerEnabled( true );

	// create the osc listener
	mOscServer = new OscServer(STUDIO_OSCLISTENER_UDP_PORT, STUDIO_OSCREMOTE_UDP_PORT);

#ifdef BACKEND_LOGGING
	// enable back-end logging
	GetBackendInterface()->SetIsLogEnabled(true);
#else
	// disable back-end logging
	GetBackendInterface()->SetIsLogEnabled(false);
#endif

	// initialize the authentication center
	GetManager()->SetSplashScreenMessage("Starting authentication center ...");
	mAuthenticationCenter = new AuthenticationCenter();

	// load the settings
	GetManager()->SetSplashScreenMessage("Loading settings ...");
	OnLoadSettings();

	// init networkserver/osc listener after settings are loaded
	mNetworkServer->Init();
	mOscServer->Init();

	// base class init
	MainWindowBase::Init();

	// fullscreen mode for screenshots: hide window decorations (title bar etc)
#ifdef FULLSCREEN_MODE
	setWindowFlags(Qt::CustomizeWindowHint);
#endif

	// enable drag&drop support
	setAcceptDrops(true);

	setDockNestingEnabled( true );

	
	setDockOptions( AnimatedDocks | AllowNestedDocks );

	setFocusPolicy(Qt::StrongFocus);

	// create the menu bar
	GetManager()->SetSplashScreenMessage("Creating menus ...");
	LogDetailedInfo("Creating main menu ...");

	QWidget* menuWidget = new QWidget();
	mMenuBar = new QMenuBar(menuWidget);
#ifdef NEUROMORE_PLATFORM_OSX
    mMenuBar->setNativeMenuBar(false);
#endif
	mMenuBar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

	QHBoxLayout* menuLayout = new QHBoxLayout();
	menuLayout->setMargin(0);
	menuLayout->setSpacing(0);
	menuLayout->setSizeConstraint(QLayout::SetMaximumSize);
	menuLayout->addWidget(mMenuBar);
	menuWidget->setLayout(menuLayout);

	// active BCI combo box
	mActiveBciLabel = new QLabel("Active BCI: ");
	menuLayout->addWidget(mActiveBciLabel);

	mActiveBciCombo = new QComboBox();
	mActiveBciCombo->setMinimumWidth(120);
	mActiveBciCombo->setObjectName("LayoutCombo");
	menuLayout->addWidget(mActiveBciCombo);

	connect( mActiveBciCombo, &QComboBox::currentTextChanged, this, &MainWindow::OnBciSelectionChanged );
	mActiveBciCombo->setCurrentIndex( -1 );

	ReInitBciDeviceCombo();

	// spacer
	QWidget* spacerWidget = new QWidget();
	spacerWidget->setMinimumWidth(5);
	spacerWidget->setMaximumWidth(5);
	menuLayout->addWidget(spacerWidget);

	// layout combo box
	QLabel* layoutLabel = new QLabel("Layout: ");
	menuLayout->addWidget(layoutLabel);
	
	mLayoutComboBox = new LayoutComboBox();
	menuLayout->addWidget(mLayoutComboBox);
	GetLayoutManager()->SetComboBox( mLayoutComboBox );
	
	// spacer
	spacerWidget = new QWidget();
	spacerWidget->setMinimumWidth(5);
	spacerWidget->setMaximumWidth(5);
	menuLayout->addWidget(spacerWidget);

	mSelectSessionUserLabel = new QLabel("User: ");
	menuLayout->addWidget(mSelectSessionUserLabel);

	mSelectSessionUserButton = new QPushButton("Select User"); // note: don't know the user yet, it wasn't set yet
	mSelectSessionUserButton->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Users.png") );
	menuLayout->addWidget(mSelectSessionUserButton);
	connect (mSelectSessionUserButton, &QPushButton::clicked, this, &MainWindow::SelectSessionUser);

	// hide both by default, enable them during post authentication
	mSelectSessionUserButton->setVisible(false);
	mSelectSessionUserLabel->setVisible(false);

	setMenuWidget(menuWidget);

	//
	// file menu
	//
	QMenu* fileMenu = mMenuBar->addMenu( tr("&File") );
	
	// open file
	//QAction* openAction = fileMenu->addAction( tr("&Open"), this, &MainWindow::OnOpenFile );
	//openAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Open.png") );
	//openAction->setEnabled(false);

	// save file
	mSaveAction = fileMenu->addAction( tr("&Save"), this, &MainWindow::OnSaveDesign );
	mSaveAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Save.png") );
	mSaveAction->setVisible(false);

	// close file
	mCloseAction = fileMenu->addAction( tr("&Close"), this, &MainWindow::OnCloseDesign );
	mCloseAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Clear.png") );
	mCloseAction->setVisible(false);

	mExitAction = fileMenu->addSeparator();

	// exit application action
	mExitAction = fileMenu->addAction( tr("E&xit"), this, &MainWindow::OnExit );
	mExitAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Exit.png") );

	//
	// EDIT menu
	//

	mEditMenu = mMenuBar->addMenu( tr("&Edit") );
	
	// reset engine 
	QAction* resetEngineAction = mEditMenu->addAction(tr("Reset Engine"), this, &MainWindow::OnResetEngine);
	resetEngineAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Reset.png") );

	mEditMenu->addSeparator();
	
	QAction* settingsAction = mEditMenu->addAction( tr("&Settings"), this, &MainWindow::OnSettings );
	settingsAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Gear.png") );

	//
	// VIEW menu
	//
	
	mLayoutMenu = GetLayoutManager()->ConstructMenu();
	mPluginMenu = GetPluginManager()->ConstructMenu();
		
	// combined view menu
	QMenu* viewMenu = mMenuBar->addMenu( tr("&View") );
	viewMenu->addMenu(mPluginMenu->GetCreateMenu());			// Add Item from layout
	viewMenu->addMenu(mPluginMenu->GetRemoveMenu());			// Remove Item from layout
	viewMenu->addMenu(mLayoutMenu);								// list of layouts

#ifdef CORE_DEBUG
	// reload stylesheet
	QAction* reloadStylesheetsAction = viewMenu->addAction( tr("&Reload Stylesheets"), this, &MainWindow::OnReloadStyleSheet );
	reloadStylesheetsAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Refresh.png") );
#endif


	//
	// DESIGN menu
	//

	mDesignMenu = mMenuBar->addMenu( tr("&Design") );

	// hide by default
	mDesignMenu->menuAction()->setVisible(false);

	// reload design
	QAction* reloadDesignAction = mDesignMenu->addAction( tr("&Reload"), this, &MainWindow::OnReloadDesign );
	reloadDesignAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Refresh.png") );

	// save design
	mSaveDesignAction = mDesignMenu->addAction( tr("&Save"), this, &MainWindow::OnSaveDesign );
	mSaveDesignAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Save.png") );
	
	// close
	QAction* closeDesignAction = mDesignMenu->addAction( tr("&Close"), this, &MainWindow::OnCloseDesign);
	closeDesignAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Clear.png") );

	//
	// Design -> Settings menu
	//

	QMenu* settingsMenu = mDesignMenu->addMenu( tr("Se&ttings") );
	settingsMenu->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Settings.png"));

	// settings mode
	mEnableDesignSettingsAction = settingsMenu->addAction(tr ("&Enable Settings Mode"), this, &MainWindow::OnEnableSettingsMode);
	mEnableDesignSettingsAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/SuccessBlue.png") );
	mEnableDesignSettingsAction->setVisible(false);
	mDisableDesignSettingsAction = settingsMenu->addAction(tr ("&Remove"), this, &MainWindow::OnDisableSettingsMode);
	mDisableDesignSettingsAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Remove.png") );
	
	// reload design settings
	QAction* loadSettingsAction = settingsMenu->addAction(tr("Re&load"), this, &MainWindow::OnLoadDesignSettings);
	loadSettingsAction->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Reset.png"));

#ifndef PRODUCTION_BUILD
	// attach device settings
	mAttachDeviceSettingsAction = settingsMenu->addAction(tr("&Attach Device Settings"), this, &MainWindow::OnAttachDeviceSettings);
	mAttachDeviceSettingsAction->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/PlusBlue.png"));
#endif

	//
	// VISUALIZATIONS menu
	//

	VisualizationManager* vizManager = GetManager()->GetVisualizationManager();
	vizManager->ReInit();

	const uint32 numVisualizations = vizManager->GetNumVisualizations();
	if (numVisualizations > 0)
	{
		mVisualizationMenu = mMenuBar->addMenu( tr("&Visualizations") );

		for (uint32 i=0; i<numVisualizations; ++i)
		{
			Visualization* visualization = vizManager->GetVisualization(i);

			QAction* vizAction = mVisualizationMenu->addAction( visualization->GetName(), this, &MainWindow::OnStartVisualization );
			vizAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Eye.png") );
			vizAction->setProperty("index", i);
		}
	}

	//
	// TOOLS menu
	//

	QMenu* toolsMenu = mMenuBar->addMenu( tr("&Tools") );
	
#ifdef INCLUDE_DEVICE_INTERAXON_MUSE
	
#ifdef NEUROMORE_PLATFORM_WINDOWS
	const bool showMuseMenue = QFile( GetQtBaseManager()->GetAppDir() + "\\Muse\\muse-io.exe").exists();
#else
	const bool showMuseMenue = true;
#endif

	if (showMuseMenue == true)
	{
		// start muse io
		QAction* startMuseIOAction = toolsMenu->addAction( tr("Start MuseIO"), this, &MainWindow::OnStartMuseIO );
		startMuseIOAction->setIcon( GetQtBaseManager()->FindIcon("Images/Devices/InteraXon Muse.png") );
	}

#endif

	// tools/information menu

	QMenu* infoMenu = toolsMenu->addMenu(tr("&Information"));
	infoMenu->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Info.png"));
	
	// serial port info
	QAction* showSerialPortInfoAction = infoMenu->addAction(tr("Serial Ports"), this, &MainWindow::OnShowSerialPortInfo);
	showSerialPortInfoAction->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Info.png"));

	// bluetooth device info
	QAction* showBluetoothDeviceInfoAction = infoMenu->addAction(tr("Bluetooth Devices"), this, &MainWindow::OnShowBluetoothInfo);
	showBluetoothDeviceInfoAction->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Info.png"));


#ifndef PRODUCTION_BUILD
	// tool/dev menu
	QMenu* devMenu = toolsMenu->addMenu(tr("&Dev"));
	devMenu->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Gear.png"));

	// empty menu action for doing stuff while developing
	//QAction* doSthAction = devMenu->addAction(tr("Do Something"), this, &MainWindow::OnDoSomething);

	// crash the program intentionally
	QAction* crashAction = devMenu->addAction(tr("Crash"), this, &MainWindow::OnCrash);
	crashAction->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Remove.png"));

	// make classifier editable
	QAction* unlockClassifierAction = devMenu->addAction( tr("Unlock Classifier"), this, &MainWindow::OnUnlockClassifier );
	unlockClassifierAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Tool.png") );

	// reload parameters
	QAction* resizeWindowAction = devMenu->addAction(tr("Resize Window"), this, &MainWindow::OnResizeWindowAction);
	resizeWindowAction->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Tool.png"));

	// export node markdown documentation
	QAction* exportNodeDocMarkdown = devMenu->addAction(tr("Export Node Docs"), this, &MainWindow::OnExportNodeDocMarkdown);
	exportNodeDocMarkdown->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Tool.png"));

	// export node markdown table
	QAction* exportNodeTableMarkdown = devMenu->addAction(tr("Export Node Table"), this, &MainWindow::OnExportNodeTableMarkdown);
	exportNodeTableMarkdown->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Tool.png"));


#endif

	//
	// HELP menu
	//
	QMenu* helpMenu = mMenuBar->addMenu( tr("&Help") );

#ifdef USE_AUTOUPDATE
	QAction* updateAction = helpMenu->addAction( tr("Check For Updates"), this, &MainWindow::OnCheckForUpdates );
	updateAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Refresh.png") );
#endif

	// sign out
	QAction* userLogoutAction = helpMenu->addAction( tr("&Log out"), this, &MainWindow::OnLogOut );
	userLogoutAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Exit.png") );

	helpMenu->addSeparator();

	QAction* visitWebsiteAction = helpMenu->addAction( tr("Website"), this, &MainWindow::OnVisitWebsite );
	visitWebsiteAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Internet.png") );

	QAction* visitHelpCenterAction = helpMenu->addAction( tr("Account"), this, &MainWindow::OnVisitAccount );
	visitHelpCenterAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Internet.png") );

	QAction* visitDocsAction = helpMenu->addAction( tr("Documentation"), this, &MainWindow::OnVisitDocumentation );
	visitDocsAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Internet.png") );

	QAction* feedbackAction = helpMenu->addAction( tr("Support"), this, &MainWindow::OnVisitSupport );
	feedbackAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Support.png") );

	helpMenu->addSeparator();

	QAction* licensesAction = helpMenu->addAction( tr("Licenses"), this, &MainWindow::OnLicense );
	licensesAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Info.png") );

	QAction* aboutAction = helpMenu->addAction( tr("About"), this, &MainWindow::OnAbout );
	aboutAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Info.png") );

	// disable the user interface on default
	setEnabled(false);
}


void MainWindow::OnPostAuthenticationInit()
{
	// DEVICES

	// register devices
	GetManager()->SetSplashScreenMessage("Registering devices");
	LogDetailedInfo("Registering devices");
	DeviceInventory::RegisterDevices();

	// register device drivers
	GetManager()->SetSplashScreenMessage("Registering device drivers");
	LogDetailedInfo("Registering device drivers");

	// from extern repo:
	DriverInventory::RegisterDrivers(); 
	
	// studio built in:
	#ifdef INCLUDE_DEVICE_TEST
	if (GetUser()->ReadAllowed(TestDevice::GetRuleName()))
		GetDeviceManager()->AddDeviceDriver(new TestDeviceDriver());
	#endif

	// load device configs (requires all devices to be present)
	GetManager()->SetSplashScreenMessage("Loading device definitions...");
	LoadDeviceConfigs();


	// PLUGINS

	// register plugins
	GetManager()->SetSplashScreenMessage("Registering plugins ...");
	LogDetailedInfo("Registering plugin prototypes ...");

	// main plugins
	GetPluginManager()->RegisterPlugin( new SessionControlPlugin() );
	GetPluginManager()->RegisterPlugin( new ClassifierPlugin() );
	GetPluginManager()->RegisterPlugin( new BackendFileSystemPlugin() );
	GetPluginManager()->RegisterPlugin( new DevicesPlugin() );
	GetPluginManager()->RegisterPlugin( new SpectrumAnalyzerSettingsPlugin() );
	GetPluginManager()->RegisterPlugin( new ParameterControlPlugin() );

	// OpenGL 2D plugins
	GetPluginManager()->RegisterPlugin(new RawWaveformPlugin());
	GetPluginManager()->RegisterPlugin(new FeedbackPlugin());
	GetPluginManager()->RegisterPlugin(new Spectrogram2DPlugin());
	GetPluginManager()->RegisterPlugin(new ViewPlugin());
	GetPluginManager()->RegisterPlugin(new ViewSpectrumPlugin());

	// experience & state machine
	GetPluginManager()->RegisterPlugin( new StateMachinePlugin() );
	GetPluginManager()->RegisterPlugin( new ExperiencePlugin() );
	GetPluginManager()->RegisterPlugin( new ExperienceSelectionPlugin() );

	// others
	GetPluginManager()->RegisterPlugin( new EngineStatusPlugin() );

#ifdef USE_QTDATAVISUALIZATION
	// Qt data visualization plugins
	GetPluginManager()->RegisterPlugin( new SpectrogramSurfacePlugin() );
	GetPluginManager()->RegisterPlugin( new SpectrogramBandsPlugin() );
	GetPluginManager()->RegisterPlugin( new SpectrogramWaterfallPlugin() );
#endif

#ifndef PRODUCTION_BUILD
	// include these plugins only in DEV version
	GetPluginManager()->RegisterPlugin( new NetworkTestClientPlugin() );

	// example plugins (only for development version)
	GetPluginManager()->RegisterPlugin( new ExampleOpenGLPlugin() );
	
		// development plugins
	
		GetPluginManager()->RegisterPlugin( new LatencyTestPlugin() );
		GetPluginManager()->RegisterPlugin( new EEGElectrodePlacementPlugin() );
		GetPluginManager()->RegisterPlugin( new HeatmapPlugin() );
	
	#ifdef OPENCV_SUPPORT
		//GetPluginManager()->RegisterPlugin( new LoretaPlugin() );
	#endif 
	
	#endif
	
	// LAYOUTS
	GetManager()->SetSplashScreenMessage("Initializing layout ...");

	// automatically loads the default layout
	LogDetailedInfo("Searching for available layouts ...");

	// update layouts menu
	User* user = GetUser();

	bool uiCustomization = false;
	if (user != NULL && user->FindRule("STUDIO_SETTING_CustomLayouts") != NULL)
		uiCustomization = true;
#ifdef CORE_DEBUG
	 uiCustomization = true;
#endif
	
	 GetLayoutManager()->EnableCustomizing( uiCustomization );

	//GetLayoutManager()->ReInit(); // EnableCustomizing calls ReInit() internally

	// Select start layout
#ifdef NEUROMORE_BRANDING_ANT
	const bool useClinicianLayout = 
		user->FindRule("ROLE_ClinicClinician") != NULL || 
		user->FindRule("ROLE_ClinicOperator")  != NULL;
	if (useClinicianLayout)
	{
		LogDetailedInfo("Switching to Clinician layout ...");
		GetLayoutManager()->SwitchToLayoutByName("Clinician");
	}
	else
	{
		LogDetailedInfo("Switching to first layout ...");
		GetLayoutManager()->SwitchToLayoutByIndex(0);
	}
#else
	// auto use the first layout when calling the first time
	LogDetailedInfo("Switching to first layout ...");
	GetLayoutManager()->SwitchToLayoutByIndex( 0 );
#endif

	// select user button
	const bool allowSelectUser = (user->FindRule("STUDIO_SETTING_SelectUser") != NULL);
	mSelectSessionUserLabel->setVisible(allowSelectUser);
	mSelectSessionUserButton->setVisible(allowSelectUser);


	// show the main window
	LogInfo("Showing main window ...");
	show();

	// UNCLEAN ?
	// load settings again, for things that affect devices, layouts plugins, etc
	GetManager()->SetSplashScreenMessage("Loading settings ...");
	OnLoadSettings();


	GetManager()->SetSplashScreenMessage("Initializing network components ...");

	// enable network server (server broadcast, realtime update messages)
	LogInfo("Enabling Network Server ...");
	mNetworkServer->SetRealtimeFeedbackEnabled(true);
	mNetworkServer->SetServerAnnounceEnabled(true);

	// cloud storage is governed by rule
	if (user != NULL && user->FindRule("STUDIO_SETTING_CloudStorage") != NULL)
	{
		// start the uploader
		LogDetailedInfo("Starting neuromore Cloud uploader ...");
		GetBackendInterface()->GetUploader()->SetIsEnabled(true);
		GetBackendInterface()->GetUploader()->Start();
	}
	else
	{
		GetBackendInterface()->GetUploader()->SetIsEnabled(false);
	}


	// update windows menu
	mPluginMenu->ReInit();

	// hide the splash screen
	CloseSplashScreen();


	// license agreement window
	License license;
	license.Load(GetAuthenticationCenter()->GetLicenseFilename().AsChar());

	if (license.mAgreedToLicense == false)
	{
		LicenseAgreementWindow licenseAgreementWindow(true, this);
		if (licenseAgreementWindow.exec() == QDialog::Rejected)
		{
			GetQtBaseManager()->ForceTerminateApplication();
		}
		else
		{
			license.mAgreedToLicense = true;
			license.Save( GetAuthenticationCenter()->GetLicenseFilename().AsChar() );
		}
	}

#ifndef PRODUCTION_BUILD
	// production backend while dev version message box (EDIT: This is OK since Open-Source)
	//if (GetBackendInterface()->GetNetworkAccessManager()->GetActiveServerPresetIndex() == 0)
		//QMessageBox::warning(this, "WARNING", "You are using the Production (AWS) backend with a development version of neuromore Studio.\n\nPlease switch back to the Test (AWS) backend." );
#endif
}


// update the application title
void MainWindow::UpdateTitle(const String userName)
{
	LogDetailedInfo("Updating main window title ...");

	// add the application name
	String windowTitle = GetManager()->GetAppName();
	windowTitle += " ";

	// add the user name
	if (userName.IsEmpty() == false)
		windowTitle += "- " + userName + " - ";

#ifdef PRODUCTION_BUILD
	    windowTitle.FormatAdd( "v%s", GetManager()->GetVersion().AsString().AsChar() );
#else
	NetworkAccessManager* networkAccessManager = GetQtBaseManager()->GetBackendInterface()->GetNetworkAccessManager();
	const ServerPreset& serverPreset = networkAccessManager->GetActiveServerPreset();

	// add the AWS server name
	windowTitle.FormatAdd( "(BUILD: %s) - (%s)", CORE_DATE, serverPreset.mName.AsChar() );
#endif

#ifdef CORE_DEBUG
	windowTitle += " - DEBUG VERSION";
#else
	#ifndef PRODUCTION_BUILD
		windowTitle += " - DEVELOPMENT VERSION";
	#endif
#endif

	setWindowTitle( windowTitle.AsChar() );

	LogDetailedInfo("Main window title updated to '%s' ...", windowTitle.AsChar() );
}


void MainWindow::OnBciSelectionChanged(const QString& text)
{
	// get the new active headset based on the index
	int index = mActiveBciCombo->findText( text );
	if (index == -1)
		return;

	Device* device = GetDeviceManager()->GetDevice(index);
	if (device->GetBaseType() != BciDevice::BASE_TYPE_ID)
		return;

	BciDevice* ActiveBci = static_cast<BciDevice*>(device);

	// set the new active headset
	GetEngine()->SetActiveBci( ActiveBci );
}


void MainWindow::ReselectBciDeviceCombo()
{
	// get the active headset
	BciDevice* activeBci = GetEngine()->GetActiveBci();

	// select the active headset
	mActiveBciCombo->blockSignals(true);

	if (activeBci != NULL)
		{
			int index = mActiveBciCombo->findText( activeBci->GetName().AsChar() );
			mActiveBciCombo->setCurrentIndex( index );
		}
		else
			mActiveBciCombo->setCurrentIndex( -1 );

	mActiveBciCombo->blockSignals(false);
}


void MainWindow::ReInitBciDeviceCombo()
{
	// clear the combo box
	mActiveBciCombo->clear();

	DeviceManager* deviceManager = GetDeviceManager();
	const uint32 numDevices = deviceManager->GetNumDevices();
	
	// show box only if there is more than one BCI in the device manager
	uint32 numBCIDevices = 0;
	for (uint32 i=0; i<numDevices; ++i)
		numBCIDevices += (deviceManager->GetDevice(i)->GetBaseType() == BciDevice::BASE_TYPE_ID);

	const bool showBciCombo = (numBCIDevices >= 2);
	
	mActiveBciLabel->setVisible(showBciCombo);
	mActiveBciCombo->setVisible(showBciCombo);

	if (showBciCombo == false)
		return;

	// get the number of devices, iterate through and add them to the combo box
	for (uint32 i=0; i<numDevices; ++i)
	{
		Device*			device		= deviceManager->GetDevice(i);
		
		if (device->GetBaseType() != BciDevice::BASE_TYPE_ID)
			continue;

		BciDevice*		bciDevice	= static_cast<BciDevice*>(device);
		QString			bciName		= bciDevice->GetName().AsChar();

		mActiveBciCombo->addItem( bciName );
	}

	// select the active headset
	ReselectBciDeviceCombo();
}


void MainWindow::CloseSplashScreen()
{
	LogInfo("Closing splash screen ...");
	GetManager()->CloseSplashScreen(this);
	LogDebug("Splash screen closed");
}


void MainWindow::ShowUpgradeWindow(const char* upgradeReason)
{
	UpgradeWindow loginWindow( GetMainWindow(), upgradeReason );
	loginWindow.exec();
}


void MainWindow::SelectSessionUser()
{
	mSessionUserSelectionWindow = new SelectUserWindow(*GetUser(), this, true);
	connect (mSessionUserSelectionWindow, &SelectUserWindow::OnUserSelected, this, &MainWindow::OnSessionUserSelected);
	connect (mSessionUserSelectionWindow, &SelectUserWindow::OnCreateProtocol, this, &MainWindow::OnCreateProtocolForUser);
	connect (mSessionUserSelectionWindow, &SelectUserWindow::close, this, &MainWindow::OnSessionUserSelectionCanceled);

	mSessionUserSelectionWindow->show();
}


void MainWindow::OnSessionUserSelectionCanceled()
{
	// close window
	mSessionUserSelectionWindow->close();
	mSessionUserSelectionWindow->deleteLater();
	mSessionUserSelectionWindow = NULL;
}


void MainWindow::OnSessionUserSelected(const User& user)
{
	GetEngine()->SetSessionUser(user);

	// dealloc window
	mSessionUserSelectionWindow->close();
	mSessionUserSelectionWindow->deleteLater();
	mSessionUserSelectionWindow = NULL;

	// refresh the experience plugin using the selected user
	if (ExperienceSelectionPlugin* p = (ExperienceSelectionPlugin*)GetPluginManager()->FindFirstActivePluginByType(ExperienceSelectionPlugin::GetStaticTypeUuid()))
		p->OnPostAuthenticationInit();

	// refresh the filesystem plugin using the selected user
	if (BackendFileSystemPlugin* p = (BackendFileSystemPlugin*)GetPluginManager()->FindFirstActivePluginByType(BackendFileSystemPlugin::GetStaticTypeUuid()))
		p->OnPostAuthenticationInit();
}

void MainWindow::OnCreateProtocolForUser(const User& user)
{
   GetEngine()->SetSessionUser(user);

   // dealloc window
   mSessionUserSelectionWindow->close();
   mSessionUserSelectionWindow->deleteLater();
   mSessionUserSelectionWindow = NULL;

   // create experience wizard with this user
   mExperienceWizardWindow = new ExperienceWizardWindow(user, this);
   connect(mExperienceWizardWindow, &ExperienceWizardWindow::OnExperienceCreated, this, &MainWindow::OnCreatedProtocolForUser);
   
   mExperienceWizardWindow->show();
}

void MainWindow::OnCreatedProtocolForUser(const User& user)
{
   mExperienceWizardWindow->close();
   mExperienceWizardWindow->deleteLater();
   mExperienceWizardWindow = NULL;

   SelectSessionUser();

   // refresh the experience plugin using the selected user
   if (ExperienceSelectionPlugin* p = (ExperienceSelectionPlugin*)GetPluginManager()->FindFirstActivePluginByType(ExperienceSelectionPlugin::GetStaticTypeUuid()))
      p->OnPostAuthenticationInit();

   // refresh the filesystem plugin using the selected user
   if (BackendFileSystemPlugin* p = (BackendFileSystemPlugin*)GetPluginManager()->FindFirstActivePluginByType(BackendFileSystemPlugin::GetStaticTypeUuid()))
      p->OnPostAuthenticationInit();
}

// reload the style sheet
void MainWindow::OnReloadStyleSheet()
{
	LogDebug("Reloading stylesheet");
	if (QtBaseManager::LoadStyleSheet(GetQtBaseManager()->GetAppDir() + "Styles\\Default.style", this) == false)
		LogError("Failed to load default style sheet");
}


// exit application
void MainWindow::OnExit()
{
	// TODO: put all de-init code in the destructor of the MainWindow class though
	// because when pressing alt f4 or the close cross this function won't be called
	close();
}


// called when we want to check for updates
void MainWindow::OnCheckForUpdates()
{
#ifdef USE_AUTOUPDATE
	if (AutoUpdate::IsUpdateAvailable() == true)
	{
		if (QMessageBox::question(NULL, "Update Available", "Would you like to install the available update? Click yes to install the update or no to go back.", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
		//if (MessageBox( NULL, L"Would you like to install the available update? Click Yes to install the update or no to skip updating this time.", L"Update Available", MB_YESNO | MB_ICONQUESTION|MB_TOPMOST) == IDYES)
		{
			// start the auto updater and quit directly
			close();
			AutoUpdate::StartUpdateTool();

		}
	}
	else
		QMessageBox::information(NULL, "No Updates Available", "There are no updates available.", QMessageBox::Ok);
#endif
}


// show the about dialog
void MainWindow::OnAbout()
{
	if (mAboutWindow == NULL)
		mAboutWindow = new AboutWindow(this);

	mAboutWindow->show();
}


// show the licenses dialog
void MainWindow::OnLicense()
{
	LicenseAgreementWindow licenseAgreementWindow(false, this);
	licenseAgreementWindow.exec();
}


// show the website
void MainWindow::OnVisitWebsite()
{
	QUrl url( GetManager()->GetWebsite() );
	QDesktopServices::openUrl(url);
}


// show the helpcenter
void MainWindow::OnVisitAccount()
{
	QUrl url( GetManager()->GetAccountUrl() );
	QDesktopServices::openUrl(url);
}


// show the docs
void MainWindow::OnVisitDocumentation()
{
	QUrl url( GetManager()->GetDocumentationUrl() );
	QDesktopServices::openUrl(url);
}


// send email to support team using outlook e.g.
void MainWindow::OnVisitSupport()
{
	User* user = GetUser();
	
	// construct link
	String link;
	link.Format("mailto:%s?subject=Support Request: %s (%s)&body=Please let us know how we can help you.", GetManager()->GetSupportEMail(), GetManager()->GetAppName().AsChar(), user->GetId() );

	QUrl url( link.AsChar() );
	QDesktopServices::openUrl(url);
}


// user wants to close the window: ask to save dirty files
void MainWindow::closeEvent(QCloseEvent* event)
{
	String message;

	// session is running
	if (GetSession()->IsRunning() == true)
	{
		message = "A session is running. Do you really want to terminate it and close the program?";
		QMessageBox::StandardButton result = QMessageBox::warning( this, "Session is running", message.AsChar(), QMessageBox::Yes | QMessageBox::No);

		if (result == QMessageBox::Yes)
		{
			// no need to do anything, just exit and let the engine die
		}
		else // stop exiting
		{
			event->ignore();
			return;
		}
	}

	// NOTE this code is similar to that in CloseFile, but we can't save the files during shutdown, so it has to be a separate codepath

	// check if active classifier is dirty
	Classifier* activeClassifier = GetEngine()->GetActiveClassifier();
	if (activeClassifier != NULL && activeClassifier->IsDirty() && activeClassifier->GetCreud().Update() == true)
	{
		message.Format( "The classifier \"%s\" was modified. Discard changes and exit?", activeClassifier->GetName());								//Do you want to save the changes?

		QMessageBox::StandardButton result = QMessageBox::warning( this, "Unsaved Changes", message.AsChar(), QMessageBox::Discard | QMessageBox::Cancel ); // QMessageBox::Save | 
		switch (result)
		{
			// FIXME save doesn't work because we need to wait until the REST call is through.. don't know how to do that here
			//case QMessageBox::Save:		GetBackendInterface()->GetFileSystem()->SaveGraph( GetUser()->GetToken(), activeClassifier->GetUuid(), activeClassifier );	break;
			case QMessageBox::Discard:	break;
			case QMessageBox::Cancel: default: event->ignore(); return;
		}
	}
	
	// check if active statemachine is dirty
	StateMachine* activeStatemachine = GetEngine()->GetActiveStateMachine();
	if (activeStatemachine != NULL && activeStatemachine->IsDirty() && activeStatemachine->GetCreud().Update() == true)
	{
		message.Format( "The state machine \"%s\" was modified. Discard changes and exit?", activeStatemachine->GetName());									//Do you want to save the changes?

		QMessageBox::StandardButton result = QMessageBox::warning( this, "Unsaved Changes", message.AsChar(),  QMessageBox::Discard | QMessageBox::Cancel ); // QMessageBox::Save  |
		switch (result)
		{
			// FIXME save doesn't work because we need to wait until the REST call is through.. don't know how to do that here
			//case QMessageBox::Save:		GetBackendInterface()->GetFileSystem()->SaveGraph( GetUser()->GetToken(), activeStatemachine->GetUuid(), activeStatemachine );	break;
			case QMessageBox::Discard:	break;
			case QMessageBox::Cancel: default: event->ignore();	return;
		}
	}

	// check if active experience is dirty
	Experience* experience = GetEngine()->GetActiveExperience();
	if (experience != NULL && experience->IsDirty() && experience->GetCreud().Update() == true)
	{
		message.Format( "The design \"%s\" was modified. Discard changes and exit?", experience->GetName());											//Do you want to save the changes?

		QMessageBox::StandardButton result = QMessageBox::warning( this, "Unsaved Changes", message.AsChar(),  QMessageBox::Discard | QMessageBox::Cancel ); // QMessageBox::Save  |
		switch (result)
		{
			// FIXME save doesn't work because we need to wait until the REST call is through.. don't know how to do that here
			//case QMessageBox::Save:		GetBackendInterface()->GetFileSystem()->SaveGraph( GetUser()->GetToken(), activeStatemachine->GetUuid(), activeStatemachine );	break;
			case QMessageBox::Discard:	break;
			case QMessageBox::Cancel: default: event->ignore();	return;
		}
	}

	// accept event and let program close
	event->accept();
}


// key pressed
void MainWindow::keyPressEvent(QKeyEvent* event)
{
	// layout switching
	for (uint32 i=0; i<10; ++i)
	{
		// construct layout event name
		//mTempString.Format( "Layout #%i", i + 1 );

		if (event->key() == (Qt::Key_F1 + i) == true)
		{
			// don't switch during session
			if (GetSession()->IsRunning() == true)
			{
				event->ignore();
				return;
			}

			// switch to layout
			if (i < GetLayoutManager()->GetNumLayouts())
				GetLayoutManager()->SwitchToLayoutByIndex( i );

			event->accept();
			return;
		}
	}

	QMainWindow::keyPressEvent(event);
}


// key released
void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
	// layout switching
	for (uint32 i=0; i<10; ++i)
	{
		//mTempString.Format( "Layout #%i", i + 1 );

		if (event->key() == (Qt::Key_F1 + i) == true)
		{
			event->accept();
			return;
		}
	}

	QMainWindow::keyReleaseEvent(event);
}


// find the menu action for a given plugin
QAction* MainWindow::FindAction(QList<QAction*>& actionList, Plugin* plugin)
{
	QString pluginTypeUuid = plugin->GetTypeUuid();

	const uint32 numActions = actionList.count();
	for (uint32 i=0; i<numActions; ++i)
	{
		if (actionList.at(i)->data().toString().compare( pluginTypeUuid ) == 0)
			return actionList.at(i);
	}

	return NULL;
}


//
// EVENTS
//

// lock parts of the UI on session start
void MainWindow::OnStartSession()
{
	mMenuBar->setEnabled(false);

	// experiment: lock layout,
	GetPluginManager()->LockActivePlugins(true);

	// disable other ui elements
	mActiveBciCombo->setEnabled(false);
	mLayoutComboBox->setEnabled(false);
	mSelectSessionUserButton->setEnabled(false);
}


void MainWindow::OnStopSession()
{
	mMenuBar->setEnabled(true);

	// experiment: unlock layout
	//if (GetLayoutManager()->IsCustomizingEnabled() == true)
	GetPluginManager()->LockActivePlugins(false);

	// enable other ui elements again
	mActiveBciCombo->setEnabled(true);
	mLayoutComboBox->setEnabled(true);
	mSelectSessionUserButton->setEnabled(true);
}

void MainWindow::OnActiveExperienceChanged(Experience* experience)
{
	const bool hasExperience = (experience != NULL);
	const bool hasWriteableExperience = hasExperience && experience->GetCreud().Update();

	// actions that are only shown if a design is loaded (and writeable in some cases)
	mCloseAction->setVisible(hasExperience);
	mSaveAction->setVisible(hasWriteableExperience);

	// show design menu only if its writeable
	mDesignMenu->menuAction()->setVisible(hasWriteableExperience);

	// reuse the enable-settings code
	if (hasExperience == true)
	{
		if (experience->HasSettings())
			OnEnableSettingsMode();
		else
			OnDisableSettingsMode();
	}
}


//
// SETTINGS
//

// show the settings window
void MainWindow::OnSettings()
{
	if (mSettingsWindow == NULL)
	{
		mSettingsWindow = new SettingsWindow(this);
		mSettingsWindow->Init();

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// General category

		const char* categoryName = "General";
		PropertyTreeWidget* generalPropertyWidget = mSettingsWindow->FindPropertyWidgetByName(categoryName);
		if (generalPropertyWidget == NULL)
			generalPropertyWidget = mSettingsWindow->AddCategory(categoryName, "Images/Icons/Gear.png", false);

		connect( generalPropertyWidget->GetPropertyManager(), &PropertyManager::ValueChanged, this, &MainWindow::OnValueChanged );

		mShowFPSProperty				= generalPropertyWidget->GetPropertyManager()->AddBoolProperty("", "Show FPS", GetShowFPS(), false);
		mShowPerformanceInfoProperty	= generalPropertyWidget->GetPropertyManager()->AddBoolProperty("", "Show Performance Info", GetShowPerformanceInfo(), false);

		// log level
		Array<String> logLevelComboValues;
		const uint32 numLogPresets = CORE_LOGMANAGER.GetNumLogPresets();
		for (uint32 i=0; i<numLogPresets; ++i)
			logLevelComboValues.Add( CORE_LOGMANAGER.GetLogPreset(i)->GetName() );
		mLogLevelProperty = generalPropertyWidget->GetPropertyManager()->AddComboBoxProperty("", "Log Level", logLevelComboValues, CORE_LOGMANAGER.GetActiveLogPresetIndex(), false);
		
		// auto sync
		mAutoSyncProperty = generalPropertyWidget->GetPropertyManager()->AddBoolProperty("", "Sync Engine Automatically", GetEngine()->GetAutoSyncSetting(), true);

		// performance
		mEngineUpdateRateProperty = generalPropertyWidget->GetPropertyManager()->AddFloatSpinnerProperty("Performance", "Engine Update Rate (Hz)", GetEngineUpdateRate(), GetEngineUpdateRate(), FLT_MIN, FLT_MAX);
		mInterfaceUpdateRateProperty = generalPropertyWidget->GetPropertyManager()->AddFloatSpinnerProperty("Performance", "Interface Update Rate (Hz)", GetInterfaceUpdateRate(), GetInterfaceUpdateRate(), FLT_MIN, FLT_MAX);
		mRealtimeInterfaceUpdateRateProperty = generalPropertyWidget->GetPropertyManager()->AddFloatSpinnerProperty("Performance", "Realtime Interface Update Rate (Hz)", GetRealtimeUIUpdateRate(), GetRealtimeUIUpdateRate(), FLT_MIN, FLT_MAX);
	
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Devices category

		categoryName = "Devices";
		PropertyTreeWidget* devicePropertyWidget = mSettingsWindow->FindPropertyWidgetByName(categoryName);
		if (devicePropertyWidget == NULL)
			devicePropertyWidget = mSettingsWindow->AddCategory(categoryName, "Images/Icons/Devices.png", false);

		connect(devicePropertyWidget->GetPropertyManager(), &PropertyManager::ValueChanged, this, &MainWindow::OnValueChanged);

		// global device autodetection
		mAutoDetectionProperty = devicePropertyWidget->GetPropertyManager()->AddBoolProperty("", "Automatic Device Search", GetEngine()->GetAutoDetectionSetting(), true);

		// power line frequency
		Array<String> powerLineFreqComboValues;
		powerLineFreqComboValues.Add("Auto");
		powerLineFreqComboValues.Add("50 Hz");
		powerLineFreqComboValues.Add("60 Hz");
		mPowerLineFrequencyTypeProperty = devicePropertyWidget->GetPropertyManager()->AddComboBoxProperty("", "Local Power Line Frequency", powerLineFreqComboValues, EngineManager::POWERLINEFREQ_AUTO, false);

		// drift correction
		mDriftCorrectionEnableProperty = devicePropertyWidget->GetPropertyManager()->AddBoolProperty("Drift Correction", "Enable Drift Correction", GetEngine()->GetDriftCorrectionSettings().mIsEnabled, true);
		mDriftCorrectionMaxSyncDriftProperty = devicePropertyWidget->GetPropertyManager()->AddFloatSpinnerProperty("Drift Correction", "Max Drift in Seconds", GetEngine()->GetDriftCorrectionSettings().mMaxDriftUntilSync, 2, 0, FLT_MAX);
		mDriftCorrectionMaxForwardDriftProperty = devicePropertyWidget->GetPropertyManager()->AddFloatSpinnerProperty("Drift Correction", "Drift Tolerance (Early Signal) in Seconds", GetEngine()->GetDriftCorrectionSettings().mMaxForwardDrift, 0.1f, 0.0f, FLT_MAX);
		mDriftCorrectionMaxBackwardDriftProperty = devicePropertyWidget->GetPropertyManager()->AddFloatSpinnerProperty("Drift Correction", "Drift Tolerance (Late Signal) in Seconds", GetEngine()->GetDriftCorrectionSettings().mMaxBackwardDrift, 0.5f, 0.0f, FLT_MAX);

		// device driver global enable settings (per device)
		const uint32 numDeviceSystems = GetDeviceManager()->GetNumDeviceDrivers();
		for (uint32 i = 0; i < numDeviceSystems; ++i)
		{
			DeviceDriver* driver = GetDeviceManager()->GetDeviceDriver(i);
			mTempString.Format("Enable %s", driver->GetName());

			Property* property = devicePropertyWidget->GetPropertyManager()->AddBoolProperty("Drivers", mTempString.AsChar(), driver->IsEnabled(), true, false);
			mDeviceAutoDetectionProperties.Add(property);
		}

#ifdef INCLUDE_DEVICE_GENERIC_AUDIO
		// Audio Settings
		DeviceDriver* audioDriverBase = GetDeviceManager()->FindDeviceDriverByType(AudioDriver::TYPE_ID);
		if (audioDriverBase != NULL)
		{
			AudioDriver* audioDriver = static_cast<AudioDriver*>(audioDriverBase);
			mAudioInputUpdateRateProperty = devicePropertyWidget->GetPropertyManager()->AddFloatSpinnerProperty("Audio", "Input Update Rate (Hz)", audioDriver->GetInputUpdateRate(), audioDriver->GetInputUpdateRate(), FLT_MIN, FLT_MAX);
			mAudioOutputUpdateRateProperty = devicePropertyWidget->GetPropertyManager()->AddFloatSpinnerProperty("Audio", "Output Update Rate (Hz)", audioDriver->GetOutputUpdateRate(), audioDriver->GetOutputUpdateRate(), FLT_MIN, FLT_MAX);
			mAudioOutputBufferSizeProperty = devicePropertyWidget->GetPropertyManager()->AddFloatSpinnerProperty("Audio", "Output Buffer Size (ms)", audioDriver->GetOutputBufferSize(), audioDriver->GetOutputBufferSize(), FLT_MIN, FLT_MAX);
			mAudioOutputDelayProperty = devicePropertyWidget->GetPropertyManager()->AddFloatSpinnerProperty("Audio", "Output Delay (ms)", audioDriver->GetOutputDelay(), audioDriver->GetOutputDelay(), FLT_MIN, FLT_MAX);
		}
#endif

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// neuromore Cloud category

		//NOTE: production build doesn't have anything in the cloud category right now!

#ifndef PRODUCTION_BUILD
		categoryName = "Cloud";
		PropertyTreeWidget* cloudPropertyWidget = mSettingsWindow->FindPropertyWidgetByName(categoryName);
		if (cloudPropertyWidget == NULL)
			cloudPropertyWidget = mSettingsWindow->AddCategory(categoryName, "Images/Icons/Cloud.png", false);

		connect( cloudPropertyWidget->GetPropertyManager(), &PropertyManager::ValueChanged, this, &MainWindow::OnValueChanged);

		// get the server preset names for the combo box
		Array<String> serverPresetNames;
		const uint32 numServerPresets = GetBackendInterface()->GetNetworkAccessManager()->GetNumPresets();
		for (uint32 i=0; i<numServerPresets; ++i)
			serverPresetNames.Add( GetBackendInterface()->GetNetworkAccessManager()->GetPreset(i).mName );

		mServerPresetProperty				= cloudPropertyWidget->GetPropertyManager()->AddComboBoxProperty( "", "Server", serverPresetNames, GetBackendInterface()->GetNetworkAccessManager()->GetActiveServerPresetIndex() );
		mLogBackendProperty					= cloudPropertyWidget->GetPropertyManager()->AddBoolProperty("", "Backend (REST) Communication Logging", GetBackendInterface()->GetNetworkAccessManager()->IsLogEnabled() );
#endif

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Network category

		categoryName = "Network";
		PropertyTreeWidget* networkPropertyWidget = mSettingsWindow->FindPropertyWidgetByName(categoryName);
		if (networkPropertyWidget == NULL)
			networkPropertyWidget = mSettingsWindow->AddCategory(categoryName, "Images/Icons/Network.png", false);
			
		connect(networkPropertyWidget->GetPropertyManager(), &PropertyManager::ValueChanged, this, &MainWindow::OnValueChanged);
	
		mNetworkServerEnableTCPProperty = networkPropertyWidget->GetPropertyManager()->AddBoolProperty("Studio Server", "Enable TCP server", GetNetworkServer()->GetListenerEnabled());
		mNetworkServerEnableBroadcastProperty = networkPropertyWidget->GetPropertyManager()->AddBoolProperty("Studio Server", "Broadcast server announce", GetNetworkServer()->GetBroadcastServerAnnounce());
		
		const int32 announceInterval = GetNetworkServer()->GetAnnounceInterval();
		mNetworkServerAnnounceIntervalProperty = networkPropertyWidget->GetPropertyManager()->AddIntProperty("Studio Server", "Announce interval in seconds", announceInterval, announceInterval, 1, CORE_INT32_MAX);
		
		const double clientTimeout = GetNetworkServer()->GetClientTimeout();
		mNetworkServerClientTimeoutProperty = networkPropertyWidget->GetPropertyManager()->AddFloatSpinnerProperty("Studio Server", "Client timeout in seconds", clientTimeout, clientTimeout, 0.5, FLT_MAX);
		
		const int32 oscInputPort = GetOscServer()->GetListenPort();
		const int32 maxPort = (1 << 16) - 1; // highest allowed port number
		mOSCInputPortProperty = networkPropertyWidget->GetPropertyManager()->AddIntProperty("OSC Server", "Input UDP port", oscInputPort, oscInputPort, 0, maxPort);
		
		String oscLocalEndpoint = FromQtString(GetOscServer()->GetLocalEndpoint().toString());
		mOSCLocalEndpointProperty = networkPropertyWidget->GetPropertyManager()->AddStringProperty("OSC Server", "Local Endpoint", oscLocalEndpoint.AsChar(), "0.0.0.0");

		String oscReceivingHost = FromQtString(GetOscServer()->GetRemoteHost().toString());
		mOSCRemoteHostProperty = networkPropertyWidget->GetPropertyManager()->AddStringProperty("OSC Server", "Target Host/IP", oscReceivingHost.AsChar(), "127.0.0.1");
		
		const int32 oscOutputPort = GetOscServer()->GetRemoteUdpPort();
		mOSCRemotePortProperty = networkPropertyWidget->GetPropertyManager()->AddIntProperty("OSC Server", "Target UDP port", oscOutputPort, oscOutputPort, 0, maxPort);
		
		// add all categories from the plugins
		mSettingsWindow->AddCategoriesFromPlugin(NULL);
	}

	mSettingsWindow->show();
}


// called when any of the values from the settings window changed
void MainWindow::OnValueChanged(Property* property)
{
	// set the fps flag
	if (property == mShowFPSProperty)
		SetShowFPS(property->AsBool());

	// set the performance info flag
	if (property == mShowPerformanceInfoProperty)
		SetShowPerformanceInfo(property->AsBool());

	// set the log level
	if (property == mLogLevelProperty)
		CORE_LOGMANAGER.SetActiveLogLevelPresetIndex( property->AsInt() );

	// engine auto sync
	if (property == mAutoSyncProperty)
		GetEngine()->SetAutoSyncSetting(property->AsBool());

	// performance
	if (property == mEngineUpdateRateProperty)
		SetEngineUpdateRate(property->AsFloat());
	if (property == mInterfaceUpdateRateProperty)
		SetInterfaceUpdateRate(property->AsFloat());
	if (property == mRealtimeInterfaceUpdateRateProperty)
		SetRealtimeUIUpdateRate(property->AsFloat());
	
	// global device autodetection
	if (property == mAutoDetectionProperty)
		GetEngine()->SetAutoDetectionSetting(property->AsBool());

	// set the power line frequency setting
	if (property == mPowerLineFrequencyTypeProperty)
		GetEngine()->SetPowerLineFrequencyType((EngineManager::EPowerLineFrequencyType)property->AsInt());

	// drift correction 
	if (property == mDriftCorrectionEnableProperty)
		GetEngine()->GetDriftCorrectionSettings().mIsEnabled = property->AsBool();
	
	if (property == mDriftCorrectionMaxSyncDriftProperty)
		GetEngine()->GetDriftCorrectionSettings().mMaxDriftUntilSync = property->AsFloat();
	
	if (property == mDriftCorrectionMaxForwardDriftProperty)
		GetEngine()->GetDriftCorrectionSettings().mMaxForwardDrift = property->AsFloat();
	
	if (property == mDriftCorrectionMaxBackwardDriftProperty)
		GetEngine()->GetDriftCorrectionSettings().mMaxBackwardDrift = property->AsFloat();

	// enable/disable device drivers
	const uint32 numDeviceAutoDetectionProperties = mDeviceAutoDetectionProperties.Size();
	CORE_ASSERT(numDeviceAutoDetectionProperties == GetDeviceManager()->GetNumDeviceDrivers());
	for (uint32 i = 0; i < numDeviceAutoDetectionProperties; ++i)
		if (property == mDeviceAutoDetectionProperties[i])
			GetDeviceManager()->GetDeviceDriver(i)->SetEnabled(property->AsBool());

#ifdef INCLUDE_DEVICE_GENERIC_AUDIO
	// Audio Settings
	DeviceDriver* audioDriverBase = GetDeviceManager()->FindDeviceDriverByType(AudioDriver::TYPE_ID);
	if (audioDriverBase != NULL)
	{
		AudioDriver* audioDriver = static_cast<AudioDriver*>(audioDriverBase);
		if (property == mAudioInputUpdateRateProperty)
			audioDriver->SetInputUpdateRate(property->AsFloat());
		if (property == mAudioOutputUpdateRateProperty)
			audioDriver->SetOutputUpdateRate(property->AsFloat());
		if (property == mAudioOutputBufferSizeProperty)
			audioDriver->SetOutputBufferSize(property->AsFloat());
		if (property == mAudioOutputDelayProperty)
			audioDriver->SetOutputDelay(property->AsFloat());
	}
#endif

#ifndef PRODUCTION_BUILD
	// neuromore Cloud settings
	bool serverPresetChanged = false;
	if (property == mServerPresetProperty)
	{
		uint32 oldServerPresetIndex = GetBackendInterface()->GetNetworkAccessManager()->GetActiveServerPresetIndex();
		uint32 newServerPresetIndex = property->AsInt();

		if (oldServerPresetIndex != newServerPresetIndex)
		{
			GetBackendInterface()->GetNetworkAccessManager()->SetActiveServerPresetIndex( newServerPresetIndex );
			serverPresetChanged = true;
		}
	}

	if (property == mLogBackendProperty)
		GetBackendInterface()->GetNetworkAccessManager()->SetLoggingEnabled( property->AsBool() );
#endif

	// Network server
	if (property == mNetworkServerEnableTCPProperty)
		GetNetworkServer()->SetListenerEnabled(property->AsBool());
	
	if (property == mNetworkServerEnableBroadcastProperty)
		GetNetworkServer()->SetBroadcastServerAnnounce(property->AsBool());

	if (property == mNetworkServerAnnounceIntervalProperty)
		GetNetworkServer()->SetAnnounceInterval(property->AsInt());

	if (property == mNetworkServerClientTimeoutProperty)
		GetNetworkServer()->SetClientTimeout(property->AsFloat());

	// reinit on some of the changes
	if (property == mNetworkServerEnableTCPProperty || property == mNetworkServerEnableBroadcastProperty || property == mNetworkServerAnnounceIntervalProperty)
		GetNetworkServer()->ReInit();


	// OSC server
	bool inputPortOverlap = false;
	if (property == mOSCInputPortProperty)
	{
		const int32 port = property->AsInt();

		// overlaps with our visualization ports
		if (port == STUDIO_NETWORKSERVER_TCP_PORT || port == STUDIO_NETWORKSERVER_UDP_PORT)
		{
			// error messages
			QMessageBox::warning(this, "Cannot update setting", "OSC input port is already in use, please choose another one.", QMessageBox::Ok);
			mOSCInputPortProperty->SetValue((int32)GetOscServer()->GetListenPort());
			inputPortOverlap = true; // prevents ReInit of OSC server
		}
		else
			GetOscServer()->SetListenPort(port);
	}

	if (property == mOSCLocalEndpointProperty)
		GetOscServer()->SetLocalEndpoint(QHostAddress(property->AsString().AsChar()));
		
	if (property == mOSCRemoteHostProperty) 
		GetOscServer()->SetRemoteHost(QHostAddress(property->AsString().AsChar()));
		
	if (property == mOSCRemotePortProperty)
		GetOscServer()->SetRemoteUdpPort(property->AsInt());

	// reinit if one setting changed
	if ((property == mOSCInputPortProperty && !inputPortOverlap) ||
		property == mOSCRemoteHostProperty ||
		property == mOSCRemotePortProperty)
	{
		GetOscServer()->ReInit();
	}

	// update and save the settings directly
	OnSaveSettings();

	

#ifndef PRODUCTION_BUILD
	// backend server change -> restart studio
	if (serverPresetChanged == true)
	{
		String message;
		message.Format( "The application will restart now.", GetBackendInterface()->GetNetworkAccessManager()->GetActiveServerPreset().mName.AsChar() );
		
		QMessageBox::information(this, "Backend Server Changed", message.AsChar(), QMessageBox::Ok);

		GetQtBaseManager()->ForceTerminateApplication();
		
		QProcess::startDetached(QCoreApplication::applicationFilePath());

	}
#endif
}


// construct the filename for the settings file
QString MainWindow::GetSettingsFilename() const
{
	QString result = QtBaseManager::GetAppDataFolder().AsChar();
	result += GetManager()->GetAppShortName();
	result += "_settings.cfg";
	return result;
}


// load settings
void MainWindow::OnLoadSettings()
{
	LogDetailedInfo("Loading application settings ...");

	QSettings settings( GetSettingsFilename(), QSettings::IniFormat, this );

	// general category

	// show fps
	bool showFPS = settings.value("showFPS", false).toBool();
	SetShowFPS(showFPS);

	// show performance info
	bool showPerformanceInfo = settings.value("showPerformanceInfo", false).toBool();
	SetShowPerformanceInfo(showPerformanceInfo);

	// log level preset
	const LogLevelPreset* logPreset = CORE_LOGMANAGER.GetActiveLogLevelPreset();
	CORE_LOGMANAGER.SetActiveLogLevelPreset( FromQtString(settings.value("logLevelPreset", logPreset->GetName()).toString()).AsChar() );

	// auto sync
	const bool useAutoSync = settings.value("engineAutoSync", GetEngine()->GetAutoSyncSetting()).toBool();
	GetEngine()->SetAutoSyncSetting(useAutoSync);

	// performance
	const float engineUpdateRate = settings.value("engineUpdateRate", GetEngineUpdateRate()).toFloat();
	SetEngineUpdateRate(engineUpdateRate);
	const float interfaceUpdateRate = settings.value("interfaceUpdateRate", GetInterfaceUpdateRate()).toFloat();
	SetInterfaceUpdateRate(interfaceUpdateRate);
	const float realtimeInterfaceUpdateRate = settings.value("realtimeInterfaceUpdateRate", GetRealtimeUIUpdateRate()).toFloat();
	SetRealtimeUIUpdateRate(realtimeInterfaceUpdateRate);

	// device detection
#ifdef NEUROMORE_BRANDING_ANT
	const bool default_autodetection = true;
#else
	const bool default_autodetection = GetEngine()->GetAutoDetectionSetting();
#endif
	const bool enableAutoDetection = settings.value("deviceAutoDetectionEnabled", default_autodetection).toBool();
	GetEngine()->SetAutoDetectionSetting(enableAutoDetection);

	// power line frequency
	const int32 powerLineFrequencyType = settings.value("powerLineFrequencyType", GetEngine()->GetPowerLineFrequencyType()).toInt();
	GetEngine()->SetPowerLineFrequencyType((EngineManager::EPowerLineFrequencyType)powerLineFrequencyType);

	// drift settings
	EngineManager::DriftCorrectionSettings& driftSettings = GetEngine()->GetDriftCorrectionSettings();
	driftSettings.mIsEnabled				= settings.value("driftCorrectionEnabled",				driftSettings.mIsEnabled).toBool();
	driftSettings.mMaxDriftUntilSync		= settings.value("driftCorrectionMaxDriftUntilSync",	driftSettings.mMaxDriftUntilSync).toFloat();
	driftSettings.mMaxForwardDrift			= settings.value("driftCorrectionMaxForwardDrift",		driftSettings.mMaxForwardDrift).toFloat();
	driftSettings.mMaxBackwardDrift			= settings.value("driftCorrectionMaxBackwardDrift",		driftSettings.mMaxBackwardDrift).toFloat();
	
	// enable/disable device system
	const uint32 numDevices = GetDeviceManager()->GetNumDeviceDrivers();
	for (uint32 i = 0; i < numDevices; ++i)
	{
		DeviceDriver* driver = GetDeviceManager()->GetDeviceDriver(i);
		// construct settings name using device driver name
		mTempString.Format("deviceDriver%sAutoDetectionEnabled", driver->GetName());
		mTempString.RemoveChars(" ");	// remove spacings from device driver name
		driver->SetEnabled(settings.value(mTempString.AsChar(), driver->IsEnabled()).toBool());
	}


#ifdef INCLUDE_DEVICE_GENERIC_AUDIO
	// Audio Settings
	DeviceDriver* audioDriverBase = GetDeviceManager()->FindDeviceDriverByType(AudioDriver::TYPE_ID);
	if (audioDriverBase != NULL)
	{
		AudioDriver* audioDriver = static_cast<AudioDriver*>(audioDriverBase);
		audioDriver->SetInputUpdateRate (settings.value("audioInputUpdateRate", audioDriver->GetInputUpdateRate()).toFloat());
		audioDriver->SetOutputUpdateRate(settings.value("audioOutputUpdateRate", audioDriver->GetOutputUpdateRate()).toFloat());
		audioDriver->SetOutputBufferSize(settings.value("audioOutputBufferSize", audioDriver->GetOutputBufferSize()).toFloat());
		audioDriver->SetOutputDelay		(settings.value("audioOutputDelay", audioDriver->GetOutputDelay()).toFloat());
	}
#endif

	// used backend
#ifdef NEUROMORE_BRANDING_ANT
	const int defaultPresetIndex = 1;
#else
	const int defaultPresetIndex = 0;
#endif
	int32 cloudServerPreset = settings.value("cloudServerPreset", defaultPresetIndex).toInt();
	GetBackendInterface()->GetNetworkAccessManager()->SetActiveServerPresetIndex(cloudServerPreset);

	bool backendLoggingEnabled = settings.value( "cloudLoggingEnabled", false ).toBool();
	GetBackendInterface()->GetNetworkAccessManager()->SetLoggingEnabled(backendLoggingEnabled);

#ifdef FORCE_DEBUGLOGGING
	GetBackendInterface()->GetNetworkAccessManager()->SetLoggingEnabled(true);
	CORE_LOGMANAGER.SetActiveLogLevelPreset( "Debug" );
#endif

	// network category
	bool networkEnableTCPServer = settings.value("networkEnableTCPServer", GetNetworkServer()->GetListenerEnabled()).toBool();
	GetNetworkServer()->SetListenerEnabled(networkEnableTCPServer); 

	bool networkBroadcastAnnounce = settings.value("networkBroadcastAnnounce", GetNetworkServer()->GetBroadcastServerAnnounce()).toBool();
	GetNetworkServer()->SetBroadcastServerAnnounce(networkBroadcastAnnounce);

	int32 networkAnnounceInterval = settings.value("networkAnnounceInterval", GetNetworkServer()->GetAnnounceInterval()).toInt();
	GetNetworkServer()->SetAnnounceInterval(networkAnnounceInterval);

	double networkClientTimeout = settings.value("networkClientTimeout", GetNetworkServer()->GetClientTimeout()).toFloat();
	GetNetworkServer()->SetClientTimeout(networkClientTimeout);

	int32 networkOscInputPort = settings.value("networkOscInputPort", GetOscServer()->GetListenPort()).toInt();
	// in case settings overlap with the main network server, set it back to the default
	if (networkOscInputPort == STUDIO_NETWORKSERVER_UDP_PORT || networkOscInputPort == STUDIO_NETWORKSERVER_TCP_PORT)
		networkOscInputPort = STUDIO_OSCLISTENER_UDP_PORT;
	GetOscServer()->SetListenPort(networkOscInputPort);

	QString networkOscLocalEndpoint = settings.value("networkOscLocalEndpoint", FromQtString(GetOscServer()->GetLocalEndpoint().toString()).AsChar()).toString();
	GetOscServer()->SetLocalEndpoint(QHostAddress(networkOscLocalEndpoint));

	QString networkOscRemoteHost = settings.value("networkOscRemoteHost", FromQtString(GetOscServer()->GetRemoteHost().toString()).AsChar()).toString();
	GetOscServer()->SetRemoteHost(QHostAddress(networkOscRemoteHost));

	int32 networkOscRemotePort = settings.value("networkOscRemotePort_v2", GetOscServer()->GetRemoteUdpPort()).toInt();
	GetOscServer()->SetRemoteUdpPort(networkOscRemotePort);

}


// save settings
void MainWindow::OnSaveSettings()
{
	LogDetailedInfo("Saving application settings ...");

	QSettings settings( GetSettingsFilename(), QSettings::IniFormat, this );

	// general category
	settings.setValue("showFPS", GetShowFPS());
	settings.setValue("showPerformanceInfo", GetShowPerformanceInfo());

	// auto sync
	settings.setValue("engineAutoSync", GetEngine()->GetAutoSyncSetting());
	
	// performance
	settings.setValue("engineUpdateRate", GetEngineUpdateRate());
	settings.setValue("interfaceUpdateRate", GetInterfaceUpdateRate());
	settings.setValue("realtimeInterfaceUpdateRate", GetRealtimeUIUpdateRate());

	// device auto detection settings
	settings.setValue("deviceAutoDetectionEnabled", GetEngine()->GetAutoDetectionSetting());

	// pwerline frequency
	settings.setValue("powerLineFrequencyType", GetEngine()->GetPowerLineFrequencyType());

	// drift settings
	EngineManager::DriftCorrectionSettings& driftSettings = GetEngine()->GetDriftCorrectionSettings();
	settings.setValue("driftCorrectionEnabled",				driftSettings.mIsEnabled);
	settings.setValue("driftCorrectionMaxDriftUntilSync",	driftSettings.mMaxDriftUntilSync);
	settings.setValue("driftCorrectionMaxForwardDrift",		driftSettings.mMaxForwardDrift);
	settings.setValue("driftCorrectionMaxBackwardDrift",	driftSettings.mMaxBackwardDrift);

	const uint32 numDeviceAutoDetectionProperties = GetDeviceManager()->GetNumDeviceDrivers();
	for (uint32 i = 0; i < numDeviceAutoDetectionProperties; ++i)
	{
		DeviceDriver* driver = GetDeviceManager()->GetDeviceDriver(i);

		// construct settings name using device driver name
		mTempString.Format("deviceDriver%sAutoDetectionEnabled", driver->GetName());
		mTempString.RemoveChars(" ");	// remove spacings from device driver name
		settings.setValue(mTempString.AsChar(), driver->IsEnabled());
	}

	// Audio Settings
#ifdef INCLUDE_DEVICE_GENERIC_AUDIO
	DeviceDriver* audioDriverBase = GetDeviceManager()->FindDeviceDriverByType(AudioDriver::TYPE_ID);
	if (audioDriverBase != NULL)
	{
		AudioDriver* audioDriver = static_cast<AudioDriver*>(audioDriverBase);
		settings.setValue("audioInputUpdateRate", audioDriver->GetInputUpdateRate());
		settings.setValue("audioOutputUpdateRate", audioDriver->GetOutputUpdateRate());
		settings.setValue("audioOutputBufferSize", audioDriver->GetOutputBufferSize());
		settings.setValue("audioOutputDelay", audioDriver->GetOutputDelay());
	}
#endif

	// log level preset
	const LogLevelPreset* logPreset = CORE_LOGMANAGER.GetActiveLogLevelPreset();
	settings.setValue("logLevelPreset", logPreset->GetName());

	// neuromore Cloud category
#ifndef PRODUCTION_BUILD
	settings.setValue("cloudServerPreset", GetBackendInterface()->GetNetworkAccessManager()->GetActiveServerPresetIndex());
	settings.setValue("cloudLoggingEnabled", GetBackendInterface()->GetNetworkAccessManager()->IsLogEnabled());
#endif

	// network category
	settings.setValue("networkEnableTCPServer", GetNetworkServer()->GetListenerEnabled());
	settings.setValue("networkBroadcastAnnounce", GetNetworkServer()->GetBroadcastServerAnnounce());
	settings.setValue("networkAnnounceInterval", GetNetworkServer()->GetAnnounceInterval());
	settings.setValue("networkClientTimeout", GetNetworkServer()->GetClientTimeout());
	settings.setValue("networkOscInputPort", GetOscServer()->GetListenPort());
	settings.setValue("networkOscLocalEndpoint", FromQtString(GetOscServer()->GetLocalEndpoint().toString()).AsChar());
	settings.setValue("networkOscRemoteHost", FromQtString(GetOscServer()->GetRemoteHost().toString()).AsChar());
	settings.setValue("networkOscRemotePort_v2", GetOscServer()->GetRemoteUdpPort());

}


// load device definitions jsons
void MainWindow::LoadDeviceConfigs(bool reload)
{
	// get access to the device config folder
	String configDir = GetQtBaseManager()->GetAppDataFolder() + "Devices\\";

	// scan the directory for subfolders
	QDir dir(configDir.AsChar());
	dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
	dir.setNameFilters(QStringList("*.json"));
	dir.setSorting(QDir::Name);

	// iterate over all files and add definitions to device manager
	QFileInfoList list = dir.entryInfoList();
	for (int i=0; i<list.size(); ++i)
	{
		QFileInfo fileInfo = list.at(i);

		// load device config and add to device manager
		QFile file(fileInfo.absoluteFilePath());
		if (file.open(QIODevice::ReadOnly) == false)
		{
			LogError("Core::Json::ParseFile() - Failed to open the file '%s'", fileInfo.absoluteFilePath().toUtf8().data());
			continue;
		}

		QString fileTextContent = file.readAll();
		
		// create config
		Json jsonParser; jsonParser.Parse( fileTextContent.toUtf8().data() );
		Device::DeviceConfig config = Device::DeviceConfig(jsonParser);
		if (config.mIsValid == false)
		{
			LogError("Error loading device definition from '%s'", fileInfo.absoluteFilePath().toUtf8().data());
			continue;
		}

		GetDeviceManager()->AddDeviceConfig(config);
	}
}


void MainWindow::OnSessionUserChanged(const User& user) 
{ 
	String name = user.CreateDisplayableName();
	
	// append space due to stylesheet problems (no margins!)
	name = name + " ";

	mSelectSessionUserButton->setText(name.AsChar()); 
}


void MainWindow::OnSaveDesign()
{
	Experience* experience = GetEngine()->GetActiveExperience();

	// require that design is writeable (don't save classifier or statemachine even if they themself are writeable)
	if (experience != NULL && experience->GetCreud().Update() == true)
	{
		// Classifier
		Classifier* classifier = GetEngine()->GetActiveClassifier();
		if (classifier != NULL)
		{
			// set uuid
			experience->SetClassifierUuid(classifier->GetUuid());

			// save classifier or its settings
			if (classifier->IsDirty() == true)
			{
				GraphSettings settings;
		
				// save classifier (if design is not locked)
				if (classifier->GetCreud().Update() == true && mUseSettingsMode == false)
					GetFileManager()->Save(classifier->GetUuid());
				// save changes only
				else if (mUseSettingsMode == true) 
					classifier->CreateSettings(settings, true);

				experience->SetClassifierSettings(settings);
				classifier->SetIsDirty(false);
			}
		}
		else
		{
			experience->SetClassifierUuid("");
		}

		// State Machine
		StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
		if (stateMachine != NULL)
		{
			// set uuid
			experience->SetStateMachineUuid(stateMachine->GetUuid());

			if (stateMachine->IsDirty() == true)
			{
				GraphSettings settings;

				// save statemachine (if design is not locked)
				if (stateMachine->GetCreud().Update() == true && mUseSettingsMode == false)
					GetFileManager()->Save(stateMachine->GetUuid());
				// save changes only (if design is locked)
				else if (mUseSettingsMode == true) 
					stateMachine->CreateSettings(settings, true);
			
				experience->SetStateMachineSettings(settings);
				stateMachine->SetIsDirty(false);
			}
		}
		else
		{
			experience->SetStateMachineUuid("");
		}

		// save design file
		GetFileManager()->Save(experience->GetUuid());
	}
}


// close the currently open design
void MainWindow::OnCloseDesign(bool force)
{
	Experience* experience = GetEngine()->GetActiveExperience();
	if (experience == NULL)
		return;

	// make sure the experience is opened
	const uint32 index = GetFileManager()->FindOpenFile(experience->GetUuid());
	if (index == CORE_INVALIDINDEX32)
	{
		CORE_ASSERT(false);
		return;
	}

	// now close the file
	if (force == true)
		GetFileManager()->CloseWithPrompt(experience->GetUuid());
	else
		GetFileManager()->Close(experience->GetUuid());

	// unload everything from engine _after_ unloading the file
	GetEngine()->Unload();
	GetEngine()->Reset();
}


// trigger an engine reset
void MainWindow::OnResetEngine()
{
	if (GetSession()->IsRunning() == true)
		return;

	GetEngine()->Reset();

	// also reload design settings
	OnLoadDesignSettings();
}


// discard all changes and reload design from source
void MainWindow::OnReloadDesign()
{
	Experience* experience = GetEngine()->GetActiveExperience();
	if (experience == NULL)
		return;

	// not in file manager (??)
	const uint32 index = GetFileManager()->FindOpenFile(experience->GetUuid());
	if (index == CORE_INVALIDINDEX32)
		return;

	// TODO: shouldn't the file manager reopen it??

	FileManager::ELocation location = GetFileManager()->GetOpenFile(index).GetLocation();
	String uuid = experience->GetUuid();

	// close design first
	if (GetFileManager()->CloseWithPrompt(uuid.AsChar()) == false)
		return; // close was aborted
	
	// reload design
	GetFileManager()->OpenExperience( location, uuid.AsChar(), experience->GetName(), experience->GetRevision(), true );
}


// enable settings mode (attribute changes will be saved as graphsettings)
void MainWindow::OnEnableSettingsMode()
{
	mUseSettingsMode = true;
	
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();

	if (classifier != NULL)
		classifier->SetUseSettings(true);

	if (stateMachine != NULL)
		stateMachine->SetUseSettings(true);

	mSaveDesignAction->setText("&Save (settings)");
	mEnableDesignSettingsAction->setVisible(false);
	mDisableDesignSettingsAction->setVisible(true);
}


// remove settings and revert graphs
void MainWindow::OnDisableSettingsMode()
{
	Experience* experience = GetEngine()->GetActiveExperience();
	if (experience == NULL)
		return;

	Classifier* classifier = GetEngine()->GetActiveClassifier();
	StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();

	bool reloadClassifier = false;
	bool reloadStateMachine = false;

	// ask user what to do (change graph, revert graph or cancel)
	if (classifier != NULL && experience->GetClassifierSettings().Size() > 0)
	{
		const int buttonIndex = QMessageBox::warning(this, "Design Settings", "Apply the settings to the classifier or revert it back to the original configuration?", "Apply", "Revert", "Cancel", 1, 2);
		if (buttonIndex == 1)		
			reloadClassifier = true;
		else if (buttonIndex == 2)
			return;
	}

	// ask user what to do (change graph, revert graph or cancel)
	if (stateMachine != NULL && experience->GetStateMachineSettings().Size() > 0)
	{
		const int buttonIndex = QMessageBox::warning(this, "Design Settings", "Apply the settings to the state machine or revert it back to the original configuration?", "Apply", "Revert", "Cancel", 1, 2);
		if (buttonIndex == 1)		
			reloadStateMachine = true;
		else if (buttonIndex == 2)
			return;
	}

	experience->RemoveSettings();
	mUseSettingsMode = false;

	mSaveDesignAction->setText("&Save");
	mEnableDesignSettingsAction->setVisible(true);
	mDisableDesignSettingsAction->setVisible(false);

	// TODO reload state machine and classifier from backend
	if (reloadClassifier == true)
		GetFileManager()->OpenClassifier( FileManager::LOCATION_BACKEND, classifier->GetUuid(), classifier->GetName() );
	if (reloadStateMachine == true)
		GetFileManager()->OpenClassifier( FileManager::LOCATION_BACKEND, stateMachine->GetUuid(), stateMachine->GetName() );
}


// reload design settings (including parameters)
void MainWindow::OnLoadDesignSettings()
{
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
	Experience* experience = GetEngine()->GetActiveExperience();
	
	// load settings from experience first
	if (experience != NULL)
	{
		if (classifier != NULL && experience->GetClassifierSettings().Size() > 0)
			classifier->ApplySettings(experience->GetClassifierSettings());

		if (stateMachine != NULL && experience->GetStateMachineSettings().Size() > 0)
			stateMachine->ApplySettings(experience->GetStateMachineSettings());
	}
	
	// now load classifier parameters (also may contain graph settings for classifier)
	GetBackendInterface()->GetParameters()->Load(false, *GetSessionUser(), experience, classifier);
}



#ifndef PRODUCTION_BUILD
// attach device settings that of active devices
void MainWindow::OnAttachDeviceSettings()
{
	Experience* experience = GetEngine()->GetActiveExperience();
	if (experience == NULL)
		return;
	
	// no device settings
	if (experience->GetDeviceSettings().Size() == 0)
		return;

    // TODO add device settings json from devices to settings array (copy of json subtree)
}
#endif


// user log out
void MainWindow::OnLogOut()
{
	if (QMessageBox::question(this, "Log out", "Are you sure you want to log out and close the application?", QMessageBox::Yes, QMessageBox::Cancel) == QMessageBox::Cancel)
		return;

	GetAuthenticationCenter()->RemoveLoginInfoFromLicense();

	GetQtBaseManager()->ForceTerminateApplication();
	QProcess::startDetached( QCoreApplication::applicationFilePath() );
}


// trigger a crash
void MainWindow::OnCrash()
{
	int *ptr = NULL;
    *ptr = 0;
}


// show serial port list
void MainWindow::OnShowSerialPortInfo()
{
	String info;
	SerialPort::LogPortsToString(info);

	info += "Press Ctrl+C to copy info to clipboard.";

	QMessageBox::information(this, "Serial Ports", info.AsChar(), QMessageBox::Ok);
}


// show bluetooth device list
void MainWindow::OnShowBluetoothInfo()
{
	String info;
	BluetoothHelpers::LogDevicesToString(info);

	info += "Press Ctrl+C to copy info to clipboard.";

	QMessageBox::information(this, "Bluetooth Devices", info.AsChar(), QMessageBox::Ok);
}


#ifndef PRODUCTION_BUILD
// place code here while developing, execute it via menu Tools -> Dev -> DoSomething 
void MainWindow::OnDoSomething()
{
	// start user selection window
	SelectUserWindow testWindow( *GetUser(), GetMainWindow(), true );
	testWindow.exec();
}
#endif

#ifndef PRODUCTION_BUILD
// allow editing of the graph even if the user does not have permissions
void MainWindow::OnUnlockClassifier()
{
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	if (classifier == NULL)
		return;

	Creud oldCreud = classifier->GetCreud();
	Creud newCreud( oldCreud.Create(), true, true, oldCreud.Delete(), true );

	classifier->SetCreud( newCreud );
}
#endif

#ifndef PRODUCTION_BUILD
void MainWindow::OnResizeWindowAction()
{
	EnterLabelWindow sizeWindow("Please enter window size in pixels", "1920,1080", "", this);
	if (sizeWindow.exec() == QDialog::Rejected)
		return;

	Array<String> sizes = sizeWindow.GetLabelText().Split(StringCharacter::comma);
	if (sizes.Size() != 2)
		return;

	int32 width = sizes[0].ToInt();
	int32 height = sizes[1].ToInt();
	
	// resize window
	resize(width, height);
}
#endif


void MainWindow::OnRaise()
{
	// minimizes the window first and then activate, raise, bring it to the front
	setWindowState( (windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);

	// for macOS
	raise();

	// for Windows
	activateWindow();
}


#ifndef PRODUCTION_BUILD
// export node documentation as solmarkdown
void MainWindow::OnExportNodeDocMarkdown()
{
	DocumentationExporter::ExportNodeMarkdown();
}
#endif


#ifndef PRODUCTION_BUILD
// export node documentation table as  markdown
void MainWindow::OnExportNodeTableMarkdown()
{
	DocumentationExporter::ExportNodeTableMarkdown();
}
#endif


//
// MUSE IO
//


// start Muse IO
void MainWindow::OnStartMuseIO()
{
	Array<uint32> deviceIDs;
	Array<String> deviceNames;
	Array<const Device::DeviceConfig*> deviceConfigs = GetDeviceManager()->FindDeviceConfigsByType(DeviceTypeIDs::DEVICE_TYPEID_INTERAXON_MUSE);
	
	// look for muse configs
	uint32 numConfigs = deviceConfigs.Size();
	for (uint32 i=0; i<numConfigs; ++i)
	{
		// try to get settings from config
		const Device::DeviceConfig* config = deviceConfigs[i];
		Json::Item rootItem = config->mJson.GetRootItem();

		// bluetooth mac or name
		Json::Item macItem = rootItem.Find("bluetoothMac");
		if (macItem.IsString() == true && String(macItem.GetString()).GetLength() > 0)
		{
			// add to list
			deviceIDs.Add(config->mDeviceID);
			deviceNames.Add(macItem.GetString());
		}
		else // BT MAC overrides name
		{
			Json::Item nameItem = rootItem.Find("bluetoothName");
			if (nameItem.IsString() == true && String (nameItem.GetString()).GetLength() > 0)
			{
				// add to list
				deviceIDs.Add(config->mDeviceID);
				deviceNames.Add(nameItem.GetString());
			}
		}

	}

	// use default muse config if none are set
	if (deviceIDs.Size() == 0)
	{
		deviceIDs.Add(0);
		deviceNames.Add("");
	}

	// start one muse-io per device
	const uint32 numDevices = deviceIDs.Size();
	for (uint32 i=0; i<numDevices; ++i)
	{
		String commandLine;
		const uint32 powerLineFreq = GetEngine()->GetPowerLineFrequency( GetUser() );

	#ifdef NEUROMORE_PLATFORM_WINDOWS
		//commandLine.Format("%s%s ", FromQtString(GetAppDir()).AsChar(), "Muse\\muse-io.exe");
		commandLine.Format("%s ",  "Muse\\muse-io.exe" );
	#endif

		// create muse command line string
		commandLine += CreateMuseCommandLine(deviceIDs[i], powerLineFreq, "", deviceNames[i]);
	
#ifdef NEUROMORE_PLATFORM_WINDOWS

		QProcess* process = new QProcess();
		process->setWorkingDirectory(GetQtBaseManager()->GetAppDataFolder().AsChar());
		process->startDetached(commandLine.AsChar());
#else

    String museIoFilename = FromQtString( QFileDialog::getOpenFileName( this, "Please select the MuseIO application" ) );
    QFile museIoFile(museIoFilename.AsChar());
    QString filename = museIoFile.fileName();
    museIoFile.close();
    
    String message = "Please open a terminal and run the following command:\n\n";
    message.FormatAdd("\"%s\" %s", museIoFilename.AsChar(), commandLine.AsChar());
    QMessageBox::information(this, "Please start MuseIO", message.AsChar());
    
   
    // write quoted filename
/*    String quotedFilename;
    quotedFilename.Format("\"%s\"", museIoFilename.AsChar(), commandLine.AsChar());
    shellScriptFile.WriteString(quotedFilename);
    shellScriptFile.WriteString("\n");
    
    shellScriptFile.WriteString("echo \"STARTING SCRIPT WORKS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\"\n");
    
    String test;
    test.Format("\"%s\"", shellScriptAbsoluteFilename.AsChar());//, museIoFilename.AsChar());
 //   QProcess::startDetached( test.AsChar() );
    // write parameters*/
    
 //   ./muse-io-3_7_0-build892-dev-osx --50hz --osc osc.udp://localhost:4545 --osc-eeg-path     /muse/0/eeg --osc-battery-path /muse/0/batt --osc-acc-path     /muse/0/acc --osc-status-path  /muse/0/status --osc-quant-path   /muse/0/eeq/quant --osc-bp-path      /muse/0/dsp --osc-drlref-path  /muse/0/drl
    
  //  shellScriptFile.Close();

    //String extractedFilename = "\"";
    //extractedFilename += museIoFilename;
    //extractedFilename += "\"";
    
 /*   String arg;
    arg.Format( "\"%s\"", shellScriptFilename.AsChar() );
    
    QStringList arguments;
    arguments.append(shellScriptFilename.AsChar());
    
    QProcess* process = new QProcess();
    process->setWorkingDirectory( GetQtBaseManager()->GetAppDataFolder().AsChar() );
   process->start(test.AsChar());

    process->write("echo \"TEEEEEEST\"");
    process->write(shellScriptAbsoluteFilename.AsChar());
    process->closeWriteChannel();*/
    
#endif

	}
}


String MainWindow::CreateMuseCommandLine(uint32 deviceID, uint32 powerLineFreq, String preset, String bluetoothNameOrMac)
{
	String commandLine;

	// power line notch filter
	if (powerLineFreq == 50)
		commandLine += "--50hz ";
	else // default to USA setting
		commandLine += "--60hz ";
	
	// preset
	if (preset.IsEmpty() == false)
		commandLine += " --preset " + preset;

	// device identifier
	if (bluetoothNameOrMac.IsEmpty() == false)
		commandLine += " --device " + bluetoothNameOrMac;

	commandLine += " --osc osc.udp://localhost:4545 ";

	// osc output settings.
	String oscPath; oscPath.Format("/muse/%i/", deviceID);
	commandLine += " --osc-eeg-path " + oscPath + "eeg";
	commandLine += " --osc-battery-path " + oscPath + "batt";
	commandLine += " --osc-acc-path " + oscPath + "acc";
	commandLine += " --osc-status-path " + oscPath + "status";
	commandLine += " --osc-quant-path " + oscPath + "eeg/quant";
	commandLine += " --osc-bp-path " + oscPath + "dsp";
	commandLine += " --osc-drlref-path " + oscPath + "drl ";

	return commandLine;
}


// menu item clicked, start the visualization
void MainWindow::OnStartVisualization()
{
	QAction*	action		= qobject_cast<QAction*>( sender() );
	int			vizIndex	= action->property("index").toInt();

	Visualization* visualization = GetManager()->GetVisualizationManager()->GetVisualization(vizIndex);
	if (visualization != NULL)
		visualization->Start();
}




NetworkServer*			GetNetworkServer()			{ return GetManager()->GetMainWindow()->GetNetworkServer(); }
OscServer*				GetOscServer()				{ return GetManager()->GetMainWindow()->GetOscServer(); }
AuthenticationCenter*	GetAuthenticationCenter()	{ return GetManager()->GetMainWindow()->GetAuthenticationCenter(); }
