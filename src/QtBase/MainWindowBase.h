/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_MAINWINDOWBASE_H
#define __NEUROMORE_MAINWINDOWBASE_H

// include required headers
#include "QtBaseConfig.h"
#include <QObject>
#include <QTimer>
#include <QMainWindow>
#include <Core/Timer.h>
#include <Core/FpsCounter.h>


class QTBASE_API MainWindowBase : public QMainWindow
{
	Q_OBJECT
	public:
		MainWindowBase(QWidget* parent=NULL, Qt::WindowFlags flags=0);
		virtual ~MainWindowBase();

		void Init();

		// show fps flag
		void SetShowFPS(bool show)												{ mShowFPS = show; }
		bool GetShowFPS() const													{ return mShowFPS; }

		int GetDefaultFontSize() const											{ return mDefaultFontSize; }

		Core::FpsCounter& GetOpenGLFpsCounter()									{ return mOpenGLWidgetFpsCounter; }

		// show performance info flag
		void SetShowPerformanceInfo(bool show)									{ mShowPerformanceInfo = show; }
		bool GetShowPerformanceInfo() const										{ return mShowPerformanceInfo; }

		void SetEngineTimerEnabled(bool isEnabled);
		void SetRealtimeUITimerEnabled(bool isEnabled);
		void SetInterfaceTimerEnabled(bool isEnabled);

		void SetEngineUpdateRate(double updateFPS);
		void SetRealtimeUIUpdateRate(double updateFPS);
		void SetInterfaceUpdateRate(double updateFPS);

		double GetEngineUpdateRate() const										{ return mEngineUpdateRate; }
		double GetRealtimeUIUpdateRate() const									{ return mRealtimeUIUpdateRate; }
		double GetInterfaceUpdateRate() const									{ return mInterfaceUpdateRate; }

	protected slots:
		void OnEngineUpdate();
		void OnRealtimeUIUpdate();
		void OnUpdateInterface();
		void UpdateRealtimePlugins(double timeDelta);

	private:
		QTimer*							mEngineTimer;
		double							mEngineUpdateRate;
		Core::Timer						mEngineUpdateTimer;

		QTimer*							mRealtimeUITimer;
		double							mRealtimeUIUpdateRate;
		Core::Timer						mRealtimeUIUpdateTimer;

		QTimer*							mInterfaceTimer;
		double							mInterfaceUpdateRate;

		Core::FpsCounter				mOpenGLWidgetFpsCounter;
		
		bool							mShowFPS;
		bool							mShowPerformanceInfo;

		int								mDefaultFontSize;
};


#endif
