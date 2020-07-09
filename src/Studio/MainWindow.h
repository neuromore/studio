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

#ifndef __NEUROMORE_MAINWINDOW_H
#define __NEUROMORE_MAINWINDOW_H

// include the required headers
#include "Config.h"
#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include <Core/LogManager.h>
#include <Core/Timer.h>
#include <EngineManager.h>
#include <MainWindowBase.h>
#include <AttributeWidgets/PropertyTreeWidget.h>
#include <LayoutComboBox.h>
#include <LayoutMenu.h>
#include <PluginSystem/PluginMenu.h>

#include <QMainWindow>
#include <QMenu>
#include <QTimer>
#include <QDropEvent>
#include <QUrl>
#include <QCheckBox>
#include <QProcess>

#include <FileManager.h>
#include <Networking/NetworkServer.h>
#include <Networking/OscServer.h>
#include "Windows/LoginWindow.h"
#include "Windows/SelectUserWindow.h"
#include "Windows/ExperienceWizardWindow.h"
#include "AuthenticationCenter.h"

// forward declarations
QT_FORWARD_DECLARE_CLASS(QSignalMapper)
QT_FORWARD_DECLARE_CLASS(QtVariantProperty)
QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QPushButton)


// forward declaration
class AboutWindow;
class Plugin;
class SettingsWindow;

// the main window
class MainWindow : public MainWindowBase, public Core::EventHandler
{
	Q_OBJECT
	public:
		MainWindow(QWidget* parent=NULL, Qt::WindowFlags flags=0);
		virtual ~MainWindow();

		void Init();
		void OnPostAuthenticationInit();
		void UpdateTitle(const Core::String userName);

		// settings
		SettingsWindow* GetSettingsWindow() const						{ return mSettingsWindow; }
		QString GetSettingsFilename() const;

		// misc windows
		void CloseSplashScreen();
		void ShowUpgradeWindow(const char* upgradeReason);

		// trigger user selection dialog
		void SelectSessionUser();

		// the settings mode flag
		bool IsSettingsModeEnabled() const							{ return mUseSettingsMode; }

		// core components
		NetworkServer*		  GetNetworkServer()					{ return mNetworkServer; }
		OscServer*			  GetOscServer()						{ return mOscServer; }
		AuthenticationCenter* GetAuthenticationCenter()				{ return mAuthenticationCenter; }

	private:
		void ReInitBciDeviceCombo();
		void ReselectBciDeviceCombo();

		// session user selection window
		SelectUserWindow*			mSessionUserSelectionWindow;
		ExperienceWizardWindow*	mExperienceWizardWindow;

		//
		// Settings
		// 

		SettingsWindow*				mSettingsWindow;

		Property*					mShowFPSProperty;
		Property*					mShowPerformanceInfoProperty;
		Property*					mLogLevelProperty;

		// performance
		Property*					mEngineUpdateRateProperty;
		Property*					mRealtimeInterfaceUpdateRateProperty;
		Property*					mInterfaceUpdateRateProperty;

		// devices
		Property*					mPowerLineFrequencyTypeProperty;
		Property*					mAutoSyncProperty;
		Property*					mAutoDetectionProperty;

		// drift correction
		Property*					mDriftCorrectionEnableProperty;
		Property*					mDriftCorrectionMaxSyncDriftProperty;
		Property*					mDriftCorrectionMaxForwardDriftProperty;
		Property*					mDriftCorrectionMaxBackwardDriftProperty;

		// audio
#ifdef INCLUDE_DEVICE_GENERIC_AUDIO
		Property*					mAudioInputUpdateRateProperty;
		Property*					mAudioOutputBufferSizeProperty;
		Property*					mAudioOutputUpdateRateProperty;
		Property*					mAudioOutputDelayProperty;
#endif

		// neuromore Cloud settings
#ifndef PRODUCTION_BUILD
		Property*					mServerPresetProperty;
#endif
		Property*					mLogBackendProperty;

		// NETWORK settings
		Property*					mAnnounceNetworkServerProperty;
		Property*					mNetworkServerEnableTCPProperty;
		Property*					mNetworkServerEnableBroadcastProperty;
		Property*					mNetworkServerAnnounceIntervalProperty;
		Property*					mNetworkServerClientTimeoutProperty;
		Property*					mOSCInputPortProperty;
		Property*					mOSCRemoteHostProperty;
		Property*					mOSCRemotePortProperty;
		Property*					mOSCLocalEndpointProperty;

