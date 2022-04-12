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

// include required headers
#include "WhileSessionWidget.h"
#include <ImageButton.h>
#include <QtBaseManager.h>
#include <EngineManager.h>
#include "../../MainWindow.h"
#include <QHBoxLayout>
#include <QMovie>


using namespace Core;

WhileSessionWidget::WhileSessionWidget(QWidget* parent, int buttonSize) : QWidget(parent)
{
	Session* session = GetEngine()->GetSession();

	setObjectName("TransparentWidget");
	setMinimumHeight(buttonSize);
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::MinimumExpanding);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->setMargin(0);
	setLayout(mainLayout);

	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->setMargin(0);
	mainLayout->addLayout(hLayout);

	// create the pause session button
	mPauseButton = new ImageButton("Images/Icons/PauseSession.png", buttonSize, "Pause the Session" );
	hLayout->addWidget(mPauseButton);

	// create the unpause session button (same icon as start button but without text)
	mContinueButton = new ImageButton("Images/Icons/ContinueSession.png", buttonSize, "Continue with Session");
	hLayout->addWidget(mContinueButton);

	QVBoxLayout* rightVLayout = new QVBoxLayout();
	rightVLayout->setMargin(0);
	hLayout->addLayout(rightVLayout);

	// add the stopwatch widget
	mStopwatchWidget = new StopwatchWidget(-1);
	rightVLayout->addWidget(mStopwatchWidget);
/*
	// animated stopwatch label
	mRunningAnimatedImage = new QMovie(":/Images/Icons/Loader.gif");
	//bool isValid = mAnimatedImage->isValid();
	mRunningAnimatedImage->setScaledSize(QSize(buttonSize, buttonSize));
	mRunningLabel = new QLabel();
	mRunningLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	mRunningLabel->setMinimumSize(QSize(buttonSize, buttonSize));
	mRunningLabel->setMaximumSize(QSize(buttonSize, buttonSize));
	mRunningLabel->setStyleSheet("background:transparent;");
	mRunningLabel->setAttribute(Qt::WA_TranslucentBackground, true);
	mRunningLabel->setMovie(mRunningAnimatedImage);
	mRunningAnimatedImage->start();

	hLayout->addWidget(mRunningLabel);*/

	// add the widgets for displaying points
	mPointsLabel = new QLabel();
	QFont pointsFont = mPointsLabel->font();
	pointsFont.setPixelSize(24);
	mPointsLabel->setFont(pointsFont);
	rightVLayout->addWidget(mPointsLabel, 0, Qt::AlignCenter);

	// create the stop session button
	mStopButton = new ImageButton("Images/Icons/StopSession.png", buttonSize, "End the Session");
	hLayout->addWidget(mStopButton);
}


WhileSessionWidget::~WhileSessionWidget()
{
	//delete mRunningAnimatedImage;
}


// update interface information
void WhileSessionWidget::UpdateInterface()
{
	// show pause or continue button
	const bool showContinue = GetEngine()->IsSoftPaused();
	mPauseButton->setHidden(showContinue);
	mContinueButton->setHidden(!showContinue);

	// points
	const bool hasPoints = GetSession()->HasPoints();
	mPointsLabel->setVisible(hasPoints);

	mTempString.Clear();
	if (hasPoints == true) 
		mTempString.Format("%i Points", (int)GetSession()->GetPoints());
	mPointsLabel->setText(mTempString.AsChar());

	// elapsed time
	const Time timeElapsed = GetSession()->GetElapsedTime();
	mStopwatchWidget->SetTimeInSecs(timeElapsed.InSeconds());
}
