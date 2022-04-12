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
#include "SwitchingImage.h"
#include <Core/LogManager.h>
#include <QPainter>
#include <QPaintEvent>


// constructor
SwitchingImage::SwitchingImage(QString imageFilenameA, QString imageFilenameB, int32 width, int32 height) : QLabel()
{
	QPixmap orgImageA( imageFilenameA );
	QPixmap orgImageB( imageFilenameB );

	mImageA = orgImageA.scaled(width, height);
	mImageB = orgImageB.scaled(width, height);

	// prepare data
	mTimer = new QTimer(this);
	mTimer->setTimerType( Qt::PreciseTimer );
	connect( mTimer, SIGNAL(timeout()), this, SLOT(SwitchImage()) );

	// start with image A
	mImageAShown = true;
	setPixmap( mImageA );
}


// switch the image
void SwitchingImage::SwitchImage()
{
	if (mImageAShown == true)
	{
		setPixmap( mImageB );
		mImageAShown = false;
	}
	else
	{
		setPixmap( mImageA );
		mImageAShown = true;
	}
}


void SwitchingImage::SetSwitchingCountDown(float countDownInSeconds)
{
	// start the timer if it isn't active yet
	if (mTimer->isActive() == false)
		mTimer->start();

	const int intervalInMS = countDownInSeconds * 1000;
	mTimer->setInterval( intervalInMS );
}
