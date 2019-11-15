/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_LAYOUT_H
#define __NEUROMORE_LAYOUT_H

// include required headers
#include "QtBaseConfig.h"
#include <Core/String.h>
#include <Core/Json.h>
#include <QMainWindow>


// layout
class QTBASE_API Layout : public QObject
{
	Q_OBJECT
	public:
		enum EType
		{
			BUILTIN,
			BACKEND,
			LOCAL
		};

		enum MainWindowShowMode
		{
			MAINWINDOWSHOW_NORMAL,
			MAINWINDOWSHOW_MAXIMIZED,
			MAINWINDOWSHOW_FULLSCREEN
		};

		// constructor & destructor
		Layout(EType type, const char* visualName, const char* filename, const char* ruleName, const char* uuid);
		virtual ~Layout();

		EType GetType() const										{ return mType; }

		const char* GetName() const									{ return mVisualName; }
		const Core::String& GetNameString() const					{ return mVisualName; }

		const char* GetRuleName() const								{ return mRuleName; }
		const Core::String& GetRuleNameString() const				{ return mRuleName; }

		const char* GetFilename() const								{ return mFilename; }
		const Core::String& GetFilenameString() const				{ return mFilename; }

		const char* GetUuid() const									{ return mUuid; }
		const Core::String& GetUuidString() const					{ return mUuid; }

		// TODO move these to layout manager
		static bool Save(const char* filename);
		bool Load(const char* filename);

	private slots:
		void OnShowMainWindow();
		void OnRestore();

	private:
		struct Screen
		{
			int32				mWidth;
			int32				mHeight;

			bool Save(Core::Json& json, Core::Json::Item& item) const;

			Screen();
			Screen(QScreen* screen);
			Screen(int32 width, int32 height);
		};

		struct MainWindowState
		{
			Core::String		mGeometry;
			Core::String		mState;
			int32				mPosX;
			int32				mPosY;
			int32				mWidth;
			int32				mHeight;
			bool				mIsMaximized;

			bool Save(Core::Json& json, Core::Json::Item& item) const;

			MainWindowState();
			MainWindowState(QMainWindow* mainWindow);
			MainWindowState(const char* windowGeometry, const char* windowState, int32 windowPosX, int32 windowPosY, int32 windowWidth, int32 windowHeight, bool isMaximized);
		};

		struct ScreenConfig
		{
			Screen				mScreen;
			MainWindowState		mState;

			bool Save(Core::Json& json, Core::Json::Item& item) const;
			ScreenConfig(const Screen& screen, const MainWindowState& state)				{ mScreen = screen; mState = state; }
		};

		MainWindowShowMode				mMainWindowShowMode;
		QByteArray						mMainWindowState;
		QByteArray						mMainWindowGeometry;

		Core::Array<ScreenConfig>		mScreenConfigs;
		Core::String					mVisualName;
		Core::String					mRuleName;
		Core::String					mFilename;
		Core::String					mUuid;
		EType							mType;
};


#endif
