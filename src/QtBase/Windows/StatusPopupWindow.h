/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_STATUSPOPUPWINDOW_H
#define __NEUROMORE_STATUSPOPUPWINDOW_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include <QMovie>
#include <QGraphicsOpacityEffect>
#include <QVariant>

// forward declaration
class StatusPopupManager;

class QTBASE_API StatusPopupWindow : public QDialog
{
	Q_OBJECT

	public:
		StatusPopupWindow(StatusPopupManager* manager, QWidget* parent, int32 yPos);
		virtual ~StatusPopupWindow();

		void ShowMessage(const char* text);
		void SetProgress(float progress);

		void FadeIn();
		void FadeOut();

		void MoveTo(int32 y);

		static int32 GetHalfHeight()						{ return 12; }

	private slots:
		void OnOpacityChanged(QVariant value);

	private:
		StatusPopupManager*		mManager;
		QProgressBar*			mProgressBar;
		QLabel*					mLabel;
		QMovie*					mWaitingMovie;
		QLabel*					mWaitingLabel;
		QGraphicsOpacityEffect*	mOpacityEffect;
		bool					mFadingOut;
};


#endif
