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

// include required headers
#include "StopwatchWidget.h"
#include "../AppManager.h"
#include <Core/LogManager.h>
#include <QHBoxLayout>
#include <QVBoxLayout>


using namespace Core;

// constructor
StopwatchWidget::StopwatchWidget(int32 iconSize, QWidget* parent) : QWidget(parent)
{
	// main layout
	QHBoxLayout* mainLayout = new QHBoxLayout();
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	setLayout( mainLayout );

	// load the stopwatch image
	/*QPixmap pixmap( ":/Images/Stopwatch.png" );
	pixmap = pixmap.scaledToHeight( iconSize );

	QLabel* imageLabel = new QLabel(this);
	imageLabel->setPixmap( pixmap );
	mainLayout->addWidget(imageLabel);*/

	// add the time label
	mTimeLabel = new QLabel();
	QFont timeFont = mTimeLabel->font();
	timeFont.setPixelSize(24);
	mTimeLabel->setFont(timeFont);
	mTimeLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	mainLayout->addWidget( mTimeLabel, 0, Qt::AlignCenter );

	if (iconSize != -1)
	{
		setMinimumHeight( iconSize );
		setMaximumHeight( iconSize );
	}

	Reset();
}


// destructor
StopwatchWidget::~StopwatchWidget()
{
}


// reset the time
void StopwatchWidget::Reset()
{
	SetTime(0, 0, 0);
}


// set the temperature
void StopwatchWidget::SetTime(int32 hours, int32 minutes, int32 seconds)
{
	mTempString.Format("%.2d:%.2d:%.2d", hours, minutes, seconds);
	mTimeLabel->setText( mTempString.AsChar() );
}


void StopwatchWidget::SetTimeInSecs(float totalElapsedTimeInSecs)
{
	int32 hours, minutes, seconds;

	hours		= totalElapsedTimeInSecs / 3600.0f;
	minutes		= (totalElapsedTimeInSecs - hours * 3600.0f) / 60.0f;
	seconds		= Math::FMod(totalElapsedTimeInSecs, 60.0f);

	SetTime( hours, minutes, seconds );
}