		// DEVICE CONFIGS
		void LoadDeviceConfigs(bool reload = true);
		Core::Array<Property*>		mDeviceAutoDetectionProperties;

		// Settings-Mode (allow only edit and save of graph settings)
		bool						mUseSettingsMode;

		Core::String				mTempString;

		// EVENTS ----------------------------------------------
		void OnActiveBciChanged(BciDevice* device) override	final			{ ReselectBciDeviceCombo(); }
		void OnDeviceAdded(Device* device) override	final					{ ReInitBciDeviceCombo(); }
		void OnDeviceRemoved(Device* device) override final					{ ReInitBciDeviceCombo(); }
		void OnSessionUserChanged(const User& user) override final;
		void OnStartSession() override final;
		void OnStopSession() override final;
		void OnActiveExperienceChanged(Experience* experience) override final;
		// -----------------------------------------------------
		
		//
		// UI elements
		//

		QMenuBar*					mMenuBar;
		QLabel*						mActiveBciLabel;
		QComboBox*					mActiveBciCombo;
		LayoutComboBox*				mLayoutComboBox;
		QPushButton*				mSelectSessionUserButton;
		QLabel*						mSelectSessionUserLabel;

		LayoutMenu*					mLayoutMenu;
		PluginMenu*					mPluginMenu;
		QMenu*						mEditMenu;
		QMenu*						mDesignMenu;
		QMenu*						mVisualizationMenu;
		
		QAction*					mExitAction;
		QAction*					mSaveAction;
		QAction*					mCloseAction;

		QAction*					mSaveDesignAction;
		QAction*					mEnableDesignSettingsAction;
		QAction*					mDisableDesignSettingsAction;
#ifndef PRODUCTION_BUILD
		QAction*					mAttachDeviceSettingsAction;
#endif

		// TODO move these three components to AppManager They should live next the main window, not within it. 
		//      -> Network servers can also be moved to qtbase!
		// networking
		NetworkServer*				mNetworkServer;
		OscServer*					mOscServer;
		// authentication center
		AuthenticationCenter*		mAuthenticationCenter;
		
		AboutWindow*				mAboutWindow;

		// window close event
		void closeEvent(QCloseEvent* event) override;

		// keyboard events
		void keyPressEvent(QKeyEvent* event) override;
		void keyReleaseEvent(QKeyEvent* event) override;

		QAction* FindAction(QList<QAction*>& actionList, Plugin* plugin);


	public slots:
		// bring window to front (in case another app instance is started)
		void OnRaise();


		void OnBciSelectionChanged(const QString& text);
		void OnSessionUserSelected(const User& user);
		void OnCreateProtocolForUser(const User& user);
		void OnCreatedProtocolForUser(const User& user);

		void OnSessionUserSelectionCanceled();

		// FILE menu
		//void OnOpenFile(); 
		void OnSaveDesign();
		void OnCloseDesign(bool force = true);
		void OnExit();

		// EDIT menu
		void OnSettings();
		void OnResetEngine();

		// VIEW menu
		void OnReloadStyleSheet();

		// DESIGN menu
		void OnReloadDesign();
		void OnEnableSettingsMode();
		void OnDisableSettingsMode();
		void OnLoadDesignSettings();
#ifndef PRODUCTION_BUILD
		void OnAttachDeviceSettings();
#endif
		// VISUALIZATIONS menu
		void OnStartVisualization();

		// TOOLS menu
		void OnStartMuseIO();
		void OnCrash();
		
		// TOOLS/information
		void OnShowSerialPortInfo();
		void OnShowBluetoothInfo();

		// TOOLS/dev
#ifndef PRODUCTION_BUILD
		void OnUnlockClassifier();
		void OnResizeWindowAction();
		void OnExportNodeDocMarkdown();
		void OnExportNodeTableMarkdown();
		void OnDoSomething();
#endif

		// HELP menu
		void OnLogOut();
		void OnVisitWebsite();
		void OnVisitAccount();
		void OnVisitDocumentation();
		void OnVisitSupport();
		void OnCheckForUpdates();
		void OnAbout();
		void OnLicense();

		// settings
		void OnValueChanged(Property* property);
		void OnLoadSettings();
		void OnSaveSettings();

	private:
		Core::String CreateMuseCommandLine(uint32 deviceID, uint32 powerLineFreq, Core::String preset, Core::String bluetoothNameOrMac);
};

NetworkServer*			GetNetworkServer();
OscServer*				GetOscServer();
AuthenticationCenter*	GetAuthenticationCenter();


#endif
