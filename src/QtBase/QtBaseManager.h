/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __QTBASE_MANAGER_H
#define __QTBASE_MANAGER_H

// include required files
#include <Core/StandardHeaders.h>
#include <Core/Array.h>
#include <Core/String.h>
#include "QtBaseConfig.h"
#include "AttributeWidgets/AttributeWidgetFactory.h"
#include "Backend/BackendInterface.h"
#include "FileManager.h"
#include "ExperienceAssetCache.h"
#include "LayoutManager.h"
#include "Windows/StatusPopupManager.h"
#include "MainWindowBase.h"
#include <QWidget>
#include <QWindow>
#include <QPushButton>


// forward declaration
class PluginManager;
class ProgressWindowManager;

// the Qt base manager class
class QTBASE_API QtBaseManager : public QObject
{
	Q_OBJECT
	friend class QtBaseInitializer; 

	public:
		void Init();

		// version
		Core::Version GetVersion() const									{ return mVersion; }

		inline MainWindowBase* GetMainWindow() const						{ return mMainWindow; }
		inline void SetMainWindow(MainWindowBase* mainWindow)				{ mMainWindow = mainWindow; }

		inline void SetAppDir(const QString& appDir)						{ mAppDir = appDir; }
		inline const QString& GetAppDir() const								{ return mAppDir; }

		inline PluginManager* GetPluginManager() const						{ return mPluginManager; }
		inline void SetPluginManager(PluginManager* pluginManager)			{ mPluginManager = pluginManager; }

		inline FileManager* GetFileManager() const							{ return mFileManager; }
		inline void SetFileManager(FileManager* FileManager)				{ mFileManager = FileManager; }

		inline LayoutManager*			GetLayoutManager() const			{ return mLayoutManager; }
		inline ProgressWindowManager*	GetProgressWindowManager() const	{ return mProgressWindowManager; }
		inline StatusPopupManager*		GetStatusPopupManager() const		{ return mStatusPopupManager; }
		inline BackendInterface*		GetBackendInterface() const			{ return mBackendInterface; }
		inline AttributeWidgetFactory*	GetAttributeWidgetFactory() const	{ return mAttributeWidgetFactory; }
		inline ExperienceAssetCache*	GetExperienceAssetCache()			{ return mExperienceAssetCache; }

		const QIcon& FindIcon(const char* filename);

		// enable or disable the whole application
	private:
		void EnableInterface(bool isEnabled);
	public slots:
		void UpdateInterfaceLock();
	public:
		bool IsInterfaceEnabled() const										{ return mInterfaceEnabled; }
		bool IsInterfaceLocked() const										{ return mLockInterface; }
		void SetInterfaceLocked(bool locked)								{ mLockInterface = locked; }

		// in case the interface is paused, realtime updates (like our 3d visualizations) will skip rendering
		void SetPauseInterface(bool isPaused);
		bool IsInterfacePaused() const										{ return mInterfaceUpdatePaused; }

		// information
		const char* GetCompileDate() const									{ return mCompileDate.AsChar(); }

		// folders
		static Core::String GetAppDataFolder();

		void UpdatePhysiologicalDataFolder();
		const char* GetPhysiologicalDataFolder()							{ return mPhysiologicalDataFolder.AsChar(); }

		// file system helpers
		static bool RemoveFileFromDisk(const char* filename);

		// application helpers
		static void ForceTerminateApplication();

		// file operation helpers
		static bool RemoveDirectory(const QString &dirPath);
		static bool CopyDirectory(const QString &srcPath, const QString &dstPath);

		// style sheets
		static bool LoadDefaultStyleSheet(QWidget* applyTo);
		static bool LoadStyleSheet(const QString& filename, QWidget* applyTo);

		// widget helpers
		void CenterToScreen(QWidget* widget);

		const char* ConstructHTMLLink(const char* text, const Core::Color& color = Core::Color(0.95315f, 0.609375f, 0.109375f));
		static void SetWidgetAsInvalidInput(QWidget* widget, bool isValid=false);

		static void MakeTransparentButton(QPushButton* button, const char* iconFilename, const char* toolTipText, uint32 width=20, uint32 height=20);

		//static void MakeHoverButton(QPushButton* button, const char* iconPath, const char* iconHoverPath, const char* iconDisabledPath, const char* toolTipText, uint32 width=20, uint32 height=20);
		//static Core::String CreateButtonStyle(const char* iconPath, const char* iconHoverPath, const char* iconDisabledPath);

	private:
		struct IconData
		{
			//IconData(const char* filename);
            IconData(const char* filename, QIcon* icon)     { mFilename = filename; mIcon = icon; }
			~IconData()                                     { delete mIcon; }
	
			QIcon*				mIcon;
			Core::String	mFilename;
		};

		Core::Version						mVersion;

		MainWindowBase*						mMainWindow;
		PluginManager*						mPluginManager;
		FileManager*						mFileManager;
		ExperienceAssetCache*				mExperienceAssetCache;
		AttributeWidgetFactory*				mAttributeWidgetFactory;
		BackendInterface*					mBackendInterface;
		LayoutManager*						mLayoutManager;

		// status popup manager
		StatusPopupManager*					mStatusPopupManager;

		bool								mInterfaceEnabled;
		bool								mInterfaceUpdatePaused;
		bool								mLockInterface;

		// folder paths
		Core::String						mPhysiologicalDataFolder;

		Core::Array<IconData*>				mIcons;
		QString								mAppDir;

		// progress window
		ProgressWindowManager*				mProgressWindowManager;

		Core::String						mCompileDate;
		Core::String						mTempString;

		QtBaseManager();
		virtual ~QtBaseManager();
};


// the initializer
class QTBASE_API QtBaseInitializer
{
	public:
		static bool Init(const char* appDir);
		static void Shutdown();
};


// the global
extern QTBASE_API QtBaseManager* gQtBaseManager;

// shortcuts
inline QtBaseManager*		GetQtBaseManager()		{ return gQtBaseManager; }
inline PluginManager*		GetPluginManager()		{ return gQtBaseManager->GetPluginManager(); }
inline FileManager*			GetFileManager()		{ return gQtBaseManager->GetFileManager(); }
inline const QString&		GetAppDir()				{ return gQtBaseManager->GetAppDir(); }
inline BackendInterface*	GetBackendInterface()	{ return gQtBaseManager->GetBackendInterface(); }
inline LayoutManager*		GetLayoutManager()		{ return gQtBaseManager->GetLayoutManager(); }


#endif
