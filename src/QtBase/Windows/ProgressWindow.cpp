/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include the required headers
#include "ProgressWindow.h"
#include <Core/LogManager.h>
#include "../QtBaseManager.h"
#include <QLabel>
#include <QSizePolicy>
#include <QApplication>
#include <QPixmap>
#include <QVBoxLayout>
#include <QDesktopWidget>
#include <QGraphicsDropShadowEffect>


using namespace Core;

// constructor
ProgressWindow::ProgressWindow(QWidget* parent) : QDialog(parent)
{
	LogDetailedInfo("Constructing progress window ...");

	setWindowFlags(Qt::WindowStaysOnTopHint/* | Qt::FramelessWindowHint*/);

	// update title of the about dialog
	//Core::String windowTitle = "Kicking ass...";
	//windowTitle += GetManager()->GetAppName();
	//setWindowTitle( windowTitle.AsChar() );

	// create the about dialog's layout
	QVBoxLayout* layout = new QVBoxLayout(this);



	// animated waiting label
	mWaitingSize = 50;
	mWaitingMovie = new QMovie( ":/Images/Icons/Loader.gif" );
	mWaitingMovie->setScaledSize( QSize(mWaitingSize, mWaitingSize) );
	mWaitingMovie->start();

	mWaitingLabel = new QLabel();
	mWaitingLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	mWaitingLabel->setMinimumSize( QSize(mWaitingSize, mWaitingSize) );
	mWaitingLabel->setMaximumSize( QSize(mWaitingSize, mWaitingSize) );
	mWaitingLabel->setStyleSheet("background:transparent;");
	mWaitingLabel->setAttribute( Qt::WA_TranslucentBackground, true );
	mWaitingLabel->setMovie( mWaitingMovie );

	layout->addWidget( mWaitingLabel, 0, Qt::AlignHCenter );



	// create the widgets
	mProgressText		= new QLabel();
	mSubProgressText	= new QLabel();
	mProgressBar		= new QProgressBar();
	mSubProgressBar		= new QProgressBar();

	mProgressBar->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
	mSubProgressBar->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

	layout->addWidget( mProgressText, 0, Qt::AlignHCenter );
	layout->addWidget( mProgressBar );
	layout->addWidget( mSubProgressText, 0, Qt::AlignHCenter );
	layout->addWidget( mSubProgressBar );

	mProgressBar->setTextVisible(true);
	mProgressBar->setRange(0, 1000);
	mProgressBar->setValue(0);

	mSubProgressBar->setTextVisible(true);
	mSubProgressBar->setRange(0, 1000);
	mSubProgressBar->setValue(0);

	setLayout(layout);
}


// destructor
ProgressWindow::~ProgressWindow()
{
	LogDetailedInfo("Destructing progress window ...");
	delete mWaitingMovie;
	OnProgressEnd();
}


void ProgressWindow::OnProgressStart(bool showProgressText, bool showProgressValue, bool showSubProgressText, bool showSubProgressValue)
{
	LogDetailedInfo("Preparing progress window ...");
	// control the visibility of the progess elements
	mProgressText->setVisible( showProgressText );
	mSubProgressText->setVisible( showSubProgressText );
	mProgressBar->setVisible( showProgressValue );
	mSubProgressBar->setVisible( showSubProgressValue );

	const int32 width = 600;
	int32 height = mWaitingSize + 25;

	int spacing = 5;
	if (showProgressText == true)		height += mProgressText->minimumSizeHint().height() + spacing;
	if (showSubProgressText == true)	height += mSubProgressText->minimumSizeHint().height() + spacing;
	if (showProgressValue == true)		height += mProgressBar->minimumSizeHint().height() + spacing;
	if (showSubProgressValue == true)	height += mSubProgressBar->minimumSizeHint().height() + spacing;

	// force the window size
	setMinimumWidth(width);
	setMinimumHeight(height);
	setMaximumWidth(width);
	setMaximumHeight(height);

	// move it into the center of the screen
	//const QRect mainWindowRect = QApplication::desktop()->screenGeometry();
	const QRect mainWindowRect = GetQtBaseManager()->GetMainWindow()->rect();
	move( mainWindowRect.center() - rect().center() );

	// show the window
	LogDetailedInfo("Showing progress window ...");
	show();
}


void ProgressWindow::OnProgressEnd()
{
	LogDetailedInfo("Hiding progress window ...");
	hide();
}


void ProgressWindow::OnProgressText(String text)
{
	LogDebug( "Changing progress window text to '%s' ...", text.AsChar() );
	mProgressText->setText(text.AsChar());
	LogDebug("Progress window text changed to '%s' ...", text.AsChar());
}


void ProgressWindow::OnProgressValue(float percentage)
{
	LogDebug( "Changing progress to %.2f ...", percentage * 1000.0f );
	mProgressBar->setValue(percentage * 1000.0f);
	LogDebug( "Progress changed to %.2f ...", percentage * 1000.0f );
}


void ProgressWindow::OnSubProgressText(String text)
{
	LogDebug( "Changing subprogress window text to '%s' ...", text.AsChar() );
	mSubProgressText->setText(text.AsChar());
	LogDebug("Subprogress window text changed to '%s' ...", text.AsChar());
}


void ProgressWindow::OnSubProgressValue(float percentage)
{
	LogDebug( "Changing subprogress to %.2f ...", percentage * 1000.0f );
	mSubProgressBar->setValue(percentage * 1000.0f);
	LogDebug( "Subprogress changed to %.2f ...", percentage * 1000.0f );
}
