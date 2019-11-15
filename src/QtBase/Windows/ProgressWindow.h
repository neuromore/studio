/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMOREPROGRESSWINDOW_H
#define __NEUROMOREPROGRESSWINDOW_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include <QMovie>


class QTBASE_API ProgressWindow : public QDialog
{
	Q_OBJECT
	public:
		ProgressWindow(QWidget* parent);
		virtual ~ProgressWindow();

	public slots:
		// progress window handling
		void OnProgressStart(bool showProgressText, bool showProgressValue, bool showSubProgressText, bool showSubProgressValue);
		void OnProgressEnd();

		void OnProgressText(Core::String text);
		void OnProgressValue(float percentage);

		void OnSubProgressText(Core::String text);
		void OnSubProgressValue(float percentage);

	private:
		QLabel*				mProgressText;
		QProgressBar*		mProgressBar;
		QProgressBar*		mSubProgressBar;
		QLabel*				mSubProgressText;

		uint32				mWaitingSize;
		QMovie*				mWaitingMovie;
		QLabel*				mWaitingLabel;
};


#endif
