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

#ifndef __NEUROMORE_WHILESESSIONWIDGET_H
#define __NEUROMORE_WHILESESSIONWIDGET_H

// include required headers
#include "../../Config.h"
#include <ImageButton.h>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include "../../Widgets/StopwatchWidget.h"



class WhileSessionWidget : public QWidget
{
	Q_OBJECT
	public:
		WhileSessionWidget(QWidget* parent, int buttonSize);
		virtual ~WhileSessionWidget();

		void UpdateInterface();
		QPushButton* GetStopButton() const		{ return mStopButton; }
		QPushButton* GetPauseButton() const		{ return mPauseButton; }
		QPushButton* GetContinueButton() const	{ return mContinueButton; }

	private:
		Core::String		mTempString;
		ImageButton*		mStopButton;
		ImageButton*		mPauseButton;
		ImageButton*		mContinueButton;

		StopwatchWidget*	mStopwatchWidget;
		//QMovie*			mRunningAnimatedImage;
		//QLabel*			mRunningLabel;
		QLabel*				mPointsLabel;
};


#endif
