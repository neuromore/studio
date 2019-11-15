/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "StatusPopupWindow.h"
#include <Core/LogManager.h>
#include "../QtBaseManager.h"
#include <QLabel>
#include <QSizePolicy>
#include <QApplication>
#include <QPixmap>
#include <QVBoxLayout>
#include <QDesktopWidget>
#include <QPropertyAnimation>
#include "StatusPopupManager.h"


#define STATUSPOPUPWINDOW_ANIMATIONTIME 350

using namespace Core;

// constructor
StatusPopupWindow::StatusPopupWindow(StatusPopupManager* manager, QWidget* parent, int32 yPos) : QDialog(parent)
{
	mManager = manager;
	mFadingOut = false;

	const QRect mainWindowRect = GetQtBaseManager()->GetMainWindow()->rect();
	const int32 width = mainWindowRect.width();
	const int32 halfHeight = GetHalfHeight();

	int waitingIconHeight = halfHeight*2-4;
	int labelWidth = width/2;

	setWindowFlags(Qt::WindowStaysOnTopHint/* | Qt::FramelessWindowHint*/);
					
	// create the about dialog's layout
	QHBoxLayout* layout = new QHBoxLayout();
	layout->setMargin(0);
	setLayout(layout);


	// animated waiting label
	mWaitingMovie = new QMovie( ":/Images/Icons/Loader.gif" );
	mWaitingMovie->setScaledSize( QSize(waitingIconHeight, waitingIconHeight) );
	mWaitingMovie->start();

	mWaitingLabel = new QLabel();
	mWaitingLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	mWaitingLabel->setMinimumSize( QSize(waitingIconHeight, waitingIconHeight) );
	mWaitingLabel->setMaximumSize( QSize(waitingIconHeight, waitingIconHeight) );
	mWaitingLabel->setStyleSheet("background:transparent;");
	mWaitingLabel->setAttribute( Qt::WA_TranslucentBackground, true );
	mWaitingLabel->setMovie( mWaitingMovie );

	layout->addWidget( mWaitingLabel, 0, Qt::AlignHCenter );



	mProgressBar = new QProgressBar();
	mProgressBar->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
	layout->addWidget( mProgressBar );

	mProgressBar->setTextVisible(true);
	mProgressBar->setRange(0, 1000);
	mProgressBar->setValue(0);

	mLabel = new QLabel();
	mLabel->setMinimumWidth(labelWidth);
	mLabel->setMaximumWidth(labelWidth);
	layout->addWidget(mLabel);


	// force the window size
	setMinimumWidth(width);
	setMinimumHeight( 2 * halfHeight );
	setMaximumWidth(width);
	setMaximumHeight( 2 * halfHeight );

	// move it into the center of the screen
	QPoint newCenter = mainWindowRect.center();
	newCenter.setY( yPos );
	move( newCenter - rect().center() );

	// create and set opacity effect
	mOpacityEffect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect( mOpacityEffect );

	// start fade in animation
	FadeIn();

	// show view
	show();
}


// destructor
StatusPopupWindow::~StatusPopupWindow()
{
	mManager->mActivePopups.RemoveByValue(this);
}


void StatusPopupWindow::OnOpacityChanged(QVariant value)
{
	float opacity = value.toFloat();

	// destroy when they faded out
	if (mFadingOut == true && opacity < 0.05)
	{
		mManager->mActivePopups.RemoveByValue(this);
		hide();
		deleteLater();

		mManager->UpdatePositions();
	}
}


// fade in animation
void StatusPopupWindow::FadeIn()
{
	QPropertyAnimation* opacityAnimation = new QPropertyAnimation( mOpacityEffect, "opacity" );
	connect( opacityAnimation, &QPropertyAnimation::valueChanged, this, &StatusPopupWindow::OnOpacityChanged );

	opacityAnimation->setDuration( STATUSPOPUPWINDOW_ANIMATIONTIME );
	opacityAnimation->setStartValue( 0.0 );
	opacityAnimation->setEndValue( 1.0 );
	opacityAnimation->setEasingCurve( QEasingCurve::InCubic );
	opacityAnimation->start();
}


// fade out animation
void StatusPopupWindow::FadeOut()
{
	QPropertyAnimation* opacityAnimation = new QPropertyAnimation( mOpacityEffect, "opacity" );
	connect( opacityAnimation, &QPropertyAnimation::valueChanged, this, &StatusPopupWindow::OnOpacityChanged );

	opacityAnimation->setDuration( STATUSPOPUPWINDOW_ANIMATIONTIME );
	opacityAnimation->setStartValue( 1.0 );
	opacityAnimation->setEndValue( 0.0 );
	opacityAnimation->setEasingCurve( QEasingCurve::InCubic );
	opacityAnimation->start();

	mFadingOut = true;
}


// animated vertical translation
void StatusPopupWindow::MoveTo(int32 y)
{
    QPropertyAnimation* moveAnimation = new QPropertyAnimation( this, "pos" );

    moveAnimation->setDuration( STATUSPOPUPWINDOW_ANIMATIONTIME );
    moveAnimation->setStartValue( pos() );
    moveAnimation->setEndValue( QPoint(pos().x(), y-GetHalfHeight()) );
    moveAnimation->setEasingCurve( QEasingCurve::InCubic );
    moveAnimation->start();
}


void StatusPopupWindow::ShowMessage(const char* text)
{
	mLabel->setText(text);
}


void StatusPopupWindow::SetProgress(float progress)
{
	mProgressBar->setValue(progress * 1000.0f);
}