/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMOREPROGRESSWINDOWMANAGER_H
#define __NEUROMOREPROGRESSWINDOWMANAGER_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/EventManager.h>
#include <QObject>
#include <QWidget>
#include <QMutex>


// forward declaration
class ProgressWindow;

// progress window event handler
class QTBASE_API ProgressHandler : public QObject, public Core::EventHandler
{
	Q_OBJECT
	public:
		ProgressHandler() : QObject(), EventHandler()																				{}
		virtual ~ProgressHandler() {}

		void OnProgressStart(bool showProgressText, bool showProgressValue, bool showSubProgressText, bool showSubProgressValue)	{ emit ProgressStart(showProgressText, showProgressValue, showSubProgressText, showSubProgressValue); }
		void OnProgressEnd()																										{ emit ProgressEnd(); }
		void OnProgressText(const char* text)																						{ emit ProgressText(text); }
		void OnProgressValue(float percentage)																						{ emit ProgressValue(percentage); }
		void OnSubProgressText(const char* text)																					{ emit SubProgressText(text); }
		void OnSubProgressValue(float percentage)																					{ emit SubProgressValue(percentage); }

	signals:
		void ProgressStart(bool showProgressText, bool showProgressValue, bool showSubProgressText, bool showSubProgressValue);
		void ProgressEnd();
		void ProgressText(Core::String text);
		void ProgressValue(float percentage);
		void SubProgressText(Core::String text);
		void SubProgressValue(float percentage);
};



class ProgressWindowManager : public QObject
{
	Q_OBJECT
	public:
		ProgressWindowManager();
		virtual ~ProgressWindowManager();

		inline bool IsProgressWindowActive()										{ return mIsProgressWindowActive; }

		// progress window control
	public slots:
		void ShowProgressWindow(bool showProgressText, bool showProgressValue, bool showSubProgressText, bool showSubProgressValue);
		void HideProgressWindow();
		void OnProgressText(Core::String text);
		void OnProgressValue(float percentage);
		void OnSubProgressText(Core::String text);
		void OnSubProgressValue(float percentage);

	private:
		// progress window handling
		ProgressHandler*		mProgressHandler;
		ProgressWindow*			mProgressWindow;
		bool					mIsProgressWindowActive;
};


#endif
